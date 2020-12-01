#ifndef LAK_PLATFORM_HPP
#define LAK_PLATFORM_HPP

#if defined(LAK_USE_WINAPI)
#  ifndef WIN32_MEAN_AND_LEAN
#    define WIN32_MEAN_AND_LEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <Windows.h>
#elif defined(LAK_USE_XLIB)
#  include <X11/Xlib.h>
#elif defined(LAK_USE_XCB)
#  include <xcb/xcb.h>
#elif defined(LAK_USE_SDL)
#  include <SDL/SDL.h>
#else
#  error "No implementation specified"
#endif

#include "lak/buffer.hpp"
#include "lak/compiler.hpp"
#include "lak/os.hpp"
#include "lak/span.hpp"
#include "lak/string.hpp"
#include "lak/vec.hpp"

#ifdef min
#  undef min
#endif
#ifdef max
#  undef max
#endif

namespace lak
{
  struct platform_instance
  {
#if defined(LAK_USE_WINAPI)
    HINSTANCE handle;
    WNDCLASSW window_class;
    ATOM window_class_atom;
    mutable bool opengl_initialised = false;
    mutable lak::buffer<MSG, 0x100> platform_events;
#elif defined(LAK_USE_XLIB)
    Display *handle;
    Atom wm_delete_window;
#elif defined(LAK_USE_XCB)
    xcb_connection_t *handle;
#elif defined(LAK_USE_SDL)
#else
#  error "No implementation specified"
#endif
  };

  struct cursor
  {
#if defined(LAK_USE_WINAPI)
    using platform_handle_t = HCURSOR;
#elif defined(LAK_USE_XLIB)
#  error "NYI"
#elif defined(LAK_USE_XCB)
#  error "NYI"
#elif defined(LAK_USE_SDL)
    using platform_handle_t = SDL_Cursor *;
#else
#  error "No implementation specified"
#endif

    platform_handle_t platform_handle;
  };

  bool platform_init(lak::platform_instance *i);
  bool platform_quit(lak::platform_instance *i);

  bool get_clipboard(const lak::platform_instance &i, lak::u8string *s);
  bool set_clipboard(const lak::platform_instance &i, const lak::u8string &s);
  bool set_clipboard(const lak::platform_instance &i,
                     lak::span<const char8_t> s);

  bool cursor_visible(const lak::platform_instance &i);
  void show_cursor(const lak::platform_instance &i);
  void hide_cursor(const lak::platform_instance &i);
  void set_cursor_pos(const lak::platform_instance &i, lak::vec2l_t p);
  lak::vec2l_t get_cursor_pos(const lak::platform_instance &i);
  void set_cursor(const lak::platform_instance &i, const lak::cursor &c);

  force_inline lak::platform_instance platform_init()
  {
    lak::platform_instance instance;
    ASSERT(lak::platform_init(&instance));
    return instance;
  }
}

#endif