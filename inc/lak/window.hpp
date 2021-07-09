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

#include "lak/platform.hpp"

#include "lak/events.hpp"

#include "lak/bank_ptr.hpp"
#include "lak/image.hpp"
#include "lak/memmanip.hpp"
#include "lak/string.hpp"
#include "lak/surface.hpp"
#include "lak/vec.hpp"

#include "lak/profile.hpp"

namespace lak
{
  enum struct graphics_mode
  {
    None     = 0,
    Software = 1,
    OpenGL   = 2,
    Vulkan   = 3
  };

  struct software_settings
  {
  };

  struct opengl_settings
  {
    bool double_buffered = false;
    uint8_t depth_size   = 24;
    uint8_t colour_size  = 8;
    uint8_t stencil_size = 8;
    int major            = 3;
    int minor            = 2;
  };

  struct vulkan_settings
  {
  };

  struct software_context
  {
#if defined(LAK_USE_WINAPI)
    mutable lak::image<lak::colour::bgrx8888> platform_handle;
#elif defined(LAK_USE_XLIB)
#  error "NYI"
#elif defined(LAK_USE_XCB)
#  error "NYI"
#elif defined(LAK_USE_SDL)
    SDL_Surface *platform_handle  = NULL;
    SDL_Window *sdl_window        = NULL;
#else
#  error "No implementation specified"
#endif
    using platform_handle_t = decltype(platform_handle);
  };

  struct opengl_context
  {
#if defined(LAK_USE_WINAPI)
    HGLRC platform_handle = NULL;
#elif defined(LAK_USE_XLIB)
#  error "NYI"
#elif defined(LAK_USE_XCB)
#  error "NYI"
#elif defined(LAK_USE_SDL)
    SDL_GLContext platform_handle = NULL;
    SDL_Window *sdl_window        = NULL;
#else
#  error "No implementation specified"
#endif
  };

  struct vulkan_context
  {
#if defined(LAK_USE_WINAPI)
#elif defined(LAK_USE_XLIB)
#  error "NYI"
#elif defined(LAK_USE_XCB)
#  error "NYI"
#elif defined(LAK_USE_SDL)
    SDL_Window *sdl_window        = NULL;
#else
#  error "No implementation specified"
#endif
  };

  struct window_handle
  {
#if defined(LAK_USE_WINAPI)
    HWND _platform_handle = NULL;
    HDC _device_context   = NULL;
#elif defined(LAK_USE_XLIB)
    Window *_platform_handle = NULL;
#elif defined(LAK_USE_XCB)
    xcb_window_t _platform_handle = NULL;
#elif defined(LAK_USE_SDL)
    SDL_Window *_platform_handle  = NULL;
#else
#  error "No implementation specified"
#endif

    using platform_handle_t = decltype(_platform_handle);

    const lak::platform_instance *_instance = nullptr;

#ifdef LAK_USE_WINAPI
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

    std::variant<std::monostate,
                 lak::software_context,
                 lak::opengl_context,
                 lak::vulkan_context>
      _context;

    lak::add_wconst_t<platform_handle_t> platform_handle() const
    {
      return _platform_handle;
    }

    const lak::platform_instance *instance() const { return _instance; }

    lak::graphics_mode graphics_mode() const
    {
      switch (_context.index())
      {
        case 1: return lak::graphics_mode::Software;
        case 2: return lak::graphics_mode::OpenGL;
        case 3: return lak::graphics_mode::Vulkan;
        default: FATAL("Invalid graphics mode");
        case 0: return lak::graphics_mode::None;
      }
    }

    const lak::software_context &software_context() const
    {
      ASSERT_EQUAL(graphics_mode(), lak::graphics_mode::Software);
      return std::get<lak::software_context>(_context);
    }

    const lak::opengl_context &opengl_context() const
    {
      ASSERT_EQUAL(graphics_mode(), lak::graphics_mode::OpenGL);
      return std::get<lak::opengl_context>(_context);
    }

    const lak::vulkan_context &vulkan_context() const
    {
      ASSERT_EQUAL(graphics_mode(), lak::graphics_mode::Vulkan);
      return std::get<lak::vulkan_context>(_context);
    }
  };

  using window_handle_bank = lak::bank<lak::window_handle>;
  extern template struct lak::bank<lak::window_handle>;

  /* --- create/destroy window --- */

  lak::window_handle *create_window(const lak::platform_instance &i,
                                    const lak::software_settings &s);

