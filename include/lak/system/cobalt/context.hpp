#ifndef LAK_SYSTEM_COBALT_CONTEXT_HPP
#define LAK_SYSTEM_COBALT_CONTEXT_HPP

#include "lak/system/windowing/window.hpp"

#include <RendererInterface/RendererInterface.pkg>

namespace lak
{
	namespace cobalt
	{
		struct graphics_context
		{
			::cobalt::graphics::IRendererInfo::ApiFamily api_family;
			::cobalt::graphics::IRendererInfo::ApiVersion api_version;
			::cobalt::graphics::IGraphicsDevice::Vendor vendor;

			::cobalt::graphics::IRenderer::unique_ptr renderer;
			::cobalt::graphics::IFrameBuffer::unique_ptr frame_buffer;

			lak::array<::cobalt::graphics::IRenderPassNode::unique_ptr>
			  owned_render_passes;
			lak::array<::cobalt::graphics::IRenderPassNode *> render_passes;

			inline ~graphics_context()
			{
				if (renderer) renderer->RemoveAllRenderPasses();
				render_passes.clear();
				owned_render_passes.clear();
				frame_buffer.reset();
				renderer.reset();
			}
		};
	}
}

#endif
