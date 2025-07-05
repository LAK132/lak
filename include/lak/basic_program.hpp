#ifndef LAK_BASIC_PROGRAM_HPP
#define LAK_BASIC_PROGRAM_HPP

#include "lak/bank_ptr.hpp"
#include "lak/debug.hpp"
#include "lak/events.hpp"
#include "lak/macro_utils.hpp"
#include "lak/os.hpp"
#include "lak/span.hpp"
#include "lak/window.hpp"

#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
#	include "lak/imgui/backend.hpp"
#	include "lak/imgui/widgets.hpp"
#endif

#if defined(LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL) &&                           \
  !defined(LAK_BASIC_PROGRAM_IMGUI_IMPL)
#	define LAK_BASIC_PROGRAM_IMGUI_IMPL
#endif

#ifndef LAK_BASIC_PROGRAM_MAIN
#	define LAK_BASIC_PROGRAM_MAIN main
#endif

#ifndef LAK_BASIC_PROGRAM_PREFIX
#	define LAK_BASIC_PROGRAM_PREFIX basic_
#endif

#define LAK_BASIC_PROGRAM(X) TOKEN_CONCAT(LAK_BASIC_PROGRAM_PREFIX, X)

// Implement these program_* functions in your program.
lak::optional<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *> args);
lak::optional<int> LAK_BASIC_PROGRAM(program_init)();
bool LAK_BASIC_PROGRAM(program_loop)(uint64_t counter_delta);
int LAK_BASIC_PROGRAM(program_quit)();

// Implement these window_* functions in your program.
void LAK_BASIC_PROGRAM(window_init)(lak::window &window);
void LAK_BASIC_PROGRAM(window_handle_event)(lak::window *window,
                                            lak::event &event);
void LAK_BASIC_PROGRAM(window_loop)(lak::window &window,
                                    uint64_t counter_delta);
void LAK_BASIC_PROGRAM(window_quit)(lak::window &window);

// Set these inside of program_preinit or program_init.
extern uint32_t LAK_BASIC_PROGRAM(window_target_framerate);
extern bool LAK_BASIC_PROGRAM(window_force_software);
extern lak::vec2l_t LAK_BASIC_PROGRAM(window_start_size);
extern lak::vec4f_t LAK_BASIC_PROGRAM(window_clear_colour);
extern lak::opengl_settings LAK_BASIC_PROGRAM(window_opengl_settings);
extern lak::software_settings LAK_BASIC_PROGRAM(window_software_settings);

struct LAK_BASIC_PROGRAM(window_instance)
{
	lak::window window;
#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
	ImGui::ImplContext imgui_context = nullptr;
#endif
};

LAK_BASIC_PROGRAM(window_instance) *
  LAK_BASIC_PROGRAM(find_window_instance)(const lak::window_handle *window);

const lak::bank<LAK_BASIC_PROGRAM(window_instance)> &LAK_BASIC_PROGRAM(
  window_instances)();

lak::result<lak::window &, lak::u8string> LAK_BASIC_PROGRAM(create_window)();

void LAK_BASIC_PROGRAM(destroy_window)(lak::window &window);

#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
extern ImGui::ImplContext LAK_BASIC_PROGRAM(imgui_context);
#endif
#ifdef LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL
extern ImGuiWindowFlags LAK_BASIC_PROGRAM(imgui_main_window_flags);
#endif

#endif
