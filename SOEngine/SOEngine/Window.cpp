#include "Fwin.h"
#include "Window.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	: hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX winClass = { 0 };
	winClass.cbSize = sizeof(winClass);
	winClass.style = CS_OWNDC;
	winClass.lpfnWndProc = HandleMsgSetup;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hIcon = nullptr;
	winClass.hCursor = nullptr;
	winClass.hbrBackground = nullptr;
	winClass.lpszMenuName = nullptr;
	winClass.lpszClassName = GetName();
	winClass.hIconSm = nullptr;
	RegisterClassEx(&winClass);
}

Window::Window(int width, int height, const char* name) noexcept
{
	RECT winRect;
	winRect.left = 100;
	winRect.right = width + winRect.left;
	winRect.top = 100;
	winRect.bottom = height + winRect.top;
	AdjustWindowRect(&winRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	hWind = CreateWindow
	   (WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		winRect.right - winRect.left,
		winRect.bottom - winRect.top,
		nullptr, nullptr,
		WindowClass::GetInstance(),
		this);

	ShowWindow(hWind, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWind);
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

