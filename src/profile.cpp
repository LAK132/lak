#include "lak/profile.hpp"
#include "lak/os.hpp"

#if defined(LAK_OS_WINDOWS)
#	include "lak/windows.hpp"
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
#elif defined(LAK_OS_LINUX)
	return 1'000'000'000;
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
#elif defined(LAK_OS_LINUX)
	struct timespec time;
	ASSERT(clock_gettime(CLOCK_MONOTONIC, &time) == 0);
	return (time.tv_sec * 1'000'000'000) + time.tv_nsec;
#else
#	error "OS not supported"
#endif
}

double lak::ticks_to_seconds(uint64_t ticks)
{
	return double(ticks) / double(lak::performance_frequency());
}
