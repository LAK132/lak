#include "lak/system/windowing/window.hpp"

#ifdef LAK_ENABLE_OPENGL
#	include "lak/system/opengl/gl3w.hpp"
#	include "lak/system/opengl/state.hpp"
#endif

#include "lak/system/os.hpp"

#include "lak/defer.hpp"
#include "lak/memmanip.hpp"

#include "lak/string_literals/string.hpp"

#include "impl.hpp"

#ifdef LAK_ENABLE_SOFTRENDER
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::software_settings &)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::make();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(handle.release()););

	handle->sdl_window = SDL_CreateWindow("",
	                                      SDL_WINDOWPOS_CENTERED,
	                                      SDL_WINDOWPOS_CENTERED,
	                                      720,
	                                      480,
	                                      SDL_WINDOW_RESIZABLE);

	if (!handle->sdl_window)
		return lak::err_t<lak::u8string>{
		  lak::fmt<u8"Failed to create window ({})">(SDL_GetError())};

	auto &context = handle->gc.emplace<lak::software_context>();

	context.sdl_surface = SDL_GetWindowSurface(handle->sdl_window);

	if (!context.sdl_surface)
		return lak::err_t<lak::u8string>{
		  lak::fmt<u8"Failed to get window surface ({})">(SDL_GetError())};

	context.sdl_window = handle->sdl_window;

	return lak::ok_t{handle.release()};
}
#endif

