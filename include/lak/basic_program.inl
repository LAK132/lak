#include "lak/basic_program.hpp"

#ifdef LAK_ENABLE_OPENGL
#	include "lak/system/opengl/state.hpp"
#endif

#ifdef LAK_ENABLE_COBALT
#	include "lak/system/cobalt/log_target.hpp"
#endif

#ifndef APP_NAME
#	define APP_NAME "basic window"
#endif

// Set these inside of program_preinit or program_init.
uint32_t LAK_BASIC_PROGRAM(window_target_framerate) = 60;
bool LAK_BASIC_PROGRAM(window_force_software)       = false;
lak::vec2l_t LAK_BASIC_PROGRAM(window_start_size)   = {1200, 700};
lak::cobalt_settings LAK_BASIC_PROGRAM(window_cobalt_settings);
lak::optional<lak::cobalt_renderer_settings> LAK_BASIC_PROGRAM(
  window_cobalt_renderer_settings);
lak::opengl_settings LAK_BASIC_PROGRAM(window_opengl_settings);
lak::software_settings LAK_BASIC_PROGRAM(window_software_settings);

bool LAK_BASIC_PROGRAM(platform_initialised) = false;
lak::array<LAK_BASIC_PROGRAM(window_instance_base) *> LAK_BASIC_PROGRAM(
  window_destroy_queue);

lak::array<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance_base)>>
  LAK_BASIC_PROGRAM(window_instances_bank);

lak::span<const lak::strong_ref<LAK_BASIC_PROGRAM(window_instance_base)>>
LAK_BASIC_PROGRAM(window_instances)()
{
	return LAK_BASIC_PROGRAM(window_instances_bank);
}

lak::strong_ptr<LAK_BASIC_PROGRAM(window_instance_base)> LAK_BASIC_PROGRAM(
  find_window_instance)(const lak::window_handle *window)
{
	auto p = lak::find_if(LAK_BASIC_PROGRAM(window_instances_bank).begin(),
	                      LAK_BASIC_PROGRAM(window_instances_bank).end(),
	                      [&](auto &inst)
	                      { return inst->window().handle() == window; });
	if (p != LAK_BASIC_PROGRAM(window_instances_bank).end())
		return *p;
	else
		return {};
}

#ifdef LAK_ENABLE_OPENGL
void APIENTRY
  LAK_BASIC_PROGRAM(opengl_debug_message_callback)(GLenum source,
                                                   GLenum type,
                                                   GLuint id,
                                                   GLenum severity,
                                                   GLsizei length,
                                                   const GLchar *message,
                                                   const void *userParam);
#endif

template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string> LAK_BASIC_PROGRAM(create_window)(lak::window &&wnd)
{
	RES_TRY_ASSIGN(
	  auto result =,
	  lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>::make(
	    lak::move(wnd))
	    .map_err([](auto) -> lak::u8string
	             { return u8"Failed to create window instance"; }));

	LAK_BASIC_PROGRAM(window_instances_bank).emplace_back(result);

#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
	auto current_context = ImGui::GetCurrentContext();
	DEFER(ImGui::SetCurrentContext(current_context));

	result->imgui_context =
	  ImGui::ImplCreateContext(result->window().graphics());
	ImGui::ImplSetCurrentContext(result->imgui_context);
	ImGui::ImplInit();
	ImGui::ImplInitContext(result->imgui_context, result->window());

	if (result->window().graphics() == lak::graphics_mode::Software)
	{
		ImGuiStyle &style      = ImGui::GetStyle();
		style.AntiAliasedLines = false;
		style.AntiAliasedFill  = false;
		style.WindowRounding   = 0.0f;
	}

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowRounding = 0;

	lak::init_file_modal();
#endif

	result->window().set_title(L"" APP_NAME);
	result->window().set_size(LAK_BASIC_PROGRAM(window_start_size));

	result->init();
	return lak::move_ok(result);
}

#ifdef LAK_ENABLE_SOFTRENDER
template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string>
LAK_BASIC_PROGRAM(create_window)(const lak::software_settings &settings)
{
	RES_TRY_ASSIGN(
	  auto wnd =,
	  lak::window::make(settings).and_then(
	    [&](auto &&window) -> lak::result<lak::window, lak::u8string>
	    {
		    if (window.graphics() != lak::graphics_mode::Software)
			    return lak::err_t<lak::u8string>{lak::streamify(
			      "Expected Software graphics, got ", window.graphics())};
		    return lak::move_ok(window);
	    }));
	return LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(lak::move(wnd));
}
#endif

