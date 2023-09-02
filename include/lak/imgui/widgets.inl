#include "widgets.hpp"

#include "lak/await.hpp"

template<typename R, typename... T, typename... D>
bool lak::AwaitPopup(const char *str_id,
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

/* --- vert_split_child --- */

inline lak::vert_split_child::vert_split_child()
{
	const auto content_size{ImGui::GetContentRegionAvail()};
	left_size  = content_size.x / 2;
	right_size = content_size.x / 2;
}

inline bool lak::vert_split_child::begin(const char *id, bool border)
{
	ImGui::PushID(id);
	float min_size = ImGui::GetStyle().WindowPadding.x * 4;
	lak::VertSplitter(left_size,
	                  right_size,
	                  ImGui::GetContentRegionAvail().x,
	                  min_size,
	                  min_size);
	return ImGui::BeginChild(
	  "##left", {left_size, 0}, border, ImGuiWindowFlags_NoSavedSettings);
}

inline bool lak::vert_split_child::split(bool border) const
{
	ImGui::EndChild();
	ImGui::SameLine();
	return ImGui::BeginChild(
	  "##right", {right_size, 0}, border, ImGuiWindowFlags_NoSavedSettings);
}

inline void lak::vert_split_child::end() const
{
	ImGui::EndChild();
	ImGui::PopID();
}

/* --- hori_split_child --- */

inline lak::hori_split_child::hori_split_child()
{
	const auto content_size{ImGui::GetContentRegionAvail()};
	top_size    = content_size.y / 2;
	bottom_size = content_size.y / 2;
}

inline bool lak::hori_split_child::begin(const char *id, bool border)
{
	ImGui::PushID(id);
	lak::HoriSplitter(top_size, bottom_size, ImGui::GetContentRegionAvail().y);
	return ImGui::BeginChild(
	  "##top", {0, top_size}, border, ImGuiWindowFlags_NoSavedSettings);
}

inline bool lak::hori_split_child::split(bool border) const
{
	ImGui::EndChild();
	ImGui::Spacing();
	return ImGui::BeginChild(
	  "##bottom", {0, bottom_size}, border, ImGuiWindowFlags_NoSavedSettings);
}

inline void lak::hori_split_child::end() const
{
	ImGui::EndChild();
	ImGui::PopID();
}
