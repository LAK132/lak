#include "lak/system/cobalt/renderers.hpp"
#include "lak/system/dll.hpp"

static lak::cobalt::renderer_plugin_func GetDirect3D11RendererPlugin = nullptr;

lak::cobalt::renderer_plugin_func lak::cobalt::d3d11_get_renderer_plugin()
{
	if (!GetDirect3D11RendererPlugin)
	{
		auto paths        = lak::fixed_array(lak::exe_path().parent_path());
		uintptr_t library = lak::load_library("Direct3D11Renderer.dll", paths);
		if (library != 0U)
			GetDirect3D11RendererPlugin =
			  lak::load_function<lak::cobalt::renderer_plugin_func>(
			    library, "GetDirect3D11RendererPlugin");
		else
			WARNING("Failed to load Direct3D11Renderer");
	}
	return GetDirect3D11RendererPlugin
	         ? GetDirect3D11RendererPlugin
	         : [](::cobalt::graphics::IRendererPlugin &) { return false; };
}
