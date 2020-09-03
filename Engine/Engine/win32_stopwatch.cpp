#include "win32_stopwatch.h"
#include <assert.h>

void Stopwatch::start()
{
	QueryPerformanceCounter(&startTime);
	endTime = {};
}
const double Stopwatch::stop(double multipler)
{
	assert(multipler > 0);
	QueryPerformanceCounter(&endTime);
	double time = (endTime.QuadPart - startTime.QuadPart) / multipler;
	if (time <= 0)
	{
		return -1;
	}
	else
	{
		return time;
	}
}

const double Stopwatch::getStart() const noexcept
{
	return startTime.QuadPart;
}

const double Stopwatch::getEnd() const noexcept
{
	return endTime.QuadPart;
}