#pragma once
#include <chrono>
#include<algorithm>
#include <thread>

#include "AppBase.h"

namespace game_util {
	class FpsKeeper {
		std::chrono::system_clock::time_point tp;
	public:
		FpsKeeper() {
			tp = std::chrono::system_clock::now();
		}
		
		void reset() {
			tp = std::chrono::system_clock::now();
		}
		bool keepTime() {
			bool result = AppBase::AppWait(0);
			std::this_thread::sleep_for(std::chrono::milliseconds(std::max(0ll, 1000 / 60 - 1 - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp).count())));
			tp = std::chrono::system_clock::now();
			return result;
		}
	};
}
