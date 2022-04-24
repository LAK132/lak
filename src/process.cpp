#include "lak/os.hpp"

#ifndef LAK_DONT_AUTO_COMPILE_PLATFORM_SPECIFICS
#	if defined(LAK_OS_WINDOWS)
#		include "win32/process.cpp"
#	endif

#	if defined(LAK_OS_LINUX)
#		include "linux/process.cpp"
#	endif

#	if defined(LAK_OS_APPLE)
#		include "macos/process.cpp"
#	endif
#endif
