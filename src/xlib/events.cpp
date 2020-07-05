#include "lak/events.hpp"
#include "lak/memmanip.hpp"

bool lak::platform_connect(lak::platform_instance *handle)
{
  handle->handle = XOpenDisplay(NULL);
  if (!handle->handle) return false;
  handle->wm_delete_window =
    XInterAtom(handle->handle, "WM_DELETE_WINDOW", False);
  // When we construct a window we need to `XSetWMProtocols(display, window,
  // &wm_delete_window, 1);` on it.
  return true;
}

bool lak::platform_disconnect(lak::platform_instance *handle)
{
  XCloseDisplay(handle->handle);
  handle->wm_delete_window = None;
  return true;
}

bool lak::create_window(lak::window_handle *window) {}

bool lak::destroy_window(lak::window_handle *window) {}

bool lak::next_event(const lak::platform_instance &handle, lak::event *event)
{
  if (XPending(handle.handle) < 1) return false;
  XNextEvent(handle.handle, &event->platform_event);

  const auto &xevent = event->platform_event;

  switch (xevent.type)
  {
      /* --- quit_program --- */

      // I have absolutely no idea how to handle this on Linux...

      /* --- window_close --- */

    case ClientMessage:
      if ((Atom)xevent.xclient.data.l[0] == handle.wm_delete_window)
      {
        event->type = event_type::window_close;
        lak::memset(&event->window, 0);
      }
      break;

      /* --- window_destroyed --- */

    case DestroyNotify:
      event->type = event_type::window_destroyed;
      lak::memset(&event->window, 0);
      break;

      /* --- window_created --- */

    case CreateNotify:
      event->type = event_type::window_created;
      lak::memset(&event->window, 0);
      break;

      /* --- window_changed --- */

    case ConfigureRequest:
      event->type = event_type::window_changed;
      lak::memset(&event->window, 0);
      event->window.position.x = xevent.xconfigurerequest.x;
      event->window.position.y = xevent.xconfigurerequest.y;
      event->window.size.x     = xevent.xconfigurerequest.width;
      event->window.size.y     = xevent.xconfigurerequest.height;
      break;

      /* --- window_exposed --- */

    case Expose:
      event->type = event_type::window_exposed;
      lak::memset(&event->window, 0);
      break;

      /* --- key_down --- */

    case KeyPress:
      event->type = event_type::key_down;
      lak::memset(&event->key, 0);
      break;

      /* --- key_up --- */

    case KeyRelease:
      event->type = event_type::key_up;
      lak::memset(&event->key, 0);
      break;

      /* --- mouse_button_down --- */
      /* --- mouse_wheel --- */

    case ButtonPress:
      if (xevent.xbutton.button == 0 || xevent.xbutton.button > 7)
      {
        event->type = event_type::platform_event;
        break;
      }
      lak::memset(&event->mouse, 0);
      switch (xevent.xbutton.button)
      {
        case 1: event->mouse.button = mouse_button::left; break;
        case 2: event->mouse.button = mouse_button::right; break;
        case 3: event->mouse.button = mouse_button::middle; break;
        case 4: event->mouse.wheel.y = 1.0f; break;
        case 5: event->mouse.wheel.y = -1.0f; break;
        case 6: event->mouse.wheel.x = 1.0f; break;
        case 7: event->mouse.wheel.x = -1.0f; break;
        default: break;
      }
      // event->mouse.mod = ;
      event->mouse.position.x = xevent.xbutton.x;
      event->mouse.position.y = xevent.xbutton.y;
      break;

      /* --- mouse_button_up --- */

    case ButtonRelease:
      if (xevent.xbutton.button == 0 && xevent.xbutton.button > 3)
      {
        event->type = event_type::platform_event;
        break;
      }
      lak::memset(&event->mouse, 0);
      switch (xevent.xbutton.button)
      {
        case 1: event->mouse.button = mouse_button::left; break;
        case 2: event->mouse.button = mouse_button::right; break;
        case 3: event->mouse.button = mouse_button::middle; break;
        default: break;
      }
      // event->mouse.mod = ;
      event->mouse.position.x = xevent.xbutton.x;
      event->mouse.position.y = xevent.xbutton.y;
      break;

      /* --- mouse_move --- */

    case MotionNotify:
      lak::memset(&event->mouse, 0);
      // event->mouse.mod = ;
      event->mouse.position.x = xevent.xbutton.x;
      event->mouse.position.y = xevent.xbutton.y;
      break;

    default: event->type = event_type::platform_event;
  }

  return true;
}
