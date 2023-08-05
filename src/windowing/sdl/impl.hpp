#ifndef SDL_MAIN_HANDLED
#	error SDL_MAIN_HANDLED must be defined globally
#endif

#include "lak/events.hpp"
#include "lak/platform.hpp"
#include "lak/variant.hpp"
#include "lak/window.hpp"

#include <SDL.h>
#include <SDL_syswm.h>

#ifdef None
#	undef None
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
		SDL_Surface *sdl_surface = NULL;
		SDL_Window *sdl_window   = NULL;
#endif
	};

	struct opengl_context
	{
#ifdef LAK_ENABLE_OPENGL
		SDL_GLContext sdl_glcontext = NULL;
		SDL_Window *sdl_window      = NULL;
#endif
	};

	struct vulkan_context
	{
#ifdef LAK_ENABLE_VULKAN
		SDL_Window *sdl_window = NULL;
#endif
	};

	struct metal_context
	{
#ifdef LAK_ENABLE_METAL
#endif
	};

	using graphics_context = lak::variant<std::monostate,
	                                      lak::software_context,
	                                      lak::opengl_context,
	                                      lak::vulkan_context,
	                                      lak::metal_context>;

	struct window_handle
	{
		SDL_Window *sdl_window = NULL;
		lak::graphics_context gc;

		inline lak::graphics_mode graphics_mode() const
		{
			switch (gc.index())
			{
				case 1:
					return lak::graphics_mode::Software;
				case 2:
					return lak::graphics_mode::OpenGL;
				case 3:
					return lak::graphics_mode::Vulkan;
				case 4:
					return lak::graphics_mode::Metal;
				default:
					FATAL("Invalid graphics mode");
				case 0:
					return lak::graphics_mode::None;
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

		inline const lak::vulkan_context &vulkan_context() const
		{
			ASSERT(gc.template holds<lak::vulkan_context>());
			return *gc.template get<lak::vulkan_context>();
		}

		inline const lak::metal_context &metal_context() const
		{
			ASSERT(gc.template holds<lak::metal_context>());
			return *gc.template get<lak::metal_context>();
		}
	};

	struct platform_event
	{
		SDL_Event sdl_event;
	};
}
