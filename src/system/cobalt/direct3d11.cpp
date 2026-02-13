#include "lak/system/cobalt/renderers.hpp"

#include "lak/system/compiler.hpp"

extern "C" DLL_IMPORT bool GetDirect3D11RendererInfo(
  ::cobalt::graphics::IRendererInfo &);

lak::cobalt::renderer_info_func lak::cobalt::d3d11_get_renderer_info()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererInfo &rendererInfo)
	{ return GetDirect3D11RendererInfo(rendererInfo); };
}
