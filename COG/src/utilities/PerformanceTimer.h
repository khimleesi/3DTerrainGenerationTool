#pragma once

#include <chrono>
#include <ctime>

#include "Log.h"

//Credits : https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c

template<class Resolution = std::chrono::milliseconds>
class PerformanceTimer {

public:
	using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;

private:
	const Clock::time_point mStart = Clock::now();

public:
	PerformanceTimer() = default;
	~PerformanceTimer() {
		const auto end = Clock::now();
		COG_LOG("[TIMER] Destructor Elapsed: ", std::chrono::duration_cast<Resolution>(end - mStart).count(), LOG_WARN);
	}

	inline void Stop() {
		const auto end = Clock::now();
		COG_LOG("[TIMER] Stop Elapsed: ", std::chrono::duration_cast<Resolution>(end - mStart).count(), LOG_WARN);
	}
}; 