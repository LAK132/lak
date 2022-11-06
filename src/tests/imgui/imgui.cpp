#include "lak/test.hpp"

#include "lak/events.hpp"
#include "lak/platform.hpp"
#include "lak/window.hpp"

#include "lak/imgui/imgui.hpp"

BEGIN_TEST(imgui)
{
	DEBUG("platform init");
	ASSERT(lak::platform_init());

	int result = EXIT_FAILURE;

	{
		DEBUG("create window");
		lak::window w = lak::window::make(lak::software_settings{}).UNWRAP();

		w.set_title(L"ImGui Test Window");

		auto imgui_context{ImGui::ImplCreateContext(w.graphics())};

		ImGui::ImplInit();
		ImGui::ImplInitContext(imgui_context, w);

		{
			ImGuiStyle &style      = ImGui::GetStyle();
			style.AntiAliasedLines = false;
			style.AntiAliasedFill  = false;
			style.WindowRounding   = 0.0f;

			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui::StyleColorsDark();
			ImGui::GetStyle().WindowRounding = 0;
		}

		uint32_t target_framerate = 30U;
		uint64_t last_counter     = lak::performance_counter();
		uint64_t counter_delta = lak::performance_frequency() / target_framerate;

		DEBUG("starting event loop");
		for (bool running = true; running;)
		{
			for (lak::event e; lak::next_event(&e);)
			{
				ImGui::ImplProcessEvent(imgui_context, e);

				switch (e.type)
				{
					case lak::event_type::close_window: [[fallthrough]];
					case lak::event_type::quit_program: running = false; break;
					default: break;
				}
			}

			{
				const float frame_time =
				  (float)counter_delta / lak::performance_frequency();
				ImGui::ImplNewFrame(imgui_context, w, frame_time);

				bool mainOpen = true;

				ImGuiStyle &style = ImGui::GetStyle();
				ImGuiIO &io       = ImGui::GetIO();

				ImGui::SetNextWindowPos(ImVec2(0, 0));
				ImGui::SetNextWindowSize(io.DisplaySize);
				ImVec2 old_window_padding = style.WindowPadding;
				style.WindowPadding       = ImVec2(0.0f, 0.0f);
				if (ImGui::Begin(
				      "ImGui Test Window",
				      &mainOpen,
				      ImGuiWindowFlags_AlwaysAutoResize |
				        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar |
				        ImGuiWindowFlags_NoSavedSettings |
				        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
				{
					style.WindowPadding = old_window_padding;
					if (ImGui::Button("Click Here"))
					{
						result  = EXIT_SUCCESS;
						running = false;
					}
					ImGui::End();
				}

				ImGui::ImplRender(imgui_context);
			}

			w.swap();

			const auto counter = lak::yield_frame(last_counter, target_framerate);
			counter_delta      = counter - last_counter;
			last_counter       = counter;
		}
		DEBUG("event loop finished");

		ImGui::ImplShutdownContext(imgui_context);
	}

	DEBUG("platform quit");
	lak::platform_quit();

	return result;
}
END_TEST()
