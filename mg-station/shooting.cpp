#include <vector>
#include <tuple>

#include "acex_draw_ex\include\sprite.h"
#include "acex_draw_ex\include\primitive.h"

#include "AppBase.h"

#include "shooting.h"

#define ThrowIfFalse(x) if(!x)throw(#x);
namespace {
	using acs::vector::i2d;
	using acs::vector::f2d;
	using acs::vector::f3d;

	using namespace acex::draw;
	using namespace acex::draw::ex;
}

int game_main::Shooting::main(const game_util::COMMON_RESOURCE& resoure) {
	using namespace acex::draw;
	using namespace acex::draw::ex;

	auto draw = resoure.idraw;
	auto camera2D = resoure.camera2D;
	auto screent = resoure.iscreent;
	auto& timeKeeper = *resoure.keeper;
	auto& inputmng = *resoure.inputMng;
	auto ScreenW = resoure.screenW, ScreenH = resoure.screenH;
	bool isFirstScene;

	//--描画リソース定義　始--
	acs::SIACS<IRenderResource>resShooting0;
	if (CreateIRenderResourceP(draw, resoure.ioImg, L"resource\\shooting0.png", &resShooting0) == false)throw(std::exception("Resource create err"));

	acs::SIACS<IRenderResource>rresourceGameEnd;
	if (CreateIRenderResourceP(draw, resoure.ioImg, L"resource\\game_end.png", &rresourceGameEnd) == false)throw(std::exception("Resource create err"));
	sprite::SimpleSprite spriteGameEnd(draw, 1, camera2D, rresourceGameEnd);
	spriteGameEnd[0].size = { 512,128 };
	spriteGameEnd[0].texstate = { 0,0,1,1 };
	spriteGameEnd[0].pos = { 0,0,0 };
	sprite::SimpleSprite spPlayer(draw, 1, camera2D, resShooting0);
	spPlayer[0].size = {64,64};
	spPlayer[0].texstate = { 0, 0, 1.f / 4.f, 1.f / 4.f };
	spPlayer[0].pos = { 0,0,0 };
	spPlayer[0].flag = true;
	sprite::SimpleSprite spPlayersShot(draw, 30, camera2D, resShooting0);
	int spPlayersShotClock[30];
	for (size_t i = 0; i < 30; i++)
	{
		spPlayersShot[i].size = { 32,32 };
		spPlayersShot[i].texstate = { 0, 1.f / 4.f , 1.f / 8.f, 1.f / 8.f };
		spPlayersShot[i].flag = false;
		spPlayersShotClock[i] = 0;
	}
	bool isWaitingNow = false;

	//--描画リソース定義　終--
	game_util::DrawSyncGuarder dsg(draw);//描画リソースの定義の後に置く
BEGIN:
	//--ゲーム開始時の初期化　始--
	spriteGameEnd[0].flag = false;

	timeKeeper.reset();
	isFirstScene = true;
	//--ゲーム開始時の初期化　終--

	while (timeKeeper.keepTime()) {

		acs::vector::i2d screenSize;
		AppBase::GetScreenSize(screenSize);
		//スクリーンサイズ変更時　処理
		if (AppBase::ScreenCheckResized()) {
			ThrowIfFalse(draw->ResizeScreenTarget({ (acs::uint)screenSize.x ,(acs::uint)screenSize.y }));
		}

		//--ゲームのメイン処理　始--
		//入力処理
		{
			inputmng.updateInputState();

			if (inputmng.getKeyHoldingtick(VK_ESCAPE) == 1) {
				break;
			}

			if (inputmng.getKeyHoldingtick(VK_RETURN) == 1) {
				if (isWaitingNow) isWaitingNow = false;
				else isWaitingNow = true;
			}

			acs::vector::f2d mousePos = inputmng.getMousePos({ ScreenW,ScreenH }, screenSize);

			if (false == isWaitingNow) {
				if (inputmng.getKeyHoldingtick('W') > 0)spPlayer[0].pos.y += 2;
				if (inputmng.getKeyHoldingtick('S') > 0)spPlayer[0].pos.y -= 2;
				if (inputmng.getKeyHoldingtick('A') > 0)spPlayer[0].pos.x -= 2;
				if (inputmng.getKeyHoldingtick('D') > 0)spPlayer[0].pos.x += 2;
				if (inputmng.getKeyHoldingtick(VK_SPACE) == 1) {
					for (size_t i = 0; i < 30; i++)
					{
						if (false == spPlayersShot[i].flag) {
							spPlayersShot[i].flag = true;
							spPlayersShot[i].pos = spPlayer[0].pos + f3d(0, 30, 0);
							spPlayersShotClock[i] = 0;
							break;
						}
					}
				}
			}
			
		}
		//演算処理
		if (false == isWaitingNow) {
			for (size_t i = 0; i < 30; i++)
			{
				if (spPlayersShot[i].flag) {
					spPlayersShot[i].pos += f3d(0, 6, 0);
					++spPlayersShotClock[i];
					if (spPlayersShotClock[i] % 2 == 1) {
						spPlayersShot[i].texstate = { 0, 1.f / 4.f , 1.f / 8.f, 1.f / 8.f };
					}
					else {
						spPlayersShot[i].texstate = { 1.f / 8.f, 1.f / 4.f , 1.f / 8.f, 1.f / 8.f };
					}
					if (spPlayersShot[i].pos.y >= (static_cast<float>(ScreenH / 2) + 30))spPlayersShot[i].flag = false;
				}
			}
		}
		
		//--ゲームのメイン処理　終--

		//--描画関連処理　始--
		//表示
		if (isFirstScene) isFirstScene = false;
		else draw->Present(0);

		//描画データ更新
		{
			Updater updater(draw);
			spriteGameEnd.Update(updater);
			spPlayer.Update(updater);
			spPlayersShot.Update(updater);
		}

		//描画
		{
			Drawer drawer(draw);
			float backGnd[] = { 0,0,0,0 };
			drawer->SetTargets(1, &screent, nullptr);
			drawer->ClearTarget(0, backGnd);
			spriteGameEnd.Draw(drawer);
			spPlayer.Draw(drawer);
			spPlayersShot.Draw(drawer);
		}
		//--描画関連処理　終--
	}
	return 0;
}