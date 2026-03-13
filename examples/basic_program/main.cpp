// default window name and ID for root Dear ImGui window
#define APP_NAME "basic_program"

// handle Dear ImGui context creation, event processing and rendering
#define LAK_BASIC_PROGRAM_IMGUI_IMPL

// create a full screen Dear ImGui window surrounding calls to window loop
#define LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL

// can be included in other source/header files if access to these functions is
// needed from elsewhere (ensure config defines are set first)
// #include <lak/basic_program.hpp>

#include <lak/basic_program.inl> // include .inl file only once (typically your
                                 // main.cpp or equivalent)

#include <lak/optional.hpp>

#include <implot.h>

#include <filesystem>

struct my_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	my_window() : LAK_BASIC_PROGRAM(window_api)() {}

	// store any member variables needed by this window here so they can be
	// accessed during init/handle_event/loop

	lak::optional<std::filesystem::path> dropfile;
	lak::optional<std::filesystem::path> openfile;
	lak::path_getter pgetter;

	ImTextureRef checker;

	ImPlotContext *implot_ctx = nullptr;

	virtual void init() override final
	{
		// called once window has been set up

		window().set_title(L"something other than " APP_NAME);

		lak::image<lak::vec3u8_t> checker_img;
		checker_img.resize(lak::vec2s_t{30U, 30U});
		for (size_t y = 0U; y < checker_img.size().y; ++y)
			for (size_t x = 0U; x < checker_img.size().x; ++x)
				checker_img[{x, y}].r = checker_img[{x, y}].g = checker_img[{x, y}].b =
				  (((x + y) & 1U) * 255U);
		checker = lak::CreateTexture(checker_img);

		implot_ctx = ImPlot::CreateContext();
	}

	virtual ~my_window()
	{
		lak::DestroyTexture(checker);
		if (implot_ctx) ImPlot::DestroyContext(implot_ctx);
	}

	virtual void handle_event(lak::event &event) override final
	{
		// called whenever the window receives an event
		switch (event.type)
		{
			case lak::event_type::close_window:
				destroy(); // window is added to the destroy queue (flushed at end of
				           // the current frame)
				break;
			case lak::event_type::dropfile: dropfile = event.dropfile().path; break;
		}
	}

	virtual void loop(uint64_t counter_delta) override final
	{
		// called once every frame

		ImPlot::SetCurrentContext(implot_ctx);

		ImGui::Text("Frame time: %01.2fms",
		            ((float)counter_delta * 1000U) / lak::performance_frequency());

		lak::window &wnd = window();
		ImGui::Text("Window size: %lu x %lu", wnd.size().x, wnd.size().y);
		ImGui::Text("Drawable size: %lu x %lu",
		            wnd.drawable_size().x,
		            wnd.drawable_size().y);

		if (ImGui::Button("Open file")) pgetter.open_file();
		if_let_some (auto pget, pgetter()) openfile = lak::move(pget);
		if_let_some (auto &path, openfile)
			ImGui::Text("Opened file: %s", path.generic_string().c_str());

		if_let_some (auto &path, dropfile)
			ImGui::Text("Dropped file: %s", path.generic_string().c_str());

		ImGui::Image(checker, ImVec2(200, 200));

		bool implot_demo_open = true;
		ImPlot::ShowDemoWindow(&implot_demo_open);

		bool demo_open = true;
		ImGui::ShowDemoWindow(&demo_open);
	}
};

// lak::error_code<int> -> lak::result<lak::monostate, int>
lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *> args)
{
	// called at program startup

	// return lak::ok_t{}: continue onto program_init
	// return lak::err_t{int}: quit program with that exit code
	return lak::ok_t{};
}

lak::weak_ptr<LAK_BASIC_PROGRAM(window_instance<my_window>)> my_window_ptr;

lak::error_code<int> LAK_BASIC_PROGRAM(program_init)()
{
	// called after program_preinit and once all platform initialisation is
	// complete. you can start creating windows now

	auto map_str_err = [](lak::u8string err) -> int
	{
		ERROR(err);
		return EXIT_FAILURE;
	};

	// try macros can be used thanks to the result type return value
	RES_TRY_ASSIGN(
	  my_window_ptr =,
	  LAK_BASIC_PROGRAM(create_window<my_window>)().map_err(map_str_err));
	// by not specifying a specific graphics settings struct, create_window will
	// attempt to find the first working graphics backend (settings for each are
	// pulled from the global LAK_BASIC_PROGRAM(window_*_settings) structs).

	DEBUG_EXPR(my_window_ptr.get()->window().graphics());

	// return lak::ok_t{}: continue onto program_loop
	// return lak::err_t{int}: quit program with that exit code
	return lak::ok_t{};
}

void LAK_BASIC_PROGRAM(program_handle_event)(lak::event &event)
{
	// handle non-window events
}

bool LAK_BASIC_PROGRAM(program_loop)(uint64_t counter_delta)
{
	// called once per frame

	// return true: continue program execution
	// return false: stop main program loop, continues to program_quit
	return !LAK_BASIC_PROGRAM(window_instances)().empty();
}

int LAK_BASIC_PROGRAM(program_quit)()
{
	// called after program_loop has returned false and after basic_program's
	// window bank has been cleared (if you are holding onto any window
	// pointers, clear them here)

	my_window_ptr.reset(); // this was only a weak pointer, so the window should
	                       // have already been destroyed, but this will still
	                       // deallocate the memory now

	// return value used as program exit code
	return EXIT_SUCCESS;
}
