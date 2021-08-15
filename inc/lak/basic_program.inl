#include "lak/opengl/state.hpp"

// #include "lak/windows.hpp"

#include "lak/debug.hpp"
#include "lak/events.hpp"
#include "lak/os.hpp"
#include "lak/window.hpp"

#ifndef APP_NAME
#	define APP_NAME "basic window"
#endif

#ifndef LAK_BASIC_PROGRAM_MAIN
#	define LAK_BASIC_PROGRAM_MAIN main
#endif

// Implement these basic_window_* functions in your program.
lak::optional<int> basic_window_preinit(int argc, char **argv);
void basic_window_init(lak::window &window);
void basic_window_handle_event(lak::window &window, lak::event &event);
void basic_window_loop(lak::window &window, uint64_t counter_delta);
int basic_window_quit(lak::window &window);

// Set these inside of basic_window_preinit
uint32_t basic_window_target_framerate = 60;
bool basic_window_force_software       = false;
lak::vec2l_t basic_window_start_size   = {1200, 700};
lak::vec4f_t basic_window_clear_colour = {0.0f, 0.3125f, 0.312f, 1.0f};
lak::opengl_settings basic_window_opengl_settings;
lak::software_settings basic_window_software_settings;

void APIENTRY MessageCallback(GLenum source,
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

	if (auto v = basic_window_preinit(argc, argv); v) return *v;

	lak::platform_init();
	DEFER(lak::platform_quit());

	auto window =
	  lak::window::make(basic_window_opengl_settings)
	    .and_then(
	      [&](auto &&window) -> lak::result<lak::window>
	      {
		      if (window.graphics() != lak::graphics_mode::OpenGL)
			      return lak::err_t{};

		      glViewport(0, 0, window.drawable_size().x, window.drawable_size().y);
		      glClearColor(basic_window_clear_colour.r,
		                   basic_window_clear_colour.g,
		                   basic_window_clear_colour.b,
		                   basic_window_clear_colour.a);
		      glEnable(GL_DEPTH_TEST);

#ifndef NDEBUG
		      glEnable(GL_DEBUG_OUTPUT);
		      glDebugMessageCallback(&MessageCallback, 0);
#endif

		      return lak::ok_t{lak::move(window)};
	      })
	    .or_else(
	      [&](auto &&)
	      {
		      return lak::window::make(basic_window_software_settings)
		        .and_then(
		          [&](auto &&window) -> lak::result<lak::window>
		          {
			          if (window.graphics() != lak::graphics_mode::Software)
				          return lak::err_t{};
			          return lak::ok_t{lak::move(window)};
		          });
	      })
	    .UNWRAP();

	window.set_title(L"" APP_NAME);
	window.set_size(basic_window_start_size);

	basic_window_init(window);

	uint64_t last_counter = lak::performance_counter();
	uint64_t counter_delta =
	  lak::performance_frequency() / basic_window_target_framerate;

	for (bool running = true; running;)
	{
		/* --- Handle SDL2 events --- */
		for (lak::event event; lak::next_event(&event);
		     basic_window_handle_event(window, event))
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
		}

		if (window.graphics() == lak::graphics_mode::OpenGL)
		{
			glViewport(0, 0, window.drawable_size().x, window.drawable_size().y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		basic_window_loop(window, counter_delta);

		window.swap();

		const auto counter =
		  lak::yield_frame(last_counter, basic_window_target_framerate);
		counter_delta = counter - last_counter;
		last_counter  = counter;
	}

#ifndef NDEBUG
	if (window.graphics() == lak::graphics_mode::OpenGL)
	{
		glDisable(GL_DEBUG_OUTPUT);
	}
#endif

	return basic_window_quit(window);
}
