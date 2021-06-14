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
}

#endif
