#pragma once
#include "acex_draw\include\acex_draw.h"
#include "acs_ioImage\include\acs_ioImage.h"
#include "fps_keeper.h"
#include "input_manager.h"
#include "button.h"
#include "draw_sync_helper.h"
namespace game_util {
	struct COMMON_RESOURCE {
		int screenW, screenH;//äÓñ{èÓïÒ

		acex::draw::IDraw* idraw;
		acex::draw::ITarget* iscreent;
		acex::draw::ICamPro* camera2D;

		acs::image::IioImage* ioImg;

		FpsKeeper* keeper;
		InputManager* inputMng;
	};

	enum GameCommand{
		Invalid,
		GotoMainMenu,
		StartBlockBrake,
		StartStickErase,
		StartShooting,
		EndGame,
		EndApp,
	};
}