#ifdef LAK_ENABLE_OPENGL
template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string>
LAK_BASIC_PROGRAM(create_window)(const lak::opengl_settings &settings)
{
	auto mapper = [&](auto &&window) -> lak::result<lak::window, lak::u8string>
	{
		if (window.graphics() != lak::graphics_mode::OpenGL)
			return lak::err_t<lak::u8string>{
			  lak::streamify("Expected OpenGL graphics, got ", window.graphics())};

		glViewport(0, 0, window.drawable_size().x, window.drawable_size().y);
		glEnable(GL_DEPTH_TEST);

#	ifndef NDEBUG
#		ifndef LAK_OS_APPLE
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(&LAK_BASIC_PROGRAM(opengl_debug_message_callback),
		                       0);
#		endif
#	endif

		return lak::move_ok(window);
	};
	RES_TRY_ASSIGN(auto wnd =, lak::window::make(settings).and_then(mapper));
	return LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(lak::move(wnd));
}
#endif

#ifdef LAK_ENABLE_COBALT
template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string>
LAK_BASIC_PROGRAM(create_window)(const lak::cobalt_settings &settings,
                                 const lak::cobalt_renderer_settings &renderer)
{
	RES_TRY_ASSIGN(
	  auto wnd =,
	  lak::window::make(settings, renderer)
	    .and_then(
	      [&](auto &&window) -> lak::result<lak::window, lak::u8string>
	      {
		      if (window.graphics() != lak::graphics_mode::Cobalt)
			      return lak::err_t<lak::u8string>{lak::streamify(
			        "Expected Cobalt graphics, got ", window.graphics())};

		      auto *fb = lak::cobalt_graphics_context(window.handle())
		                   .UNWRAP()
		                   .frame_buffer.get();
		      fb->DefineViewportRegion({0, 0},
		                               {uint32_t(window.drawable_size().x),
		                                uint32_t(window.drawable_size().y)});

		      return lak::move_ok(window);
	      }));
	return LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(lak::move(wnd));
}

template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string>
LAK_BASIC_PROGRAM(create_window)(const lak::cobalt_settings &settings)
{
	lak::u8string errs;

	if (LAK_BASIC_PROGRAM(window_cobalt_renderer_settings))
	{
		RES_TRY_ASSIGN_ERR(
		  errs +=,
		  LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(
		    settings, *LAK_BASIC_PROGRAM(window_cobalt_renderer_settings)));
		errs += u8"\n";
	}

	RES_TRY_ASSIGN(auto each_rsettings =,
	               lak::cobalt_renderer_settings::each_preferred().map_err(
	                 [&](auto &&) -> lak::u8string
	                 {
		                 errs += u8"Failed to get preferred renderer settings";
		                 return lak::move(errs);
	                 }));

	for (auto &rsettings : each_rsettings)
	{
		auto set_settings = [&](auto &&)
		{
			LAK_BASIC_PROGRAM(window_cobalt_renderer_settings) =
			  lak::move(rsettings);
		};

		RES_TRY_ASSIGN_ERR(
		  errs +=,
		  LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(settings, rsettings)
		    .if_ok(set_settings));
		errs += u8"\n";
	}

	return lak::move_err(errs);
}
#endif

template<typename WINDOW_CLASS>
lak::result<lak::strong_ref<LAK_BASIC_PROGRAM(window_instance<WINDOW_CLASS>)>,
            lak::u8string> LAK_BASIC_PROGRAM(create_window)()
{
	ASSERT(LAK_BASIC_PROGRAM(platform_initialised));

#if !defined(LAK_ENABLE_SOFTRENDER)
	if (LAK_BASIC_PROGRAM(window_force_software))
		WARNING(
		  "Force software rendering setting was set but "
		  "software rendering was not enabled");
#endif

	// backend priority: (forced software >) cobalt > opengl > software
	return
#if defined(LAK_ENABLE_COBALT) && !defined(LAK_ENABLE_SOFTRENDER)
	  LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(
	    LAK_BASIC_PROGRAM(window_cobalt_settings))
#elif defined(LAK_ENABLE_OPENGL) && !defined(LAK_ENABLE_SOFTRENDER)
	  LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(
	    LAK_BASIC_PROGRAM(window_opengl_settings))
#elif !(defined(LAK_ENABLE_OPENGL) || defined(LAK_ENABLE_COBALT))
	  LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(
	    LAK_BASIC_PROGRAM(window_software_settings))
#else
	  (LAK_BASIC_PROGRAM(window_force_software)
	     ? LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(
	         LAK_BASIC_PROGRAM(window_software_settings))
	     :
#	ifdef LAK_ENABLE_COBALT
	     LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(
	       LAK_BASIC_PROGRAM(window_cobalt_settings))
	       .if_err([](const lak::u8string &err)
	               { WARNING("Failed to create a Cobalt window: ", err); })
#	else
	     LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(
	       LAK_BASIC_PROGRAM(window_opengl_settings))
	       .if_err([](const lak::u8string &err)
	               { WARNING("Failed to create an OpenGL window: ", err); })
#	endif
	       .or_else(
	         [&](const lak::u8string &err)
	         {
		         WARNING(err);
		         WARNING("Attempting to create a Software window instead");
		         return LAK_BASIC_PROGRAM(create_window<WINDOW_CLASS>)(
		           LAK_BASIC_PROGRAM(window_software_settings));
	         }))
#endif
	    ;
}

