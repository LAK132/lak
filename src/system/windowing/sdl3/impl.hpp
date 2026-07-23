#ifndef SDL_MAIN_HANDLED
#	error SDL_MAIN_HANDLED must be defined globally
#endif

#include "lak/system/os.hpp"

#include "lak/system/windowing/events.hpp"
#include "lak/system/windowing/platform.hpp"
#include "lak/system/windowing/window.hpp"
#include "lak/unique_ptr.hpp"
#include "lak/variant.hpp"

#include <SDL3/SDL.h>

#ifdef LAK_ENABLE_COBALT
#	include "lak/system/cobalt/context.hpp"
#	include "lak/system/cobalt/result.hpp"
#	include "lak/system/cobalt/math.hpp"

#	ifdef LAK_OS_WINDOWS
#		include <Cobalt/RendererInterface/WindowInfoWin32.h>
#		include <Cobalt/RendererInterface/WindowSystemInfoWin32.h>
#	endif
#	ifdef LAK_OS_LINUX
#		include <Cobalt/RendererInterface/WindowInfoWayland.h>
#		include <Cobalt/RendererInterface/WindowInfoXlib.h>
#		include <Cobalt/RendererInterface/WindowSystemInfoWayland.h>
#		include <Cobalt/RendererInterface/WindowSystemInfoXlib.h>
#	endif
#	ifdef LAK_OS_APPLE
#		include <Cobalt/RendererInterface/WindowInfoAppKit.h>
#		include <Cobalt/RendererInterface/WindowSystemInfoAppKit.h>
#	endif
#endif

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

	struct cobalt_context
	{
#ifdef LAK_ENABLE_COBALT
		lak::unique_ptr<lak::cobalt::graphics_context> platform_handle;
		SDL_Window *sdl_window = nullptr;
#endif
	};

	using graphics_context = lak::variant<lak::monostate,
	                                      lak::software_context,
	                                      lak::opengl_context,
	                                      lak::cobalt_context>;

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
		SDL_Event sdl_event;
	};
}
