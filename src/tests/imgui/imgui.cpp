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

#ifdef LAK_ENABLE_OPENGL
#	include "lak/system/opengl/state.hpp"
#endif

#ifdef LAK_ENABLE_COBALT
#	include <RendererInterface/RendererInterface.pkg>
#endif

#ifdef LAK_RUN_WINDOWING_TESTS
BEGIN_TEST(imgui)
#else
int imgui_compile_test()
#endif
{
	auto do_window = [](lak::window &&w)
	{
		w.set_active();

		bool result = false;

		DEBUG(w.graphics());

		w.set_title(lak::fmt<L"ImGui Test Window ({})">(w.graphics()));

		auto imgui_context{ImGui::ImplCreateContext(w.graphics())};
		DEFER(ImGui::ImplDestroyContext(imgui_context));

		ImGui::ImplInit();
		ImGui::ImplInitContext(imgui_context, w);
		DEFER(ImGui::ImplShutdownContext(imgui_context));

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
			w.set_active();

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

			// clear window
			switch (w.graphics())
			{
#ifdef LAK_ENABLE_SOFTRENDER
				case lak::graphics_mode::Software:
					// no-op
					break;
#endif
#ifdef LAK_ENABLE_OPENGL
				case lak::graphics_mode::OpenGL:
				{
					glViewport(0, 0, w.drawable_size().x, w.drawable_size().y);
					glScissor(0, 0, w.drawable_size().x, w.drawable_size().y);
					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
					        GL_STENCIL_BUFFER_BIT);
				}
				break;
#endif
#ifdef LAK_ENABLE_COBALT
				case lak::graphics_mode::Cobalt:
				{
					const auto &cgx = lak::cobalt_graphics_context(w.handle()).UNWRAP();
					auto *rd        = cgx.renderer.get();
					auto *fb        = cgx.frame_buffer.get();
					fb->DefineViewportRegion(
					  {0, 0},
					  {uint32_t(w.drawable_size().x), uint32_t(w.drawable_size().y)});
					fb->DefineScissorRegion(
					  {0, 0},
					  {uint32_t(w.drawable_size().x), uint32_t(w.drawable_size().y)});
					auto rp = lak::cobalt_create_render_pass(w.handle()).UNWRAP();
					rp->SetAttachmentClearData(
					  ::cobalt::graphics::IFrameBuffer::AttachmentType::Color,
					  0,
					  ::cobalt::graphics::V4Float32{0.0f, 0.0f, 0.0f, 0.0f});
					rp->SetAttachmentClearData(
					  ::cobalt::graphics::IFrameBuffer::AttachmentType::Depth,
					  0,
					  ::cobalt::graphics::V4Float32(1.0f, 1.0f, 1.0f, 1.0f));
				}
				break;
#endif
				default: ASSERT_UNREACHABLE();
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
						result  = true;
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

		ASSERTF(result, "should have clicked 'Click Here'");
	};

	DEBUG("platform init");
	ASSERT(lak::platform_init());

	DEBUG("create window(s)");

#ifdef LAK_ENABLE_SOFTRENDER
	do_window(lak::window::make(lak::software_settings{}).UNWRAP());
#endif
#ifdef LAK_ENABLE_OPENGL
	do_window(lak::window::make(lak::opengl_settings{}).UNWRAP());
#endif
#ifdef LAK_ENABLE_COBALT
	{
		auto [cw, cr] = lak::window::make(lak::cobalt_settings{}).UNWRAP();
		DEBUG(cr.renderer_info.GetDisplayName());
		do_window(lak::move(cw));
	}
#endif
	lak::init_file_modal(); // force clear textures

	DEBUG("platform quit");
	lak::platform_quit();

	return EXIT_SUCCESS;
}
#ifdef LAK_RUN_WINDOWING_TESTS
END_TEST()
#endif
