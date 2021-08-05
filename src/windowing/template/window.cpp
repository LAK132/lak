#include "lak/window.hpp"

lak::window_handle *lak::create_window(const lak::platform_instance &i,
                                       const lak::software_settings &s)
{
  (void)i;
  (void)s;
  return nullptr;
}

lak::window_handle *lak::create_window(const lak::platform_instance &i,
                                       const lak::opengl_settings &s)
{
  (void)i;
  (void)s;
  return nullptr;
}

lak::window_handle *lak::create_window(const lak::platform_instance &i,
                                       const lak::vulkan_settings &s)
{
  (void)i;
  (void)s;
  return nullptr;
}

bool lak::destroy_window(const lak::platform_instance &i,
                         lak::window_handle *w)
{
  (void)i;
  (void)w;
  return false;
}

lak::wstring lak::window_title(const lak::platform_instance &i,
                               const lak::window_handle *w)
{
  (void)i;
  (void)w;
  return {};
}

bool lak::set_window_title(const lak::platform_instance &i,
                           lak::window_handle *w,
                           const lak::wstring &s)
{
  (void)i;
  (void)w;
  (void)s;
  return false;
}

lak::vec2l_t lak::window_size(const lak::platform_instance &i,
                              const lak::window_handle *w)
{
  (void)i;
  (void)w;
  return {};
}

lak::vec2l_t lak::window_drawable_size(const lak::platform_instance &i,
                                       const lak::window_handle *w)
{
  (void)i;
  (void)w;
  return {};
}

bool lak::set_window_size(const lak::platform_instance &i,
                          lak::window_handle *w,
                          lak::vec2l_t s)
{
  (void)i;
  (void)w;
  (void)s;
  return false;
}

bool lak::set_window_cursor_pos(const lak::platform_instance &i,
                                const lak::window_handle *w,
                                lak::vec2l_t p)
{
  (void)i;
  (void)w;
  (void)p;
  return false;
}

bool lak::set_opengl_swap_interval(const lak::opengl_context &c, int interval)
{
  (void)c;
  (void)interval;
  return false;
}

bool lak::swap_window(const lak::platform_instance &i, lak::window_handle *w)
{
  (void)i;
  (void)w;
  return false;
}
