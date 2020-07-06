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

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#include <strsafe.h>
// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code

#include <thread>

thread_local bool peek_message_is_safe = true;

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  lak::window *window =
    reinterpret_cast<lak::window *>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

  if (Msg == WM_CREATE)
  {
    // Get the lak::window this ptr.
    const CREATESTRUCTW *create =
      reinterpret_cast<const CREATESTRUCTW *>(lParam);

    window = reinterpret_cast<lak::window *>(create->lpCreateParams);

    SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

    // This must be called after SetWindowLongPtrW in order for the change to
    // take effect.
    SetWindowPos(hWnd,
                 NULL,
                 0,
                 0,
                 0,
                 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
  }

  if (window)
  {
    switch (Msg)
    {
      case WM_CREATE:
      case WM_DESTROY:
      case WM_CLOSE:
        window->_platform_events.push_back(MSG{hWnd, Msg, wParam, lParam});
        return 0;
      case WM_SYSCOMMAND:
        if (WPARAM w = wParam & 0xFFF0; w == SC_MOVE || w == SC_SIZE)
        {
          window->_platform_events.push_back(MSG{hWnd, Msg, wParam, lParam});
          return 0;
        }
        break;
    }
  }
  return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

void ErrorPopup(LPWSTR lpszFunction)
{
  // Retrieve the system error message for the last-error code

  LPVOID lpMsgBuf;
  LPVOID lpDisplayBuf;
  DWORD dw = GetLastError();

  FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS,           /* dwFlags */
                 NULL,                                      /* lpSource */
                 dw,                                        /* dwMessageId */
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* dwLanguageId */
                 (LPWSTR)&lpMsgBuf,                         /* lpBuffer */
                 0,                                         /* nSize */
                 NULL                                       /* Arguments */
  );

  // Display the error message and exit the process

  lpDisplayBuf = (LPVOID)LocalAlloc(
    LMEM_ZEROINIT,
    (lstrlenW((LPCWSTR)lpMsgBuf) + lstrlenW((LPCWSTR)lpszFunction) + 40) *
      sizeof(WCHAR));

  StringCchPrintfW((LPWSTR)lpDisplayBuf,
                   LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                   L"%s failed with error %d: %s",
                   lpszFunction,
                   dw,
                   lpMsgBuf);

  MessageBoxW(NULL, (LPCWSTR)lpDisplayBuf, L"Error", MB_OK);

  LocalFree(lpMsgBuf);
  LocalFree(lpDisplayBuf);
  // ExitProcess(dw);
}

bool lak::platform_connect(lak::platform_instance *handle)
{
  lak::memset(handle, 0);
  handle->handle = HINST_THISCOMPONENT;

  handle->window_class             = WNDCLASSW{};
  handle->window_class.lpfnWndProc = &WndProc;
  handle->window_class.hInstance   = handle->handle;
  handle->window_class.hIcon =
    LoadIconW(handle->handle, MAKEINTRESOURCEW(IDI_APPLICATION));
  handle->window_class.hCursor =
    LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
  handle->window_class.lpszClassName = L"lak's window class";

  handle->window_class_atom = RegisterClassW(&handle->window_class);
  if (!handle->window_class_atom)
  {
    lak::memset(handle, 0);
    ErrorPopup(L"RegiserClassW");
    // :TODO: get a proper error message from windows?
    return false;
  }

  return true;
}

bool lak::platform_disconnect(lak::platform_instance *handle)
{
  if (!UnregisterClassW(handle->window_class.lpszClassName, handle->handle))
  {
    ErrorPopup(L"UnregisterClassW");
    return false;
  }
  lak::memset(handle, 0);
  return true;
}

bool lak::create_window(const platform_instance &instance,
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
    ErrorPopup(L"CreateWindowExW");
    return false;
  }

  ShowWindow(hwnd, SW_SHOWNORMAL);

  window->platform_handle = hwnd;

  return true;
}

bool lak::destroy_window(const platform_instance &instance,
                         lak::window_handle *window)
{
  if (!DestroyWindow(window->platform_handle))
  {
    ErrorPopup(L"Destroywindow");
    return false;
  }
  window->platform_handle = NULL;
  return true;
}

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

bool next_event(const lak::platform_instance &instance,
                const lak::window *lwindow,
                HWND window,
                lak::event *event,
                MSG *previous = nullptr)
{
  // Delaying dispatch until the next time through here should let us handle
  // WM_PAINT correctly after the call to next_event.
  if (previous && previous->message)
  {
    DispatchMessageW(previous);
  }

  const UINT filter_min = 0;
  const UINT filter_max = 0;

  MSG msg = {};
  do
  {
    if (!PeekMessageW(&msg, window, filter_min, filter_max, PM_REMOVE))
    {
      if (lwindow) handle_size_move(*lwindow);

      if (previous) lak::memset(previous, 0);
      return false;
    }
    // Skip all messages handled by handle_size_move_event.
  } while (lwindow && handle_size_move_event(*lwindow, msg));

  TranslateMessage(&msg);

  if (previous)
  {
    if (msg.message == WM_DESTROY)
      lak::memset(previous, 0);
    else
      lak::memcpy(previous, &msg);
  }

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

// :TODO: If this ends up being for a windows that is about to get destroyed,
// this message should be zeroed out.
thread_local MSG previous_event = {};

bool lak::next_thread_event(const lak::platform_instance &instance,
                            lak::event *event)
{
  return next_event(
    instance, nullptr, reinterpret_cast<HWND>(-1), event, &previous_event);
}

bool lak::peek_thread_event(const lak::platform_instance &instance,
                            lak::event *event)
{
  return peek_event(instance, reinterpret_cast<HWND>(-1), event);
}

bool lak::next_window_event(const lak::platform_instance &instance,
                            const lak::window &window,
                            lak::event *event)
{
  MSG *msg = window._platform_events.front();
  if (!msg)
    return next_event(
      instance, &window, window.platform_handle(), event, &previous_event);

  // Skip all messages handled by handle_size_move_event.
  while (handle_size_move_event(window, *msg))
  {
    window._platform_events.pop_front();
    msg = window._platform_events.front();
    if (!msg)
      return next_event(
        instance, &window, window.platform_handle(), event, &previous_event);
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
