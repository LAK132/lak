#ifndef SDL_MAIN_HANDLED
#	error SDL_MAIN_HANDLED must be defined globally
#endif

#include "lak/events.hpp"
#include "lak/platform.hpp"
#include "lak/window.hpp"

#include <SDL.h>
#include <SDL_syswm.h>

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
		SDL_Surface *sdl_surface = NULL;
		SDL_Window *sdl_window   = NULL;
	};

	struct opengl_context
	{
		SDL_GLContext sdl_glcontext = NULL;
		SDL_Window *sdl_window      = NULL;
	};

	struct vulkan_context
	{
		SDL_Window *sdl_window = NULL;
	};

	using graphics_context = std::variant<std::monostate,
	                                      lak::software_context,
	                                      lak::opengl_context,
	                                      lak::vulkan_context>;

	struct window_handle
	{
		SDL_Window *sdl_window = NULL;
		lak::graphics_context gc;

		inline lak::graphics_mode graphics_mode() const
		{
			switch (gc.index())
			{
				case 1: return lak::graphics_mode::Software;
				case 2: return lak::graphics_mode::OpenGL;
				case 3: return lak::graphics_mode::Vulkan;
				default: FATAL("Invalid graphics mode");
				case 0: return lak::graphics_mode::None;
			}
		}

		inline const lak::software_context &software_context() const
		{
			ASSERT_EQUAL(graphics_mode(), lak::graphics_mode::Software);
			return std::get<lak::software_context>(gc);
		}

		inline const lak::opengl_context &opengl_context() const
		{
			ASSERT_EQUAL(graphics_mode(), lak::graphics_mode::OpenGL);
			return std::get<lak::opengl_context>(gc);
		}

		inline const lak::vulkan_context &vulkan_context() const
		{
			ASSERT_EQUAL(graphics_mode(), lak::graphics_mode::Vulkan);
			return std::get<lak::vulkan_context>(gc);
		}
	};

	struct platform_event
	{
		SDL_Event sdl_event;
	};
}
