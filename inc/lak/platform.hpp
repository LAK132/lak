#ifndef LAK_PLATFORM_HPP
#define LAK_PLATFORM_HPP

#include "lak/os.hpp"

#ifdef LAK_USE_WINAPI
#  ifndef WIN32_MEAN_AND_LEAN
#    define WIN32_MEAN_AND_LEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <Windows.h>
#endif

#ifdef LAK_USE_XLIB
#  include <X11/Xlib.h>
#endif

#ifdef LAK_USE_XCB
#  include <xcb/xcb.h>
#endif

#ifdef LAK_USE_SDL
#  include <SDL/SDL.h>
#endif

namespace lak
{
  struct platform_instance
  {
#ifdef LAK_USE_WINAPI
    HINSTANCE handle;
    WNDCLASSW window_class;
    ATOM window_class_atom;
#endif

#ifdef LAK_USE_XLIB
    Display *handle;
    Atom wm_delete_window;
#endif

#ifdef LAK_USE_XCB
    xcb_connection_t *handle;
#endif

#ifdef LAK_USE_SDL
#endif
  };

  bool platform_init(lak::platform_instance *i);
  bool platform_quit(lak::platform_instance *i);
}

#endif