void LAK_BASIC_PROGRAM(window_instance_base)::destroy()
{
	ASSERT(LAK_BASIC_PROGRAM(platform_initialised));

	LAK_BASIC_PROGRAM(window_destroy_queue).emplace_back(this);
}

LAK_BASIC_PROGRAM(window_instance_base)::~LAK_BASIC_PROGRAM(
  window_instance_base)()
{
	ASSERT(LAK_BASIC_PROGRAM(platform_initialised));

	window().set_active();

#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
	{
		auto ctx = ImGui::GetCurrentContext();
		DEFER(ImGui::SetCurrentContext(ctx));
		ImGui::ImplSetCurrentContext(imgui_context);
		ImGui::ImplShutdownContext(imgui_context);
		ImGui::ImplDestroyContext(imgui_context);
	}
#endif
	// #ifdef LAK_ENABLE_OPENGL
	// #	ifndef NDEBUG
	// 		if (window.graphics() == lak::graphics_mode::OpenGL)
	// 		{
	// 			glDisable(GL_DEBUG_OUTPUT);
	// 		}
	// #	endif
	// #endif
}

#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
ImGui::ImplContext LAK_BASIC_PROGRAM(imgui_context) = nullptr;
#endif

#ifdef LAK_ENABLE_OPENGL
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
		case GL_DEBUG_TYPE_ERROR:       DEBUG("| Type: ERROR"); break;
		case GL_DEBUG_TYPE_MARKER:      DEBUG("| Type: MARKER"); break;
		case GL_DEBUG_TYPE_OTHER:       DEBUG("| Type: OTHER"); break;
		case GL_DEBUG_TYPE_PERFORMANCE: DEBUG("| Type: PERFORMANCE"); break;
		case GL_DEBUG_TYPE_POP_GROUP:   DEBUG("| Type: POP GROUP"); break;
		case GL_DEBUG_TYPE_PORTABILITY: DEBUG("| Type: PORTABILITY"); break;
		case GL_DEBUG_TYPE_PUSH_GROUP:  DEBUG("| Type: PUSH GROUP"); break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			DEBUG("| Type: UNDEFINED BEHAVIOR");
			break;
		default: DEBUG("| Type: ", type); break;
	}
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:   DEBUG("| Severity: HIGH"); break;
		case GL_DEBUG_SEVERITY_MEDIUM: DEBUG("| Severity: MEDIUM"); break;
		case GL_DEBUG_SEVERITY_LOW:    DEBUG("| Severity: LOW"); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			DEBUG("| Severity: NOTIFICATION");
			break;
		default: DEBUG("| Severity: ", severity); break;
	}
	switch (source)
	{
		case GL_DEBUG_SOURCE_API:         DEBUG("| Source: API"); break;
		case GL_DEBUG_SOURCE_APPLICATION: DEBUG("| Source: APPLICATION"); break;
		case GL_DEBUG_SOURCE_OTHER:       DEBUG("| Source: OTHER"); break;
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
#endif

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

	if_let_err (auto v,
	            LAK_BASIC_PROGRAM(program_preinit)(
	              lak::span<char *>(argv, size_t(argc))))
		return v;

	lak::platform_init();
	LAK_BASIC_PROGRAM(platform_initialised) = true;
	DEFER(LAK_BASIC_PROGRAM(platform_initialised) = false);
	DEFER(lak::platform_quit());

	if_let_err (auto v, LAK_BASIC_PROGRAM(program_init)()) return v;

	uint64_t last_counter = lak::performance_counter();
	uint64_t counter_delta =
	  lak::performance_frequency() / LAK_BASIC_PROGRAM(window_target_framerate);

	while (LAK_BASIC_PROGRAM(program_loop)(counter_delta))
	{
		for (lak::event event; lak::next_event(&event);)
		{
			if (event.handle)
			{
				auto inst = LAK_BASIC_PROGRAM(find_window_instance)(event.handle);
				// ASSERT(!!inst);
				if (!inst) continue;
#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
				ImGui::ImplSetCurrentContext(inst->imgui_context);
				ImGui::ImplProcessEvent(inst->imgui_context, event);
#endif
				inst->handle_event(event);
			}
			else
				LAK_BASIC_PROGRAM(program_handle_event)(event);
		}

		for (const auto &_inst : LAK_BASIC_PROGRAM(window_instances_bank))
		{
			auto &inst = *_inst;

			auto &window = inst.window();

			window.set_active();

#ifdef LAK_ENABLE_OPENGL
			if (window.graphics() == lak::graphics_mode::OpenGL)
			{
				glViewport(0, 0, window.drawable_size().x, window.drawable_size().y);
				glScissor(0, 0, window.drawable_size().x, window.drawable_size().y);
				glClearColor(inst.clear_colour.r,
				             inst.clear_colour.g,
				             inst.clear_colour.b,
				             inst.clear_colour.a);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
				        GL_STENCIL_BUFFER_BIT);
			}
#endif

#ifdef LAK_ENABLE_COBALT
			if (window.graphics() == lak::graphics_mode::Cobalt)
			{
				const auto &cgx =
				  lak::cobalt_graphics_context(window.handle()).UNWRAP();
				auto *rd = cgx.renderer.get();
				auto *fb = cgx.frame_buffer.get();
				fb->DefineViewportRegion({0, 0},
				                         {uint32_t(window.drawable_size().x),
				                          uint32_t(window.drawable_size().y)});
				fb->DefineScissorRegion({0, 0},
				                        {uint32_t(window.drawable_size().x),
				                         uint32_t(window.drawable_size().y)});
				auto rp = lak::cobalt_create_render_pass(window.handle()).UNWRAP();
				rp->SetAttachmentClearData(
				  ::cobalt::graphics::IFrameBuffer::AttachmentType::Color,
				  0,
				  ::cobalt::graphics::V4Float32{inst.clear_colour.r,
				                                inst.clear_colour.g,
				                                inst.clear_colour.b,
				                                inst.clear_colour.a});
				rp->SetAttachmentClearData(
				  ::cobalt::graphics::IFrameBuffer::AttachmentType::Depth,
				  0,
				  ::cobalt::graphics::V4Float32(1.0f, 1.0f, 1.0f, 1.0f));
			}
#endif

#ifdef LAK_BASIC_PROGRAM_IMGUI_IMPL
			{
				const float frame_time =
				  (float)counter_delta / lak::performance_frequency();
				ImGui::ImplSetCurrentContext(inst.imgui_context);
				ImGui::ImplNewFrame(inst.imgui_context, window, frame_time);

#	ifdef LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL
				bool mainOpen = true;

				ImGuiStyle &style = ImGui::GetStyle();
				ImGuiIO &io       = ImGui::GetIO();

				ImGui::SetNextWindowPos(ImVec2(0, 0));
				ImGui::SetNextWindowSize(io.DisplaySize);
				ImVec2 old_window_padding = style.WindowPadding;
				style.WindowPadding       = ImVec2(0.0f, 0.0f);
				if (ImGui::Begin(APP_NAME, &mainOpen, inst.imgui_window_flags))
				{
					style.WindowPadding = old_window_padding;
					inst.loop(counter_delta);
				}
				ImGui::End();
#	else
				inst.loop(counter_delta);
#	endif

				ImGui::ImplRender(inst.imgui_context);
				lak::flush_file_modal();
			}
#else
			inst.loop(counter_delta);
#endif

			window.swap();
		}

		if (!LAK_BASIC_PROGRAM(window_destroy_queue).empty())
		{
			LAK_BASIC_PROGRAM(window_instances_bank)
			  .erase(lak::erase_if_contains(
			           LAK_BASIC_PROGRAM(window_instances_bank).begin(),
			           LAK_BASIC_PROGRAM(window_instances_bank).end(),
			           LAK_BASIC_PROGRAM(window_destroy_queue).begin(),
			           LAK_BASIC_PROGRAM(window_destroy_queue).end(),
			           [](const auto &p1, const auto *p2)
			           { return lak::equal_to<>{}(p1.get(), p2); }),
			         LAK_BASIC_PROGRAM(window_instances_bank).end());
			LAK_BASIC_PROGRAM(window_destroy_queue).clear();
		}

		const auto counter = lak::yield_frame(
		  last_counter, LAK_BASIC_PROGRAM(window_target_framerate));
		counter_delta = counter - last_counter;
		last_counter  = counter;
	}

	LAK_BASIC_PROGRAM(window_instances_bank).clear();

	return LAK_BASIC_PROGRAM(program_quit)();
}
