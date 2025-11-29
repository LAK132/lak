#ifndef LAK_BASIC_WINDOW_HPP
#define LAK_BASIC_WINDOW_HPP

#include <lak/imgui/backend.hpp>
#include <lak/imgui/widgets.hpp>

#include <lak/system/opengl/texture.hpp>

#include <lak/bit_reader.hpp>
#include <lak/span_manip.hpp>

namespace lak
{
	template<typename DERIVED>
	struct basic_window
	{
		static void menu_bar(float) {}

		static void left_region(float) {}

		static void right_region(float) {}

		static void main_region(float frame_time)
		{
			const auto content_size{ImGui::GetContentRegionAvail()};

			static float left_size  = content_size.x / 2;
			static float right_size = content_size.x / 2;

			lak::VertSplitter(left_size, right_size, content_size.x);

			ImGui::BeginChild(
			  "Left", {left_size, -1}, true, ImGuiWindowFlags_NoSavedSettings);
			DERIVED::left_region(frame_time);
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild(
			  "Right", {right_size, -1}, true, ImGuiWindowFlags_NoSavedSettings);
			DERIVED::right_region(frame_time);
			ImGui::EndChild();
		}

		static void draw(float frame_time)
		{
			if (ImGui::BeginMenuBar())
			{
				DERIVED::menu_bar(frame_time);
				ImGui::EndMenuBar();
			}

			DERIVED::main_region(frame_time);
		}
	};
}

#endif
