#include "impl.hpp"

#ifdef LAK_ENABLE_COBALT
#include "lak/system/cobalt/context.hpp"

#import <AppKit/AppKit.h>

lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>
lak::cobalt_window_info(const lak::window_handle *w)
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(w->sdl_window, &info);

	if (info.subsystem == SDL_SYSWM_COCOA)
	{
		return lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>(
		  new ::cobalt::graphics::WindowInfoAppKit(
		    (__bridge void*)info.info.cocoa.window.contentView,
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
