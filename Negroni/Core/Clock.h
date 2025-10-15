#pragma once

#include <chrono>

namespace Core
{
	class Clock
	{
	public:
		typedef uint64_t millisec;

		static millisec now()
		{
			TimePoint tp = HRClock::now();
			return tp.time_since_epoch().count();
		}

	private:
		typedef std::chrono::high_resolution_clock HRClock;
		typedef std::chrono::duration<double, std::milli> Duration;
		typedef std::chrono::time_point<HRClock, Duration> TimePoint;
	};
}
