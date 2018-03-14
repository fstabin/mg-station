#include "main_menu.h"
#include "acex_draw_ex\include\acex_draw_ex.h"
namespace {
	using acs::vector::i2d;
	using acs::vector::f2d;
	using acs::vector::f3d;
	size_t gIconAmount = 2;
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

	acs::SIACS<IRenderResource> imageRResource;
	if(false == CreateIRenderResourceP(draw, ioimg, L"resource\\game_icon.png", &imageRResource))throw (std::exception("create IRenderResource err"));
	sprite::SimpleSprite sprite(draw, gIconAmount + 1, camera2D, imageRResource);
	for (size_t i = 0; i < gIconAmount + 1; i++)
	{
		auto& state = sprite[i];
		state = { true, {0,0,0},{64,64},{0,0,0},{static_cast<float>(i) * 0.25f, 0, 0.25f, 0.5f} };
	}
	sprite[0].pos = {-64,0,0};
	sprite[1].pos = { 64,0,0 };
	sprite[2].pos = sprite[0].pos;
	sprite[2].texstate = { 3 * 0.25f, 0.5f, 0.25f, 0.5f };

	game_util::ButtonManager btmng;
	for (size_t i = 0; i < gIconAmount; i++)
	{
		btmng.createButton(i, sprite[i].pos.xy(), sprite[i].size);
	}

	size_t curIndex = 0;
	bool cursorMoved = false;//カーソルが動いたかどうかのフラグ

BEGIN:

	timeKeeper.reset();

	while (timeKeeper.keepTime()) {

		acs::vector::i2d screenSize;
		AppBase::GetScreenSize(screenSize);
		//スクリーンサイズ変更時　処理
		if (AppBase::ScreenCheckResized()) {
			if (false == draw->ResizeScreenTarget({ (acs::uint)screenSize.x ,(acs::uint)screenSize.y }))throw(std::exception("screen resize err"));
		}

		{
			inputmng.updateInputState();
			if (inputmng.getKeyHoldingtick(VK_LEFT) == 1) {
				if (curIndex == 0)curIndex = 1;
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

		draw->Present(0);
	}
	return std::pair<int, game_util::GameCommand>(0, game_util::GameCommand::EndApp);
}
