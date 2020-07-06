#ifndef WIN32_MEAN_AND_LEAN
#  define WIN32_MEAN_AND_LEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <windowsx.h>
#include <wingdi.h>

#include "lak/debug.hpp"
#include "lak/events.hpp"
#include "lak/memmanip.hpp"
#include "lak/window.hpp"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#include <strsafe.h>
// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code

#include <thread>

void win32_error_popup(LPWSTR lpszFunction);

void translate_event(const MSG &msg, lak::event *event)
{
  using namespace lak;

  lak::memcpy(&event->platform_event, &msg);

  switch (msg.message)
  {
    /* --- quit_program --- */

    // Quit the application entirely.
    case WM_QUIT:
      DEBUG("WM_QUIT");
      event->type = event_type::quit_program;
      break;

    /* --- window_close --- */

    // User is *asking* to close this window.
    case WM_CLOSE:
      DEBUG("WM_CLOSE");
      event->type = event_type::window_close;
      lak::memset(&event->window, 0);
      break;

      /* --- window_destroyed --- */

    // Window *has* been destroyed.
    case WM_DESTROY:
      DEBUG("WM_DESTROY");
      event->type = event_type::window_destroyed;
      lak::memset(&event->window, 0);
      break;

      /* --- window_created --- */

    // Window has been created.
    case WM_CREATE:
      DEBUG("WM_CREATE");
      event->type = event_type::window_created;
      lak::memset(&event->window, 0);
      break;

      /* --- window_changed --- */

    case WM_WINDOWPOSCHANGED:
      DEBUG("WM_WINDOWPOSCHANGED");
      event->type = event_type::window_changed;
      lak::memset(&event->window, 0);
      break;

      /* --- window_exposed --- */

    case WM_PAINT:
      DEBUG("WM_PAINT");
      event->type = event_type::window_exposed;
      lak::memset(&event->window, 0);
      break;

      /* --- key_down --- */

    case WM_KEYDOWN:
      event->type = event_type::key_down;
      lak::memset(&event->key, 0);
      break;

      /* --- key_up --- */

    case WM_KEYUP:
      event->type = event_type::key_up;
      lak::memset(&event->key, 0);
      break;

      /* --- mouse_button_down --- */

    case WM_LBUTTONDOWN:
      event->type = event_type::mouse_button_down;
      lak::memset(&event->mouse, 0);
      event->mouse.button = mouse_button::left;
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

    case WM_MBUTTONDOWN:
      event->type = event_type::mouse_button_down;
      lak::memset(&event->mouse, 0);
      event->mouse.button = mouse_button::middle;
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

    case WM_RBUTTONDOWN:
      event->type = event_type::mouse_button_down;
      lak::memset(&event->mouse, 0);
      event->mouse.button = mouse_button::right;
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

    case WM_XBUTTONDOWN:
      event->type = event_type::mouse_button_down;
      lak::memset(&event->mouse, 0);
      event->mouse.button =
        (GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON1 ? mouse_button::x1
                                                    : mouse_button::none) |
        (GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON2 ? mouse_button::x2
                                                    : mouse_button::none);
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

      /* --- mouse_button_up --- */

    case WM_LBUTTONUP:
      event->type = event_type::mouse_button_up;
      lak::memset(&event->mouse, 0);
      event->mouse.button = mouse_button::left;
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

    case WM_MBUTTONUP:
      event->type = event_type::mouse_button_up;
      lak::memset(&event->mouse, 0);
      event->mouse.button = mouse_button::middle;
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

    case WM_RBUTTONUP:
      event->type = event_type::mouse_button_up;
      lak::memset(&event->mouse, 0);
      event->mouse.button = mouse_button::right;
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

    case WM_XBUTTONUP:
      event->type = event_type::mouse_button_up;
      lak::memset(&event->mouse, 0);
      event->mouse.button =
        (GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON1 ? mouse_button::x1
                                                    : mouse_button::none) |
        (GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON2 ? mouse_button::x2
                                                    : mouse_button::none);
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

      /* --- mouse_move --- */

    case WM_MOUSEMOVE:
      event->type = event_type::mouse_motion;
      lak::memset(&event->mouse, 0);
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

      /* --- mouse_wheel --- */

    case WM_MOUSEWHEEL:
      event->type = event_type::mouse_wheel;
      lak::memset(&event->mouse, 0);
      event->mouse.wheel.y =
        float(GET_WHEEL_DELTA_WPARAM(msg.wParam)) / float(WHEEL_DELTA);
      // event->mouse.mod = GET_KEYSTATE_WPARAM(msg.wParam);
      event->mouse.position.x = GET_X_LPARAM(msg.lParam);
      event->mouse.position.y = GET_Y_LPARAM(msg.lParam);
      break;

    default: event->type = event_type::platform_event; break;
  }
}

