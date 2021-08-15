#include "lak/events.hpp"
#include "lak/platform.hpp"
#include "lak/window.hpp"

namespace lak
{
	struct platform_instance
	{
		xcb_connection_t *handle;
	};

	extern lak::platform_instalce *_platform_instance;

	struct cursor
	{
	};

	struct software_context
	{
	};

	struct opengl_context
	{
	};

	struct vulkan_context
	{
	};

	using graphics_context = std::variant<std::monostate,
	                                      lak::software_context,
	                                      lak::opengl_context,
	                                      lak::vulkan_context>;

	struct window_handle
	{
		xcb_window_t _platform_handle = NULL;
		lak::graphics_context gc;

		lak::graphics_mode graphics_mode() const
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

		const lak::software_context &software_context() const
		{
			ASSERT_EQUAL(graphics_mode(), lak::graphics_mode::Software);
			return std::get<lak::software_context>(gc);
		}

		const lak::opengl_context &opengl_context() const
		{
			ASSERT_EQUAL(graphics_mode(), lak::graphics_mode::OpenGL);
			return std::get<lak::opengl_context>(gc);
		}

		const lak::vulkan_context &vulkan_context() const
		{
			ASSERT_EQUAL(graphics_mode(), lak::graphics_mode::Vulkan);
			return std::get<lak::vulkan_context>(gc);
		}
	};

	struct platform_event
	{
	};
}
