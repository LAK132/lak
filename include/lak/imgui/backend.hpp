#ifndef LAK_IMGUI_BACKEND_HPP
#define LAK_IMGUI_BACKEND_HPP

#include "lak/await.hpp"
#include "lak/macro_utils.hpp"
#include "lak/span.hpp"
#include "lak/system/windowing/window.hpp"

#include <glm/mat4x4.hpp>

#include <imgui.h>

#ifdef LAK_ENABLE_SOFTRENDER
#	include "lak/softrender/texture.hpp"
#endif

#ifdef LAK_ENABLE_COBALT
#	include <RendererInterface/RendererInterface.pkg>
#endif

namespace ImGui
{
	struct _ImplContext;
	using ImplContext = _ImplContext *;

	ImplContext ImplCreateContext(lak::graphics_mode mode);

	void ImplDestroyContext(ImplContext context);

	// Run once at startup
	void ImplInit();

	// Run once per context
	void ImplInitContext(ImplContext context, const lak::window &window);

	// Run once per context
	void ImplShutdownContext(ImplContext context);

	void ImplSetCurrentContext(ImplContext context);

	void ImplSetTransform(ImplContext context, const glm::mat4x4 &transform);

	void ImplNewFrame(ImplContext context,
	                  const lak::window &window,
	                  const float delta_time,
	                  const bool call_base_new_frame = true);

	bool ImplProcessEvent(ImplContext context, const lak::event &event);

	enum struct ImplTextureColourFormat
	{
		RGBA,
		BGRA,
		RGB,
		R,
		A,
	};

	enum struct ImplTextureChannelFormat
	{
		U8,
		U16,
		F32,
	};

	ImTextureRef ImplCreateTexture(ImplContext context,
	                               const void *pixels,
	                               lak::vec2s_t size,
	                               ImplTextureColourFormat colour,
	                               ImplTextureChannelFormat channel);

#ifdef LAK_ENABLE_COBALT
	::cobalt::graphics::ITextureBuffer2D *ImplGetCobaltTexture(ImTextureRef tex);
#endif

	ImTextureRef ImplUpdateTexture(ImplContext context,
	                               ImTextureRef tex,
	                               const void *pixels,
	                               lak::vec2s_t size,
	                               ImplTextureColourFormat colour,
	                               ImplTextureChannelFormat channel,
	                               lak::span<const ImTextureRect> updates);

	void ImplDestroyTexture(ImplContext context, ImTextureRef tex);

	lak::vec2s_t ImplTextureSize(ImplContext context, ImTextureRef tex);

	void ImplUpdateTexture(ImplContext context, ImTextureData *texture);

	struct _ImplViewport;
	using ImplViewport = _ImplViewport *;

	ImplViewport ImplCreateViewport(ImplContext context,
	                                ImGui::ImplTextureColourFormat colour,
	                                ImGui::ImplTextureChannelFormat channel);

	void ImplDestroyViewport(ImplContext context, ImplViewport viewport);

	struct ImplSRViewportDetails
	{
#ifdef LAK_ENABLE_SOFTRENDER
		texture_base_t *framebuffer;
#endif
	};
	struct ImplGLViewportDetails
	{
#ifdef LAK_ENABLE_OPENGL
#endif
	};
	struct ImplCoViewportDetails
	{
#ifdef LAK_ENABLE_COBALT
		::cobalt::graphics::IRenderer *renderer;
		::cobalt::graphics::IFrameBuffer *framebuffer;
		lak::array<::cobalt::graphics::IRenderPassNode::unique_ptr> *passes;

		inline void clear_passes() const { passes->clear(); }
		inline ::cobalt::graphics::IRenderPassNode *append_pass() const
		{
			passes->reserve(passes->size() + 1U);
			auto result = passes->push_back(renderer->CreateRenderPassNode()).get();
			result->BindFrameBuffer(framebuffer);
			return result;
		}
#endif
	};

	using ImplViewportDetails = lak::variant<ImplSRViewportDetails,
	                                         ImplGLViewportDetails,
	                                         ImplCoViewportDetails>;

	ImplViewportDetails ImplBeginViewport(
	  ImplContext context,
	  ImplViewport viewport,
	  const ImVec2 &size,
	  bool *clicked          = nullptr,
	  ImGuiButtonFlags flags = ImGuiButtonFlags_None);

	void ImplEndViewport(ImplContext context, ImplViewport viewport);

	void ImplRender(ImplContext context, const bool call_base_render = true);

	void ImplRenderData(ImplContext context, ImDrawData *draw_data);

	void ImplSetClipboard(ImGuiContext *ctx, const char *text);

	const char *ImplGetClipboard(ImGuiContext *ctx);

	template<typename T>
	lak::span<T> ToSpan(ImVector<T> &vec)
	{
		return lak::span<T>(vec.Data, vec.Size);
	}

	template<typename T>
	lak::span<const T> ToSpan(const ImVector<T> &vec)
	{
		return lak::span<const T>(vec.Data, vec.Size);
	}
}

#endif
