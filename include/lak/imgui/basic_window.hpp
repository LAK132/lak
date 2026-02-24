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
		float _left_size  = -1.f;
		float _right_size = -1.f;

		void menu_bar(float) {}

		void left_region(float) {}

		void right_region(float) {}

		void main_region(float frame_time)
		{
			const auto content_size{ImGui::GetContentRegionAvail()};

			if (_left_size <= 0.f || _right_size <= 0.f)
			{
				_left_size  = content_size.x / 2;
				_right_size = content_size.x / 2;
			}

			lak::VertSplitter(_left_size, _right_size, content_size.x);

			ImGui::BeginChild(
			  "Left", {_left_size, -1}, true, ImGuiWindowFlags_NoSavedSettings);
			static_cast<DERIVED *>(this)->left_region(frame_time);
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild(
			  "Right", {_right_size, -1}, true, ImGuiWindowFlags_NoSavedSettings);
			static_cast<DERIVED *>(this)->right_region(frame_time);
			ImGui::EndChild();
		}

		void draw(float frame_time)
		{
			if (ImGui::BeginMenuBar())
			{
				static_cast<DERIVED *>(this)->menu_bar(frame_time);
				ImGui::EndMenuBar();
			}

			static_cast<DERIVED *>(this)->main_region(frame_time);
		}
	};
}

#endif
