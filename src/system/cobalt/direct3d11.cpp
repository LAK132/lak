#include "lak/system/cobalt/renderers.hpp"

#include "loader.hpp"

extern "C" DLL_IMPORT bool GetDirect3D11RendererPlugin(
  ::cobalt::graphics::IRendererPlugin &);

lak::cobalt::renderer_plugin_func lak::cobalt::d3d11_get_renderer_plugin()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererPlugin &rendererInfo)
	{
#ifdef LAK_COMPILER_MSVC
		TRY_RENDERER_FUNCTION(GetDirect3D11RendererPlugin(rendererInfo))
#else
		return GetDirect3D11RendererPlugin(rendererInfo);
#endif
	};
}
