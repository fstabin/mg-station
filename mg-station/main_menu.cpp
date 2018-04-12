#include "main_menu.h"
#include "acex_draw_ex\include\acex_draw_ex.h"
namespace {
	using acs::vector::i2d;
	using acs::vector::f2d;
	using acs::vector::f3d;
	size_t gIconAmount = 3;
}

std::pair<int, game_util::GameCommand> game_main::MainMenu::main(const game_util::COMMON_RESOURCE &resoure)
{
	using namespace acex::draw;
	using namespace acex::draw::ex;

	auto draw = resoure.idraw;
	auto camera2D = resoure.camera2D;
	auto screent = resoure.iscreent;
	auto ioimg = resoure.ioImg;
	auto& timeKeeper = *resoure.keeper;
	auto& inputmng = *resoure.inputMng;
	auto ScreenW = resoure.screenW, ScreenH = resoure.screenH;
	bool isFirstScene;
	
	//描画リソース定義　始
	acs::SIACS<IRenderResource> imageRResource;
	if(false == CreateIRenderResourceP(draw, ioimg, L"resource\\game_icon.png", &imageRResource))throw (std::exception("create IRenderResource err"));
	sprite::SimpleSprite sprite(draw, gIconAmount + 1, camera2D, imageRResource);
	for (size_t i = 0; i < gIconAmount + 1; i++)
	{
		auto& state = sprite[i];
		state = { true, {0,0,0},{64,64},{0,0,0},{static_cast<float>(i) * 0.25f, 0, 0.25f, 0.5f} };
	}
	sprite[0].pos = {-128,0,0};
	sprite[1].pos = { 0,0,0 };
	sprite[2].pos = { 128,0,0 };
	sprite[gIconAmount].pos = sprite[0].pos;
	sprite[gIconAmount].texstate = { 3 * 0.25f, 0.5f, 0.25f, 0.5f };
	//描画リソース定義　終
	game_util::DrawSyncGuarder dsg(draw);//描画リソースの定義の後に置く
	
	//その他初期化　始
	game_util::ButtonManager btmng;
	for (size_t i = 0; i < gIconAmount; i++)
	{
		btmng.createButton(i, sprite[i].pos.xy(), sprite[i].size);
	}

	size_t curIndex = 0;
	bool cursorMoved = false;//カーソルが動いたかどうかのフラグ
							
	//その他初期化　終

BEGIN:
	//ゲーム開始時の初期化　始
	timeKeeper.reset();
	isFirstScene = true;
	//ゲーム開始時の初期化　終

	while (timeKeeper.keepTime()) {

		acs::vector::i2d screenSize;
		AppBase::GetScreenSize(screenSize);
		//スクリーンサイズ変更時　処理
		if (AppBase::ScreenCheckResized()) {
			if (false == draw->ResizeScreenTarget({ (acs::uint)screenSize.x ,(acs::uint)screenSize.y }))throw(std::exception("screen resize err"));
		}
		
		//--ゲームのメイン処理　始--
		{
			inputmng.updateInputState();
			if (inputmng.getKeyHoldingtick(VK_LEFT) == 1) {
				if (curIndex == 0)curIndex = gIconAmount - 1;
				else --curIndex;
				cursorMoved = true;
			}
			if (inputmng.getKeyHoldingtick(VK_RIGHT) == 1) {
				if (curIndex + 1 == gIconAmount)curIndex = 0;
				else ++curIndex;
				cursorMoved = true;
			}
			if (inputmng.getKeyHoldingtick(VK_ESCAPE) == 1) {
				return std::pair<int, game_util::GameCommand>(0, game_util::GameCommand::EndApp);
			}
			
			auto ID_to_Command = [](size_t id)->auto{
				switch (id)
				{
				case 0:
					return game_util::GameCommand::StartBlockBrake;
					break;
				case 1:
					return game_util::GameCommand::StartStickErase;
					break;
				case 2:
					return game_util::GameCommand::StartShooting;
					break;
				default:
					return game_util::GameCommand::Invalid;
					break;
				}
			};
			if (inputmng.getKeyHoldingtick(VK_SPACE) == 1 ){
				game_util::GameCommand command = ID_to_Command(curIndex);
				if (command != game_util::GameCommand::Invalid)return std::pair<int, game_util::GameCommand>(0, command);
			}
		
			auto pButton = btmng.getHittingButton(inputmng.getMousePos({ ScreenW ,ScreenH }, screenSize));
			if (nullptr != pButton) {
				curIndex = pButton->getButtonID();
				cursorMoved = true;
				if (inputmng.getMouseHoldingtick(0) == 1) {
					game_util::GameCommand command = ID_to_Command(curIndex);
					if (command != game_util::GameCommand::Invalid)return std::pair<int, game_util::GameCommand>(0, command);
				}
			}
		}

		if (cursorMoved) {
			cursorMoved = false;
			sprite[gIconAmount].pos = sprite[curIndex].pos;
		}
		//--ゲームのメイン処理　終--

		//--描画関連処理　始--
		//表示
		if (isFirstScene) isFirstScene = false;
		else draw->Present(0);

		//描画データ更新
		{
			Updater updater(draw);
			sprite.Update(updater);
		}

		//描画
		{
			Drawer context(draw);
			float backGnd[] = { 0,0,0,0 };
			context->SetTargets(1, &screent, nullptr);
			context->ClearTarget(0, backGnd);
			sprite.Draw(context);
		}
		//--描画関連処理　終--
	}
	return std::pair<int, game_util::GameCommand>(0, game_util::GameCommand::EndApp);
}
