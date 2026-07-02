#include "lak/system/cobalt/renderers.hpp"

#include "lak/system/compiler.hpp"

extern "C" DLL_IMPORT bool GetDirect3D12RendererPlugin(
  ::cobalt::graphics::IRendererPlugin &);

lak::cobalt::renderer_plugin_func lak::cobalt::d3d12_get_renderer_plugin()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererPlugin &rendererInfo)
	{ return GetDirect3D12RendererPlugin(rendererInfo); };
}