  lak::window_handle *create_window(const lak::platform_instance &i,
                                    const lak::opengl_settings &s);

  lak::window_handle *create_window(const lak::platform_instance &i,
                                    const lak::vulkan_settings &s);

  bool destroy_window(const lak::platform_instance &i, lak::window_handle *w);

  /* --- window state --- */

  lak::wstring window_title(const lak::platform_instance &i,
                            const lak::window_handle *w);

  bool set_window_title(const lak::platform_instance &i,
                        lak::window_handle *w,
                        const lak::wstring &s);

  lak::vec2l_t window_size(const lak::platform_instance &i,
                           const lak::window_handle *w);

  lak::vec2l_t window_drawable_size(const lak::platform_instance &i,
                                    const lak::window_handle *w);

  bool set_window_size(const lak::platform_instance &i,
                       lak::window_handle *w,
                       lak::vec2l_t s);

  bool set_window_cursor_pos(const lak::platform_instance &i,
                             const lak::window_handle *w,
                             lak::vec2l_t p);

  // :TODO: bool set_window_drawable_size(const lak::platform_instance &i,
  //                                      lak::window_handle *w,
  //                                      lak::vec2l_t s);

  /* --- graphics control --- */

  // :TODO: This probably belongs in the platform header.
  bool set_opengl_swap_interval(const lak::opengl_context &c, int interval);

  bool swap_window(const lak::platform_instance &i, lak::window_handle *w);

  // Yield this thread until the target framerate is achieved.
  uint64_t yield_frame(const uint64_t last_counter,
                       const uint32_t target_framerate);

  /* --- window wrapper class --- */

  struct window
  {
  private:
    const lak::platform_instance &_platform_instance;
    lak::unique_bank_ptr<lak::window_handle> _handle;

  public:
    inline window(window &&w)
    : _platform_instance(w._platform_instance), _handle(lak::move(w._handle))
    {
      ASSERT(_handle);
    }

    inline window(const lak::platform_instance &i,
                  const lak::software_settings &s)
    : _platform_instance(i),
      _handle(lak::unique_bank_ptr<lak::window_handle>::from_raw_bank_ptr(
        lak::create_window(_platform_instance, s)))
    {
      ASSERT(_handle);
    }

    inline window(const lak::platform_instance &i,
                  const lak::opengl_settings &s)
    : _platform_instance(i),
      _handle(lak::unique_bank_ptr<lak::window_handle>::from_raw_bank_ptr(
        lak::create_window(_platform_instance, s)))
    {
      ASSERT(_handle);
    }

    inline window(const lak::platform_instance &i,
                  const lak::vulkan_settings &s)
    : _platform_instance(i),
      _handle(lak::unique_bank_ptr<lak::window_handle>::from_raw_bank_ptr(
        lak::create_window(_platform_instance, s)))
    {
      ASSERT(_handle);
    }

    inline ~window()
    {
      if (handle()) lak::destroy_window(_platform_instance, handle());
    }

    inline const auto &platform_instance() const { return _platform_instance; }

    inline auto platform_handle() { return _handle->_platform_handle; }

    inline auto platform_handle() const { return _handle->_platform_handle; }

    inline lak::window_handle *handle() { return _handle.get(); }

    inline const lak::window_handle *handle() const { return _handle.get(); }

    inline lak::graphics_mode graphics() const
    {
      return _handle->graphics_mode();
    }

    inline lak::wstring title() const
    {
      return lak::window_title(_platform_instance, handle());
    }

    inline window &set_title(const lak::wstring &title)
    {
      ASSERT(lak::set_window_title(_platform_instance, handle(), title));
      return *this;
    }

    inline lak::vec2l_t size() const
    {
      return lak::window_size(_platform_instance, handle());
    }

    inline lak::vec2l_t drawable_size() const
    {
      return lak::window_drawable_size(_platform_instance, handle());
    }

    inline window &set_size(lak::vec2l_t size)
    {
      ASSERT(lak::set_window_size(_platform_instance, handle(), size));
      return *this;
    }

    inline const window &set_cursor_pos(lak::vec2l_t pos) const
    {
      ASSERT(lak::set_window_cursor_pos(_platform_instance, handle(), pos));
      return *this;
    }

    inline bool swap()
    {
      return lak::swap_window(_platform_instance, handle());
    }
  };
}

#include <ostream>

[[maybe_unused]] static std::ostream &operator<<(std::ostream &strm,
                                                 lak::graphics_mode mode)
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
