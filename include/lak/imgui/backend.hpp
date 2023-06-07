#ifndef LAK_IMGUI_BACKEND_HPP
#define LAK_IMGUI_BACKEND_HPP

#include "lak/await.hpp"
#include "lak/macro_utils.hpp"
#include "lak/span.hpp"
#include "lak/window.hpp"

#include <glm/mat4x4.hpp>

#include <imgui.h>

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

	void ImplRender(ImplContext context, const bool call_base_render = true);

	void ImplRenderData(ImplContext context, ImDrawData *draw_data);

	void ImplSetClipboard(void *, const char *text);

	const char *ImplGetClipboard(char **clipboard);

	ImTextureID ImplGetFontTexture(ImplContext context);

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
