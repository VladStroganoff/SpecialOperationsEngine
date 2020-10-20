#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	window(800,600,"Special Operations Engine")
{}

int App::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages()) // do we quit? is there input?
		{
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	window.Gfx().ClearBuffer(c, c, 1.0f);
	window.Gfx().DrawTriangle();
	window.Gfx().EndFrame();
}