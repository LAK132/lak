#include "lak/system/cobalt/renderers.hpp"

extern "C" __declspec(dllimport) bool GetDirect3D11RendererInfo(
  ::cobalt::graphics::IRendererInfo &);

lak::cobalt::renderer_info_func lak::cobalt::d3d11_get_renderer_info()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererInfo &rendererInfo)
	{ return GetDirect3D11RendererInfo(rendererInfo); };
}
