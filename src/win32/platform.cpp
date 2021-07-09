#include "src/win32/platform.hpp"

#include "lak/debug.hpp"
#include "lak/image.hpp"
#include "lak/memmanip.hpp"
#include "lak/platform.hpp"
#include "lak/strconv.hpp"
#include "lak/window.hpp"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#include <strsafe.h>
// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code

#include <thread>

template<typename TO, typename FROM>
bool set_bits(HBITMAP hBitmap, lak::image_view<FROM> image)
{
  lak::image<TO, true> bitmap_pixels(image.size());
  lak::blit(lak::image_subview(bitmap_pixels), lak::image_subview(image));
  return ::SetBitmapBits(
           hBitmap, bitmap_pixels.contig_size_bytes(), bitmap_pixels.data()) ==
         bitmap_pixels.contig_size_bytes();
}

template<typename COLOUR>
bool paint_image(HWND hWnd, lak::image_view<COLOUR> image)
{
  PAINTSTRUCT ps;
  HDC hDC = ::BeginPaint(hWnd, &ps);
  if (!hDC)
  {
    ERROR(win32_error_string(L"BeginPaint"));
    return false;
  }
  DEFER(::EndPaint(hWnd, &ps));

  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),
    1 /* version */,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_GDI,
    lak::colour::is_indexed_v<COLOUR> ? PFD_TYPE_COLORINDEX : PFD_TYPE_RGBA,
    32 /* colour depth */,
    0 /* ignored */,
    0 /* ignored */,
    0 /* ignored */,
    0 /* ignored */,
    0 /* ignored */,
    0 /* ignored */,
    0 /* no alpha buffer */,
    0 /* ignored */,
    0 /* no accumulation buffer */,
    0 /* ignored */,
    0 /* ignored */,
    0 /* ignored */,
    0 /* ignored */,
    0 /* Z buffer depth */,
    0 /* stencil buffer depth */,
    0 /* auxiliary buffer depth */,
    PFD_MAIN_PLANE,
    0 /* reserved */,
    0 /* ignored */,
    0 /* ignored */,
    0 /* ignored */};

  int pfi = ::ChoosePixelFormat(hDC, &pfd);
  if (pfi == 0)
  {
    ERROR(win32_error_string(L"ChoosePixelFormat"));
    return false;
  }

  if (::DescribePixelFormat(hDC, pfi, sizeof(pfd), &pfd) == 0)
  {
    ERROR(win32_error_string(L"DescribePixelFormat"));
    return false;
  }

  HDC memDC = ::CreateCompatibleDC(hDC);
  if (!memDC)
  {
    ERROR(win32_error_string(L"CreateCompatibleDC"));
    return false;
  }
  DEFER(::DeleteDC(memDC));

  // :NOTE: not using memDC here, that would create a monochrome bitmap
  HBITMAP hBitmap =
    ::CreateCompatibleBitmap(hDC, image.size().x, image.size().y);
  if (!hBitmap)
  {
    ERROR(win32_error_string(L"CreateCompatibleBitmap"));
    return false;
  }
  DEFER(::DeleteObject(hBitmap));

  // work out what the colour type of hDC is and construct a buffer from
  // that, then blit image to it.

  HGDIOBJ oldBitmap = ::SelectObject(memDC, hBitmap);
  DEFER(::SelectObject(memDC, oldBitmap));

  if (pfd.iPixelType == PFD_TYPE_RGBA)
  {
    switch (pfd.cColorBits)
    {
      case 32:
        if (!set_bits<lak::colour::bgrx8888>(hBitmap, image))
        {
          ERROR("set_bits bgrx8888 failed");
          return false;
        }
        break;

      case 24:
        if (!set_bits<lak::colour::bgr888>(hBitmap, image))
        {
          ERROR("set_bits bgr888 failed");
          return false;
        }
        break;

      case 16:
        if (!set_bits<lak::colour::bgr555>(hBitmap, image))
        {
          ERROR("set_bits bgr555 failed");
          return false;
        }
        break;

      default: FATAL(pfd.cColorBits); return false;
    }
  }
  else if (pfd.iPixelType == PFD_TYPE_COLORINDEX)
  {
    switch (pfd.cColorBits)
    {
      case 256: // lak::colour::i8
      case 16:  // lak::colour::i4
      case 2:   // lak::colour::i1
      default: FATAL(pfd.cColorBits); return false;
    }
  }
  else
  {
    FATAL(pfd.iPixelType);
    return false;
  }

  return BitBlt(hDC,            /* destination device context handle */
                0,              /* destination x */
                0,              /* destination y */
                image.size().x, /* destination width */
                image.size().y, /* destination height */
                memDC,          /* source device context handle */
                0,              /* source x */
                0,              /* source y */
                SRCCOPY         /* raster-operation */
                ) != 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  const lak::window_handle *window =
    reinterpret_cast<const lak::window_handle *>(
      GetWindowLongPtrW(hWnd, GWLP_USERDATA));

  if (Msg == WM_CREATE)
  {
    // Get the lak::window this ptr.
    const CREATESTRUCTW *create =
      reinterpret_cast<const CREATESTRUCTW *>(lParam);

    window =
      reinterpret_cast<const lak::window_handle *>(create->lpCreateParams);

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

  if (Msg == WM_UNICHAR && wParam == UNICODE_NOCHAR) return true;

  if (window)
  {
    switch (Msg)
    {
      case WM_CLOSE:
      case WM_CREATE:
      case WM_DESTROY:
      case WM_WINDOWPOSCHANGED:
        window->instance()->platform_events.push_back(
          MSG{hWnd, Msg, wParam, lParam});
        return 0;

      case WM_SYSCOMMAND:
        if (WPARAM w = wParam & 0xFFF0; w == SC_MOVE || w == SC_SIZE)
        {
          window->instance()->platform_events.push_back(
            MSG{hWnd, Msg, wParam, lParam});
          return 0;
        }
        break;

      case WM_UNICHAR:
        if (wParam != UNICODE_NOCHAR) return false;
        break;

      case WM_PAINT:
        if (window->graphics_mode() == lak::graphics_mode::Software)
        {
          if (window->software_context().platform_handle.contig_size() > 0)
          {
            ASSERT(paint_image(
              window->_platform_handle,
              lak::image_view(window->software_context().platform_handle)));
          }
          return 0;
        }
        break;
    }
  }
  return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

lak::wstring win32_error_string(LPCWSTR lpszFunction)
{
  // Retrieve the system error message for the last-error code

  DWORD dw = GetLastError();

  LPWSTR lpMsgBuf;
  FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS,           /* dwFlags */
                 NULL,                                      /* lpSource */
                 dw,                                        /* dwMessageId */
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* dwLanguageId */
                 (LPWSTR)&lpMsgBuf,                         /* lpBuffer */
                 0,                                         /* nSize */
                 NULL                                       /* Arguments */
  );
  DEFER(::LocalFree(lpMsgBuf));

  LPWSTR lpDisplayBuf = (LPWSTR)LocalAlloc(
    LMEM_ZEROINIT,
    (lstrlen(lpMsgBuf) + lstrlen(lpszFunction) + 40) * sizeof(WCHAR));
  DEFER(::LocalFree(lpDisplayBuf));

  StringCchPrintfW(lpDisplayBuf,
                   LocalSize(lpDisplayBuf) / sizeof(WCHAR),
                   L"%s failed with error %d: %s",
                   lpszFunction,
                   dw,
                   lpMsgBuf);

  return lak::wstring(lpDisplayBuf);
}

