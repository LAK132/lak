// begin weird include ordering
#ifdef LAK_ENABLE_OPENGL
#	include "lak/system/opengl/gl3w.hpp"

#	include <GL/GL.h>
#endif

#include "lak/system/win32/windows.hpp"
// end weird include ordering

#include "lak/debug.hpp"
#include "lak/defer.hpp"
#include "lak/image.hpp"
#include "lak/result.hpp"

#include "lak/string_literals/string.hpp"
#include "lak/string_literals/view.hpp"

#include "impl.hpp"

void win32_error_popup(LPCWSTR lpszFunction);
lak::wstring win32_error_string(LPCWSTR lpszFunction);

// template<typename COLOUR>
// void init_bitmap_info(lak::software_context &context)
// {
//   if constexpr (lak::colour::is_indexed_v<COLOUR>)
//   {
//     constexpr auto max_index =
//       decltype(std::declval<COLOUR &>().I())::max_value;
//     static_assert(max_index == 0xFF);
//     constexpr size_t size = sizeof(BITMAPINFO) + (sizeof(RGBQUAD) *
//     max_index); context.bitmap_info   = static_cast<BITMAPINFO
//     *>(malloc(size)); std::memset(context.bitmap_info, 0, size);
//     context.bitmap_info->bmiHeader.biClrUsed = max_index;
//   }
//   else
//   {
//     static_assert(sizeof(COLOUR) == 2 || sizeof(COLOUR) == 4);
//     context.bitmap_info =
//       static_cast<BITMAPINFO *>(malloc(sizeof(BITMAPINFO)));
//     lak::bzero(context.bitmap_info);
//     context.bitmap_info->bmiHeader.biClrUsed = 0;
//   }
// }

#ifdef LAK_ENABLE_SOFTRENDER
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::software_settings &)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(handle.release()));

	DWORD style = WS_OVERLAPPEDWINDOW;

	// CS_OWNDC means that each window has its own unique HDC that doesn't need
	// to be released.
	ASSERT((lak::_platform_instance->window_class.style & CS_OWNDC) != 0);

	handle->_platform_handle = ::CreateWindowExW(
	  0,                                                   /* styles */
	  lak::_platform_instance->window_class.lpszClassName, /* class name */
	  L"insert window name here",                          /* window name */
	  style,                                               /* style */
	  CW_USEDEFAULT,                                       /* x */
	  CW_USEDEFAULT,                                       /* y */
	  720,                                                 /* width */
	  480,                                                 /* height */
	  nullptr,                                             /* parent */
	  nullptr,                                             /* menu */
	  lak::_platform_instance->handle,                     /* hInstance */
	  handle.get()                                         /* user data */
	);

	if (!handle->_platform_handle)
	{
		return lak::err_t{lak::fmt<u8"Failed to create window: {}">(
		  win32_error_string(L"CreateWindowExW"))};
	}

	handle->_device_context = ::GetDC(handle->_platform_handle);

	ASSERT_NOT_EQUAL(
	  ::GetDeviceCaps(handle->_device_context, RASTERCAPS) & RC_BITBLT, 0);

	if (!handle->_device_context)
	{
		return lak::err_t{lak::fmt<u8"Failed to get window device context: {}">(
		  win32_error_string(L"GetDC"))};
	}

	// this is also touched in handle_size_move
	auto &context  = handle->gc.emplace<lak::software_context>();
	using colour_t = typename decltype(context.platform_handle)::value_type;
	context.platform_handle.resize({720, 480});
	ASSERT(context.platform_handle.contig_size_bytes() > 0);

	// static_assert(lak::is_same_v<colour_t, lak::colour::i8> ||
	//               lak::is_same_v<colour_t, lak::colour::rgb555>);

	// if constexpr (lak::colour::is_indexed_v<colour_t>)
	// {
	//   constexpr auto max_index =
	//     decltype(std::declval<colour_t &>().I())::max_value;
	//   static_assert(max_index == 0xFF);
	//   constexpr size_t size = sizeof(BITMAPINFO) + (sizeof(RGBQUAD) *
	//   max_index); context.bitmap_info   = static_cast<BITMAPINFO
	//   *>(malloc(size)); std::memset(context.bitmap_info, 0, size);

	//   auto &header     = context.bitmap_info->bmiHeader;
	//   header.biClrUsed = max_index;
	// }
	// else
	// {
	//   static_assert(sizeof(colour_t) == 2 || sizeof(colour_t) == 4);
	//   context.bitmap_info =
	//     static_cast<BITMAPINFO *>(malloc(sizeof(BITMAPINFO)));
	//   lak::bzero(context.bitmap_info);

	//   auto &header     = context.bitmap_info->bmiHeader;
	//   header.biClrUsed = 0;
	// }
	// init_bitmap_info<colour_t>();
	// auto &header         = context.bitmap_info->bmiHeader;
	// header.biSize        = sizeof(header);
	// header.biWidth       = ;
	// header.biHeight      = ;
	// header.biPlanes      = 1;
	// header.biBitCount    = ;
	// header.biCompression = BI_BITFIELDS;
	// header.biSizeImage   = context.platform_handle.contig_size_bytes();
	// // header.biXPelsPerMeter = 0;
	// // header.biYPelsPerMeter = 0;
	// // header.biClrImportant = 0;

	::ShowWindow(handle->_platform_handle, SW_SHOWNORMAL);

	return lak::ok_t{handle.release()};
}
#endif

