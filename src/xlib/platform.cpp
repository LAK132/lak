#include "lak/platform.hpp"
#include "lak/debug.hpp"

bool lak::platform_init(lak::platform_instance *handle)
{
  handle->handle = XOpenDisplay(NULL);
  if (!handle->handle) return false;
  handle->wm_delete_window =
    XInterAtom(handle->handle, "WM_DELETE_WINDOW", False);
  // When we construct a window we need to `XSetWMProtocols(display, window,
  // &wm_delete_window, 1);` on it.
  return true;
}

bool lak::platform_quit(lak::platform_instance *handle)
{
  XCloseDisplay(handle->handle);
  handle->wm_delete_window = None;
  return true;
}
