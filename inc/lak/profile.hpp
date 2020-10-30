#ifndef LAK_PROFILE_HPP
#define LAK_PROFILE_HPP

#include "lak/stdint.hpp"

namespace lak
{
  // ticks per second
  uint64_t performance_frequency();

  // ticks
  uint64_t performance_counter();
}

#endif
