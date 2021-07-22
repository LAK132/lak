#ifndef LAK_WINDOWS_HPP
#define LAK_WINDOWS_HPP

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#  define NOMINMAX
#endif

#ifndef UNICODE
#  define UNICODE
#endif

#undef ERROR

#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>

#ifdef LAK_DEBUG_HPP
// restore ERROR
#  include "lak/debug.hpp"
#endif

#endif
