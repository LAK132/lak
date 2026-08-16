#include "lak/system/cobalt/renderers.hpp"
#include "lak/system/dll.hpp"
#include "lak/system/os.hpp"

static lak::cobalt::renderer_plugin_func GetVulkanRendererPlugin = nullptr;

lak::cobalt::renderer_plugin_func lak::cobalt::vk_get_renderer_plugin()
{
	if (!GetVulkanRendererPlugin)
	{
		const static auto paths = []() -> lak::array<lak::fs::path>
		{
			lak::array<lak::fs::path> result;
			result.push_back(lak::exe_path().parent_path());
			if (result.back().has_parent_path())
				result.push_back(result.back().parent_path() / "lib");
			return result;
		}();
#if defined(LAK_OS_WINDOWS)
		uintptr_t library = lak::load_library("VulkanRenderer.dll", paths);
#elif defined(LAK_OS_LINUX)
		uintptr_t library = lak::load_library("libVulkanRenderer.so", paths);
#elif defined(LAK_OS_APPLE)
		uintptr_t library = lak::load_library("libVulkanRenderer.dylib", paths);
#endif
		if (library != 0U)
			GetVulkanRendererPlugin =
			  lak::load_function<lak::cobalt::renderer_plugin_func>(
			    library, "GetVulkanRendererPlugin");
		else
			WARNING("Failed to load VulkanRenderer");
	}
	return GetVulkanRendererPlugin ? GetVulkanRendererPlugin
	                               : [](::cobalt::graphics::IRendererPlugin &)
	{ return false; };
}
