#include "lak/os.hpp"

#ifndef LAK_DONT_AUTO_COMPILE_PLATFORM_SPECIFICS
#	if defined(LAK_OS_WINDOWS)
#		include "win32/wrapper.cpp"
#	endif
#endif
