#ifndef LAK_BASIC_PROGRAM_HPP
#define LAK_BASIC_PROGRAM_HPP

// see README.md or src/basic_program.cpp for examples

#include "lak/debug.hpp"
#include "lak/macro_utils.hpp"
#include "lak/shared_ptr.hpp"
#include "lak/span.hpp"

#include "lak/system/os.hpp"
#include "lak/system/windowing/events.hpp"
#include "lak/system/windowing/window.hpp"

#if defined(LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL) &&                           \
  !defined(LAK_BASIC_PROGRAM_IMGUI_IMPL)
#	define LAK_BASIC_PROGRAM_IMGUI_IMPL
#endif

#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
#	include "lak/imgui/backend.hpp"
#	include "lak/imgui/widgets.hpp"
#endif

#ifndef LAK_BASIC_PROGRAM_MAIN
#	define LAK_BASIC_PROGRAM_MAIN main
#endif

#ifndef LAK_BASIC_PROGRAM_PREFIX
#	define LAK_BASIC_PROGRAM_PREFIX basic_
#endif

#define LAK_BASIC_PROGRAM(X) TOKEN_CONCAT(LAK_BASIC_PROGRAM_PREFIX, X)

// Implement these program_* functions in your program.
lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(
  lak::span<char *> args);
lak::error_code<int> LAK_BASIC_PROGRAM(program_init)();
bool LAK_BASIC_PROGRAM(program_loop)(uint64_t counter_delta);
int LAK_BASIC_PROGRAM(program_quit)();
void LAK_BASIC_PROGRAM(program_handle_event)(lak::event &event);

// Set these inside of program_preinit or program_init.
extern uint32_t LAK_BASIC_PROGRAM(window_target_framerate);
extern bool LAK_BASIC_PROGRAM(window_force_software);
extern lak::vec2l_t LAK_BASIC_PROGRAM(window_start_size);
extern lak::cobalt_settings LAK_BASIC_PROGRAM(window_cobalt_settings);
extern lak::opengl_settings LAK_BASIC_PROGRAM(window_opengl_settings);
extern lak::software_settings LAK_BASIC_PROGRAM(window_software_settings);

// Implement init, handle_event and loop in your window class.
struct LAK_BASIC_PROGRAM(window_api)
{
	virtual void init()                          = 0;
	virtual void handle_event(lak::event &event) = 0;
	virtual void loop(uint64_t counter_delta)    = 0;

	virtual void destroy()        = 0;
	virtual lak::window &window() = 0;
};
struct LAK_BASIC_PROGRAM(window_instance_base)
: virtual public LAK_BASIC_PROGRAM(window_api)
{
	lak::window _window;
	lak::vec4f_t clear_colour{0.0f, 0.3125f, 0.312f, 1.0f};
#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
	ImGui::ImplContext imgui_context = nullptr;
#endif
#ifdef LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL
	ImGuiWindowFlags imgui_window_flags =
	  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar |
	  ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings |
	  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
#endif

	inline LAK_BASIC_PROGRAM(window_instance_base)(lak::window &&w)
	: LAK_BASIC_PROGRAM(window_api)(), _window(lak::move(w))
	{
	}

	virtual ~LAK_BASIC_PROGRAM(window_instance_base)();

	virtual void destroy() override final;
	virtual lak::window &window() override final { return _window; }
};
template<typename WINDOW_CLASS>
struct LAK_BASIC_PROGRAM(window_instance)
: public LAK_BASIC_PROGRAM(window_instance_base), public WINDOW_CLASS
{
	inline LAK_BASIC_PROGRAM(window_instance)(lak::window &&w)
	: LAK_BASIC_PROGRAM(window_api)(),
	  LAK_BASIC_PROGRAM(window_instance_base)(lak::move(w)),
	  WINDOW_CLASS()
	{
	}
	virtual ~LAK_BASIC_PROGRAM(window_instance)() = default;
};

lak::strong_ptr<LAK_BASIC_PROGRAM(window_instance_base)> LAK_BASIC_PROGRAM(
  find_window_instance)(const lak::window_handle *window);

lak::span<const lak::strong_ref<LAK_BASIC_PROGRAM(window_instance_base)>>
  LAK_BASIC_PROGRAM(window_instances)();

template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string>
  LAK_BASIC_PROGRAM(create_window)();

template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string>
  LAK_BASIC_PROGRAM(create_window)(lak::window &&);

#ifdef LAK_ENABLE_SOFTRENDER
template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string>
  LAK_BASIC_PROGRAM(create_window)(const lak::software_settings &);
#endif

#ifdef LAK_ENABLE_OPENGL
template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string>
  LAK_BASIC_PROGRAM(create_window)(const lak::opengl_settings &);
#endif

#ifdef LAK_ENABLE_COBALT
template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string>
  LAK_BASIC_PROGRAM(create_window)(const lak::cobalt_settings &);
#endif

#endif
