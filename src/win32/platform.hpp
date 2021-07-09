#ifndef LAK_WIN32_PLATFORM_HPP
#define LAK_WIN32_PLATFORM_HPP

#include "lak/windows.hpp"
#include <windowsx.h>
#include <wingdi.h>

typedef const char *(WINAPI *PFNWGLGETEXTENSIONSSTRINGARB)(HDC hdc);
PFNWGLGETEXTENSIONSSTRINGARB wglGetExtensionsStringARB = nullptr;

bool has_pixel_format = false;
typedef BOOL(WINAPI *PFNWGLCHOOSEPIXELFORMATARB)(HDC hdc,
                                                 const INT *piAttribIList,
                                                 const FLOAT *pfAttribFList,
                                                 UINT nMaxFormats,
                                                 INT *piFormats,
                                                 UINT *nNumFormats);
PFNWGLCHOOSEPIXELFORMATARB wglChoosePixelFormatARB = nullptr;

typedef BOOL(WINAPI *PFNWGLGETPIXELFORMATATTRIBIVARB)(HDC hdc,
                                                      INT iPixelFormat,
                                                      INT iLayerPlane,
                                                      UINT nAttributes,
                                                      const INT *piAttributes,
                                                      INT *piValues);
PFNWGLGETPIXELFORMATATTRIBIVARB wglGetPixelFormatAttribivARB = nullptr;

bool has_swap_control      = false;
bool has_swap_control_tear = false;
typedef BOOL(WINAPI *PFNWGLGETPIXELFORMATATTRIBFVARB)(HDC hdc,
                                                      INT iPixelFormat,
                                                      INT iLayerPlane,
                                                      UINT nAttributes,
                                                      const INT *piAttributes,
                                                      FLOAT *pfValues);
PFNWGLGETPIXELFORMATATTRIBFVARB wglGetPixelFormatAttribfvARB = nullptr;

typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXT)(INT);
PFNWGLSWAPINTERVALEXT wglSwapIntervalEXT = nullptr;

typedef INT(WINAPI *PFNWGLGETSWAPINTERVALEXT)();
PFNWGLGETSWAPINTERVALEXT wglGetSwapIntervalEXT = nullptr;

#endif