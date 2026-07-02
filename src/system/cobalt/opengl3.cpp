#include "lak/system/cobalt/renderers.hpp"

#include "lak/system/compiler.hpp"

extern "C" DLL_IMPORT bool GetOpenGL3RendererPlugin(
  ::cobalt::graphics::IRendererPlugin &);

lak::cobalt::renderer_plugin_func lak::cobalt::ogl3_get_renderer_plugin()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererPlugin &rendererInfo)
	{ return GetOpenGL3RendererPlugin(rendererInfo); };
}
