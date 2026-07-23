#include "impl.hpp"

#ifdef LAK_ENABLE_COBALT
#include "lak/system/cobalt/context.hpp"

#import <AppKit/AppKit.h>

lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>
lak::cobalt_window_info(const lak::window_handle *w)
{
	if (std::strcmp(SDL_GetCurrentVideoDriver(), "cocoa") == 0)
	{
		return lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>(
		  new ::cobalt::graphics::WindowInfoAppKit(
		    (__bridge void *)SDL_GetPointerProperty(
		      SDL_GetWindowProperties(window),
		      SDL_PROP_WINDOW_COCOA_WINDOW_POINTER,
		      NULL),
		    lak::cobalt::from_lak(lak::vec2u32_t(lak::window_drawable_size(w)))),
		  [](auto *p)
		  { delete static_cast<::cobalt::graphics::WindowInfoAppKit *>(p); });
	}
	else
	{
		ASSERT_UNREACHABLE();
		return {};
	}
}

#endif
