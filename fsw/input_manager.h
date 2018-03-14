#pragma once
#include "AppBase.h"

namespace game_util {
	class InputManager {
		AppBase::MOUSESTATS mMstat;
		unsigned long mHoldingtick[256 + 2];
	public:
		InputManager() {
			for (auto& a : mHoldingtick)a = 0;
		}

		void updateInputState() {
			for (size_t i = 0; i < 256; i++)
			{
				if (AppBase::CheckKeyDown(i))++mHoldingtick[i];
				else mHoldingtick[i] = 0;
			}

			AppBase::GetMouseStats(mMstat);

			if (mMstat.left) {
				++mHoldingtick[256];
			}
			else mHoldingtick[256] = 0;
			if (mMstat.right)++mHoldingtick[257];
			else mHoldingtick[257] = 0;
		}

		acs::vector::f2d getMousePos(acs::vector::i2d virtualScreenSize, acs::vector::i2d  realScreenSize) const {
			return { virtualScreenSize.x * (((float)mMstat.pos.x / (float)realScreenSize.x) - 0.5f), -(virtualScreenSize.y * (((float)mMstat.pos.y / (float)realScreenSize.y) - 0.5f))};
		}

		unsigned long getKeyHoldingtick(unsigned char code) const {
			return mHoldingtick[code];
		}

		unsigned long getMouseHoldingtick(unsigned char code) const {
			if (code >= 2)return 0;
			return mHoldingtick[256 + code];
		}
	};

}