#ifdef LAK_ENABLE_OPENGL
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::opengl_settings &settings)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::make();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(handle.release()););

	// :TODO: SDL2 documentation says this should be called *after*
	// SDL_GL_SetAttribute but that's causing the screen to stay perminently
	// black? Do we need to create a "fake" context first, init gl3w, destroy the
	// context, then set attributes and create the real context?
	handle->sdl_window =
	  SDL_CreateWindow("",
	                   SDL_WINDOWPOS_CENTERED,
	                   SDL_WINDOWPOS_CENTERED,
	                   720,
	                   480,
	                   SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	if (!handle->sdl_window)
		return lak::err_t<lak::u8string>{
		  lak::fmt<u8"Failed to create window ({})">(SDL_GetError())};

	auto &context = handle->gc.emplace<lak::opengl_context>();

#	define SET_ATTRIB(A, B)                                                    \
		if (SDL_GL_SetAttribute(A, B))                                            \
		{                                                                         \
			return lak::err_t<lak::u8string>{                                       \
			  lak::streamify("Failed to set " #A " to " #B " (", B, ")")};          \
		}
	SET_ATTRIB(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
	SET_ATTRIB(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SET_ATTRIB(SDL_GL_DOUBLEBUFFER, settings.double_buffered);
	SET_ATTRIB(SDL_GL_DEPTH_SIZE, settings.depth_size);
	SET_ATTRIB(SDL_GL_RED_SIZE, settings.colour_size);
	SET_ATTRIB(SDL_GL_GREEN_SIZE, settings.colour_size);
	SET_ATTRIB(SDL_GL_BLUE_SIZE, settings.colour_size);
	SET_ATTRIB(SDL_GL_STENCIL_SIZE, settings.stencil_size);
	SET_ATTRIB(SDL_GL_CONTEXT_MAJOR_VERSION, settings.major);
	SET_ATTRIB(SDL_GL_CONTEXT_MINOR_VERSION, settings.minor);
#	undef SET_ATTRIB

	if (context.sdl_glcontext = SDL_GL_CreateContext(handle->sdl_window);
	    !context.sdl_glcontext)
		return lak::err_t<lak::u8string>{
		  u8"Failed to create an OpenGL context: "_str +
		  reinterpret_cast<const char8_t *>(SDL_GetError())};

	if (gl3wInit() != GL3W_OK)
		return lak::err_t<lak::u8string>{u8"Failed to initialise gl3w"_str};

	if_let_err (const auto err, lak::opengl::get_error())
		return lak::err_t<lak::u8string>{
		  lak::streamify("OpenGL in bad state ("_view, err, ")"_view)};

	if (SDL_GL_MakeCurrent(handle->sdl_window, context.sdl_glcontext) != 0)
		return lak::err_t<lak::u8string>{
		  u8"Failed to make context current for window: "_str +
		  reinterpret_cast<const char8_t *>(SDL_GetError())};

	if (SDL_GL_SetSwapInterval(-1) != 0 && SDL_GL_SetSwapInterval(1) != 0 &&
	    SDL_GL_SetSwapInterval(0) != 0)
		return lak::err_t<lak::u8string>{
		  u8"Failed to set swap interval: "_str +
		  reinterpret_cast<const char8_t *>(SDL_GetError())};

	context.sdl_window = handle->sdl_window;

	return lak::ok_t{handle.release()};
}
#endif

#ifdef LAK_ENABLE_COBALT
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::cobalt_settings &settings,
  const lak::cobalt_renderer_settings &rsettings)
{
	ASSERT((rsettings.device) || (rsettings.device_enumerator));
	auto device = rsettings.device
	                ? rsettings.device
	                : rsettings.device_enumerator->GetPreferredDevice();
	if (!device)
	{
		return lak::err_t<lak::u8string>{
		  lak::streamify("Failed to get preferred graphics device")};
	}

	auto handle = lak::unique_bank_ptr<lak::window_handle>::make();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(handle.release()););

	ASSERT(SDL_SetHint(SDL_HINT_VIDEO_EXTERNAL_CONTEXT, "1"));

	handle->sdl_window = SDL_CreateWindow("",
	                                      SDL_WINDOWPOS_CENTERED,
	                                      SDL_WINDOWPOS_CENTERED,
	                                      720,
	                                      480,
	                                      SDL_WINDOW_RESIZABLE);

	auto renderer =
	  device->CreateRenderer(rsettings.features, rsettings.options);

	if (!handle->sdl_window)
		return lak::err_t<lak::u8string>{
		  lak::fmt<u8"Failed to create window ({})">(SDL_GetError())};

	if (auto info = lak::cobalt_window_system_info(handle.get());
	    !renderer->Initialize(*info))
	{
		return lak::err_t<lak::u8string>{
		  lak::streamify("Failed to initialise cobalt renderer")};
	}

	auto &context = handle->gc.emplace<lak::cobalt_context>();
	auto fb       = renderer->CreateFrameBuffer();
	if (!fb)
	{
		return lak::err_t<lak::u8string>{
		  lak::streamify("Failed to create framebuffer")};
	}

	auto window_info = lak::cobalt_window_info(handle.get());
	RES_TRY(
	  lak::cobalt::as_result(
	    fb->BindWindow(*window_info, settings.depth_mode, settings.colour_mode))
	    .map_err([](auto &&) { return u8"Failed to bind window"_str; }));

	context.platform_handle = new lak::cobalt::graphics_context{};
	context.platform_handle->api_family =
	  rsettings.renderer_plugin.GetApiFamily();
	context.platform_handle->api_version =
	  rsettings.renderer_plugin.GetTargetApiVersion();
	context.platform_handle->vendor       = device->GetVendor();
	context.platform_handle->frame_buffer = lak::move(fb);
	context.platform_handle->renderer     = lak::move(renderer);

	context.sdl_window = handle->sdl_window;

	return lak::ok_t{handle.release()};
}
#endif

bool lak::destroy_window(lak::window_handle *handle)
{
	ASSERT(handle);

	switch (handle->graphics_mode())
	{
		case lak::graphics_mode::None:
		{
		}
		break;

#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
		{
		}
		break;
#endif

#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			SDL_GL_DeleteContext(handle->opengl_context().sdl_glcontext);
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

	if (handle->sdl_window) SDL_DestroyWindow(handle->sdl_window);

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
	switch (interval)
	{
		case lak::window_swap_interval::vsync_adaptive: [[fallthrough]];
		case lak::window_swap_interval::vsync_off:      [[fallthrough]];
		case lak::window_swap_interval::vsync_on:
			return SDL_GL_SetSwapInterval(static_cast<int>(interval)) == 0;
		default: BOUNDS_ASSERT_UNREACHABLE(return false);
	}
}
#endif

/* --- Cobalt --- */

#ifdef LAK_ENABLE_COBALT
lak::unique_ptr<::cobalt::graphics::IRenderer::WindowSystemInfoBase>
lak::cobalt_window_system_info(const lak::window_handle *w)
{
	LAK_UNUSED(w);
	auto driver = SDL_GetCurrentVideoDriver();
#	ifdef LAK_OS_LINUX
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(w->sdl_window, &info) != SDL_TRUE)
	{
		ERROR(SDL_GetError());
		ASSERT_UNREACHABLE();
	}
#	endif

#	ifdef LAK_OS_WINDOWS
	if (std::strcmp(driver, "windows") == 0)
	{
		return lak::unique_ptr<
		  ::cobalt::graphics::IRenderer::WindowSystemInfoBase>(
		  new ::cobalt::graphics::WindowSystemInfoWin32(),
		  [](auto *p)
		  { delete static_cast<::cobalt::graphics::WindowSystemInfoWin32 *>(p); });
	}
	else
#	endif

#	ifdef LAK_OS_LINUX
#		ifdef SDL_VIDEO_DRIVER_X11
	  if (std::strcmp(driver, "x11") == 0)
	{
		return lak::unique_ptr<
		  ::cobalt::graphics::IRenderer::WindowSystemInfoBase>(
		  new ::cobalt::graphics::WindowSystemInfoXlib(info.info.x11.display),
		  [](auto *p)
		  { delete static_cast<::cobalt::graphics::WindowSystemInfoXlib *>(p); });
	}
	else
#		endif
#		ifdef SDL_VIDEO_DRIVER_WAYLAND
	  if (std::strcmp(driver, "wayland") == 0)
	{
		return lak::unique_ptr<
		  ::cobalt::graphics::IRenderer::WindowSystemInfoBase>(
		  new ::cobalt::graphics::WindowSystemInfoWayland(info.info.wl.display),
		  [](auto *p)
		  {
			  delete static_cast<::cobalt::graphics::WindowSystemInfoWayland *>(p);
		  });
	}
	else
#		endif
#	endif

#	ifdef LAK_OS_APPLE
	  if (std::strcmp(driver, "cocoa") == 0)
	{
		return lak::unique_ptr<
		  ::cobalt::graphics::IRenderer::WindowSystemInfoBase>(
		  new ::cobalt::graphics::WindowSystemInfoAppKit(),
		  [](auto *p)
		  {
			  delete static_cast<::cobalt::graphics::WindowSystemInfoAppKit *>(p);
		  });
	}
	else
#	endif

	{
		ASSERT_UNREACHABLE();
	}
}

#	ifndef LAK_OS_APPLE
lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>
lak::cobalt_window_info(const lak::window_handle *w)
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(w->sdl_window, &info) != SDL_TRUE)
	{
		ERROR(SDL_GetError());
		ASSERT_UNREACHABLE();
	}

#		ifdef LAK_OS_WINDOWS
	if (info.subsystem == SDL_SYSWM_WINDOWS)
	{
		return lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>(
		  new ::cobalt::graphics::WindowInfoWin32(
		    info.info.win.window,
		    info.info.win.hinstance,
		    lak::cobalt::from_lak(lak::vec2u32_t(lak::window_drawable_size(w)))),
		  [](auto *p)
		  { delete static_cast<::cobalt::graphics::WindowInfoWin32 *>(p); });
	}
	else
#		endif

#		ifdef LAK_OS_LINUX
#			ifdef SDL_VIDEO_DRIVER_X11
	  if (info.subsystem == SDL_SYSWM_X11)
	{
		return lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>(
		  new ::cobalt::graphics::WindowInfoXlib(
		    info.info.x11.display,
		    info.info.x11.window,
		    lak::cobalt::from_lak(lak::vec2u32_t(lak::window_drawable_size(w)))),
		  [](auto *p)
		  { delete static_cast<::cobalt::graphics::WindowInfoXlib *>(p); });
	}
	else
#			endif
#			ifdef SDL_VIDEO_DRIVER_WAYLAND
	  if (info.subsystem == SDL_SYSWM_WAYLAND)
	{
		return lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>(
		  new ::cobalt::graphics::WindowInfoWayland(
		    info.info.wl.display,
		    info.info.wl.surface,
		    lak::cobalt::from_lak(lak::vec2u32_t(lak::window_drawable_size(w)))),
		  [](auto *p)
		  { delete static_cast<::cobalt::graphics::WindowInfoWayland *>(p); });
	}
	else
#			endif
#		endif

	{
		ASSERT_UNREACHABLE();
	}
}
#	endif
#endif