void win32_error_popup(LPCWSTR lpszFunction)
{
  auto str = win32_error_string(lpszFunction);
  MessageBoxW(NULL, str.c_str(), L"Error", MB_OK);
}

bool init_opengl(lak::platform_instance *handle)
{
  return false;
#if 0
  auto hwnd = CreateWindowW(/* class name */,
                            /* window name */,
                            (WS_POPUP | WS_DISABLED),
                            0,
                            0,
                            10,
                            10,
                            NULL,
                            NULL,
                            /* instance */,
                            NULL);

  if (!hwnd)
  {
    ERROR("Failed to create dummy window for initialising OpenGL");
    return false;
  }

  /* pump events */

  auto hdc = GetDC(hwnd);

  PIXELFORMATDESCRIPTOR format;
  format.nSize    = sizeof(format);
  format.nVersion = 1;
  format.dwFlags  = (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL);
  // if (config.double_buffer)
  // {
  //   format.dwFlags |= PFD_DOUBLEBUFFER;
  // }
  // if (config.stereo)
  // {
  //   format.dwFlags |= PFD_STEREO;
  // }
  format.iLayerType = PFD_MAIN_PLANE;
  format.iPixelType = PFD_TYPE_RGBA;
  // format.cRedBits   = config.red_size;
  // format.cGreenBits = config.green_size;
  // format.cBlueBits  = config.blue_size;
  // format.cAlphaBits = config.alpha_size;
  // if (config.buffer_size)
  // {
  //   format.cColorBits = config.buffer_size - config.alpha_size;
  // }
  // else
  {
    format.cColorBits =
      (format.cRedBits + format.cGreenBits + format.cBlueBits);
  }
  // format.cAccumRedBits   = config.accum_red_size;
  // format.cAccumGreenBits = config.accum_green_size;
  // format.cAccumBlueBits  = config.accum_blue_size;
  // format.cAccumAlphaBits = config.accum_alpha_size;
  format.cAccumBits = (format.cAccumRedBits + format.cAccumGreenBits +
                       format.cAccumBlueBits + format.cAccumAlphaBits);
  // format.cDepthBits      = config.depth_size;
  // format.cStencilBits    = config.stencil_size;

  SetPixelFormat(hdc, ChoosePixelFormat(hdc, &format), &format);

  auto hglrc = wglCreateContext(hdc);

  if (!hglrc)
  {
    DestroyWindow(hwnd);
    ERROR("Failed to create OpenGL context");
    return false;
  }

  wglMakeCurrent(hdc, hglrc);

  wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARB)wglGetProcAddress(
    "wglGetExtensionsStringARB");

  auto extensions = lak::string_view(
    wglGetExtensionsStringARB ? wglGetExtensionsStringARB(hdc) : nullptr);

  auto has_extension = [&extensions](const char *extension) -> bool {
    return lak::find_subspan(extensions, lak::string_view(extension)).size() >
           0;
  };

  if (has_extension("WGL_ARB_pixel_format"))
  {
    wglChoosePixelFormatARB =
      (PFNWGLCHOOSEPIXELFORMATARB)wglGetProcAddress("wglChoosePixelFormatARB");
    wglGetPixelFormatAttribivARB =
      (PFNWGLGETPIXELFORMATATTRIBIVARB)wglGetProcAddress(
        "wglGetPixelFormatAttribivARB");
    wglGetPixelFormatAttribfvARB =
      (PFNWGLGETPIXELFORMATATTRIBFVARB)wglGetProcAddress(
        "wglGetPixelFormatAttribfvARB");
  }
  else
  {
    wglChoosePixelFormatARB      = NULL;
    wglGetPixelFormatAttribivARB = NULL;
  }
  has_pixel_format = wglChoosePixelFormatARB && wglGetPixelFormatAttribivARB;

  if (has_extension("WGL_EXT_swap_control"))
  {
    wglSwapIntervalEXT =
      (PFNWGLSWAPINTERVALEXT)wglGetProcAddress("wglSwapIntervalEXT");
    wglGetSwapIntervalEXT =
      (PFNWGLGETSWAPINTERVALEXT)wglGetProcAddress("wglGetSwapIntervalEXT");
    has_swap_control_tear = has_extension("WGL_EXT_swap_control_tear");
  }
  else
  {
    wglSwapIntervalEXT    = NULL;
    wglGetSwapIntervalEXT = NULL;
  }
  has_swap_control = wglSwapIntervalEXT && wglGetSwapIntervalEXT;

  wglMakeCurrent(hdc, NULL);
  wglDeleteContext(hglrc);
  ReleaseDC(hwnd, hdc);
  DestroyWindow(hwnd);
  /* pump events */
