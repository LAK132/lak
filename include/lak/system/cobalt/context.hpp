#ifndef LAK_SYSTEM_COBALT_CONTEXT_HPP
#define LAK_SYSTEM_COBALT_CONTEXT_HPP

#include "lak/array.hpp"
#include "lak/format.hpp"

#include <RendererInterface/RendererInterface.pkg>
#include <RendererSupport/VendorName.h>

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

template<typename CHAR>
struct lak::format_traits<::cobalt::graphics::IRendererInfo::ApiFamily, CHAR>
{
	static lak::string<CHAR> to_string(
	  ::cobalt::graphics::IRendererInfo::ApiFamily family)
	{
		switch (family)
		{
			case ::cobalt::graphics::IRendererInfo::ApiFamily::OpenGL:
				return lak::strconv<CHAR>("OpenGL"_view);
			case ::cobalt::graphics::IRendererInfo::ApiFamily::OpenGLES:
				return lak::strconv<CHAR>("OpenGLES"_view);
			case ::cobalt::graphics::IRendererInfo::ApiFamily::Direct3D:
				return lak::strconv<CHAR>("Direct3D"_view);
			case ::cobalt::graphics::IRendererInfo::ApiFamily::Vulkan:
				return lak::strconv<CHAR>("Vulkan"_view);
			case ::cobalt::graphics::IRendererInfo::ApiFamily::Metal:
				return lak::strconv<CHAR>("Metal"_view);
			default: ASSERT_UNREACHABLE(); return lak::strconv<CHAR>("Unknown"_view);
		}
	}
};

template<typename CHAR>
struct lak::format_traits<::cobalt::graphics::IRendererInfo::ApiVersion, CHAR>
{
	static lak::string<CHAR> to_string(
	  ::cobalt::graphics::IRendererInfo::ApiVersion version)
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
