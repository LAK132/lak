#include "platform.hpp"

#include <GL/GL.h>
#include <GL/gl3w.h>

#include "lak/debug.hpp"
#include "lak/defer.hpp"
#include "lak/image.hpp"
#include "lak/window.hpp"

void win32_error_popup(LPWSTR lpszFunction);

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

lak::window_handle *lak::create_window(const lak::platform_instance &instance,
                                       const lak::software_settings &settings)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(instance, handle.get()));

	handle->_instance = &instance;

	DWORD style = WS_OVERLAPPEDWINDOW;

	// CS_OWNDC means that each window has its own unique HDC that doesn't need
	// to be released.
	ASSERT((instance.window_class.style & CS_OWNDC) != 0);

	handle->_platform_handle =
	  ::CreateWindowExW(0,                                   /* styles */
	                    instance.window_class.lpszClassName, /* class name */
	                    L"insert window name here",          /* window name */
	                    style,                               /* style */
	                    CW_USEDEFAULT,                       /* x */
	                    CW_USEDEFAULT,                       /* y */
	                    720,                                 /* width */
	                    480,                                 /* height */
	                    nullptr,                             /* parent */
	                    nullptr,                             /* menu */
	                    instance.handle,                     /* hInstance */
	                    handle.get()                         /* user data */
	  );

	if (!handle->_platform_handle)
	{
		win32_error_popup(L"CreateWindowExW");
		ERROR("Failed to create window");
		return nullptr;
	}

	handle->_device_context = ::GetDC(handle->_platform_handle);

	ASSERT_NOT_EQUAL(
	  ::GetDeviceCaps(handle->_device_context, RASTERCAPS) & RC_BITBLT, 0);

	if (!handle->_device_context)
	{
		win32_error_popup(L"GetDC");
		ERROR("Failed to get window device context");
		return nullptr;
	}

	// this is also touched in handle_size_move
	auto &context  = handle->_context.emplace<lak::software_context>();
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

	return handle.release();
}

lak::window_handle *lak::create_window(const lak::platform_instance &instance,
                                       const lak::opengl_settings &settings)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(instance, handle.get()));

	handle->_instance = &instance;

	DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	// CS_OWNDC means that each window has its own unique HDC that doesn't need
	// to be released.
	ASSERT((instance.window_class.style & CS_OWNDC) != 0);

	handle->_platform_handle =
	  ::CreateWindowExW(0,                                   /* styles */
	                    instance.window_class.lpszClassName, /* class name */
	                    L"insert window name here",          /* window name */
	                    style,                               /* style */
	                    CW_USEDEFAULT,                       /* x */
	                    CW_USEDEFAULT,                       /* y */
	                    720,                                 /* width */
	                    480,                                 /* height */
	                    nullptr,                             /* parent */
	                    nullptr,                             /* menu */
	                    instance.handle,                     /* hInstance */
	                    handle.get()                         /* user data */
	  );

	if (!handle->_platform_handle)
	{
		win32_error_popup(L"CreateWindowExW");
		ERROR("Failed to create window");
		return nullptr;
	}

	handle->_device_context = ::GetDC(handle->_platform_handle);

	if (!handle->_device_context)
	{
		win32_error_popup(L"GetDC");
		ERROR("Failed to get window device context");
		return nullptr;
	}

	PIXELFORMATDESCRIPTOR format = {};
	format.nSize                 = sizeof(PIXELFORMATDESCRIPTOR);
	format.nVersion              = 1;
	format.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	format.iPixelType   = PFD_TYPE_RGBA;
	format.cColorBits   = 32;
	format.cDepthBits   = 24;
	format.cStencilBits = 8;
	format.iLayerType   = PFD_MAIN_PLANE;

	int iformat = ::ChoosePixelFormat(handle->_device_context, &format);

	if (!iformat)
	{
		win32_error_popup(L"ChoosePixelFormat");
		ERROR("Failed to choose pixel format");
		return nullptr;
	}

	::SetPixelFormat(handle->_device_context, iformat, &format);

	auto &context           = handle->_context.emplace<lak::opengl_context>();
	context.platform_handle = ::wglCreateContext(handle->_device_context);

	if (!context.platform_handle)
	{
		win32_error_popup(L"wglCreateContext");
		ERROR("Failed to create OpenGL context");
		return nullptr;
	}

	if (!::wglMakeCurrent(handle->_device_context, context.platform_handle))
	{
		win32_error_popup(L"wglMakeCurrent");
		ERROR("Failed to make OpenGL context current");
		return nullptr;
	}

	::ShowWindow(handle->_platform_handle, SW_SHOWNORMAL);

	return handle.release();
}

