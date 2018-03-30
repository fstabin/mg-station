#pragma once
#include "acex_draw\include\acex_draw.h"

namespace game_util {
	//描画APIのリソースの開放時に同期を行いアプリの強制終了を防ぐ
	class DrawSyncGuarder{
		acex::draw::IDraw* mDraw;
	public:
		DrawSyncGuarder(acex::draw::IDraw* draw) :mDraw(draw){

		}

		~DrawSyncGuarder() {
			mDraw->WaitDrawDone();
		}
	};
}
