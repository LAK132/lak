#include "lak/system/cobalt/renderers.hpp"

#include "loader.hpp"

extern "C" DLL_IMPORT bool GetOpenGL4RendererPlugin(
  ::cobalt::graphics::IRendererPlugin &);

lak::cobalt::renderer_plugin_func lak::cobalt::ogl4_get_renderer_plugin()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererPlugin &rendererInfo)
	{
#ifdef LAK_COMPILER_MSVC
		TRY_RENDERER_FUNCTION(GetOpenGL4RendererPlugin(rendererInfo))
#else
		return GetOpenGL4RendererPlugin(rendererInfo);
#endif
	};
}
