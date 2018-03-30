#pragma once
#if defined WIN32 | defined _WIN64
#include <minwindef.h>
#endif

#include "acs\include\def.h"
#include "acs\include\vector.h"
namespace AppBase {
	extern HINSTANCE hInstance;
	extern HWND hMainWindow;
	struct MOUSESTATS {
		acs::vector::i2d pos;
		bool left;
		bool right;
	};

	bool ScreenSetup(int enableResize = 0);//�X�N���[���̕\��
	bool ScreenSetAspect(const acs::vector::i2d&);
	bool ScreenSetSize(const acs::vector::i2d&);//�X�N���[���̃T�C�Y�ݒ�(�s�N�Z��)
	int ScreenCheckResized(int flag = 0);//0�t���O����,1���u

	 //�A�v�����I�����Ȃ���΂Ȃ�Ȃ��Ƃ�false��Ԃ�
	bool AppWait(acs::ulong dwMillisec);
	void StartLClickCheck();
	bool CheckLClicked();
	void StartRClickCheck();
	bool CheckRClicked();

	void GetMouseStats(AppBase::MOUSESTATS &stats);

	bool CheckKeyDown(unsigned char);

	bool GetScreenSize(acs::vector::i2d&);
}