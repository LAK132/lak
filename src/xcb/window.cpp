#include "lak/window.hpp"

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
  return false;
}

bool lak::create_opengl_window(const lak::platform_instance &instance,
                               const lak::opengl_settings &settings,
                               lak::window_handle *window,
                               lak::window *lwindow)
{
  return false;
}

bool lak::destroy_opengl_window(const lak::platform_instance &instance,
                                lak::window_handle *window)
{
  return false;
}

bool lak::swap_opengl_window(const lak::platform_instance &instance,
                             const lak::window_handle &window)
{
  return false;
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
  return 0;
}

uint64_t lak::performance_counter()
{
  return 0;
}
