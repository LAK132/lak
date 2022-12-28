#include "lak/opengl/state.hpp"

// #include "lak/windows.hpp"

#include "lak/debug.hpp"
#include "lak/events.hpp"
#include "lak/macro_utils.hpp"
#include "lak/os.hpp"
#include "lak/window.hpp"

#ifdef LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL
#	include "lak/imgui/imgui.hpp"
#endif

#ifndef APP_NAME
#	define APP_NAME "basic window"
#endif

#ifndef LAK_BASIC_PROGRAM_MAIN
#	define LAK_BASIC_PROGRAM_MAIN main
#endif

#ifndef LAK_BASIC_PROGRAM_PREFIX
#	define LAK_BASIC_PROGRAM_PREFIX basic_
#endif

#define LAK_BASIC_PROGRAM(X) TOKEN_CONCAT(LAK_BASIC_PROGRAM_PREFIX, X)

// Implement these window_* functions in your program.
lak::optional<int> LAK_BASIC_PROGRAM(window_preinit)(int argc, char **argv);
void LAK_BASIC_PROGRAM(window_init)(lak::window &window);
void LAK_BASIC_PROGRAM(window_handle_event)(lak::window &window,
                                            lak::event &event);
void LAK_BASIC_PROGRAM(window_loop)(lak::window &window,
                                    uint64_t counter_delta);
int LAK_BASIC_PROGRAM(window_quit)(lak::window &window);

// Set these inside of window_preinit
uint32_t LAK_BASIC_PROGRAM(window_target_framerate) = 60;
bool LAK_BASIC_PROGRAM(window_force_software)       = false;
lak::vec2l_t LAK_BASIC_PROGRAM(window_start_size)   = {1200, 700};
lak::vec4f_t LAK_BASIC_PROGRAM(window_clear_colour) = {
  0.0f, 0.3125f, 0.312f, 1.0f};
lak::opengl_settings LAK_BASIC_PROGRAM(window_opengl_settings);
lak::software_settings LAK_BASIC_PROGRAM(window_software_settings);

#ifdef LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL
ImGui::ImplContext LAK_BASIC_PROGRAM(imgui_context) = nullptr;
#endif

void APIENTRY
LAK_BASIC_PROGRAM(opengl_debug_message_callback)(GLenum source,
                                                 GLenum type,
                                                 GLuint id,
                                                 GLenum severity,
                                                 GLsizei length,
                                                 const GLchar *message,
                                                 const void *userParam)
{
	LAK_UNUSED(id);
	LAK_UNUSED(userParam);

	if (type == GL_DEBUG_TYPE_OTHER &&
	    severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	DEBUG("GL CALLBACK: ", (type == GL_DEBUG_TYPE_ERROR ? "** ERROR **" : ""));
	DEBUG("| Error code: ", glGetError());
	switch (type)
	{
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			DEBUG("| Type: DEPRECATED BEHAVIOR");
			break;
		case GL_DEBUG_TYPE_ERROR: DEBUG("| Type: ERROR"); break;
		case GL_DEBUG_TYPE_MARKER: DEBUG("| Type: MARKER"); break;
		case GL_DEBUG_TYPE_OTHER: DEBUG("| Type: OTHER"); break;
		case GL_DEBUG_TYPE_PERFORMANCE: DEBUG("| Type: PERFORMANCE"); break;
		case GL_DEBUG_TYPE_POP_GROUP: DEBUG("| Type: POP GROUP"); break;
		case GL_DEBUG_TYPE_PORTABILITY: DEBUG("| Type: PORTABILITY"); break;
		case GL_DEBUG_TYPE_PUSH_GROUP: DEBUG("| Type: PUSH GROUP"); break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			DEBUG("| Type: UNDEFINED BEHAVIOR");
			break;
		default: DEBUG("| Type: ", type); break;
	}
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH: DEBUG("| Severity: HIGH"); break;
		case GL_DEBUG_SEVERITY_MEDIUM: DEBUG("| Severity: MEDIUM"); break;
		case GL_DEBUG_SEVERITY_LOW: DEBUG("| Severity: LOW"); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			DEBUG("| Severity: NOTIFICATION");
			break;
		default: DEBUG("| Severity: ", severity); break;
	}
	switch (source)
	{
		case GL_DEBUG_SOURCE_API: DEBUG("| Source: API"); break;
		case GL_DEBUG_SOURCE_APPLICATION: DEBUG("| Source: APPLICATION"); break;
		case GL_DEBUG_SOURCE_OTHER: DEBUG("| Source: OTHER"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			DEBUG("| Source: SHADER COMPILER");
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: DEBUG("| Source: THIRD PARTY"); break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			DEBUG("| Source: WINDOW SYSTEM");
			break;
		default: DEBUG("| Source: ", source); break;
	}
	DEBUG("| Message:\n", lak::string_view(message, length), "\n");
}

int LAK_BASIC_PROGRAM_MAIN(int argc, char **argv)
{
	/* --- Debugger initialisation --- */

	std::set_terminate(lak::terminate_handler);

#ifdef LAK_OS_WINDOWS
	/* --- Enable SGR codes in the Windows terminal --- */

	do
	{
		HANDLE hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE)
		{
			WARNING(::GetLastError());
			break;
		}

		DWORD dwMode = 0;
		if (!::GetConsoleMode(hOut, &dwMode))
		{
			WARNING(::GetLastError());
			break;
		}

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!::SetConsoleMode(hOut, dwMode))
		{
			WARNING(::GetLastError());
			break;
		}
	} while (false);
