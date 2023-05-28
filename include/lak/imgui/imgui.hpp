#ifndef LAK_IMGUI_HPP
#define LAK_IMGUI_HPP

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

namespace lak
{
	template<typename R, typename... T, typename... D>
	bool AwaitPopup(const char *str_id,
	                bool &open,
	                std::thread *&staticThread,
	                std::atomic<bool> &staticFinished,
	                R (*callback)(T...),
	                const std::tuple<D...> &callbackData)
	{
		if (ImGui::BeginPopup(str_id, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (lak::await(staticThread, &staticFinished, callback, callbackData))
			{
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				open = false;
				return false;
			}
			open = true;
		}
		else
		{
			open = false;
			ImGui::OpenPopup(str_id);
		}

		return true;
	}

	bool VertSplitter(float &left,
	                  float &right,
	                  float width,
	                  float leftMin  = 8.0f,
	                  float rightMin = 8.0f,
	                  float length   = -1.0f);

	bool HoriSplitter(float &top,
	                  float &bottom,
	                  float height,
	                  float topMin    = 8.0f,
	                  float bottomMin = 8.0f,
	                  float length    = -1.0f);

	struct vert_split_child
	{
		float left_size;
		float right_size;

		inline vert_split_child()
		{
			const auto content_size{ImGui::GetContentRegionAvail()};
			left_size  = content_size.x / 2;
			right_size = content_size.x / 2;
		}

		inline bool begin(const char *id, bool border = true)
		{
			ImGui::PushID(id);
			lak::VertSplitter(
			  left_size, right_size, ImGui::GetContentRegionAvail().x);
			return ImGui::BeginChild(
			  "##left", {left_size, 0}, border, ImGuiWindowFlags_NoSavedSettings);
		}

		inline bool split(bool border = true) const
		{
			ImGui::EndChild();
			ImGui::SameLine();
			return ImGui::BeginChild(
			  "##right", {right_size, 0}, border, ImGuiWindowFlags_NoSavedSettings);
		}

		inline void end() const
		{
			ImGui::EndChild();
			ImGui::PopID();
		}
	};

	struct hori_split_child
	{
		float top_size;
		float bottom_size;

		inline hori_split_child()
		{
			const auto content_size{ImGui::GetContentRegionAvail()};
			top_size    = content_size.y / 2;
			bottom_size = content_size.y / 2;
		}

		inline bool begin(const char *id, bool border = true)
		{
			ImGui::PushID(id);
			lak::HoriSplitter(
			  top_size, bottom_size, ImGui::GetContentRegionAvail().y);
			return ImGui::BeginChild(
			  "##top", {0, top_size}, border, ImGuiWindowFlags_NoSavedSettings);
		}

		inline bool split(bool border = true) const
		{
			ImGui::EndChild();
			ImGui::Spacing();
			return ImGui::BeginChild("##bottom",
			                         {0, bottom_size},
			                         border,
			                         ImGuiWindowFlags_NoSavedSettings);
		}

		inline void end() const
		{
			ImGui::EndChild();
			ImGui::PopID();
		}
	};

	bool TreeNode(const char *fmt, ...);

	struct tree_node
	{
		bool _result;

		template<typename... ARGS>
		tree_node(const char *fmt, ARGS &&...args)
		: _result(lak::TreeNode(fmt, lak::forward<ARGS>(args)...))
		{
			ImGui::Separator();
		}

		~tree_node()
		{
			if (_result)
			{
				ImGui::Separator();
				ImGui::TreePop();
			}
		}

		operator bool() const { return _result; }
	};

#define LAK_TREE_NODE(...)                                                    \
	if (lak::tree_node UNIQUIFY(TREE_NODE_)(__VA_ARGS__); UNIQUIFY(TREE_NODE_))
}

#endif
