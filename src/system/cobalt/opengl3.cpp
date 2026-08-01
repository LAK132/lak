#include "lak/system/cobalt/renderers.hpp"
#include "lak/system/dll.hpp"
#include "lak/system/os.hpp"

static lak::cobalt::renderer_plugin_func GetOpenGL3RendererPlugin = nullptr;

lak::cobalt::renderer_plugin_func lak::cobalt::ogl3_get_renderer_plugin()
{
	if (!GetOpenGL3RendererPlugin)
	{
		auto paths = lak::fixed_array(lak::exe_path().parent_path());
#if defined(LAK_OS_WINDOWS)
		uintptr_t library = lak::load_library("OpenGL3Renderer.dll", paths);
#elif defined(LAK_OS_LINUX)
		uintptr_t library = lak::load_library("libOpenGL3Renderer.so", paths);
#elif defined(LAK_OS_APPLE)
		uintptr_t library = lak::load_library("libOpenGL3Renderer.dylib", paths);
#endif
		if (library != 0U)
			GetOpenGL3RendererPlugin =
			  lak::load_function<lak::cobalt::renderer_plugin_func>(
			    library, "GetOpenGL3RendererPlugin");
		else
			WARNING("Failed to load OpenGL3Renderer");
	}
	return GetOpenGL3RendererPlugin ? GetOpenGL3RendererPlugin
	                                : [](::cobalt::graphics::IRendererPlugin &)
	{ return false; };
}
