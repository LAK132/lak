#ifndef SDL_MAIN_HANDLED
#  error SDL_MAIN_HANDLED must be defined globally
#endif
#ifndef LAK_USE_SDL
#  define LAK_USE_SDL
#endif

#include <SDL.h>

#include <GL/gl3w.h>

#include "lak/opengl/state.hpp"

#include "lak/defer.hpp"
#include "lak/memmanip.hpp"
#include "lak/window.hpp"

lak::window_handle *lak::create_window(const lak::platform_instance &instance,
                                       const lak::software_settings &)
{
  auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
  ASSERT(handle);

  DEFER(if (handle) lak::destroy_window(instance, handle.get()););

  handle->_platform_handle = SDL_CreateWindow("",
                                              SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED,
                                              720,
                                              480,
                                              SDL_WINDOW_RESIZABLE);

  if (!handle->_platform_handle)
  {
    ERROR("Failed to create window");
    return nullptr;
  }

  auto &context = handle->_context.emplace<lak::software_context>();

  context.platform_handle = SDL_GetWindowSurface(handle->_platform_handle);

  if (!context.platform_handle)
  {
    ERROR("Failed to get window surface");
    return nullptr;
  }

  context.sdl_window = handle->_platform_handle;

  return handle.release();
}

lak::window_handle *lak::create_window(const lak::platform_instance &instance,
                                       const lak::opengl_settings &settings)
{
  auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
  ASSERT(handle);

  DEFER(if (handle) lak::destroy_window(instance, handle.get()););

  // :TODO: SDL2 documentation says this should be called *after*
  // SDL_GL_SetAttribute but that's causing the screen to stay perminently
  // black? Do we need to create a "fake" context first, init gl3w, destroy the
  // context, then set attributes and create the real context?
  handle->_platform_handle =
    SDL_CreateWindow("",
                     SDL_WINDOWPOS_CENTERED,
                     SDL_WINDOWPOS_CENTERED,
                     720,
                     480,
                     SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

  if (!handle->_platform_handle)
  {
    ERROR("Failed to create window");
    return nullptr;
  }

  auto &context = handle->_context.emplace<lak::opengl_context>();

#define SET_ATTRIB(A, B)                                                      \
  if (SDL_GL_SetAttribute(A, B))                                              \
  {                                                                           \
    WARNING("Failed to set " #A " to " #B " (", B, ")");                      \
    return nullptr;                                                           \
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

  if (context.platform_handle = SDL_GL_CreateContext(handle->_platform_handle);
      !context.platform_handle)
  {
    ERROR("Failed to create an OpenGL context");
    ERROR(SDL_GetError());
    return nullptr;
  }

  if (gl3wInit() != GL3W_OK)
  {
    ERROR("Failed to initialise gl3w");
    return nullptr;
  }

  if (!lak::opengl::check_error())
  {
    ERROR("OpenGL in bad state");
    return nullptr;
  }

  if (SDL_GL_MakeCurrent(handle->_platform_handle, context.platform_handle) !=
      0)
  {
    ERROR("Failed to make context current for window");
    ERROR(SDL_GetError());
    return nullptr;
  }

  if (SDL_GL_SetSwapInterval(-1) != 0 && SDL_GL_SetSwapInterval(1) != 0 &&
      SDL_GL_SetSwapInterval(0) != 0)
  {
    ERROR("Failed to set swap interval");
    ERROR(SDL_GetError());
    return nullptr;
  }

  context.sdl_window = handle->_platform_handle;

  return handle.release();
}

lak::window_handle *lak::create_window(const lak::platform_instance &instance,
                                       const lak::vulkan_settings &)
{
  auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
  ASSERT(handle);

  DEFER(if (handle) lak::destroy_window(instance, handle.get()););

  handle->_platform_handle =
    SDL_CreateWindow("",
                     SDL_WINDOWPOS_CENTERED,
                     SDL_WINDOWPOS_CENTERED,
                     720,
                     480,
                     SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

  if (!handle->_platform_handle)
  {
    ERROR("Failed to create window");
    return nullptr;
  }

  [[maybe_unused]] auto &context =
    handle->_context.emplace<lak::vulkan_context>();

  return handle.release();
}

bool lak::destroy_window(const lak::platform_instance &,
                         lak::window_handle *handle)
{
  ASSERT(handle);

  switch (handle->graphics_mode())
  {
    case lak::graphics_mode::None:
    {
    }
    break;

    case lak::graphics_mode::Software:
    {
    }
    break;

    case lak::graphics_mode::OpenGL:
    {
      SDL_GL_DeleteContext(handle->opengl_context().platform_handle);
    }
    break;

    case lak::graphics_mode::Vulkan:
    {
    }
    break;
  }
  handle->_context.emplace<std::monostate>();

  if (handle->_platform_handle) SDL_DestroyWindow(handle->_platform_handle);

  return true;
}

/* --- OpenGL --- */

// :TODO: Actually put this in a header somewhere.
bool lak::set_opengl_swap_interval(const lak::opengl_context &, int interval)
{
  return SDL_GL_SetSwapInterval(interval) == 0;
}

/* --- Window helper functions --- */

lak::wstring lak::window_title(const lak::platform_instance &,
                               const lak::window_handle *handle)
{
  return lak::to_wstring(
    lak::as_u8string(SDL_GetWindowTitle(handle->_platform_handle)));
}

bool lak::set_window_title(const lak::platform_instance &,
                           lak::window_handle *handle,
                           const lak::wstring &str)
{
  SDL_SetWindowTitle(
    handle->_platform_handle,
    reinterpret_cast<const char *>(lak::to_u8string(str).c_str()));
  return true;
}

lak::vec2l_t lak::window_size(const lak::platform_instance &,
                              const lak::window_handle *handle)
{
  int w, h;
  SDL_GetWindowSize(handle->_platform_handle, &w, &h);
  return {w, h};
}

bool lak::set_window_cursor_pos(const lak::platform_instance &,
                                const lak::window_handle *handle,
                                lak::vec2l_t pos)
{
  ASSERT(pos.x < INT_MAX && pos.x > INT_MIN);
  ASSERT(pos.y < INT_MAX && pos.y > INT_MIN);
  // :TODO: clamp instead of cast
  SDL_WarpMouseInWindow(handle->_platform_handle, (int)pos.x, (int)pos.y);
  return true;
}

lak::vec2l_t lak::window_drawable_size(const lak::platform_instance &,
                                       const lak::window_handle *handle)
{
  int w = 0, h = 0;
  switch (handle->graphics_mode())
  {
    case lak::graphics_mode::Software:
    {
      auto *surface = SDL_GetWindowSurface(handle->_platform_handle);
      ASSERT(surface);
      w = surface->w;
      h = surface->h;
    }
    break;
    case lak::graphics_mode::OpenGL:
      SDL_GL_GetDrawableSize(handle->_platform_handle, &w, &h);
      break;
    case lak::graphics_mode::Vulkan:
      // SDL_Vulkan_GetDrawableSize(handle->_platform_handle, &w, &h);
      break;
    default: FATAL("Invalid graphics mode (", handle->graphics_mode(), ")");
  }
  return {w, h};
}

bool lak::set_window_size(const lak::platform_instance &,
                          lak::window_handle *handle,
                          lak::vec2l_t size)
{
  ASSERT_GREATER(std::numeric_limits<int>::max(), size.x);
  ASSERT_GREATER(std::numeric_limits<int>::max(), size.y);

  SDL_SetWindowSize(handle->_platform_handle,
                    static_cast<int>(size.x),
                    static_cast<int>(size.y));
  return true;
}

bool lak::swap_window(const lak::platform_instance &,
                      lak::window_handle *handle)
{
  switch (handle->graphics_mode())
  {
    case lak::graphics_mode::Software:
    {
      return SDL_UpdateWindowSurface(handle->software_context().sdl_window) ==
             0;
    }

    case lak::graphics_mode::OpenGL:
    {
      SDL_GL_SwapWindow(handle->opengl_context().sdl_window);
      return true;
    }

    default: return false;
  }
}
