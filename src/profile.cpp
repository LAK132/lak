#include "lak/profile.hpp"
#include "lak/os.hpp"

#if defined(LAK_OS_WINDOWS)
#	include "lak/windows.hpp"
#elif defined(LAK_OS_APPLE)
#	include <mach/mach_time.h>
#elif defined(LAK_OS_LINUX)
#	include <unistd.h>
#	if _POSIX_TIMERS <= 0
#		error "Timers not available"
#	endif
#	ifndef _POSIX_MONOTONIC_CLOCK
#		error "Clock is not monotonic"
#	endif
#	include <time.h>
#else
#	error "OS not supported"
#endif

#include "lak/debug.hpp"

uint64_t lak::performance_frequency()
{
#if defined(LAK_OS_WINDOWS)
	LARGE_INTEGER result;
	ASSERT(::QueryPerformanceFrequency(&result));
	return result.QuadPart;
#elif defined(LAK_OS_APPLE)
	mach_timebase_info_data_t timebase_info;
	ASSERT(mach_timebase_info(&timebase_info) == KERN_SUCCESS);
	// ticks * (.numer / .denom) = nanoseconds
	// ticks / (.denom / .numer) = nanoseconds
	// ticks / (1'000'000'000 * (.denom / .numer)) = seconds
	return uint64_t((1'000'000'000U * uintmax_t(timebase_info.denom)) /
	                timebase_info.numer);
#elif defined(LAK_OS_LINUX)
	return 1'000'000'000U;
#else
#	error "OS not supported"
#endif
}

uint64_t lak::performance_counter()
{
#if defined(LAK_OS_WINDOWS)
	LARGE_INTEGER result;
	ASSERT(::QueryPerformanceCounter(&result));
	return result.QuadPart;
#elif defined(LAK_OS_APPLE)
	return mach_absolute_time();
#elif defined(LAK_OS_LINUX)
	struct timespec time;
	ASSERT(clock_gettime(CLOCK_MONOTONIC, &time) == 0);
	return uint64_t((uintmax_t(time.tv_sec) * 1'000'000'000U) +
	                uintmax_t(time.tv_nsec));
#else
#	error "OS not supported"
#endif
}

double lak::ticks_to_seconds(uint64_t ticks)
{
	return double(ticks) / double(lak::performance_frequency());
}
