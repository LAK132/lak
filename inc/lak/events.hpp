#ifndef LAK_EVENTS_HPP
#define LAK_EVENTS_HPP

#include "lak/os.hpp"

#ifdef LAK_USE_WINAPI
#  include <Windows.h>
#endif

#ifdef LAK_USE_XLIB
#  include <X11/Xlib.h>
#endif

#ifdef LAK_USE_XCB
#  include <xcb/xcb.h>
#endif

#include "lak/buffer.hpp"
#include "lak/debug.hpp"
#include "lak/vec.hpp"

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
  };

  bool platform_connect(platform_instance *h);
  bool platform_disconnect(platform_instance *h);

  struct window_handle
  {
#ifdef LAK_USE_WINAPI
    HWND platform_handle;
#endif

#ifdef LAK_USE_XLIB
    Window *platform_handle;
#endif

#ifdef LAK_USE_XLIB
    xcb_window_t platform_handle;
#endif
  };

  struct window;

  bool create_window(const platform_instance &i,
                     window_handle *wh,
                     window *w = nullptr);
  bool destroy_window(const platform_instance &i, window_handle *wh);

  struct window
  {
  private:
    const platform_instance &_platform_instance;
    window_handle _handle;

  public:
#ifdef LAK_USE_WINAPI
    mutable lak::buffer<MSG, 0x100> _platform_events;
    mutable bool _moving   = false;
    mutable bool _resizing = false;
    mutable POINT _cursor_start;
    mutable RECT _window_start;
    enum
    {
      top    = 1 << 1,
      bottom = 1 << 2,
      left   = 1 << 3,
      right  = 1 << 4
    };
    mutable unsigned char _side = 0;
#endif

    inline window(const platform_instance &i)
    : _platform_instance(i), _handle()
    {
      create_window(_platform_instance, &_handle, this);
    }
    inline ~window() { destroy_window(_platform_instance, &_handle); }

    inline const auto &platform_handle() const
    {
      return _handle.platform_handle;
    }
  };

  enum struct event_type
  {
    platform_event = 0,

    quit_program,

    window_close,     // user request
    window_destroyed, // was destroyed
    window_created,   // was created
    window_changed,
    window_exposed,

    key_down,
    key_up,

    mouse_button_down,
    mouse_button_up,
    mouse_motion,
    mouse_wheel
  };

  struct window_event
  {
    lak::vec2l_t position;
    lak::vec2l_t size;
  };

  struct key_event
  {
  };

  enum struct mouse_button : unsigned int
  {
    none   = 0,
    left   = 1 << 0,
    middle = 1 << 1,
    right  = 1 << 2,
    x1     = 1 << 3,
    x2     = 1 << 4
  };

  static inline mouse_button operator|(mouse_button m1, mouse_button m2)
  {
    return static_cast<mouse_button>(static_cast<unsigned int>(m1) |
                                     static_cast<unsigned int>(m2));
  }

  struct mouse_event
  {
    lak::vec2l_t position;
    lak::vec2f_t wheel;
    mouse_button button;
    // modifier key mask.
  };

  struct event
  {
    event_type type;

    union
    {
      window_event window;
      key_event key;
      mouse_event mouse;
    };

#ifdef LAK_USE_WINAPI
    MSG platform_event;
#endif

#ifdef LAK_USE_XLIB
    XEvent platform_event;
#endif

#ifdef LAK_USE_XCB
    xcb_generic_event_t platform_event;
#endif
  };

  bool next_thread_event(const platform_instance &i, event *e);
  bool peek_thread_event(const platform_instance &i, event *e);

  bool next_window_event(const platform_instance &i,
                         const window &w,
                         event *e);
  bool peek_window_event(const platform_instance &i,
                         const window &w,
                         event *e);
}

#endif