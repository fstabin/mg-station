#pragma once
#include "acex_draw\include\acex_draw.h"

namespace game_util {
	//�`��API�̃��\�[�X�̊J�����ɓ������s���A�v���̋����I����h��
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
