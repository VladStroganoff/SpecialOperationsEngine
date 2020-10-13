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
		if (const auto ecode = Window::ProcessMessage()) // do we quit? is there input?
		{
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	const float t = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << "s";
	window.SetTitle(oss.str());
}