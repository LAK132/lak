#ifndef LAK_EVENTS_HPP
#define LAK_EVENTS_HPP

#include "lak/platform.hpp"

#include "lak/buffer.hpp"
#include "lak/debug.hpp"
#include "lak/vec.hpp"

namespace lak
{
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

  static inline lak::mouse_button operator|(lak::mouse_button m1,
                                            lak::mouse_button m2)
  {
    return static_cast<lak::mouse_button>(static_cast<unsigned int>(m1) |
                                          static_cast<unsigned int>(m2));
  }

  struct mouse_event
  {
    lak::vec2l_t position;
    lak::vec2f_t wheel;
    lak::mouse_button button;
    // modifier key mask.
  };

  struct event
  {
    lak::event_type type;

    union
    {
      lak::window_event window;
      lak::key_event key;
      lak::mouse_event mouse;
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

#ifdef LAK_USE_SDL
    SDL_Event platform_event;
#endif
  };

  bool next_thread_event(const lak::platform_instance &i, lak::event *e);
  bool peek_thread_event(const lak::platform_instance &i, lak::event *e);
}

#endif