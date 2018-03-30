#pragma once

#include "AppBase.h"

#include "game_common.h"

namespace game_main {

	class MainMenu {

	public:
		std::pair<int, game_util::GameCommand> main(const game_util::COMMON_RESOURCE&);
	};

}