#include "lak/system/windowing/events.hpp"
#include "lak/system/windowing/platform.hpp"
#include "lak/system/windowing/window.hpp"
#include "lak/variant.hpp"

namespace lak
{
	struct platform_instance
	{
		Display *handle;
		Atom wm_delete_window;
	};

	extern lak::platform_instance *_platform_instance;

	struct cursor
	{
	};

	struct software_context
	{
	};

	struct opengl_context
	{
	};

	using graphics_context = std::variant<std::monostate,
	                                      lak::software_context,
	                                      lak::opengl_context>;

	struct window_handle
	{
		Window *_platform_handle = NULL;
		lak::graphics_context gc;

		inline lak::graphics_mode graphics_mode() const
		{
			switch (gc.index())
			{
				case 1:
					return lak::graphics_mode::Software;
				case 2:
					return lak::graphics_mode::OpenGL;
				default:
					FATAL("Invalid graphics mode");
					[[fallthrough]];
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
	};

	struct platform_event
	{
	};
}
