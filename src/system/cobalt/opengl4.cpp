#include "lak/system/cobalt/renderers.hpp"
#include "lak/system/dll.hpp"
#include "lak/system/os.hpp"

static lak::cobalt::renderer_plugin_func GetOpenGL4RendererPlugin = nullptr;

lak::cobalt::renderer_plugin_func lak::cobalt::ogl4_get_renderer_plugin()
{
	if (!GetOpenGL4RendererPlugin)
	{
		auto paths = lak::fixed_array(lak::exe_path().parent_path());
#if defined(LAK_OS_WINDOWS)
		uintptr_t library = lak::load_library("OpenGL4Renderer.dll", paths);
#elif defined(LAK_OS_LINUX)
		uintptr_t library = lak::load_library("libOpenGL4Renderer.so", paths);
#elif defined(LAK_OS_APPLE)
		uintptr_t library = lak::load_library("libOpenGL4Renderer.dylib", paths);
#endif
		if (library != 0U)
			GetOpenGL4RendererPlugin =
			  lak::load_function<lak::cobalt::renderer_plugin_func>(
			    library, "GetOpenGL4RendererPlugin");
		else
			WARNING("Failed to load OpenGL4Renderer");
	}
	return GetOpenGL4RendererPlugin ? GetOpenGL4RendererPlugin
	                                : [](::cobalt::graphics::IRendererPlugin &)
	{ return false; };
}
