#pragma once
#include "AppBase.h"

namespace game_util {
	class InputManager {
		AppBase::MOUSESTATS mMstat;
		//³‚ÌŽž-‰Ÿ‚³‚ê‚Ä‚¢‚éŽžŠÔ
		//•‰‚ÌŽž-—£‚³‚ê‚Ä‚¢‚éŽžŠÔ
		long long mHoldingtick[256 + 2];
	public:
		InputManager() {
			for (auto& a : mHoldingtick)a = 0;
		}

		void updateInputState() {
			auto setHoldingTick = [this](size_t index, bool isHolded)-> void {
				if (isHolded) {
					if (mHoldingtick[index] < 0)mHoldingtick[index] = 0;
					++mHoldingtick[index];
				}
				else {
					if (mHoldingtick[index] > 0)mHoldingtick[index] = 0;
					--mHoldingtick[index];
				}
			};

			for (size_t i = 0; i < 256; i++)
			{
				setHoldingTick(i, AppBase::CheckKeyDown(i));
			}

			AppBase::GetMouseStats(mMstat);

			setHoldingTick(256, mMstat.left);
			setHoldingTick(257, mMstat.right);
		}

		acs::vector::f2d getMousePos(acs::vector::i2d virtualScreenSize, acs::vector::i2d  realScreenSize) const {
			return { virtualScreenSize.x * (((float)mMstat.pos.x / (float)realScreenSize.x) - 0.5f), -(virtualScreenSize.y * (((float)mMstat.pos.y / (float)realScreenSize.y) - 0.5f))};
		}

		long long getKeyHoldingtick(unsigned char code) const {
			return mHoldingtick[code];
		}

		long long getMouseHoldingtick(unsigned char code) const {
			if (code >= 2)return 0;
			return mHoldingtick[256 + code];
		}
	};

}
