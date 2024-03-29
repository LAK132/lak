#ifndef LAK_OPENGL_GL3W_HPP
#define LAK_OPENGL_GL3W_HPP

#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#	define NOMINMAX
#endif

#ifndef UNICODE
#	define UNICODE
#endif

#undef ERROR

#include <GL/gl3w.h>

#ifdef LAK_DEBUG_HPP
// restore ERROR
#	include "lak/debug.hpp"
#endif

#endif
