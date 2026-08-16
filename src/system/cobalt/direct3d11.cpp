#include "lak/system/cobalt/renderers.hpp"
#include "lak/system/dll.hpp"

static lak::cobalt::renderer_plugin_func GetDirect3D11RendererPlugin = nullptr;

lak::cobalt::renderer_plugin_func lak::cobalt::d3d11_get_renderer_plugin()
{
	if (!GetDirect3D11RendererPlugin)
	{
		const static auto paths = []() -> lak::array<lak::fs::path>
		{
			lak::array<lak::fs::path> result;
			result.push_back(lak::exe_path().parent_path());
			if (result.back().has_parent_path())
				result.push_back(result.back().parent_path() / "lib");
			return result;
		}();
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
