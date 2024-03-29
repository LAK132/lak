#ifndef LAK_PROFILE_HPP
#define LAK_PROFILE_HPP

#include "lak/macro_utils.hpp"
#include "lak/stdint.hpp"

namespace lak
{
	// ticks per second
	uint64_t performance_frequency();

	// ticks
	uint64_t performance_counter();

	double ticks_to_seconds(uint64_t ticks);

#define SCOPED_TIMER(FUNC)                                                    \
	const uint64_t UNIQUIFY(DEBUG_TIMER_) = lak::performance_counter();         \
	DEFER(FUNC((lak::performance_counter() - UNIQUIFY(DEBUG_TIMER_))));

#define DEBUG_SCOPED_TIMER(...)                                               \
	const uint64_t UNIQUIFY(DEBUG_TIMER_) = lak::performance_counter();         \
	DEFER(DEBUG(__VA_ARGS__ __VA_OPT__(, )(lak::performance_counter() -         \
	                                       UNIQUIFY(DEBUG_TIMER_))));

	struct timer
	{
		uint64_t _begin;
		uint64_t _end;

		inline void start() { _begin = lak::performance_counter(); }

		inline void stop() { _end = lak::performance_counter(); }

		uint64_t ticks() const { return _end - _begin; }

		double seconds() const { return lak::ticks_to_seconds(ticks()); }
	};
}

#endif
