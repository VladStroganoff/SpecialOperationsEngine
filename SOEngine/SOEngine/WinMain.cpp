
#include "Window.h"
#include "WindowsMessageMap.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)


{
	static WindowsMessageMap map;


	try
	{
	Window window(800, 300, "Special Operations Engine");
	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (window.keyboard.KeyIsPressed(VK_MENU))
		{
			MessageBox(nullptr, "Pressed some bullshit", "Pressed spacebar",0);
		}

	}

	if (gResult == -1)
	{
		return -1;
	}

	return msg.wParam;
	}
	catch (const GreatException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "StandardException: ", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No deets: ", "Unknonwn exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}