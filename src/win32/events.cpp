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

// FIFO queue
static const size_t thread_events_max_size                   = 0x10000;
thread_local MSG thread_events_queue[thread_events_max_size] = {};
thread_local size_t thread_events_pos                        = 0;
thread_local size_t thread_events_size                       = 0;

void push_event(const MSG &msg)
{
  const size_t new_event_index =
    (thread_events_pos + thread_events_size) % thread_events_max_size;
  auto &event = thread_events_queue[new_event_index];
  lak::memcpy(&event, &msg);
  if (thread_events_size < thread_events_max_size)
    ++thread_events_size;
  else
    thread_events_pos = (thread_events_pos + 1) % thread_events_max_size;
}

bool peek_event(MSG *event)
{
  if (thread_events_size == 0) return false;

  lak::memcpy(event, &thread_events_queue[thread_events_pos]);

  return true;
}

bool pop_event(MSG *event)
{
  if (!peek_event(event)) return false;

  --thread_events_size;
  thread_events_pos = (thread_events_pos + 1) % thread_events_max_size;

  return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  switch (Msg)
  {
    case WM_CREATE:
    case WM_DESTROY:
    case WM_CLOSE: push_event(MSG{hWnd, Msg, wParam, lParam}); return 0;
    case WM_SYSCOMMAND:
      if (WPARAM w = wParam & 0xFFF0; w == SC_MOVE || w == SC_SIZE)
      {
        push_event(MSG{hWnd, Msg, wParam, lParam});
        return 0;
      }
      break;
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
  handle->window_class.hIcon       = LoadIcon(handle->handle, IDI_APPLICATION);
  handle->window_class.hCursor     = LoadCursor(handle->handle, IDC_ARROW);
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
                        lak::window_handle *window)
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
                    nullptr                              /* user data */
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

// :TODO: If this ends up being for a windows that is about to get destroyed,
// this message should be zeroed out.
thread_local MSG previous_event = {};

bool lak::next_event(const lak::platform_instance &instance, lak::event *event)
{
  MSG msg = {};
  if (!pop_event(&msg))
  {
    // there was no events stashed from the window proc.

    // Delaying dispatch until the next time through here should let us handle
    // WM_PAINT correctly after the call to next_event.
    if (previous_event.message)
    {
      DispatchMessageW(&previous_event);
    }

    const HWND window     = NULL;
    const UINT filter_min = 0;
    const UINT filter_max = 0;

    if (!PeekMessageW(&msg, window, filter_min, filter_max, PM_REMOVE))
    {
      lak::memset(&previous_event, 0);
      return false;
    }

    TranslateMessage(&msg);

    lak::memcpy(&previous_event, &msg);
  }

  // :TODO: if we get WM_SYSCOMMAND with SC_MOVE or SC_SIZE then don't return
  // mouse events until we get a message telling us the user has let go of the
  // nonclient area.

  if (msg.message == WM_DESTROY && msg.hwnd == previous_event.hwnd)
    lak::memset(&previous_event, 0);

  translate_event(msg, event);

  return true;
}

bool lak::peek_event(const lak::platform_instance &instance, lak::event *event)
{
  MSG msg = {};
  if (!peek_event(&msg))
  {
    // there was no events stashed from the window proc.

    const HWND window     = NULL;
    const UINT filter_min = 0;
    const UINT filter_max = 0;

    if (!PeekMessageW(&msg, window, filter_min, filter_max, 0))
    {
      return false;
    }
  }

  // :TODO: if we get WM_SYSCOMMAND with SC_MOVE or SC_SIZE then don't return
  // mouse events until we get a message telling us the user has let go of the
  // nonclient area.

  translate_event(msg, event);

  return true;
}
