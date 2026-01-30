#include "lak/system/win32/wrapper.hpp"
#include "lak/system/windowing/events.hpp"
#include "lak/system/windowing/platform.hpp"
#include "lak/system/windowing/window.hpp"
#include "lak/unique_ptr.hpp"
#include "lak/variant.hpp"

#ifdef LAK_ENABLE_COBALT
#	include "lak/system/cobalt/context.hpp"
#	include "lak/system/cobalt/result.hpp"

#	include <RendererInterface/WindowSystemInfoWin32.h>
#	include <RendererInterface/WindowInfoWin32.h>
#endif

namespace lak
{
	struct platform_instance
	{
		HINSTANCE handle;
		WNDCLASSW window_class;
		ATOM window_class_atom;
		mutable bool opengl_initialised = false;
		mutable lak::buffer<MSG, 0x100> platform_events;
	};

	extern lak::platform_instance *_platform_instance;

	struct cursor
	{
		HCURSOR platform_handle;
	};

	struct software_context
	{
#ifdef LAK_ENABLE_SOFTRENDER
		mutable lak::image<lak::colour::bgrx8888> platform_handle;
#endif
	};

	struct opengl_context
	{
#ifdef LAK_ENABLE_OPENGL
		HGLRC platform_handle = NULL;
#endif
	};

	struct cobalt_context
	{
#ifdef LAK_ENABLE_COBALT
		lak::unique_ptr<lak::cobalt::graphics_context> platform_handle;
#endif
	};

	using graphics_context = lak::variant<std::monostate,
	                                      lak::software_context,
	                                      lak::opengl_context,
	                                      lak::cobalt_context>;

	struct window_handle
	{
		HWND _platform_handle = NULL;
		HDC _device_context   = NULL;

		mutable bool _moving   = false;
		mutable bool _resizing = false;
		mutable bool _hovered  = false;
		mutable POINT _cursor_start;
		mutable RECT _window_start;
		enum
		{
			top    = 1 << 1,
			bottom = 1 << 2,
			left   = 1 << 3,
			right  = 1 << 4
		};
		mutable unsigned char _side = 0;

		lak::graphics_context gc;

		inline lak::graphics_mode graphics_mode() const
		{
			switch (gc.index())
			{
				case 1:  return lak::graphics_mode::Software;
				case 2:  return lak::graphics_mode::OpenGL;
				case 3:  return lak::graphics_mode::Cobalt;
				default: FATAL("Invalid graphics mode"); [[fallthrough]];
				case 0:  return lak::graphics_mode::None;
			}
		}

		inline const lak::software_context &software_context() const
		{
			ASSERT(gc.template holds<lak::software_context>());
			return *gc.template get<lak::software_context>();
		}

		inline const lak::opengl_context &opengl_context() const
		{
			ASSERT(gc.template holds<lak::opengl_context>());
			return *gc.template get<lak::opengl_context>();
		}

		inline const lak::cobalt_context &cobalt_context() const
		{
			ASSERT(gc.template holds<lak::cobalt_context>());
			return *gc.template get<lak::cobalt_context>();
		}
	};

	struct platform_event
	{
		MSG msg;
	};
}

#ifdef LAK_ENABLE_OPENGL
typedef const char *(WINAPI *PFNWGLGETEXTENSIONSSTRINGARB)(HDC hdc);
extern PFNWGLGETEXTENSIONSSTRINGARB wglGetExtensionsStringARB;

extern bool has_pixel_format;
typedef BOOL(WINAPI *PFNWGLCHOOSEPIXELFORMATARB)(HDC hdc,
                                                 const INT *piAttribIList,
                                                 const FLOAT *pfAttribFList,
                                                 UINT nMaxFormats,
                                                 INT *piFormats,
                                                 UINT *nNumFormats);
extern PFNWGLCHOOSEPIXELFORMATARB wglChoosePixelFormatARB;

typedef BOOL(WINAPI *PFNWGLGETPIXELFORMATATTRIBIVARB)(HDC hdc,
                                                      INT iPixelFormat,
                                                      INT iLayerPlane,
                                                      UINT nAttributes,
                                                      const INT *piAttributes,
                                                      INT *piValues);
extern PFNWGLGETPIXELFORMATATTRIBIVARB wglGetPixelFormatAttribivARB;

extern bool has_swap_control;
extern bool has_swap_control_tear;
typedef BOOL(WINAPI *PFNWGLGETPIXELFORMATATTRIBFVARB)(HDC hdc,
                                                      INT iPixelFormat,
                                                      INT iLayerPlane,
                                                      UINT nAttributes,
                                                      const INT *piAttributes,
                                                      FLOAT *pfValues);
extern PFNWGLGETPIXELFORMATATTRIBFVARB wglGetPixelFormatAttribfvARB;

typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXT)(INT);
extern PFNWGLSWAPINTERVALEXT wglSwapIntervalEXT;

typedef INT(WINAPI *PFNWGLGETSWAPINTERVALEXT)();
extern PFNWGLGETSWAPINTERVALEXT wglGetSwapIntervalEXT;
#endif