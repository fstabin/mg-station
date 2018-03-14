#include "stdafx.h"
#define CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <thread>
#include <atomic>
#include <combaseapi.h>
#include <shared_mutex>
#include <Windows.h>
#include <WindowsX.h>
#include<algorithm>

#include "resource.h"
#include "AppBase.h"

#define MAX_LOADSTRING 100

#define WM_USER_SCREEN_SETUP ( WM_USER + 1 )
#define WM_USER_SCREEN_SETSIZE ( WM_USER_SCREEN_SETUP + 1 )

using acs::vector::i2d;
using acs::vector::f2d;

// グローバル変数:
HINSTANCE AppBase::hInstance;
HWND AppBase::hMainWindow;

//std::mutex WindowGuard;
std::atomic_uint screenSizew = CW_USEDEFAULT;
std::atomic_uint screenSizeh = CW_USEDEFAULT;
std::atomic_int screenResized = 0;
i2d screenAspect = { 0,0 };
std::atomic_int screenSizeChanging = 0;

i2d screenSysSize = { 0,0 };

WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

std::shared_mutex mutCursor;
POINT pCursor;
std::atomic_bool bWindowClosed = false;
std::atomic_uint bLClick = false;
std::atomic_uint bRClick = false;

std::atomic_bool bKeyStates[255];

extern int main();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
ATOM                RegisterWindowClass(HINSTANCE hInstance, WNDPROC proc);
HWND                CreateMyWindow(HINSTANCE, int);

void MainThreadFunc();
int WindowThreadFunc();
using AppBase::hMainWindow;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// グローバル文字列を初期化
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_FSW, szWindowClass, MAX_LOADSTRING);

	if (FAILED(
		CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		return FALSE;
	}

	AppBase::hInstance = hInstance;
	//初期化

	{

		//ウィンドウ作成
		RegisterWindowClass(hInstance, WndProc);
		if (!(hMainWindow = CreateMyWindow(hInstance, nCmdShow)))
		{
			return FALSE;
		}

	}
	for (size_t i = 0; i < 255; i++)
	{
		bKeyStates[i] = false;
	}
	int RenderResult;
	{
		std::thread thAppMain;
		thAppMain = std::thread(MainThreadFunc);
		RenderResult = WindowThreadFunc();
		thAppMain.join();
	}

	CoUninitialize();
	return RenderResult;
}

void MainThreadFunc() {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	try
	{
		main();
	}
	catch (const std::exception& except)
	{
		MessageBoxA(hMainWindow, except.what(), "Err", MB_ICONINFORMATION);
	}
	PostMessage(hMainWindow, WM_CLOSE, 0, 0);
	CoUninitialize();
}
int WindowThreadFunc() {
	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(AppBase::hInstance, MAKEINTRESOURCE(IDC_FSW));
	while (true) {
		if (GetMessage(&msg, nullptr, 0, 0) > 0)
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else break;
	}
	bWindowClosed = true;
	return (int)msg.wParam;
}

ATOM RegisterWindowClass(HINSTANCE hInstance, WNDPROC proc)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FSW));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FSW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
HWND CreateMyWindow(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return nullptr;
	}
	return hWnd;
}
void SetBestWindowSize(HWND hWnd) {
	RECT rClient;
	RECT rWindow;
	::GetClientRect(hWnd, &rClient);
	::GetWindowRect(hWnd, &rWindow);

	if (screenSizew == CW_USEDEFAULT) {
		return;
	}
	if (screenAspect.x != 0) {
		f2d faspect = f2d(screenAspect);
		float rw = static_cast<float>(screenSizew) / faspect.x;
		float rh = static_cast<float>(screenSizeh) / faspect.y;
		float offs = std::min(rw, rh);
		i2d size = i2d(faspect * offs);
		screenSizew = size.x;
		screenSizeh = size.y;
	}
	::screenSysSize = {
		(rWindow.right - rWindow.left) - (rClient.right - rClient.left) ,
		(rWindow.bottom - rWindow.top) - (rClient.bottom - rClient.top)
	};
	int newWidth = screenSysSize.x + screenSizew;
	int newHeight = screenSysSize.y + screenSizeh;


	::SetWindowPos(hWnd, nullptr, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);
}
void SetWindowMode(HWND hWnd, WPARAM resize) {
	LONG lv = (resize == 1) ? WS_THICKFRAME : 0;
	::SetWindowLong(hWnd, GWL_STYLE, lv | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU);
	::UpdateWindow(hWnd);
	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);
}

