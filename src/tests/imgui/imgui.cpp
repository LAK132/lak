#include "lak/test.hpp"

#include "lak/system/windowing/events.hpp"
#include "lak/system/windowing/platform.hpp"
#include "lak/system/windowing/window.hpp"

#include "lak/imgui/backend.hpp"
#include "lak/imgui/widgets.hpp"

#include "misc/cpp/imgui_stdlib.h"

#include "TextEditor.h"

#include "imgui_node_editor.h"

#include "imgui_memory_editor.h"

#ifdef LAK_RUN_WINDOWING_TESTS
BEGIN_TEST(imgui)
#else
int imgui_compile_test()
#endif
{
	DEBUG("platform init");
	ASSERT(lak::platform_init());

	int result = EXIT_FAILURE;

	{
		DEBUG("create window");
#if defined(LAK_ENABLE_SOFTRENDER)
		lak::window w = lak::window::make(lak::software_settings{}).UNWRAP();
#elif defined(LAK_ENABLE_OPENGL)
		lak::window w = lak::window::make(lak::opengl_settings{}).UNWRAP();
#elif defined(LAK_ENABLE_COBALT)
		auto [w, r] = lak::window::make(lak::cobalt_settings{}).UNWRAP();
#else
#	error no graphics backend
#endif

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

		lak::init_file_modal();

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
					default:                            break;
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
					static lak::path_getter pg;
					if (ImGui::Button("Open Folder Dialog")) pg.folder();
					if_let_some (std::filesystem::path path, pg()) DEBUG(path);

					static lak::vert_split_child vs;

					vs.begin("vs");

					static TextEditor te;
					te.Render("Text");

					vs.split();

					static byte_t me_mem[0x1000] = {};
					static MemoryEditor me;
					me.DrawContents(me_mem, sizeof(me_mem));

					vs.end();

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
#ifdef LAK_RUN_WINDOWING_TESTS
END_TEST()
#endif