#endif
}

bool lak::platform_init(lak::platform_instance *handle)
{
  lak::bzero(handle);
  handle->handle = HINST_THISCOMPONENT;

  handle->window_class             = WNDCLASSW{};
  handle->window_class.style       = CS_OWNDC;
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
    lak::bzero(handle);
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
  lak::bzero(handle);
  return true;
}

bool lak::get_clipboard(const lak::platform_instance &i, lak::u8string *s)
{
  if (!OpenClipboard(NULL))
  {
    WARNING("Failed to open clipboard");
    return false;
  }

  HANDLE clipboard_data = GetClipboardData(CF_UNICODETEXT);
  if (!clipboard_data)
  {
    WARNING("Failed to get clipboard data");
    return false;
  }

  wchar_t *data = (wchar_t *)GlobalLock(clipboard_data);
  if (!data)
  {
    WARNING("Failed to lock clipboard data");
    return false;
  }

  *s = lak::to_u8string(data);

  ASSERT(GlobalUnlock(clipboard_data));
  ASSERT(CloseClipboard());

  return true;
}

bool lak::set_clipboard(const lak::platform_instance &i,
                        const lak::u8string &s)
{
  return lak::set_clipboard(i, lak::string_view(s));
}

bool lak::set_clipboard(const lak::platform_instance &i,
                        lak::span<const char8_t> s)
{
  [&]() -> lak::winapi::result<lak::monostate> {
    std::wstring str = lak::to_wstring(s);

    WINAPI_EXPECT(::OpenClipboard(NULL));

    DEFER(::CloseClipboard());

    WINAPI_EXPECT(::EmptyClipboard());

    size_t size = (str.size() + 1) * sizeof(wchar_t);

    RES_TRY_ASSIGN(HGLOBAL clipboard_data =,
                   lak::winapi::invoke_nullerr(::GlobalAlloc, NULL, size));

    {
      RES_TRY_ASSIGN(
        wchar_t *data = (wchar_t *),
        lak::winapi::invoke_nullerr(::GlobalLock, clipboard_data));

      DEFER(lak::winapi::invoke_bool<true>(::GlobalUnlock, clipboard_data)
              .EXPECT("GlobalUnlock failed"));

      std::memcpy(data, str.c_str(), size);
    }

    return lak::winapi::invoke_bool<true>(
             ::SetClipboardData, CF_UNICODETEXT, clipboard_data)
      .map_err([&](auto err) {
        lak::winapi::invoke_bool<true>(::GlobalFree, clipboard_data)
          .EXPECT("GlobalFree failed");
        return err;
      });
  }()
             .EXPECT("set_clipboard failed");
  return true;
}

bool lak::cursor_visible(const lak::platform_instance &i)
{
  CURSORINFO info;
  GetCursorInfo(&info);
  return info.flags != 0;
}

void lak::show_cursor(const lak::platform_instance &i)
{
  if (!lak::cursor_visible(i)) ShowCursor(true);
}

void lak::hide_cursor(const lak::platform_instance &i)
{
  if (lak::cursor_visible(i)) ShowCursor(false);
}

void lak::set_cursor_pos(const lak::platform_instance &i, lak::vec2l_t p)
{
  ASSERT(p.x < INT_MAX && p.x > INT_MIN);
  ASSERT(p.y < INT_MAX && p.y > INT_MIN);
  // :TODO: clamp instead of cast
  ASSERT(SetCursorPos((int)p.x, (int)p.y));
}

lak::vec2l_t lak::get_cursor_pos(const lak::platform_instance &i)
{
  POINT p;
  ASSERT(GetCursorPos(&p));
  return {(long)p.x, (long)p.y};
}

void lak::set_cursor(const lak::platform_instance &i, const lak::cursor &c)
{
  SetCursor(c.platform_handle);
}
