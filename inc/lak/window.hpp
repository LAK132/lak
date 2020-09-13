/*
Typical usage for an OpenGL program:

int main()
{
  lak::core_init();

  lak::window window(...);

  window.init_opengl(...);

  uint32_t framerate = 60;

  auto last_counter = lak::performance_counter();

  // main loop
  while(...)
  {
    // event handlers

    // update code

    // draw code

    window.swap();

    last_counter = lak::yield_frame(last_counter, framerate);
  }

  window.close();

  lak::core_quit();
}
*/

#ifndef LAK_WINDOW_HPP
#define LAK_WINDOW_HPP

#include "lak/events.hpp"
#include "lak/platform.hpp"

#include "lak/string.hpp"
#include "lak/vec.hpp"

namespace lak
{
  enum struct graphics_mode
  {
    None     = 0,
    Software = 1,
    OpenGL   = 2,
    Vulkan   = 3
  };

  struct window_handle
  {
#ifdef LAK_USE_WINAPI
    HWND platform_handle = NULL;
#endif

#ifdef LAK_USE_XLIB
    Window *platform_handle = NULL;
#endif

#ifdef LAK_USE_XCB
    xcb_window_t platform_handle = {};
#endif

#ifdef LAK_USE_SDL
    SDL_Window *platform_handle  = NULL;
    SDL_GLContext opengl_context = NULL;
#endif
  };

  struct window;

  /* --- create/destroy window --- */

  bool create_software_window(const lak::platform_instance &i,
                              lak::window_handle *wh,
                              lak::window *w = nullptr);
  bool destroy_software_window(const lak::platform_instance &i,
                               lak::window_handle *wh);
  bool swap_software_window(const lak::platform_instance &i,
                            const lak::window_handle &w);

  struct opengl_settings
  {
    bool double_buffered = false;
    uint8_t depth_size   = 24;
    uint8_t colour_size  = 8;
    uint8_t stencil_size = 8;
    int major            = 3;
    int minor            = 2;
  };

  bool create_opengl_window(const lak::platform_instance &i,
                            const lak::opengl_settings &s,
                            lak::window_handle *wh,
                            lak::window *w = nullptr);
  bool destroy_opengl_window(const lak::platform_instance &i,
                             lak::window_handle *wh);
  bool swap_opengl_window(const lak::platform_instance &i,
                          const lak::window_handle &w);

  struct vulkan_settings
  {
  };

  bool create_vulkan_window(const lak::platform_instance &i,
                            const lak::vulkan_settings &s,
                            lak::window_handle *wh,
                            lak::window *w = nullptr);
  bool destroy_vulkan_window(const lak::platform_instance &i,
                             lak::window_handle *wh);
  bool swap_vulkan_window(const lak::platform_instance &i,
                          const lak::window_handle &w);

  /* --- window state --- */

  lak::wstring window_title(const lak::platform_instance &i,
                            const lak::window_handle &w);

  bool set_window_title(const lak::platform_instance &i,
                        const lak::window_handle &w,
                        const lak::wstring &s);

  lak::vec2l_t window_size(const lak::platform_instance &i,
                           const lak::window_handle &w);

  lak::vec2l_t window_drawable_size(const lak::platform_instance &i,
                                    const lak::window_handle &w);

  bool set_window_size(const lak::platform_instance &i,
                       const lak::window_handle &w,
                       lak::vec2l_t s);

  /* --- window events --- */

  bool next_window_event(const lak::platform_instance &i,
                         const lak::window_handle &w,
                         lak::event *e);
  bool peek_window_event(const lak::platform_instance &i,
                         const lak::window_handle &w,
                         lak::event *e);

  bool next_window_event(const lak::platform_instance &i,
                         const lak::window &w,
                         lak::event *e);
  bool peek_window_event(const lak::platform_instance &i,
                         const lak::window &w,
                         lak::event *e);

  /* --- performance --- */

  uint64_t performance_frequency();

  uint64_t performance_counter();

  // Yield this thread until the target framerate is achieved.
  uint64_t yield_frame(const uint64_t last_counter,
                       const uint32_t target_framerate);

  /* --- window wrapper class --- */

  struct window
  {
  private:
    const lak::platform_instance &_platform_instance;
    lak::window_handle _handle;
    lak::graphics_mode _graphics_mode;

  public:
    // :TODO: should this be moved into window_handle?
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

    inline window(const lak::platform_instance &i)
    : _platform_instance(i),
      _handle(),
      _graphics_mode(lak::graphics_mode::Software)
    {
      if (!lak::create_software_window(_platform_instance, &_handle, this))
        _graphics_mode = lak::graphics_mode::None;
    }

    inline window(const lak::platform_instance &i,
                  const lak::opengl_settings &s)
    : _platform_instance(i),
      _handle(),
      _graphics_mode(lak::graphics_mode::OpenGL)
    {
      if (!lak::create_opengl_window(_platform_instance, s, &_handle, this))
        _graphics_mode = lak::graphics_mode::None;
    }

    inline window(const lak::platform_instance &i,
                  const lak::vulkan_settings &s)
    : _platform_instance(i),
      _handle(),
      _graphics_mode(lak::graphics_mode::Vulkan)
    {
      if (!lak::create_vulkan_window(_platform_instance, s, &_handle, this))
        _graphics_mode = lak::graphics_mode::None;
    }

    inline ~window()
    {
      switch (_graphics_mode)
      {
        case lak::graphics_mode::Software:
          lak::destroy_software_window(_platform_instance, &_handle);
          break;
        case lak::graphics_mode::OpenGL:
          lak::destroy_opengl_window(_platform_instance, &_handle);
          break;
        case lak::graphics_mode::Vulkan:
          lak::destroy_vulkan_window(_platform_instance, &_handle);
          break;
      }
    }

    inline lak::graphics_mode graphics() const { return _graphics_mode; }

    inline const auto &platform_handle() const
    {
      return _handle.platform_handle;
    }

    inline lak::wstring title() const
    {
      return lak::window_title(_platform_instance, _handle);
    }

    inline window &set_title(const lak::wstring &title)
    {
      lak::set_window_title(_platform_instance, _handle, title);
      return *this;
    }

    inline lak::vec2l_t size() const
    {
      return lak::window_size(_platform_instance, _handle);
    }

    inline lak::vec2l_t drawable_size() const
    {
      return lak::window_drawable_size(_platform_instance, _handle);
    }

    inline window &set_size(lak::vec2l_t size)
    {
      lak::set_window_size(_platform_instance, _handle, size);
      return *this;
    }

    // Update back buffer.
    inline bool swap()
    {
      switch (_graphics_mode)
      {
        case lak::graphics_mode::Software:
          return lak::swap_software_window(_platform_instance, _handle);

        case lak::graphics_mode::OpenGL:
          return lak::swap_opengl_window(_platform_instance, _handle);

        case lak::graphics_mode::Vulkan:
          return lak::swap_vulkan_window(_platform_instance, _handle);
      }
    }
  };
}

#include <ostream>

static std::ostream &operator<<(std::ostream &strm, lak::graphics_mode mode)
{
  switch (mode)
  {
    case lak::graphics_mode::OpenGL: strm << "OpenGL"; break;
    case lak::graphics_mode::Software: strm << "Software"; break;
    case lak::graphics_mode::Vulkan: strm << "Vulkan"; break;
    default: strm << "None"; break;
  }
  return strm;
}

#endif
