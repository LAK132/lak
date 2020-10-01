#include "lak/window.hpp"

#include <thread>

uint64_t lak::yield_frame(const uint64_t last_counter,
                          const uint32_t target_framerate)
{
  const uint64_t target_count =
    (lak::performance_frequency() / target_framerate) + last_counter;

  for (uint64_t count = lak::performance_counter();
       target_count > last_counter
         ? (count < target_count)
         : (count < target_count || count > last_counter);
       count = lak::performance_counter())
    std::this_thread::yield();

  return lak::performance_counter();
}

#ifdef LAK_USE_WINAPI
#  include "lak/src/win32/window.cpp"
#endif

#ifdef LAK_USE_XLIB
#  include "lak/src/xlib/window.cpp"
#endif

#ifdef LAK_USE_XCB
#  include "lak/src/xcb/window.cpp"
#endif

#ifdef LAK_USE_SDL
#  include "lak/src/sdl/window.cpp"
#endif
