#ifndef LAK_SYSTEM_COBALT_CONTEXT_HPP
#define LAK_SYSTEM_COBALT_CONTEXT_HPP

#include "lak/array.hpp"
#include "lak/format.hpp"

#include <Cobalt/RendererInterface/RendererInterface.pkg>
#include <Internal/RendererSupport/VendorName.h>

namespace lak
{
	namespace cobalt
	{
		struct graphics_context
		{
			::cobalt::graphics::IRendererPlugin::ApiFamily api_family;
			::cobalt::graphics::IRendererPlugin::ApiVersion api_version;
			::cobalt::graphics::IGraphicsDevice::Vendor vendor;

			::cobalt::graphics::IRenderer::unique_ptr renderer;
			::cobalt::graphics::IFrameBuffer::unique_ptr frame_buffer;

			lak::array<::cobalt::graphics::IRenderPassNode::unique_ptr>
			  owned_render_passes;
			lak::array<::cobalt::graphics::IRenderPassNode *> render_passes;
			lak::array<::cobalt::graphics::IRenderPassNode *> compute_passes;

			inline ~graphics_context()
			{
				render_passes.clear();
				compute_passes.clear();
				owned_render_passes.clear();
				frame_buffer.reset();
				if (renderer)
				{
					renderer->RemoveAllRenderPasses();
					renderer->WaitForDeferredDeletionComplete();
				}
				renderer.reset();
			}
		};
	}
}

template<typename CHAR>
struct lak::format_traits<::cobalt::graphics::IRendererPlugin::ApiFamily, CHAR>
{
	static lak::string<CHAR> to_string(
	  ::cobalt::graphics::IRendererPlugin::ApiFamily family)
	{
		switch (family)
		{
			case ::cobalt::graphics::IRendererPlugin::ApiFamily::OpenGL:
				return lak::strconv<CHAR>("OpenGL"_view);
			case ::cobalt::graphics::IRendererPlugin::ApiFamily::OpenGLES:
				return lak::strconv<CHAR>("OpenGLES"_view);
			case ::cobalt::graphics::IRendererPlugin::ApiFamily::Direct3D:
				return lak::strconv<CHAR>("Direct3D"_view);
			case ::cobalt::graphics::IRendererPlugin::ApiFamily::Vulkan:
				return lak::strconv<CHAR>("Vulkan"_view);
			case ::cobalt::graphics::IRendererPlugin::ApiFamily::Metal:
				return lak::strconv<CHAR>("Metal"_view);
			default:
				BOUNDS_ASSERT_UNREACHABLE(return lak::strconv<CHAR>("Unknown"_view));
		}
	}
};

template<typename CHAR>
struct lak::format_traits<::cobalt::graphics::IRendererPlugin::ApiVersion,
                          CHAR>
{
	static lak::string<CHAR> to_string(
	  ::cobalt::graphics::IRendererPlugin::ApiVersion version)
	{
		return lak::fmt<CHAR, "{:d}.{:d}">(version.major, version.minor);
	}
};

template<typename CHAR>
struct lak::format_traits<::cobalt::graphics::IGraphicsDevice::Vendor, CHAR>
{
	static lak::string<CHAR> to_string(
	  ::cobalt::graphics::IGraphicsDevice::Vendor vendor)
	{
		return lak::strconv<CHAR>(::cobalt::graphics::VendorToVendorName(vendor));
	}
};

#endif
