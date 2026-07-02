#include "lak/system/cobalt/renderers.hpp"

#include "lak/system/compiler.hpp"

extern "C" DLL_IMPORT bool GetOpenGL4RendererPlugin(
  ::cobalt::graphics::IRendererPlugin &);

lak::cobalt::renderer_plugin_func lak::cobalt::ogl4_get_renderer_plugin()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererPlugin &rendererInfo)
	{ return GetOpenGL4RendererPlugin(rendererInfo); };
}
