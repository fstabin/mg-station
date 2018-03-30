#pragma once
#include <chrono>

#include "acs\include\def.h"
#include "acs_ioImage\include\acs_ioImage.h"

#include "acex_draw\include\acex_draw.h"
#include "acex_draw_ex\include\acex_draw_ex.h"

#pragma comment (lib, "acex_draw_dx12.lib")
#pragma comment (lib, "acex_draw_ex.lib")
#pragma comment (lib, "acs_ioImage.lib")

#include "AppBase.h"

#include "game_common.h"
#include "main_menu.h"
#include "block_blake.h"
#include "Stick_erase.h"

#define ThrowIfFalse(x) if(!x)throw(#x);

int ScreenW = 960, ScreenH = 640;

int main() {
	
	using namespace acex::draw;
	using namespace acex::draw::ex;
	acs::SIACS<IDraw> draw;
	acs::SIACS<ITarget> screent;
	AppBase::ScreenSetSize({ ScreenW ,ScreenH });
	AppBase::ScreenSetAspect({ ScreenW ,ScreenH });
	{
		acs::vector::i2d ss;
		AppBase::GetScreenSize(ss);
		INIT_DESC id;
		id.hWnd = AppBase::hMainWindow;
		id.Size = { (acs::uint) ss.x, (acs::uint)ss.y };
		id.useWarpDevice = false;
		ThrowIfFalse(CreateDraw(&id, &draw));
		ThrowIfFalse(draw->GetScreenTarget(&screent));
	}
	Camera camera2D(draw);
	camera2D.setpos({ 0, 0, 0 });
	camera2D.setsize(acs::vector::f2d( ScreenW , ScreenH ));
	camera2D.setdirection({ 0,0,1 });
	camera2D.setup({ 0,1,0 });
	camera2D.setzrange({ 0,1 });
	camera2D.setmode(CM_2D);

	acs::SIACS<acs::image::IioImage> ioImg;
	if(acs::image::CreateioImage(&ioImg) == false)throw std::exception("ImageModuleErr");

	game_util::FpsKeeper timeKeeper;
	game_util::InputManager inputmng;

	game_util::COMMON_RESOURCE  resource = {
		ScreenW, ScreenH,
		draw,
		screent,
		camera2D,
		ioImg,
		&timeKeeper,
		&inputmng
	};

	//描画データ更新
	{
		Updater updater(draw);
		camera2D.Update(updater);
	}

	AppBase::ScreenSetup(true);

	acs::SIACS<IRenderResource> irrGameIcons;
	if (CreateIRenderResourceP(draw,ioImg,L"resource\\game_icon.png", &irrGameIcons) == false)throw std::exception("create resource err");

	game_util::GameCommand command = game_util::GameCommand::GotoMainMenu;
	int result;
	using game_util::GameCommand;
	while (command != GameCommand::EndApp) {
		switch (command)
		{
		case GameCommand::GotoMainMenu:
			{
				game_main::MainMenu game;
				auto pr =  game.main(resource);
				result = pr.first;
				command = pr.second;
				if (result != 0)return result;
			}
			break;
		case GameCommand::StartBlockBrake:
		{
			game_main::BlockBrake game;
			result = game.main(resource);
			command = GameCommand::EndGame;
			if (result != 0)return result;
		}
			break;
		case GameCommand::StartStickErase:
		{
			game_main::StickErase game;
			result = game.main(resource);
			command = GameCommand::EndGame;
			if (result != 0)return result;
		}
			break;
		case GameCommand::EndGame:
			if (AppBase::AppWait(0) == false) {
				command = GameCommand::EndApp;
			}
			else {
				command = GameCommand::GotoMainMenu;
			}
			break;
		case GameCommand::EndApp:
			break;
		default:
			command = GameCommand::EndGame;
			break;
		}
	}

	if (result != 0)return result;
	
	return 0;
}