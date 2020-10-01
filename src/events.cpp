#include "lak/events.hpp"

#ifdef LAK_USE_WINAPI
#  include "lak/src/win32/events.cpp"
#endif

#ifdef LAK_USE_XLIB
#  include "lak/src/xlib/events.cpp"
#endif

#ifdef LAK_USE_XCB
#  include "lak/src/xcb/events.cpp"
#endif

#ifdef LAK_USE_SDL
#  include "lak/src/sdl/events.cpp"
#endif
