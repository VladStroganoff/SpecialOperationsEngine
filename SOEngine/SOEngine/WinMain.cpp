
#include "Window.h"
#include "WindowsMessageMap.h"
#include <sstream>

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


		while (!window.mouse.IsEmpty())
		{
			const auto event = window.mouse.Read();
			if (event.GetType() == Mouse::Event::Type::Move)
			{
				std::ostringstream oss;
				oss << "Mouse Position: (" << event.GetPosX() << "," << event.GetPosY() << ")";
				window.SetTitle(oss.str());
			}
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