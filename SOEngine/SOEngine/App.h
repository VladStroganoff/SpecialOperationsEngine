#pragma once
#include "Window.h"
#include "Time.h"

class App
{
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	Window window;
	Time timer;
};