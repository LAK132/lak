#include "lak/window.hpp"

#include <thread>

namespace lak
{
  uint64_t yield_frame(const uint64_t last_counter,
                       const uint32_t target_framerate)
  {
    const uint64_t target_count =
      (performance_frequency() / target_framerate) + last_counter;

    for (uint64_t count = performance_counter();
         target_count > last_counter
           ? (count < target_count)
           : (count < target_count || count > last_counter);
         count = performance_counter())
      std::this_thread::yield();

    return performance_counter();
  }
}
