#pragma once
#include "hwindows.h"  // first
#include "types.h"
#include <chrono>
#include <thread>
#include <assert.h>

class Performance
{
public:
	LARGE_INTEGER timeElapsed;
	LARGE_INTEGER frequency;
	LARGE_INTEGER startTime;
	LARGE_INTEGER endTime = {};
	LARGE_INTEGER elapsedMicroseconds = {};
	LARGE_INTEGER elapsedMicrosecondsCarry = {};
public:
	Performance()
	{
		QueryPerformanceFrequency(&timeElapsed);
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&startTime);
	}
public:
	void Update();

	void LimitFps(int target);  // Not entirely accurate
	const float getFps() const noexcept;
	const bool hasTimePassed(float seconds) noexcept;
	uint8 getFrameMask() const noexcept;
};