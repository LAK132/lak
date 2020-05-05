#include <SDL.h>

#include <GL/gl3w.h>

#include "lak/debug.hpp"
#include "lak/window.hpp"

#include <thread>

namespace lak
{
  void core_init()
  {
    SDL_SetMainReady();
    ASSERTF(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0,
            "Failed to initialise SDL. Make sure you system has SDL2 "
            "installed or the SDL dll is in the same directory as this "
            "program");
  }

  void core_quit() { SDL_Quit(); }

  struct window::window_impl
  {
    union graphics_context
    {
      void *ptr;
      SDL_GLContext sdl_gl;
    };

    SDL_DisplayMode display_mode;
    SDL_Window *wnd          = nullptr;
    graphics_context context = {nullptr};
  };

  window::window(const std::string &title, vec2u32_t size, uint32_t flags)
  : _title(title), _size(size)
  {
    ASSERT(size.x > 0);
    ASSERT(size.y > 0);

    _impl = std::make_unique<lak::window::window_impl>();

    if (!_impl)
    {
      ERROR("Failed to allocate window");
      return;
    }

    const int display = 0;
    SDL_GetCurrentDisplayMode(display, &_impl->display_mode);

    _impl->wnd = SDL_CreateWindow(_title.c_str(),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  _size.x,
                                  _size.y,
                                  flags);

    if (!_impl->wnd) ERROR("Failed to create window");
  }

  void window::close()
  {
    if (_impl)
    {
      if (_graphics_mode == graphics_mode::OpenGL && _impl->context.sdl_gl)
        stop_opengl();
      else if (_graphics_mode == graphics_mode::Software && _impl->context.ptr)
        stop_software();

      SDL_DestroyWindow(_impl->wnd);

      _impl.reset();
    }
  }

  bool window::init_opengl(opengl_settings settings)
  {
    ASSERT(_impl);
    if (!_impl) return false;

    ASSERT(_graphics_mode == graphics_mode::None);
    if (_graphics_mode != graphics_mode::None) return false;

#define SET_ATTRIB(A, B)                                                      \
  if (SDL_GL_SetAttribute(A, B))                                              \
  {                                                                           \
    WARNING("Failed to set " #A " to " #B " (" << B << ")");                  \
    return false;                                                             \
  }
    SET_ATTRIB(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
    SET_ATTRIB(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SET_ATTRIB(SDL_GL_DOUBLEBUFFER, settings.double_buffered);
    SET_ATTRIB(SDL_GL_DEPTH_SIZE, settings.depth_size);
    SET_ATTRIB(SDL_GL_RED_SIZE, settings.colour_size);
    SET_ATTRIB(SDL_GL_GREEN_SIZE, settings.colour_size);
    SET_ATTRIB(SDL_GL_BLUE_SIZE, settings.colour_size);
    SET_ATTRIB(SDL_GL_STENCIL_SIZE, settings.stencil_size);
    SET_ATTRIB(SDL_GL_CONTEXT_MAJOR_VERSION, settings.major);
    SET_ATTRIB(SDL_GL_CONTEXT_MINOR_VERSION, settings.minor);
#undef SET_ATTRIB

    if (!(_impl->context.sdl_gl = SDL_GL_CreateContext(_impl->wnd)))
    {
      ERROR("Failed to create an OpenGL context");
      return false;
    }

    if (gl3wInit() != GL3W_OK)
    {
      ERROR("Failed to initialise gl3w");
      stop_opengl();
      return false;
    }

    SDL_GL_MakeCurrent(_impl->wnd, _impl->context.sdl_gl);

    ASSERTF(SDL_GL_SetSwapInterval(-1) == 0 || SDL_GL_SetSwapInterval(1) == 0,
            "Failed to set swap interval");

    _graphics_mode = graphics_mode::OpenGL;

    return true;
  }

  void window::stop_opengl()
  {
    ASSERT(_impl);
    ASSERT(_impl->context.sdl_gl);
    SDL_GL_DeleteContext(_impl->context.sdl_gl);
    _impl->context.sdl_gl = nullptr;
    _graphics_mode        = graphics_mode::None;
  }

  bool window::init_software() { return false; }

  void window::stop_software() { _graphics_mode = graphics_mode::None; }

  void window::swap()
  {
    switch (_graphics_mode)
    {
      case lak::window::graphics_mode::OpenGL:
        SDL_GL_SwapWindow(_impl->wnd);
        break;
      case lak::window::graphics_mode::Software:
        ASSERT(SDL_UpdateWindowSurface(_impl->wnd) == 0);
        break;
    }
  }

  uint64_t performance_frequency() { return SDL_GetPerformanceFrequency(); }

  uint64_t performance_counter() { return SDL_GetPerformanceCounter(); }

  uint64_t yield_frame(const uint64_t last_counter,
                       const uint32_t target_framerate)
  {
    const uint64_t target_count =
      (performance_frequency() / target_framerate) + last_counter;

    for (uint64_t count = performance_counter();
         target_count > last_counter
           ? (count < target_count)
           : (count < target_count || count > last_counter);
         count = performance_counter())
      std::this_thread::yield();

    return performance_counter();
  }
}