#ifdef LAK_ENABLE_OPENGL
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::opengl_settings &settings)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(handle.release()));

	DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	// CS_OWNDC means that each window has its own unique HDC that doesn't need
	// to be released.
	ASSERT((lak::_platform_instance->window_class.style & CS_OWNDC) != 0);

	handle->_platform_handle = ::CreateWindowExW(
	  0,                                                   /* styles */
	  lak::_platform_instance->window_class.lpszClassName, /* class name */
	  L"insert window name here",                          /* window name */
	  style,                                               /* style */
	  CW_USEDEFAULT,                                       /* x */
	  CW_USEDEFAULT,                                       /* y */
	  720,                                                 /* width */
	  480,                                                 /* height */
	  nullptr,                                             /* parent */
	  nullptr,                                             /* menu */
	  lak::_platform_instance->handle,                     /* hInstance */
	  handle.get()                                         /* user data */
	);

	if (!handle->_platform_handle)
	{
		return lak::err_t{lak::fmt<u8"Failed to create window: {}">(
		  win32_error_string(L"CreateWindowExW"))};
	}

	handle->_device_context = ::GetDC(handle->_platform_handle);

	if (!handle->_device_context)
	{
		return lak::err_t{lak::fmt<u8"Failed to get window device context: {}">(
		  win32_error_string(L"GetDC"))};
	}

	PIXELFORMATDESCRIPTOR format = {};
	format.nSize                 = sizeof(PIXELFORMATDESCRIPTOR);
	format.nVersion              = 1;
	format.dwFlags               = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
	                 (settings.double_buffered ? PFD_DOUBLEBUFFER : 0);
	format.iPixelType   = PFD_TYPE_RGBA;
	format.cColorBits   = settings.colour_size * 4;
	format.cDepthBits   = settings.depth_size;
	format.cStencilBits = settings.stencil_size;
	format.iLayerType   = PFD_MAIN_PLANE;

	int iformat = ::ChoosePixelFormat(handle->_device_context, &format);

	if (!iformat)
	{
		return lak::err_t{lak::fmt<u8"Failed to choose pixel format: {}">(
		  win32_error_string(L"ChoosePixelFormat"))};
	}

	::SetPixelFormat(handle->_device_context, iformat, &format);

	auto &context           = handle->gc.emplace<lak::opengl_context>();
	context.platform_handle = ::wglCreateContext(handle->_device_context);
	// :TODO: check context version against settings version

	if (!context.platform_handle)
	{
		return lak::err_t{lak::fmt<u8"Failed to create OpenGL context: {}">(
		  win32_error_string(L"wglCreateContext"))};
	}

	if (!::wglMakeCurrent(handle->_device_context, context.platform_handle))
	{
		return lak::err_t{lak::fmt<u8"Failed to make OpenGL context current: {}">(
		  win32_error_string(L"wglMakeCurrent"))};
	}

	::ShowWindow(handle->_platform_handle, SW_SHOWNORMAL);

	return lak::ok_t{handle.release()};
}
#endif

