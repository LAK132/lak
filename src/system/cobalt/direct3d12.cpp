#include "lak/system/cobalt/renderers.hpp"
#include "lak/system/dll.hpp"

static lak::cobalt::renderer_plugin_func GetDirect3D12RendererPlugin = nullptr;

lak::cobalt::renderer_plugin_func lak::cobalt::d3d12_get_renderer_plugin()
{
	if (!GetDirect3D12RendererPlugin)
	{
		auto paths        = lak::fixed_array(lak::exe_path().parent_path());
		uintptr_t library = lak::load_library("Direct3D12Renderer.dll", paths);
		if (library != 0U)
			GetDirect3D12RendererPlugin =
			  lak::load_function<lak::cobalt::renderer_plugin_func>(
			    library, "GetDirect3D12RendererPlugin");
		else
			WARNING("Failed to load Direct3D12Renderer");
	}
	return GetDirect3D12RendererPlugin
	         ? GetDirect3D12RendererPlugin
	         : [](::cobalt::graphics::IRendererPlugin &) { return false; };
}
