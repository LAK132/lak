#ifndef LAK_EVENTS_HPP
#define LAK_EVENTS_HPP

#include "lak/platform.hpp"

#include "lak/buffer.hpp"
#include "lak/debug.hpp"
#include "lak/string.hpp"
#include "lak/vec.hpp"

#include <variant>

namespace lak
{
  struct window;
  struct window_handle;

  enum struct event_type
  {
    platform_event = 0,

    quit_program,

    window_closed,
    window_changed,
    window_exposed,

    dropfile,

    key_down,
    key_up,

    character,

    button_down,
    button_up,

    motion,

    wheel
  };

  struct window_event
  {
    lak::vec2l_t position;
    lak::vec2l_t size;
  };

  struct dropfile_event
  {
    lak::vec2l_t position;
    std::wstring path;
  };

  enum struct mod_key : uint32_t
  {
    none  = 0,
    shift = 1 << 0,
    ctrl  = 1 << 1,
    alt   = 1 << 2,
    super = 1 << 3
  };

  static inline lak::mod_key operator|(lak::mod_key m1, lak::mod_key m2)
  {
    return static_cast<lak::mod_key>(static_cast<uint32_t>(m1) |
                                     static_cast<uint32_t>(m2));
  }

  static inline lak::mod_key operator&(lak::mod_key m1, lak::mod_key m2)
  {
    return static_cast<lak::mod_key>(static_cast<uint32_t>(m1) &
                                     static_cast<uint32_t>(m2));
  }

  struct key_event
  {
    uint32_t scancode;
    lak::mod_key mod;
  };

  struct character_event
  {
    char32_t code;
  };

  enum struct mouse_button : uint32_t
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
    return static_cast<lak::mouse_button>(static_cast<uint32_t>(m1) |
                                          static_cast<uint32_t>(m2));
  }

  struct button_event
  {
    lak::mouse_button button;
  };

  struct motion_event
  {
    lak::vec2l_t position;
  };

  struct wheel_event
  {
    lak::vec2f_t wheel;
  };

  struct event
  {
    using state_t = std::variant<std::monostate,
                                 lak::window_event,
                                 lak::dropfile_event,
                                 lak::key_event,
                                 lak::character_event,
                                 lak::button_event,
                                 lak::motion_event,
                                 lak::wheel_event>;

#ifdef LAK_USE_WINAPI
    using platform_event_t = MSG;
#endif

#ifdef LAK_USE_XLIB
    using platform_event_t = XEvent;
#endif

#ifdef LAK_USE_XCB
    using platform_event_t = xcb_generic_event_t;
#endif

#ifdef LAK_USE_SDL
    using platform_event_t = SDL_Event;
#endif

    lak::event_type type             = lak::event_type::platform_event;
    const lak::window_handle *handle = nullptr;
    platform_event_t platform_event;
    state_t _state;

    const lak::window_event &window() const
    {
      return std::get<lak::window_event>(_state);
    }

    const lak::dropfile_event &dropfile() const
    {
      return std::get<lak::dropfile_event>(_state);
    }

    const lak::key_event &key() const
    {
      return std::get<lak::key_event>(_state);
    }

    const lak::character_event &character() const
    {
      return std::get<lak::character_event>(_state);
    }

    const lak::motion_event &motion() const
    {
      return std::get<lak::motion_event>(_state);
    }

    const lak::button_event &button() const
    {
      return std::get<lak::button_event>(_state);
    }

    const lak::wheel_event &wheel() const
    {
      return std::get<lak::wheel_event>(_state);
    }

    inline event(lak::event_type t,
                 const platform_event_t &e,
                 const state_t &s)
    : type(t), handle(nullptr), platform_event(e), _state(s)
    {
    }

    inline event(lak::event_type t, const platform_event_t &e)
    : type(t), handle(nullptr), platform_event(e), _state()
    {
    }

    inline event(lak::event_type t,
                 const lak::window_handle *w,
                 const platform_event_t &e,
                 const state_t &s)
    : type(t), handle(w), platform_event(e), _state(s)
    {
    }

    inline event(lak::event_type t,
                 const lak::window_handle *w,
                 const platform_event_t &e)
    : type(t), handle(w), platform_event(e), _state()
    {
    }

    event()              = default;
    event(const event &) = default;
    ~event()             = default;
    event &operator=(const event &) = default;
  };

  static_assert(std::is_default_constructible_v<lak::event>,
                "lak::event must be default constructible");

  bool next_thread_event(const lak::platform_instance &i, lak::event *e);
  bool peek_thread_event(const lak::platform_instance &i, lak::event *e);
}

#endif