// :TODO: Call this from handle_size_move_event but also let the other
// functions call this when they don't get any events out of the queue.
bool handle_size_move(const lak::window &window)
{
  if (window._moving)
  {
    POINT cursor;
    ASSERT(GetCursorPos(&cursor) != 0);
    const RECT &rect = window._window_start;
    MoveWindow(window.platform_handle(),
               rect.left + cursor.x - window._cursor_start.x,
               rect.top + cursor.y - window._cursor_start.y,
               rect.right - rect.left,
               rect.bottom - rect.top,
               TRUE);
    return true;
  }
  else if (window._resizing)
  {
    POINT cursor;
    ASSERT(GetCursorPos(&cursor) != 0);
    const RECT &rect = window._window_start;
    RECT diff        = {0, 0, 0, 0};
    if (window._side & window.left)
    {
      diff.left  = cursor.x - window._cursor_start.x;
      diff.right = -diff.left;
    }
    if (window._side & window.top)
    {
      diff.top    = cursor.y - window._cursor_start.y;
      diff.bottom = -diff.top;
    }
    if (window._side & window.right)
    {
      diff.right = cursor.x - window._cursor_start.x;
    }
    if (window._side & window.bottom)
    {
      diff.bottom = cursor.y - window._cursor_start.y;
    }
    MoveWindow(window.platform_handle(),
               rect.left + diff.left,
               rect.top + diff.top,
               std::max(0L, (rect.right - rect.left) + diff.right),
               std::max(0L, (rect.bottom - rect.top) + diff.bottom),
               TRUE);
    return true;
  }

  return false;
}

// Returns true if the event has handled by this function.
bool handle_size_move_event(const lak::window &window, const MSG &msg)
{
  switch (msg.message)
  {
    case WM_SYSCOMMAND:
    {
      switch (msg.wParam & 0xFFF0)
      {
        case SC_SIZE:
        {
          switch (msg.wParam & 0xF)
          {
            case 0x1: window._side = window.left; break;
            case 0x2: window._side = window.right; break;
            case 0x3: window._side = window.top; break;
            case 0x4: window._side = window.top | window.left; break;
            case 0x5: window._side = window.top | window.right; break;
            case 0x6: window._side = window.bottom; break;
            case 0x7: window._side = window.bottom | window.left; break;
            case 0x8: window._side = window.bottom | window.right; break;
            default:
              FATAL("Invalid side");
              window._side = 0;
              break;
          }

          // screen coords.
          ASSERT(GetWindowRect(msg.hwnd, &window._window_start) != 0);
          ASSERT(GetCursorPos(&window._cursor_start) != 0);
          window._resizing = true;
          return true;
        }

        case SC_MOVE:
        {
          // screen coords.
          ASSERT(GetWindowRect(msg.hwnd, &window._window_start) != 0);
          ASSERT(GetCursorPos(&window._cursor_start) != 0);
          window._moving = true;
          return true;
        }
      }
      return true;
    }

    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
      if (window._moving || window._resizing)
      {
        window._moving   = false;
        window._resizing = false;
        return true;
      }
      break;

    case WM_MOUSEMOVE:
    case WM_MOUSELEAVE:
    case WM_MOUSEHOVER:
    case WM_NCMOUSEMOVE:
    case WM_NCMOUSELEAVE:
    case WM_NCMOUSEHOVER: return handle_size_move(window);
  }

  return false;
}

// :TODO: If this ends up being for a windows that is about to get destroyed,
// this message should be zeroed out.
thread_local MSG previous_event = {};

bool next_event(const lak::platform_instance &instance,
                const lak::window *lwindow,
                HWND window,
                lak::event *event)
{
  // Delaying dispatch until the next time through here should let us handle
  // WM_PAINT correctly after the call to next_event.
  if (previous_event.message)
  {
    DispatchMessageW(&previous_event);
  }

  const UINT filter_min = 0;
  const UINT filter_max = 0;

  MSG msg = {};
  do
  {
    if (!PeekMessageW(&msg, window, filter_min, filter_max, PM_REMOVE))
    {
      if (lwindow) handle_size_move(*lwindow);

      lak::memset(&previous_event, 0);
      return false;
    }
    // Skip all messages handled by handle_size_move_event.
  } while (lwindow && handle_size_move_event(*lwindow, msg));

  TranslateMessage(&msg);

  if (msg.message == WM_DESTROY)
    lak::memset(&previous_event, 0);
  else
    lak::memcpy(&previous_event, &msg);

  translate_event(msg, event);

  return true;
}

bool peek_event(const lak::platform_instance &instance,
                HWND window,
                lak::event *event)
{
  const UINT filter_min = 0;
  const UINT filter_max = 0;

  MSG msg = {};
  if (!PeekMessageW(&msg, window, filter_min, filter_max, 0))
  {
    return false;
  }

  translate_event(msg, event);

  return true;
}

bool lak::next_thread_event(const lak::platform_instance &instance,
                            lak::event *event)
{
  return next_event(instance, nullptr, reinterpret_cast<HWND>(-1), event);
}

bool lak::peek_thread_event(const lak::platform_instance &instance,
                            lak::event *event)
{
  return peek_event(instance, reinterpret_cast<HWND>(-1), event);
}
