#include "lak/events.hpp"

#ifndef LAK_DONT_AUTO_COMPILE_PLATFORM_SPECIFICS
#  ifdef LAK_USE_WINAPI
#    include "src/win32/events.cpp"
#  endif

#  ifdef LAK_USE_XLIB
#    include "src/xlib/events.cpp"
#  endif

#  ifdef LAK_USE_XCB
#    include "src/xcb/events.cpp"
#  endif

#  ifdef LAK_USE_SDL
#    include "src/sdl/events.cpp"
#  endif
#endif