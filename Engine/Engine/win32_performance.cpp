#include "win32_performance.h"
#include "time.h"

void Performance::Update()
{
	assert(frequency.QuadPart > 0);

	QueryPerformanceCounter(&endTime);
	elapsedMicroseconds.QuadPart = endTime.QuadPart - startTime.QuadPart;

	elapsedMicroseconds.QuadPart *= TIME::SECONDS_MULTIPLYER;
	elapsedMicroseconds.QuadPart /= frequency.QuadPart;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startTime);
}

void Performance::LimitFps(int32 target)
{
	assert(target > 0);
	while (getFps() > target)
	{
		std::this_thread::sleep_for(
			std::chrono::milliseconds(
			(int32)TIME::MILISECONDS_MULTIPLYER / target
				)
			);
		QueryPerformanceCounter(&elapsedMicroseconds);
	}
}

const float Performance::getFps() const noexcept
{
	if (elapsedMicroseconds.QuadPart > 0)
	{
		return (float)(TIME::SECONDS_MULTIPLYER / elapsedMicroseconds.QuadPart);
	}
	else
	{
		return 0.0;
	}
}

const bool Performance::hasTimePassed(float seconds) noexcept
{
	if (elapsedMicroseconds.QuadPart > 0)
	{
		// check if second has passed
		if (elapsedMicrosecondsCarry.QuadPart > TIME::SECONDS_MULTIPLYER * seconds)
		{
			elapsedMicrosecondsCarry.QuadPart = 0;
			return true;
		}
		else
		{
			elapsedMicrosecondsCarry.QuadPart += elapsedMicroseconds.QuadPart;
		}
	}

	return false;
}

uint8 Performance::getFrameMask() const noexcept
{
	return startTime.QuadPart & (long)1 << 7;
}