/* --- Window helper functions --- */

lak::wstring lak::window_title(const lak::window_handle *handle)
{
	return lak::to_wstring(lak::as_u8string(
	  lak::astring_view::from_c_str(SDL_GetWindowTitle(handle->sdl_window))));
}

bool lak::set_window_title(lak::window_handle *handle, const lak::wstring &str)
{
	SDL_SetWindowTitle(
	  handle->sdl_window,
	  reinterpret_cast<const char *>(lak::to_u8string(str).c_str()));
	return true;
}

lak::vec2l_t lak::window_size(const lak::window_handle *handle)
{
	int w, h;
	SDL_GetWindowSize(handle->sdl_window, &w, &h);
	return {w, h};
}

bool lak::set_window_cursor_pos(const lak::window_handle *handle,
                                lak::vec2l_t pos)
{
	ASSERT(pos.x < INT_MAX && pos.x > INT_MIN);
	ASSERT(pos.y < INT_MAX && pos.y > INT_MIN);
	// :TODO: clamp instead of cast
	SDL_WarpMouseInWindow(handle->sdl_window, (int)pos.x, (int)pos.y);
	return true;
}

lak::vec2l_t lak::window_drawable_size(const lak::window_handle *handle)
{
	switch (handle->graphics_mode())
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
		{
			auto *surface = SDL_GetWindowSurface(handle->sdl_window);
			ASSERT(surface);
			return {long(surface->w), long(surface->h)};
		}
		break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			int w, h;
			SDL_GL_GetDrawableSize(handle->sdl_window, &w, &h);
			return {long(w), long(h)};
		}
		break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt: return lak::window_size(handle); break;
