#include "lak/events.hpp"

#ifndef LAK_DONT_AUTO_COMPILE_PLATFORM_SPECIFICS
#	ifdef LAK_USE_WINAPI
#		include "win32/events.cpp"
#	endif

#	ifdef LAK_USE_XLIB
#		include "xlib/events.cpp"
#	endif

#	ifdef LAK_USE_XCB
#		include "xcb/events.cpp"
#	endif

#	ifdef LAK_USE_SDL
#		include "sdl/events.cpp"
#	endif
#endif
