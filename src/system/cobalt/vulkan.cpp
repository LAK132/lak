#include "lak/system/cobalt/renderers.hpp"

#include "loader.hpp"

extern "C" DLL_IMPORT bool GetVulkanRendererPlugin(
  ::cobalt::graphics::IRendererPlugin &);

lak::cobalt::renderer_plugin_func lak::cobalt::vk_get_renderer_plugin()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererPlugin &rendererInfo)
	{
#ifdef LAK_COMPILER_MSVC
		TRY_RENDERER_FUNCTION(GetVulkanRendererPlugin(rendererInfo))
#else
		return GetVulkanRendererPlugin(rendererInfo);
#endif
	};
}
