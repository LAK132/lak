#include "lak/platform.hpp"

#ifndef LAK_DONT_AUTO_COMPILE_PLATFORM_SPECIFICS
#  ifdef LAK_USE_WINAPI
#    include "src/win32/platform.cpp"
#  endif

#  ifdef LAK_USE_XLIB
#    include "src/xlib/platform.cpp"
#  endif

#  ifdef LAK_USE_XCB
#    include "src/xcb/platform.cpp"
#  endif

#  ifdef LAK_USE_SDL
#    include "src/sdl/platform.cpp"
#  endif
#endif
