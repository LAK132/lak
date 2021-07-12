#ifndef LAK_SURFACE_HPP
#define LAK_SURFACE_HPP

#if defined(LAK_USE_WINAPI)
#  include "lak/windows.hpp"
void win32_error_popup(LPWSTR lpszFunction);
#elif defined(LAK_USE_XLIB)
#  include <X11/Xlib.h>
#elif defined(LAK_USE_XCB)
#  include <xcb/xcb.h>
#elif defined(LAK_USE_SDL)
#  include <SDL/SDL.h>
#else
#  error "No implementation specified"
#endif

#include "lak/array.hpp"
#include "lak/colour.hpp"
#include "lak/defer.hpp"
#include "lak/window.hpp"

#if 0
namespace lak
{
  template<typename COLOUR>
  struct surface
  {
    static_assert(lak::is_colour_v<COLOUR>);
    using value_type = COLOUR;

    lak::image_view<COLOUR> _pixels;

#  if defined(LAK_USE_WINAPI)
    lak::image<COLOUR, true> _platform_handle;
#  elif defined(LAK_USE_XLIB)
#    error "NYI"
#  elif defined(LAK_USE_XCB)
#    error "NYI"
#  elif defined(LAK_USE_SDL)
#  else
#    error "No implementation specified"
#  endif

    operator bool() const { return _platform_handle != NULL; }

    surface() = default;

    surface(const lak::platform_instance &instance,
            const lak::window_handle *handle,
            lak::vec2s_t size)
    {
      init(instance, handle, size);
    }

    void init(const lak::platform_instance &instance,
              const lak::window_handle *handle,
              lak::vec2s_t size)
    {
      clear();

#  if defined(LAK_USE_WINAPI)
      // WinAPI bitmap scanlines are padded to end on a LONG data-type boundary

      /*
      unless otherwise stated, bmiColors should be empty and biCompression
      should not be BI_BITFIELDS.

      biBitCount =
      0:
        JPEG or PNG
      1: (lak::colour::i1)
        monochrome indexed (bmiColors is palette)
      4: (lak::colour::i4)
        16 colours indexed (bmiColors is palette) high nyble is first pixel,
        low nyble is second pixel.
      8: (lak::colour::i8)
        256 colours indexed (bmiColors is palette)
      16:
        biCompression =
        BI_RGB: (lak::colour::le::bgrx5551)
          colour is LE bgrx5551
        BI_BITFIELDS:
          bmiColors defines bitmasks (3 DWORDs: R, G, B)
      24: (lak::colour::le::bgr888)
        2^24 colours, LE bgr888
      32:
        biCompression =
        BI_RGB: (lak::colour::le::bgrx8888)
          colour is LE bgrx8888
        BI_BITFIELDS:
          bmiColors defines bitmasks (3 DWORDs: R, G, B)
      */

      if constexpr (lak::colour::is_indexed_v<COLOUR>)
      {
        if constexpr (lak::is_same_v<COLOUR, lak::colour::i8>)
        {
          _bitmapinfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) +
                                             (256 * sizeof(RGBQUAD)));

          lak::bzero(&_bitmapinfo->bmiHeader);

          _bitmapinfo->bmiHeader.biBitCount    = 8;
          _bitmapinfo->bmiHeader.biCompression = BI_RGB;
          _bitmapinfo->bmiHeader.biSizeImage =
            size.x * size.y * sizeof(COLOUR);

          auto palette = lak::span<RGBQUAD, 256>(_bitmapinfo->bmiColors);

          constexpr auto xterm_palette =
            lak::xterm_palette<lak::colour::bgr888>();

          static_assert(palette.size_byes() ==
                        xterm_palette.size() * sizeof(xterm_palette[0]));

          std::memcpy(
            palette.data(), xterm_palette.data(), palette.size_bytes());
        }
        else
        {
          ASSERT_NYI();
          /*
          static_assert(lak::is_same_v<COLOUR, lak::colour::i1> ||
                        lak::is_same_v<COLOUR, lak::colour::i4>);
          */
        }
      }
      else if constexpr (lak::is_same_v<COLOUR, lak::colour::bgrx5551> ||
                         lak::is_same_v<COLOUR, lak::colour::bgr888> /*||
                         lak::is_same_v<COLOUR, lak::colour::bgrx8888> */)
      {
        _bitmapinfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO));

        lak::bzero(&_bitmapinfo->bmiHeader);

        _bitmapinfo->bmiHeader.biBitCount    = sizeof(COLOUR);
        _bitmapinfo->bmiHeader.biCompression = BI_RGB;
        _bitmapinfo->bmiHeader.biSizeImage = size.x * size.y * sizeof(COLOUR);
      }
      else
      {
        ASSERT_NYI();
      }

      _bitmapinfo->bmiHeader.biXPelsPerMeter = 2835 /* ~72 dpi */;
      _bitmapinfo->bmiHeader.biYPelsPerMeter = 2835 /* ~72 dpi */;
      _bitmapinfo->bmiHeader.biSize          = sizeof(_bitmapinfo->bmiHeader);
      _bitmapinfo->bmiHeader.biWidth         = size.x;
      _bitmapinfo->bmiHeader.biHeight        = -(LONG)size.y; // - = top-down
      _bitmapinfo->bmiHeader.biPlanes        = 1; // should always be 1
      // number of colours in the lookup table that are actually used by
      // indexed colours (0 means 2^biBitCount)
      _bitmapinfo->bmiHeader.biClrUsed = 0;
      // number of colours "required" for displaying the bitmap (0 means all)
      _bitmapinfo->bmiHeader.biClrImportant = 0;

      COLOUR *pixels;
      _platform_handle = ::CreateDIBSection(
        NULL, // handle->_device_context,
        NULL, // _bitmapinfo,
        lak::colour::is_indexed_v<COLOUR> ? DIB_PAL_COLORS : DIB_RGB_COLORS,
        (void **)&pixels,
        NULL,
        0 /* ignored because hSection is NULL */);

      _pixels = lak::image_view<COLOUR>(
        lak::span<COLOUR>(pixels, size.x * size.y), size);

#  elif defined(LAK_USE_XLIB)
#    error "NYI"
#  elif defined(LAK_USE_XCB)
#    error "NYI"
#  elif defined(LAK_USE_SDL)
#    error "NYI"
#  else
#    error "No implementation specified"
#  endif
    }

    void clear()
    {
#  if defined(LAK_USE_WINAPI)
      if (_platform_handle) ASSERT(DeleteObject(_platform_handle));
      if (_bitmapinfo) free(_bitmapinfo);
#  elif defined(LAK_USE_XLIB)
#    error "NYI"
#  elif defined(LAK_USE_XCB)
#    error "NYI"
#  elif defined(LAK_USE_SDL)
#    error "NYI"
#  else
#    error "No implementation specified"
#  endif
    }

    ~surface() { clear(); }

    lak::image_view<COLOUR> data() { return _pixels; }

    lak::image_view<const COLOUR> data() const { return _pixels; }
  };
}
#endif

#endif