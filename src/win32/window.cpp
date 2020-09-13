#ifndef WIN32_MEAN_AND_LEAN
#  define WIN32_MEAN_AND_LEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <windowsx.h>
#include <wingdi.h>

#include "lak/debug.hpp"
#include "lak/window.hpp"

void win32_error_popup(LPWSTR lpszFunction);

bool handle_size_move(const lak::window &window);

bool handle_size_move_event(const lak::window &window, const MSG &msg);

bool next_event(const lak::platform_instance &instance,
                const lak::window *lwindow,
                HWND window,
                lak::event *event);

bool peek_event(const lak::platform_instance &instance,
                HWND window,
                lak::event *event);

bool lak::create_software_window(const lak::platform_instance &instance,
                                 lak::window_handle *window,
                                 lak::window *lwindow)
{
  HWND hwnd =
    CreateWindowExW(0,                                   /* styles */
                    instance.window_class.lpszClassName, /* class name */
                    L"insert window name here",          /* window name */
                    WS_OVERLAPPEDWINDOW,                 /* style */
                    CW_USEDEFAULT,                       /* x */
                    CW_USEDEFAULT,                       /* y */
                    960,                                 /* width */
                    720,                                 /* height */
                    nullptr,                             /* parent */
                    nullptr,                             /* menu */
                    instance.handle,                     /* hInstance */
                    lwindow                              /* user data */
    );

  if (hwnd == nullptr)
  {
    win32_error_popup(L"CreateWindowExW");
    return false;
  }

  ShowWindow(hwnd, SW_SHOWNORMAL);

  window->platform_handle = hwnd;

  return true;
}

bool lak::destroy_software_window(const lak::platform_instance &instance,
                                  lak::window_handle *window)
{
  if (!DestroyWindow(window->platform_handle))
  {
    win32_error_popup(L"Destroywindow");
    return false;
  }
  window->platform_handle = NULL;
  return true;
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
  std::vector<wchar_t> str;
  str.resize(GetWindowTextLengthW(window.platform_handle) + 1);
  GetWindowTextW(window.platform_handle, str.data(), str.size());
  return lak::wstring(str.data());
}

bool lak::set_window_title(const lak::platform_instance &instance,
                           const lak::window_handle &window,
                           const lak::wstring &str)
{
  return SetWindowTextW(window.platform_handle, str.c_str());
}

lak::vec2l_t lak::window_size(const lak::platform_instance &instance,
                              const lak::window_handle &window)
{
  RECT rect;
  ASSERT(GetWindowRect(window.platform_handle, &rect));
  return lak::vec2l_t{std::max(0L, rect.right - rect.left),
                      std::max(0L, rect.bottom - rect.top)};
}

lak::vec2l_t lak::window_drawable_size(const lak::platform_instance &instance,
                                       const lak::window_handle &window)
{
  RECT rect;
  ASSERT(GetClientRect(window.platform_handle, &rect));
  return lak::vec2l_t{std::max(0L, rect.right - rect.left),
                      std::max(0L, rect.bottom - rect.top)};
}

bool lak::set_window_size(const lak::platform_instance &instance,
                          const lak::window_handle &window,
                          lak::vec2l_t size)
{
  RECT rect;
  return GetWindowRect(window.platform_handle, &rect) &&
         MoveWindow(
           window.platform_handle, rect.left, rect.top, size.x, size.y, TRUE);
}

bool lak::next_window_event(const lak::platform_instance &instance,
                            const lak::window_handle &window,
                            lak::event *event)
{
  return next_event(instance, nullptr, window.platform_handle, event);
}

bool lak::peek_window_event(const lak::platform_instance &instance,
                            const lak::window_handle &window,
                            lak::event *event)
{
  return peek_event(instance, window.platform_handle, event);
}

bool lak::next_window_event(const lak::platform_instance &instance,
                            const lak::window &window,
                            lak::event *event)
{
  MSG *msg = window._platform_events.front();
  if (!msg)
    return next_event(instance, &window, window.platform_handle(), event);

  // Skip all messages handled by handle_size_move_event.
  while (handle_size_move_event(window, *msg))
  {
    window._platform_events.pop_front();
    msg = window._platform_events.front();
    if (!msg)
      return next_event(instance, &window, window.platform_handle(), event);
  }

  translate_event(*msg, event);

  window._platform_events.pop_front();

  return true;
}

bool lak::peek_window_event(const lak::platform_instance &instance,
                            const lak::window &window,
                            lak::event *event)
{
  MSG *msg = window._platform_events.front();
  if (!msg) return peek_event(instance, window.platform_handle(), event);

  translate_event(*msg, event);

  return true;
}

uint64_t lak::performance_frequency()
{
  return 0;
}

uint64_t lak::performance_counter()
{
  return 0;
}
