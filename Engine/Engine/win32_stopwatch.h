#pragma once
#include "hwindows.h"  // first
#include "types.h"
#include "time.h"

class Stopwatch
{
private:
	LARGE_INTEGER startTime = {};
	LARGE_INTEGER endTime = {};
public:
	void start();
	const double stop(double multipler = 1.0);
	const double getStart() const noexcept;
	const double getEnd() const noexcept;
};

//class Stopwatch
//{
//private:
//	static LARGE_INTEGER start;
//	static LARGE_INTEGER end;
//public:
//	Stopwatch() noexcept {};
//	~Stopwatch();
//	Stopwatch(const Stopwatch&) = delete;
//	Stopwatch& operator=(const Stopwatch&) = delete;
//	static Stopwatch stopwatch;
//	static Stopwatch& GetStopwatch()
//	{
//		return stopwatch;
//	}
//	static void start()
//	{
//		QueryPerformanceCounter(&start);
//		end = {};
//	}
//	static const double stop()
//	{
//		double time = -1;
//		QueryPerformanceCounter(&end);
//		double time = end.QuadPart - start.QuadPart;
//		if (time <= 0)
//		{
//			return -1;
//		}
//		else
//		{
//			return time;
//		}
//	}
//
//	static const double toSeconds(double microseconds)
//	{
//		return microseconds * 1000000.0;
//	}
//
//	static const double toMiliseconds(double microseconds)
//	{
//		return microseconds * 1000.0;
//	}
//
//	static const double getStart()
//	{
//		return start.QuadPart;
//	}
//
//	static const double getEnd()
//	{
//		return end.QuadPart;
//	}
//};