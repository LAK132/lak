#ifndef LAK_SYSTEM_COBALT_RENDERERS_HPP
#define LAK_SYSTEM_COBALT_RENDERERS_HPP

#include <RendererInterface/RendererInterface.pkg>

namespace lak
{
	namespace cobalt
	{
		using renderer_info_func = bool (*)(
		  unsigned int indexNo, ::cobalt::graphics::IRendererInfo &rendererInfo);

#ifdef LAK_ENABLE_COBALT_OGL3
		renderer_info_func ogl3_get_renderer_info();
#endif

#ifdef LAK_ENABLE_COBALT_OGL4
		renderer_info_func ogl4_get_renderer_info();
#endif

#ifdef LAK_ENABLE_COBALT_D3D11
		renderer_info_func d3d11_get_renderer_info();
#endif

#ifdef LAK_ENABLE_COBALT_D3D12
		renderer_info_func d3d12_get_renderer_info();
#endif

#ifdef LAK_ENABLE_COBALT_VK
		renderer_info_func vk_get_renderer_info();
#endif
	}
}

#endif
