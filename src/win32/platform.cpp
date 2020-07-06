#ifndef WIN32_MEAN_AND_LEAN
#  define WIN32_MEAN_AND_LEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <windowsx.h>
#include <wingdi.h>

#include "lak/debug.hpp"
#include "lak/memmanip.hpp"
#include "lak/platform.hpp"
#include "lak/window.hpp"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#include <strsafe.h>
// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code

#include <thread>

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

void win32_error_popup(LPWSTR lpszFunction)
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

bool lak::platform_init(lak::platform_instance *handle)
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
    win32_error_popup(L"RegiserClassW");
    // :TODO: get a proper error message from windows?
    return false;
  }

  return true;
}

bool lak::platform_quit(lak::platform_instance *handle)
{
  if (!UnregisterClassW(handle->window_class.lpszClassName, handle->handle))
  {
    win32_error_popup(L"UnregisterClassW");
    return false;
  }
  lak::memset(handle, 0);
  return true;
}