#ifdef LAK_ENABLE_COBALT
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::cobalt_settings &s, const lak::cobalt_renderer_settings &r)
{
	ASSERT((r.device) || (r.device_enumerator));
	auto device =
	  r.device ? r.device : r.device_enumerator->GetPreferredDevice();
	if (!device)
	{
		return lak::err_t{u8"Failed to get preferred graphics device"_str};
	}

	auto renderer = device->CreateRenderer(r.features, r.options);

	auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
	ASSERT(handle);

	if (auto window_system_info = lak::cobalt_window_system_info(handle.get());
	    !renderer->Initialize(*window_system_info))
	{
		return lak::err_t{u8"Failed to initialise cobalt renderer"_str};
	}

	DEFER(if (handle) lak::destroy_window(handle.release()));

	DWORD style = WS_OVERLAPPEDWINDOW;

	// CS_OWNDC means that each window has its own unique HDC that doesn't need
	// to be released.
	ASSERT((lak::_platform_instance->window_class.style & CS_OWNDC) != 0);

	::cobalt::graphics::V2UInt32 window_size = {720, 480};

	handle->_platform_handle = ::CreateWindowExW(
	  0,                                                   /* styles */
	  lak::_platform_instance->window_class.lpszClassName, /* class name */
	  L"insert window name here",                          /* window name */
	  style,                                               /* style */
	  CW_USEDEFAULT,                                       /* x */
	  CW_USEDEFAULT,                                       /* y */
	  window_size.X(),                                     /* width */
	  window_size.Y(),                                     /* height */
	  nullptr,                                             /* parent */
	  nullptr,                                             /* menu */
	  lak::_platform_instance->handle,                     /* hInstance */
	  handle.get()                                         /* user data */
	);

	if (!handle->_platform_handle)
	{
		return lak::err_t{lak::fmt<u8"Failed to create window: {}">(
		  win32_error_string(L"CreateWindowExW"))};
	}

	handle->_device_context = ::GetDC(handle->_platform_handle);

	if (!handle->_device_context)
	{
		return lak::err_t{lak::fmt<u8"Failed to get window device context: {}">(
		  win32_error_string(L"GetDC"))};
	}

	auto &context = handle->gc.emplace<lak::cobalt_context>();
	auto fb       = renderer->CreateFrameBuffer();
	if (!fb)
	{
		return lak::err_t{u8"Failed to create framebuffer"_str};
	}

	auto window_info = lak::cobalt_window_info(handle.get());
	RES_TRY(lak::cobalt::as_result(
	          fb->BindWindow(*window_info, s.depth_mode, s.colour_mode))
	          .map_err([](auto &&) { return u8"Failed to bind window"_str; }));

	context.platform_handle             = new lak::cobalt::graphics_context{};
	context.platform_handle->api_family = r.renderer_plugin.GetApiFamily();
	context.platform_handle->api_version =
	  r.renderer_plugin.GetTargetApiVersion();
	context.platform_handle->vendor       = device->GetVendor();
	context.platform_handle->frame_buffer = lak::move(fb);
	context.platform_handle->renderer     = lak::move(renderer);

	::ShowWindow(handle->_platform_handle, SW_SHOWNORMAL);

	return lak::ok_t{handle.release()};
}
#endif

bool lak::destroy_window(lak::window_handle *handle)
{
	ASSERT(handle);

	if (!::DestroyWindow(handle->_platform_handle))
	{
		win32_error_popup(L"DestroyWindow");
		ERROR("Failed to destroy window");
		return false;
	}

	switch (handle->graphics_mode())
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
		{
		}
		break;
#endif

#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			auto &context = handle->opengl_context();
			if (context.platform_handle)
			{
				// wglMakeCurrent(context.device_context, NULL) isn't needed here as
				// we aren't crossing threads.
				::wglDeleteContext(context.platform_handle);
			}
		}
		break;
#endif

#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
		{
			handle->gc.template get<lak::cobalt_context>()->platform_handle.reset();
		}
		break;
#endif

		default:
		{
			ASSERT_UNREACHABLE();
		}
		break;
	}

	handle->gc.emplace<lak::monostate>();
	handle->_platform_handle = NULL;

	lak::bank<lak::window_handle>::destroy(handle);

	return true;
}

/* --- graphics control --- */

lak::graphics_mode lak::window_graphics_mode(const lak::window_handle *w)
{
	return w->graphics_mode();
}

/* --- OpenGL --- */

#ifdef LAK_ENABLE_OPENGL
bool lak::set_opengl_swap_interval(const lak::opengl_context &,
                                   lak::window_swap_interval interval)
{
	if (!has_swap_control) return false;
	switch (interval)
	{
		case lak::window_swap_interval::vsync_adaptive:
			if (!has_swap_control_tear) return false;
			[[fallthrough]];
		case lak::window_swap_interval::vsync_off: [[fallthrough]];
		case lak::window_swap_interval::vsync_on:
			return wglSwapIntervalEXT(static_cast<int>(interval));
		default: BOUNDS_ASSERT_UNREACHABLE(return false);
	}
}
#endif

/* --- Cobalt --- */

#ifdef LAK_ENABLE_COBALT
lak::unique_ptr<::cobalt::graphics::IRenderer::WindowSystemInfoBase>
lak::cobalt_window_system_info(const lak::window_handle *)
{
	return lak::unique_ptr<::cobalt::graphics::IRenderer::WindowSystemInfoBase>(
	  new ::cobalt::graphics::WindowSystemInfoWin32(),
	  [](auto *p)
	  { delete static_cast<::cobalt::graphics::WindowSystemInfoWin32 *>(p); });
}

lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>
lak::cobalt_window_info(const lak::window_handle *w)
{
	return lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>(
	  new ::cobalt::graphics::WindowInfoWin32(
	    w->_platform_handle,
	    lak::_platform_instance->handle,
	    lak::cobalt::from_lak(lak::vec2u32_t(lak::window_drawable_size(w)))),
	  [](auto *p)
	  { delete static_cast<::cobalt::graphics::WindowInfoWin32 *>(p); });
}
#endif

/* --- Window helper functions --- */

lak::wstring lak::window_title(const lak::window_handle *handle)
{
	std::vector<wchar_t> str;
	str.resize(::GetWindowTextLengthW(handle->_platform_handle) + 1);
	::GetWindowTextW(
	  handle->_platform_handle, str.data(), static_cast<int>(str.size()));
	return lak::wstring(str.begin(), str.end());
}

bool lak::set_window_title(lak::window_handle *handle, const lak::wstring &str)
{
	return ::SetWindowTextW(handle->_platform_handle, str.c_str());
}

lak::vec2l_t lak::window_size(const lak::window_handle *handle)
{
	RECT rect;
	ASSERT(::GetWindowRect(handle->_platform_handle, &rect));
	return lak::vec2l_t{std::max(0L, rect.right - rect.left),
	                    std::max(0L, rect.bottom - rect.top)};
}

lak::vec2l_t lak::window_drawable_size(const lak::window_handle *handle)
{
	RECT rect;
	ASSERT(::GetClientRect(handle->_platform_handle, &rect));
	return lak::vec2l_t{std::max(0L, rect.right - rect.left),
	                    std::max(0L, rect.bottom - rect.top)};
}

bool lak::set_window_size(lak::window_handle *handle, lak::vec2l_t size)
{
	RECT rect;
	if (::GetWindowRect(handle->_platform_handle, &rect) &&
	    ::MoveWindow(
	      handle->_platform_handle, rect.left, rect.top, size.x, size.y, TRUE))
	{
		lak::window_handle_resize(handle);
		return true;
	}
	return false;
}

bool lak::set_window_cursor_pos(const lak::window_handle *handle,
                                lak::vec2l_t pos)
{
	ASSERT(pos.x < INT_MAX && pos.x > INT_MIN);
	ASSERT(pos.y < INT_MAX && pos.y > INT_MIN);
	POINT relative = {(int)pos.x, (int)pos.y};
	return ::ClientToScreen(handle->_platform_handle, &relative) &&
	       ::SetCursorPos(relative.x, relative.y);
}

bool lak::set_active_window(const lak::window_handle *handle)
{
	switch (handle->graphics_mode())
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software: return true;
#endif

#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			if (!::wglMakeCurrent(handle->_device_context,
			                      handle->opengl_context().platform_handle))
			{
				// lak::streamify("Failed to make OpenGL context current: "_view,
				//                win32_error_string(L"wglMakeCurrent"));
				return false;
			}
			else
				return true;
		}
#endif

#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
		{
			return true;
		}
#endif
	}

	return false;
}

bool lak::swap_window(lak::window_handle *handle)
{
	switch (handle->graphics_mode())
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
		{
			return RedrawWindow(
			  handle->_platform_handle,
			  nullptr,
			  nullptr,
			  RDW_INTERNALPAINT /* trigger WM_PAINT */ |
			    RDW_INVALIDATE /* invalidate the region */ |
			    RDW_UPDATENOW /* handle WM_PAINT before RedrawWindow returns */);
		}
#endif

#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			return ::SwapBuffers(handle->_device_context);
		}
#endif

#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
		{
			auto &ctx = handle->cobalt_context();
			auto *rd  = ctx.platform_handle->renderer.get();
			rd->SetRenderPasses(ctx.platform_handle->render_passes.data(),
			                    ctx.platform_handle->render_passes.size());
			rd->StartNewFrame();
			rd->WaitForDrawComplete();
			rd->RemoveAllRenderPasses();
			ctx.platform_handle->render_passes.clear();
			ctx.platform_handle->owned_render_passes.clear();
			return true;
		}
#endif

		default: return false;
	}
}

void lak::window_handle_resize(const lak::window_handle *handle)
{
#ifdef LAK_ENABLE_SOFTRENDER
	if (handle->graphics_mode() == lak::graphics_mode::Software)
	{
		auto actual = lak::window_drawable_size(handle);
		handle->software_context().platform_handle.resize(lak::vec2s_t(actual));
	}
#endif

#ifdef LAK_ENABLE_COBALT
	if (handle->graphics_mode() == lak::graphics_mode::Cobalt)
	{
		auto actual = lak::window_drawable_size(handle);
		lak::cobalt::as_result(
		  handle->cobalt_context()
		    .platform_handle->frame_buffer->NotifyWindowResized(
		      {(uint32_t)actual.x, (uint32_t)actual.y}))
		  .IF_ERR_WARN("NotifyWindowResized failed");
	}
#endif
}

#include "../common/window.inl"
