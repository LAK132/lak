#ifndef SDL_MAIN_HANDLED
#	error SDL_MAIN_HANDLED must be defined globally
#endif

#include "lak/system/windowing/events.hpp"
#include "lak/system/windowing/platform.hpp"
#include "lak/system/windowing/window.hpp"
#include "lak/variant.hpp"

#include <SDL.h>
#include <SDL_syswm.h>

#ifdef None
#	undef None
#endif

#ifdef Status
#	undef Status
#endif

namespace lak
{
	struct platform_instance
	{
	};

	extern lak::platform_instance *_platform_instance;

	struct cursor
	{
		SDL_Cursor *platform_handle;
	};

	struct software_context
	{
#ifdef LAK_ENABLE_SOFTRENDER
		SDL_Surface *sdl_surface = nullptr;
		SDL_Window *sdl_window   = nullptr;
#endif
	};

	struct opengl_context
	{
#ifdef LAK_ENABLE_OPENGL
		SDL_GLContext sdl_glcontext = nullptr;
		SDL_Window *sdl_window      = nullptr;
#endif
	};

	using graphics_context = lak::variant<std::monostate,
	                                      lak::software_context,
	                                      lak::opengl_context>;

	struct window_handle
	{
		SDL_Window *sdl_window = nullptr;
		lak::graphics_context gc;

		inline lak::graphics_mode graphics_mode() const
		{
			switch (gc.index())
			{
				case 1:  return lak::graphics_mode::Software;
				case 2:  return lak::graphics_mode::OpenGL;
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
	};

	struct platform_event
	{
		SDL_Event sdl_event;
	};
}
