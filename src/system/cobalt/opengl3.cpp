#include "lak/system/cobalt/renderers.hpp"

#include "loader.hpp"

extern "C" DLL_IMPORT bool GetOpenGL3RendererPlugin(
  ::cobalt::graphics::IRendererPlugin &);

lak::cobalt::renderer_plugin_func lak::cobalt::ogl3_get_renderer_plugin()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererPlugin &rendererInfo)
	{
#ifdef LAK_COMPILER_MSVC
		TRY_RENDERER_FUNCTION(GetOpenGL3RendererPlugin(rendererInfo))
#else
		return GetOpenGL3RendererPlugin(rendererInfo);
#endif
	};
}