lak::window_handle *lak::create_window(const lak::platform_instance &instance,
                                       const lak::vulkan_settings &settings)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(instance, handle.get()));

	handle->_instance = &instance;

	DWORD style = WS_OVERLAPPEDWINDOW;

	// CS_OWNDC means that each window has its own unique HDC that doesn't need
	// to be released.
	ASSERT((instance.window_class.style & CS_OWNDC) != 0);

	handle->_platform_handle =
	  ::CreateWindowExW(0,                                   /* styles */
	                    instance.window_class.lpszClassName, /* class name */
	                    L"insert window name here",          /* window name */
	                    style,                               /* style */
	                    CW_USEDEFAULT,                       /* x */
	                    CW_USEDEFAULT,                       /* y */
	                    720,                                 /* width */
	                    480,                                 /* height */
	                    nullptr,                             /* parent */
	                    nullptr,                             /* menu */
	                    instance.handle,                     /* hInstance */
	                    handle.get()                         /* user data */
	  );

	if (!handle->_platform_handle)
	{
		win32_error_popup(L"CreateWindowExW");
		ERROR("Failed to create window");
		return nullptr;
	}

	handle->_device_context = ::GetDC(handle->_platform_handle);

	if (!handle->_device_context)
	{
		win32_error_popup(L"GetDC");
		ERROR("Failed to get window device context");
		return nullptr;
	}

	// auto &context = handle->_context.emplace<lak::vulkan_context>();

	::ShowWindow(handle->_platform_handle, SW_SHOWNORMAL);

	return handle.release();
}

bool lak::destroy_window(const lak::platform_instance &instance,
                         lak::window_handle *handle)
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
		case lak::graphics_mode::Software:
		{
		}
		break;

		case lak::graphics_mode::OpenGL:
		{
			auto &context = handle->opengl_context();
			if (context.platform_handle)
			{
				// wglMakeCurrent(context.device_context, NULL) isn't needed here as we
				// aren't crossing threads.
				::wglDeleteContext(context.platform_handle);
			}
		}
		break;
	}

	handle->_context.emplace<std::monostate>();
	handle->_platform_handle = NULL;
	handle->_instance        = nullptr;

	lak::bank<lak::window_handle>::destroy(handle);

	return true;
}

/* --- OpenGL --- */

// :TODO: Actually put this in a header somewhere.
bool lak::set_opengl_swap_interval(const lak::opengl_context &context,
                                   int interval)
{
	return has_swap_control && wglSwapIntervalEXT(interval);
}

/* --- Window helper functions --- */

lak::wstring lak::window_title(const lak::platform_instance &instance,
                               const lak::window_handle *handle)
{
	std::vector<wchar_t> str;
	str.resize(::GetWindowTextLengthW(handle->_platform_handle) + 1);
	::GetWindowTextW(handle->_platform_handle, str.data(), str.size());
	return lak::wstring(str.begin(), str.end());
}

bool lak::set_window_title(const lak::platform_instance &instance,
                           lak::window_handle *handle,
                           const lak::wstring &str)
{
	return ::SetWindowTextW(handle->_platform_handle, str.c_str());
}

lak::vec2l_t lak::window_size(const lak::platform_instance &instance,
                              const lak::window_handle *handle)
{
	RECT rect;
	ASSERT(::GetWindowRect(handle->_platform_handle, &rect));
	return lak::vec2l_t{std::max(0L, rect.right - rect.left),
	                    std::max(0L, rect.bottom - rect.top)};
}

lak::vec2l_t lak::window_drawable_size(const lak::platform_instance &instance,
                                       const lak::window_handle *handle)
{
	RECT rect;
	ASSERT(::GetClientRect(handle->_platform_handle, &rect));
	return lak::vec2l_t{std::max(0L, rect.right - rect.left),
	                    std::max(0L, rect.bottom - rect.top)};
}

bool lak::set_window_size(const lak::platform_instance &instance,
                          lak::window_handle *handle,
                          lak::vec2l_t size)
{
	RECT rect;
	return ::GetWindowRect(handle->_platform_handle, &rect) &&
	       ::MoveWindow(handle->_platform_handle,
	                    rect.left,
	                    rect.top,
	                    size.x,
	                    size.y,
	                    TRUE);
}

bool lak::set_window_cursor_pos(const lak::platform_instance &instance,
                                const lak::window_handle *handle,
                                lak::vec2l_t pos)
{
	ASSERT(pos.x < INT_MAX && pos.x > INT_MIN);
	ASSERT(pos.y < INT_MAX && pos.y > INT_MIN);
	POINT relative = {(int)pos.x, (int)pos.y};
	return ::ClientToScreen(handle->_platform_handle, &relative) &&
	       ::SetCursorPos(relative.x, relative.y);
}

bool lak::swap_window(const lak::platform_instance &instance,
                      lak::window_handle *handle)
{
	switch (handle->graphics_mode())
	{
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

		case lak::graphics_mode::OpenGL:
		{
			return ::SwapBuffers(handle->_device_context);
		}

		default: return false;
	}
}
