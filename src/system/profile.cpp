#include "lak/system/profile.hpp"

double lak::ticks_to_seconds(uint64_t ticks)
{
	return double(ticks) / double(lak::performance_frequency());
}
