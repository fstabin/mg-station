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

	bool ScreenSetup(int enableResize = 0);//スクリーンの表示
	bool ScreenSetAspect(const acs::vector::i2d&);
	bool ScreenSetSize(const acs::vector::i2d&);//スクリーンのサイズ設定(ピクセル)
	int ScreenCheckResized(int flag = 0);//0フラグ消去,1放置

	 //アプリが終了しなければならないときfalseを返す
	bool AppWait(acs::ulong dwMillisec);
	void StartLClickCheck();
	bool CheckLClicked();
	void StartRClickCheck();
	bool CheckRClicked();

	void GetMouseStats(AppBase::MOUSESTATS &stats);

	bool CheckKeyDown(unsigned char);

	bool GetScreenSize(acs::vector::i2d&);
}