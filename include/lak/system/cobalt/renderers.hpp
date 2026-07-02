#ifndef LAK_SYSTEM_COBALT_RENDERERS_HPP
#define LAK_SYSTEM_COBALT_RENDERERS_HPP

#include <Cobalt/RendererInterface/RendererInterface.pkg>

namespace lak
{
	namespace cobalt
	{
		using renderer_plugin_func = bool (*)(
		  unsigned int indexNo, ::cobalt::graphics::IRendererPlugin &rendererInfo);

#ifdef LAK_ENABLE_COBALT_OGL3
		renderer_plugin_func ogl3_get_renderer_plugin();
#endif

#ifdef LAK_ENABLE_COBALT_OGL4
		renderer_plugin_func ogl4_get_renderer_plugin();
#endif

#ifdef LAK_ENABLE_COBALT_D3D11
		renderer_plugin_func d3d11_get_renderer_plugin();
#endif

#ifdef LAK_ENABLE_COBALT_D3D12
		renderer_plugin_func d3d12_get_renderer_plugin();
#endif

#ifdef LAK_ENABLE_COBALT_VK
		renderer_plugin_func vk_get_renderer_plugin();
#endif
	}
}

#endif
