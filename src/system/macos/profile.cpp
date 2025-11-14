#include "lak/system/profile.hpp"
#include "lak/system/os.hpp"

#include <mach/mach_time.h>

#include "lak/debug.hpp"

uint64_t lak::performance_frequency()
{
	mach_timebase_info_data_t timebase_info;
	ASSERT(mach_timebase_info(&timebase_info) == KERN_SUCCESS);
	// ticks * (.numer / .denom) = nanoseconds
	// ticks / (.denom / .numer) = nanoseconds
	// ticks / (1'000'000'000 * (.denom / .numer)) = seconds
	return uint64_t((1'000'000'000U * uintmax_t(timebase_info.denom)) /
	                timebase_info.numer);
}

uint64_t lak::performance_counter() { return mach_absolute_time(); }
