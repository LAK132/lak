#include "lak/system/profile.hpp"
#include "lak/system/os.hpp"

#include <unistd.h>
#if _POSIX_TIMERS <= 0
#	error "Timers not available"
#endif
#ifndef _POSIX_MONOTONIC_CLOCK
#	error "Clock is not monotonic"
#endif
#include <time.h>

#include "lak/debug.hpp"

uint64_t lak::performance_frequency() { return 1'000'000'000U; }

uint64_t lak::performance_counter()
{
	struct timespec time;
	ASSERT(clock_gettime(CLOCK_MONOTONIC, &time) == 0);
	return uint64_t((uintmax_t(time.tv_sec) * 1'000'000'000U) +
	                uintmax_t(time.tv_nsec));
}
