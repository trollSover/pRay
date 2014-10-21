#include "Window.h"
#include "IApplication.h"

extern IApplication* g_pApplication;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool Window::Init(HINSTANCE hInstance, HWND& hWnd, Resolution resolution, LPCSTR appName)
{
	WNDCLASSEX wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC)WindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= appName;
	wc.cbSize			= sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
		return false;

	int x = (GetSystemMetrics(SM_CXSCREEN) - resolution.width) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - resolution.height) / 2;

	//hWnd = CreateWindowEx(	WS_EX_APPWINDOW, appName, appName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
	//						x, y, resolution.width, resolution.height, NULL, NULL, hInstance, NULL);

	hWnd = CreateWindowEx(	WS_EX_APPWINDOW, appName, appName, WS_BORDER | WS_CAPTION,
							x, y, resolution.width, resolution.height, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return false;

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	return true;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return g_pApplication->ApplicationProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}