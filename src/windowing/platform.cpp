#include "lak/platform.hpp"

#ifndef LAK_DONT_AUTO_COMPILE_PLATFORM_SPECIFICS
#  ifdef LAK_USE_WINAPI
#    include "win32/platform.cpp"
#  endif

#  ifdef LAK_USE_XLIB
#    include "xlib/platform.cpp"
#  endif

#  ifdef LAK_USE_XCB
#    include "xcb/platform.cpp"
#  endif

#  ifdef LAK_USE_SDL
#    include "sdl/platform.cpp"
#  endif
#endif
