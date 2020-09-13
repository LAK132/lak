#include "lak/window.hpp"
#include "lak/memmanip.hpp"

bool lak::create_software_window(const lak::platform_instance &instance,
                                 lak::window_handle *window,
                                 lak::window *lwindow)
{
  return false;
}

bool lak::destroy_software_window(const lak::platform_instance &instance,
                                  lak::window_handle *window)
{
  return false;
}

bool lak::swap_software_window(const lak::platform_instance &instance,
                               const lak::window_handle &window)
{
  return SDL_UpdateWindowSurface(window.platform_handle) == 0;
}

bool lak::create_opengl_window(const lak::platform_instance &instance,
                               const lak::opengl_settings &settings,
                               lak::window_handle *window,
                               lak::window *lwindow)
{
  // :TODO: SDL2 documentation says this should be called *after*
  // SDL_GL_SetAttribute but that's causing the screen to stay perminently
  // black? Do we need to create a "fake" context first, init gl3w, destroy the
  // context, then set attributes and create the real context?
  window->platform_handle = SDL_CreateWindow("",
                                             SDL_WINDOWPOS_CENTERED,
                                             SDL_WINDOWPOS_CENTERED,
                                             480,
                                             720,
                                             SDL_WINDOW_OPENGL);

  if (!window->platform_handle)
  {
    ERROR("Failed to create window");
    return false;
  }

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

  if (!(window->opengl_context =
          SDL_GL_CreateContext(window->platform_handle)))
  {
    ERROR("Failed to create an OpenGL context");
    return false;
  }

  if (gl3wInit() != GL3W_OK)
  {
    ERROR("Failed to initialise gl3w");
    lak::destroy_opengl_window(instance, window);
    return false;
  }

  SDL_GL_MakeCurrent(window->platform_handle, window->opengl_context);

  if (SDL_GL_SetSwapInterval(-1) != 0 && SDL_GL_SetSwapInterval(1) != 0)
  {
    ERROR("Failed to set swap interval");
    lak::destroy_opengl_window(instance, window);
    return false;
  }

  return true;
}

bool lak::destroy_opengl_window(const lak::platform_instance &instance,
                                lak::window_handle *window)
{
  SDL_GL_DeleteContext(window->opengl_context);
  SDL_DestroyWindow(window->platform_handle);
  lak::memset(window, 0);
  return true;
}

bool lak::swap_opengl_window(const lak::platform_instance &instance,
                             const lak::window_handle &window)
{
  SDL_GL_SwapWindow(window.platform_handle);
  return true;
}

bool lak::create_vulkan_window(const lak::platform_instance &instance,
                               const lak::vulkan_settings &settings,
                               lak::window_handle *window,
                               lak::window *lwindow)
{
  return false;
}

bool lak::destroy_vulkan_window(const lak::platform_instance &instance,
                                lak::window_handle *window)
{
  return false;
}

bool lak::swap_vulkan_window(const lak::platform_instance &instance,
                             const lak::window_handle &window)
{
  return false;
}

lak::wstring lak::window_title(const lak::platform_instance &instance,
                               const lak::window_handle &window)
{
  return {};
}

bool lak::set_window_title(const lak::platform_instance &instance,
                           const lak::window_handle &window,
                           const lak::wstring &str)
{
  return false;
}

lak::vec2l_t lak::window_size(const lak::platform_instance &instance,
                              const lak::window_handle &window)
{
  return {};
}

lak::vec2l_t lak::window_drawable_size(const lak::platform_instance &instance,
                                       const lak::window_handle &window)
{
  return {};
}

bool lak::set_window_size(const lak::platform_instance &instance,
                          const lak::window_handle &window,
                          lak::vec2l_t size)
{
  return false;
}

bool lak::next_window_event(const lak::platform_instance &instance,
                            const lak::window &window,
                            lak::event *event)
{
  return false;
}

bool lak::peek_window_event(const lak::platform_instance &instance,
                            const lak::window &window,
                            lak::event *event)
{
  return false;
}

uint64_t lak::performance_frequency()
{
  return SDL_GetPerformanceFrequency();
}

uint64_t lak::performance_counter()
{
  return SDL_GetPerformanceCounter();
}