#endif
		default: FATAL("Invalid graphics mode (", handle->graphics_mode(), ")");
	}
	return {0, 0};
}

bool lak::set_window_size(lak::window_handle *handle, lak::vec2l_t size)
{
	ASSERT_LESS(size.x, INT_MAX);
	ASSERT_LESS(size.y, INT_MAX);

	SDL_SetWindowSize(
	  handle->sdl_window, static_cast<int>(size.x), static_cast<int>(size.y));

	lak::window_handle_resize(handle);

	return true;
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
			return SDL_GL_MakeCurrent(handle->opengl_context().sdl_window,
			                          handle->opengl_context().sdl_glcontext) == 0;
#endif

#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt: return true;
#endif

		default: return false;
	}
}

bool lak::swap_window(lak::window_handle *handle)
{
	switch (handle->graphics_mode())
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
		{
			return SDL_UpdateWindowSurface(handle->software_context().sdl_window) ==
			       0;
		}
#endif

#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			SDL_GL_SwapWindow(handle->opengl_context().sdl_window);
			return true;
		}
#endif

#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
		{
			auto &ctx = handle->cobalt_context();
			auto *rd  = ctx.platform_handle->renderer.get();
			rd->WaitForOutputCaptureComplete();
			rd->RemoveAllRenderPasses();
			if (!ctx.platform_handle->compute_passes.empty())
			{
				rd->SetRenderPasses(ctx.platform_handle->compute_passes.data(),
				                    ctx.platform_handle->compute_passes.size());
				rd->StartNewFrame();
				rd->WaitForOutputCaptureComplete();
				rd->RemoveAllRenderPasses();
			}
			rd->SetRenderPasses(ctx.platform_handle->render_passes.data(),
			                    ctx.platform_handle->render_passes.size());
			rd->StartNewFrame();
			ctx.platform_handle->compute_passes.clear();
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
	LAK_UNUSED(handle);
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
