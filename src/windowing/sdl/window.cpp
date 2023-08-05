#include "lak/window.hpp"

#include "lak/opengl/gl3w.hpp"
#include "lak/opengl/state.hpp"

#include "lak/defer.hpp"
#include "lak/memmanip.hpp"
#include "lak/os.hpp"

#include "impl.hpp"

#ifdef LAK_ENABLE_SOFTRENDER
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::software_settings &)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(handle.release()););

	handle->sdl_window = SDL_CreateWindow("",
	                                      SDL_WINDOWPOS_CENTERED,
	                                      SDL_WINDOWPOS_CENTERED,
	                                      720,
	                                      480,
	                                      SDL_WINDOW_RESIZABLE);

	if (!handle->sdl_window)
		return lak::err_t<lak::u8string>{u8"Failed to create window"_str};

	auto &context = handle->gc.emplace<lak::software_context>();

	context.sdl_surface = SDL_GetWindowSurface(handle->sdl_window);

	if (!context.sdl_surface)
		return lak::err_t<lak::u8string>{u8"Failed to get window surface"_str};

	context.sdl_window = handle->sdl_window;

	return lak::ok_t{handle.release()};
}
#endif

#ifdef LAK_ENABLE_OPENGL
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::opengl_settings &settings)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
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
		return lak::err_t<lak::u8string>{u8"Failed to create window"_str};

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

#ifdef LAK_ENABLE_VULKAN
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::vulkan_settings &)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::create();
	ASSERT(handle);

	DEFER(if (handle) lak::destroy_window(handle.release()););

	handle->sdl_window =
	  SDL_CreateWindow("",
	                   SDL_WINDOWPOS_CENTERED,
	                   SDL_WINDOWPOS_CENTERED,
	                   720,
	                   480,
	                   SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

	if (!handle->sdl_window)
		return lak::err_t<lak::u8string>{u8"Failed to create window"_str};

	[[maybe_unused]] auto &context = handle->gc.emplace<lak::vulkan_context>();

	return lak::ok_t{handle.release()};
}
#endif

#ifdef LAK_ENABLE_METAL
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::metal_settings &s)
{
	ASSERT_NYI();
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

#ifdef LAK_ENABLE_VULKAN
		case lak::graphics_mode::Vulkan:
		{
		}
		break;
#endif

#ifdef LAK_ENABLE_METAL
		case lak::graphics_mode::Metal:
		{
		}
		break;
#endif

		default:
		{
			ASSERT_UNREACHABLE();
		}
		break;
	}
	handle->gc.emplace<std::monostate>();

	if (handle->sdl_window) SDL_DestroyWindow(handle->sdl_window);

	lak::bank<lak::window_handle>::destroy(handle);

	return true;
}

/* --- OpenGL --- */

lak::graphics_mode lak::window_graphics_mode(const lak::window_handle *w)
{
	return w->graphics_mode();
}

#ifdef LAK_ENABLE_OPENGL
bool lak::set_opengl_swap_interval(const lak::opengl_context &, int interval)
{
	return SDL_GL_SetSwapInterval(interval) == 0;
}
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
	int w = 0, h = 0;
	switch (handle->graphics_mode())
	{
		case lak::graphics_mode::Software:
		{
			auto *surface = SDL_GetWindowSurface(handle->sdl_window);
			ASSERT(surface);
			w = surface->w;
			h = surface->h;
		}
		break;
		case lak::graphics_mode::OpenGL:
			SDL_GL_GetDrawableSize(handle->sdl_window, &w, &h);
			break;
		case lak::graphics_mode::Vulkan:
			// SDL_Vulkan_GetDrawableSize(handle->sdl_window, &w, &h);
			break;
		default:
			FATAL("Invalid graphics mode (", handle->graphics_mode(), ")");
	}
	return {w, h};
}

bool lak::set_window_size(lak::window_handle *handle, lak::vec2l_t size)
{
	ASSERT_LESS(size.x, INT_MAX);
	ASSERT_LESS(size.y, INT_MAX);

	SDL_SetWindowSize(
	  handle->sdl_window, static_cast<int>(size.x), static_cast<int>(size.y));
	return true;
}

bool lak::swap_window(lak::window_handle *handle)
{
	switch (handle->graphics_mode())
	{
		case lak::graphics_mode::Software:
		{
			return SDL_UpdateWindowSurface(handle->software_context().sdl_window) ==
			       0;
		}

		case lak::graphics_mode::OpenGL:
		{
			SDL_GL_SwapWindow(handle->opengl_context().sdl_window);
			return true;
		}

		default:
			return false;
	}
}

#include "../common/window.inl"
