#pragma once
#include <chrono>

class Time {
public:
	Time();
	float Mark();
	float Peek();
private:
	std::chrono::steady_clock::time_point last;
};