#endif

	/* --- Window initialisation --- */

	if (auto v = LAK_BASIC_PROGRAM(window_preinit)(argc, argv); v) return *v;

	lak::platform_init();
	DEFER(lak::platform_quit());

	auto make_software = [&]()
	{
		return lak::window::make(LAK_BASIC_PROGRAM(window_software_settings))
		  .and_then(
		    [&](auto &&window) -> lak::result<lak::window, lak::u8string>
		    {
			    if (window.graphics() != lak::graphics_mode::Software)
				    return lak::err_t<lak::u8string>{lak::streamify(
				      "Expected Software graphics, got ", window.graphics())};
			    return lak::move_ok(window);
		    });
	};

	auto window =
	  (LAK_BASIC_PROGRAM(window_force_software)
	     ? make_software()
	     : lak::window::make(LAK_BASIC_PROGRAM(window_opengl_settings))
	         .and_then(
	           [&](auto &&window) -> lak::result<lak::window, lak::u8string>
	           {
		           if (window.graphics() != lak::graphics_mode::OpenGL)
			           return lak::err_t<lak::u8string>{lak::streamify(
			             "Expected OpenGL graphics, got ", window.graphics())};

		           glViewport(
		             0, 0, window.drawable_size().x, window.drawable_size().y);
		           glClearColor(LAK_BASIC_PROGRAM(window_clear_colour).r,
		                        LAK_BASIC_PROGRAM(window_clear_colour).g,
		                        LAK_BASIC_PROGRAM(window_clear_colour).b,
		                        LAK_BASIC_PROGRAM(window_clear_colour).a);
		           glEnable(GL_DEPTH_TEST);

#ifndef NDEBUG
		           glEnable(GL_DEBUG_OUTPUT);
		           glDebugMessageCallback(
		             &LAK_BASIC_PROGRAM(opengl_debug_message_callback), 0);
#endif

		           return lak::move_ok(window);
	           })
	         .or_else(
	           [&](const lak::u8string &err)
	           {
		           WARNING("Failed to create an OpenGL window: ", err);
		           WARNING("Attempting to create a Software window instead");
		           return make_software();
	           }))
	    .UNWRAP();

	window.set_title(L"" APP_NAME);
	window.set_size(LAK_BASIC_PROGRAM(window_start_size));

#ifdef LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL
	LAK_BASIC_PROGRAM(imgui_context) =
	  ImGui::ImplCreateContext(window.graphics());
	ImGui::ImplInit();
	ImGui::ImplInitContext(LAK_BASIC_PROGRAM(imgui_context), window);

	if (window.graphics() == lak::graphics_mode::Software)
	{
		ImGuiStyle &style      = ImGui::GetStyle();
		style.AntiAliasedLines = false;
		style.AntiAliasedFill  = false;
		style.WindowRounding   = 0.0f;
	}

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowRounding = 0;
#endif

	LAK_BASIC_PROGRAM(window_init)(window);

	uint64_t last_counter = lak::performance_counter();
	uint64_t counter_delta =
	  lak::performance_frequency() / LAK_BASIC_PROGRAM(window_target_framerate);

	for (bool running = true; running;)
	{
		/* --- Handle SDL2 events --- */
		for (lak::event event; lak::next_event(&event);)
		{
			switch (event.type)
			{
				case lak::event_type::close_window: [[fallthrough]];
				case lak::event_type::quit_program:
				{
					running = false;
				}
				break;

				default: break;
			}

#ifdef LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL
			ImGui::ImplProcessEvent(LAK_BASIC_PROGRAM(imgui_context), event);
#endif

			LAK_BASIC_PROGRAM(window_handle_event)(window, event);
		}

		if (window.graphics() == lak::graphics_mode::OpenGL)
		{
			glViewport(0, 0, window.drawable_size().x, window.drawable_size().y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

#ifdef LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL
		{
			const float frame_time =
			  (float)counter_delta / lak::performance_frequency();
			ImGui::ImplNewFrame(
			  LAK_BASIC_PROGRAM(imgui_context), window, frame_time);

			bool mainOpen = true;

			ImGuiStyle &style = ImGui::GetStyle();
			ImGuiIO &io       = ImGui::GetIO();

			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(io.DisplaySize);
			ImVec2 old_window_padding = style.WindowPadding;
			style.WindowPadding       = ImVec2(0.0f, 0.0f);
			if (ImGui::Begin(
			      APP_NAME,
			      &mainOpen,
			      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar |
			        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings |
			        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
			{
				style.WindowPadding = old_window_padding;
				LAK_BASIC_PROGRAM(window_loop)(window, counter_delta);
				ImGui::End();
			}

			ImGui::ImplRender(LAK_BASIC_PROGRAM(imgui_context));
		}
#else
		LAK_BASIC_PROGRAM(window_loop)(window, counter_delta);
#endif

		window.swap();

		const auto counter = lak::yield_frame(
		  last_counter, LAK_BASIC_PROGRAM(window_target_framerate));
		counter_delta = counter - last_counter;
		last_counter  = counter;
	}

#ifndef NDEBUG
	if (window.graphics() == lak::graphics_mode::OpenGL)
	{
		glDisable(GL_DEBUG_OUTPUT);
	}
#endif

	const int result = LAK_BASIC_PROGRAM(window_quit)(window);

	ImGui::ImplShutdownContext(LAK_BASIC_PROGRAM(imgui_context));

	return result;
}
