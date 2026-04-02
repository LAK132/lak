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
#include <lak/string_literals/string.hpp>

#include <lak/system/file.hpp>

#include <implot.h>

#include <filesystem>

struct my_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	my_window() : LAK_BASIC_PROGRAM(window_api)() {}

	// store any member variables needed by this window here so they can be
	// accessed during init/handle_event/loop

	lak::optional<std::filesystem::path> dropfile;
	lak::optional<std::filesystem::path> openfile;
	lak::optional<size_t> openfile_size;
	lak::path_getter pgetter;

	ImTextureRef checker;

	ImPlotContext *implot_ctx = nullptr;

	lak::ImViewport viewport = nullptr;

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

		viewport = lak::CreateViewport(ImGui::ImplTextureColourFormat::RGBA,
		                               ImGui::ImplTextureChannelFormat::U8);
	}

	virtual ~my_window()
	{
		lak::DestroyTexture(checker);
		if (implot_ctx) ImPlot::DestroyContext(implot_ctx);
		if (viewport) lak::DestroyViewport(viewport);
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
		if_let_some (auto pget, pgetter())
		{
			openfile      = lak::move(pget);
			openfile_size = lak::nullopt;
		}
		if_let_some (auto &path, openfile)
		{
			ImGui::Text("Opened file: %s", path.generic_string().c_str());
			ImGui::SameLine();
			if (ImGui::Button("Load (map)"))
			{
				if_let_ok (auto f, lak::map_file(path).IF_ERR())
				{
					DEBUG_EXPR(f.data.size());
					openfile_size = f.data.size();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Load (read)"))
			{
				if_let_ok (auto f, lak::read_file(path).IF_ERR())
				{
					DEBUG_EXPR(f.size());
					openfile_size = f.size();
				}
			}
		}
		if_let_some (size_t size, openfile_size)
		{
			ImGui::Text("%s", lak::fmt<"Opened file size: {}">(size).c_str());
		}

		if_let_some (auto &path, dropfile)
			ImGui::Text("Dropped file: %s", path.generic_string().c_str());

		ImGui::Image(checker, ImVec2(200, 200));

		{
			lak::vec4f_t clear_colour{0.5f, 0.3125f, 0.3125f, 1.0f};

			bool viewport_clicked = false;
			auto vp =
			  lak::BeginViewport(viewport, ImVec2(200, 200), &viewport_clicked);

			vp.visit(lak::overloaded{
			  [&](const lak::ImSRViewportDetails &vpd)
			  {
				  (void)vpd;
#ifdef LAK_ENABLE_SOFTRENDER
				  ASSERT_EQUAL(vpd.framebuffer->type, texture_type_t::COLOR32);
				  auto *fb = (texture_color32_t *)vpd.framebuffer;
				  color32_t cc{uint8_t(clear_colour.r * 255),
				               uint8_t(clear_colour.g * 255),
				               uint8_t(clear_colour.b * 255),
				               uint8_t(clear_colour.a * 255)};
				  for (size_t y = 0; y < fb->h; ++y)
					  for (size_t x = 0; x < fb->w; ++x) fb->at(x, y) = cc;
#endif
			  },
			  [&](const lak::ImGLViewportDetails &vpd)
			  {
				  (void)vpd;
#ifdef LAK_ENABLE_OPENGL
				  glClearColor(
				    clear_colour.r, clear_colour.g, clear_colour.b, clear_colour.a);
				  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
			  },
			  [&](const lak::ImCoViewportDetails &vpd)
			  {
				  (void)vpd;
#ifdef LAK_ENABLE_COBALT
				  vpd.clear_passes();
				  auto clear_pass = vpd.append_pass();
				  clear_pass->SetAttachmentClearData(
				    ::cobalt::graphics::IFrameBuffer::AttachmentType::Color,
				    0,
				    ::cobalt::graphics::V4Float32{
				      clear_colour.r, clear_colour.g, clear_colour.b, clear_colour.a});
				  clear_pass->SetAttachmentClearData(
				    ::cobalt::graphics::IFrameBuffer::AttachmentType::Depth,
				    0,
				    ::cobalt::graphics::V4Float32{1.f, 1.f, 1.f, 1.f});
#endif
			  },
			});

			lak::EndViewport(viewport);
			if (viewport_clicked)
			{
				DEBUG("Viewport clicked!");
			}
		}

		bool implot_demo_open = true;
		ImPlot::ShowDemoWindow(&implot_demo_open);

		bool demo_open = true;
		ImGui::ShowDemoWindow(&demo_open);
	}
};

lak::graphics_mode forced_graphics_mode = lak::graphics_mode::None;

// lak::error_code<int> -> lak::result<lak::monostate, int>
lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *> args)
{
	// called at program startup

	if (args.size() >= 2U)
	{
		if (args[1] == "--software"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Software;
		}
		else if (args[1] == "--opengl"_str)
		{
			forced_graphics_mode = lak::graphics_mode::OpenGL;
		}
		else if (args[1] == "--cobalt"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Cobalt;
		}
	}

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

	switch (forced_graphics_mode)
	{
		case lak::graphics_mode::None:
		{
			// try macros can be used thanks to the result type return value
			RES_TRY_ASSIGN(
			  my_window_ptr =,
			  LAK_BASIC_PROGRAM(create_window<my_window>)().map_err(map_str_err));
			// by not specifying a specific graphics settings struct, create_window
			// will attempt to find the first working graphics backend (settings for
			// each are pulled from the global LAK_BASIC_PROGRAM(window_*_settings)
			// structs).
		}
		break;
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
		{
			RES_TRY_ASSIGN(my_window_ptr =,
			               LAK_BASIC_PROGRAM(create_window<my_window>)(
			                 LAK_BASIC_PROGRAM(window_software_settings))
			                 .map_err(map_str_err));
		}
		break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			RES_TRY_ASSIGN(my_window_ptr =,
			               LAK_BASIC_PROGRAM(create_window<my_window>)(
			                 LAK_BASIC_PROGRAM(window_opengl_settings))
			                 .map_err(map_str_err));
		}
		break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
		{
			RES_TRY_ASSIGN(my_window_ptr =,
			               LAK_BASIC_PROGRAM(create_window<my_window>)(
			                 LAK_BASIC_PROGRAM(window_cobalt_settings))
			                 .map_err(map_str_err));
		}
		break;
#endif
		default:
			ERROR(
			  lak::fmt<u8"Graphics mode {} not available">(forced_graphics_mode));
			return lak::err_t{EXIT_FAILURE};
	}

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
