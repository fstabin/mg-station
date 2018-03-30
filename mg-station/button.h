#pragma once
#include<list>
#include "acs\include\vector.h"
namespace game_util {
	class ButtonManager;
	class Button {
		size_t mButtonID;
		acs::vector::f2d mPos;
		acs::vector::f2d mHitBoxSize;
	
		Button(size_t aButtonID, acs::vector::f2d aPos, acs::vector::f2d aHitBoxSize) :mButtonID(aButtonID), mPos(aPos), mHitBoxSize(aHitBoxSize) {

		}
	public:

		friend class ButtonManager;

		bool isHitting(acs::vector::f2d virtualCursorPos) {
			auto dur = virtualCursorPos - mPos;
			return (std::abs(dur.x) <= (mHitBoxSize.x / 2)) && (std::abs(dur.y) <= (mHitBoxSize.y / 2 ));
		}
	
		size_t getButtonID() { return mButtonID; };
		
	};

	class ButtonManager {
		std::list<Button>mButtonLIst;
	public:
		Button& createButton(size_t aButtonID, acs::vector::f2d aPos, acs::vector::f2d aHitBoxSize) {
			 mButtonLIst.push_back(Button(aButtonID, aPos, aHitBoxSize));
			 return mButtonLIst.back();
		}
	
		Button* getHittingButton(acs::vector::f2d aCurSorPos) {
			for (auto& button : mButtonLIst) {
				if(button.isHitting(aCurSorPos))return &button;
			}
			return nullptr;
		}
	};
}