void CheckSize(WPARAM wParam, LPARAM lParam)
{
	if (screenAspect.x == 0)return;
	RECT    *rc;
	float     cwidth, cheight;//
	float     mincw, minch;//クライアント幅、高さ（最小）

	f2d xy = screenAspect;
	float ypx = xy.y / xy.x;
	float xpy = xy.x / xy.y;
	i2d sss = screenSysSize;

	rc = (RECT *)lParam;
	mincw = (float)(GetSystemMetrics(SM_CXMINTRACK) - sss.x);
	minch = mincw * ypx;

	cwidth = (float)(rc->right - rc->left - sss.x);
	cheight = (float)(rc->bottom - rc->top - sss.y);

	switch (wParam) {
	case    WMSZ_TOP:
		cwidth = cheight * xpy;
		if (cwidth < mincw) {
			rc->right = rc->left + (int)mincw + sss.x;
			rc->top = rc->bottom - (int)minch - sss.y;
		}
		else {
			rc->right = rc->left + (int)cwidth + sss.x;
			rc->top = rc->bottom - (int)cheight - sss.y;
		}
		break;
	case    WMSZ_BOTTOM:
		cwidth = cheight * xpy;
		if (cwidth < mincw) {
			rc->right = rc->left + (int)mincw + sss.x;
			rc->bottom = rc->top + (int)minch + sss.y;
		}
		else {
			rc->right = rc->left + (int)cwidth + sss.x;
			rc->bottom = rc->top + (int)cheight + sss.y;
		}
		break;
	case    WMSZ_LEFT:
	case    WMSZ_RIGHT:
		rc->bottom = rc->top + (int)(cwidth*(ypx)) + sss.y;
		break;
	case    WMSZ_TOPLEFT:
		if ((cwidth / cheight) < (xpy)) {
			rc->left = rc->right - (int)(cheight*(xpy)) + sss.x;
		}
		else {
			rc->top = rc->bottom - (int)(cwidth*(ypx)) - sss.y;
		}
		break;
	case    WMSZ_TOPRIGHT:
		if ((cwidth / cheight) < (xpy)) {
			rc->right = rc->left + (int)(cheight*(xpy)) + sss.x;
		}
		else {
			rc->top = rc->bottom - (int)(cwidth*(ypx)) - sss.y;
		}
		break;
	case    WMSZ_BOTTOMLEFT:
		if ((cwidth / cheight) < (xpy)) {
			rc->left = rc->right - (int)(cheight*(xpy)) - sss.x;
		}
		else {
			rc->bottom = rc->top + (int)(cwidth*(ypx)) + sss.y;
		}
		break;
	case    WMSZ_BOTTOMRIGHT:
		if ((cwidth / cheight) < (xpy)) {
			rc->right = rc->left + (int)(cheight*(xpy)) + sss.x;
		}
		else {
			rc->bottom = rc->top + (int)(cwidth*(ypx)) + sss.y;
		}
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ENTERSIZEMOVE:
		screenSizeChanging = 1;
		break;
	case WM_EXITSIZEMOVE:
		screenSizeChanging = 0;
		break;
	case WM_SIZING:
		CheckSize(wParam, lParam);
		return TRUE;
	case WM_SIZE:
		switch (wParam)
		{
		case SIZE_RESTORED:
		case SIZE_MAXIMIZED:
			screenSizew = lParam & 0xFFFF;        // xサイズ
			screenSizeh = (lParam >> 16) & 0xFFFF; // yサイズ
			screenResized = 1;
		default:
			break;
		}
		return TRUE;
	case WM_MOUSEMOVE:
	{
		std::lock_guard<std::shared_mutex>(std::ref(mutCursor));
		pCursor.x = GET_X_LPARAM(lParam);
		pCursor.y = GET_Y_LPARAM(lParam);
	}
	break;
	case WM_LBUTTONDOWN:
		if (((bLClick | bRClick) & 0b10) == 0)SetCapture(hWnd);
		bLClick |= 0b11;
		break;
	case WM_LBUTTONUP:
		bLClick &= 0b01;
		if (((bLClick | bRClick) & 0b10) == 0)ReleaseCapture();
		break;
	case WM_RBUTTONDOWN:
		if (((bLClick | bRClick) & 0b10) == 0)SetCapture(hWnd);
		bRClick |= 0b11;
		break;
	case WM_RBUTTONUP:
		bRClick &= 0b01;
		if (((bLClick | bRClick) & 0b10) == 0)ReleaseCapture();
		break;
	case WM_KEYDOWN:
		if (wParam > 256)break;
		bKeyStates[wParam] = true;;
		break;
	case WM_KEYUP:
		if (wParam > 255)break;
		bKeyStates[wParam] = false;
		break;
	case WM_KILLFOCUS:
		for (size_t i = 0; i < 255; i++)
		{
			bKeyStates[i] = false;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_USER_SCREEN_SETUP:
		SetWindowMode(hWnd, wParam);
		break;
	case WM_USER_SCREEN_SETSIZE:
		SetBestWindowSize(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

bool AppBase::AppWait(acs::ulong dwMillisec) {
	std::this_thread::sleep_for(std::chrono::milliseconds(dwMillisec));
	//ウィンドウが閉じられたかどうかを返す
	for (DWORD i = 0; i <= dwMillisec; i++)
	{
		if (bWindowClosed)return false;
	}
	return true;
}

void AppBase::StartLClickCheck() {
	bLClick &= 0b10;
}
bool AppBase::CheckLClicked() {
	return ((bLClick & 0b01) == 0b01);
}

void AppBase::StartRClickCheck() {
	bRClick &= 0b10;
}
bool AppBase::CheckRClicked() {
	return ((bRClick & 0b01) == 0b01);
}

void AppBase::GetMouseStats(MOUSESTATS &stats) {
	std::lock_guard<std::shared_mutex>(std::ref(mutCursor));
	stats.pos.x = pCursor.x;
	stats.pos.y = pCursor.y;
	stats.left = ((bLClick & 0b10) == 0b10);
	stats.right = ((bRClick & 0b10) == 0b10);
}

bool AppBase::CheckKeyDown(unsigned char code) {
	return bKeyStates[code];
}

bool AppBase::GetScreenSize(acs::vector::i2d& size) {
	size.x = screenSizew;
	size.y = screenSizeh;
	return true;
}

bool AppBase::ScreenSetup(int enableResize) {
	::PostMessageW(hMainWindow, WM_USER_SCREEN_SETUP, enableResize, 0);
	return true;
}
bool AppBase::ScreenSetAspect(const acs::vector::i2d& aspect) {
	screenAspect = aspect;
	::PostMessageW(hMainWindow, WM_USER_SCREEN_SETSIZE, 0, 0);
	return true;
}
bool AppBase::ScreenSetSize(const acs::vector::i2d& size) {
	screenSizew = size.x;
	screenSizeh = size.y;
	::PostMessageW(hMainWindow, WM_USER_SCREEN_SETSIZE, 0, 0);
	return true;
}
int  AppBase::ScreenCheckResized(int flag) {
	int f = screenResized & (screenSizeChanging ^ 0b01);
	if (f)screenResized &= flag;
	return (f != 0);
}