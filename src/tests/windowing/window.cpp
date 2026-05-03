#include "lak/test.hpp"

#include "lak/string_literals/string.hpp"

#include "lak/system/windowing/events.hpp"
#include "lak/system/windowing/platform.hpp"
#include "lak/system/windowing/window.hpp"

#ifdef LAK_ENABLE_SOFTRENDER
#	ifdef LAK_RUN_WINDOWING_TESTS
BEGIN_TEST(softrender_window)
#	else
int softrender_window_compile_test()
#	endif
{
	DEBUG("platform init");
	ASSERT(lak::platform_init());

	{
		DEBUG("create window");
		lak::window w = lak::window::make(lak::software_settings{}).UNWRAP();

		w.set_title(L"Test Window");
		auto title = w.title();
		ASSERT_EQUAL(title.c_str(), L"Test Window"_str);

		ASSERT_EQUAL(w.graphics(), lak::graphics_mode::Software);

		DEBUG("starting event loop");
		for (bool running = true; running;)
		{
			for (lak::event e; lak::next_event(&e);)
			{
				switch (e.type)
				{
					case lak::event_type::close_window: [[fallthrough]];
					case lak::event_type::quit_program: running = false; break;
					default:                            break;
				}
			}

			w.swap();
		}
		DEBUG("event loop finished");
	}

	DEBUG("platform quit");
	lak::platform_quit();

	return 0;
}
#	ifdef LAK_RUN_WINDOWING_TESTS
END_TEST()
#	endif
#endif

#ifdef LAK_ENABLE_OPENGL
#	include "lak/system/opengl/state.hpp"

#	ifdef LAK_RUN_WINDOWING_TESTS
BEGIN_TEST(opengl_window)
#	else
int opengl_window_compile_test()
#	endif
{
	DEBUG("platform init");
	ASSERT(lak::platform_init());

	{
		DEBUG("create window");
		lak::window w = lak::window::make(lak::opengl_settings{}).UNWRAP();

		w.set_title(L"Test Window");
		auto title = w.title();
		ASSERT_EQUAL(title.c_str(), L"Test Window"_str);

		ASSERT_EQUAL(w.graphics(), lak::graphics_mode::OpenGL);

		DEBUG("starting event loop");
		for (bool running = true; running;)
		{
			for (lak::event e; lak::next_event(&e);)
			{
				switch (e.type)
				{
					case lak::event_type::close_window: [[fallthrough]];
					case lak::event_type::quit_program: running = false; break;
					default:                            break;
				}
			}

			w.set_active();

			glViewport(0, 0, w.drawable_size().x, w.drawable_size().y);
			glScissor(0, 0, w.drawable_size().x, w.drawable_size().y);
			glClearColor(0.0f, 0.3125f, 0.312f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
			        GL_STENCIL_BUFFER_BIT);

			w.swap();
		}
		DEBUG("event loop finished");
	}

	DEBUG("platform quit");
	lak::platform_quit();

	return 0;
}
#	ifdef LAK_RUN_WINDOWING_TESTS
END_TEST()
#	endif
#endif

#ifdef LAK_ENABLE_COBALT

#	ifdef LAK_RUN_WINDOWING_TESTS
BEGIN_TEST(cobalt_window)
#	else
int cobalt_window_compile_test()
#	endif
{
	DEBUG("platform init");
	ASSERT(lak::platform_init());

	{
		DEBUG("create window");
		auto [w, r] = lak::window::make(lak::cobalt_settings{}).UNWRAP();

		w.set_title(L"Test Window");
		auto title = w.title();
		ASSERT_EQUAL(title.c_str(), L"Test Window"_str);

		ASSERT_EQUAL(w.graphics(), lak::graphics_mode::Cobalt);

		DEBUG("starting event loop");
		for (bool running = true; running;)
		{
			for (lak::event e; lak::next_event(&e);)
			{
				switch (e.type)
				{
					case lak::event_type::close_window: [[fallthrough]];
					case lak::event_type::quit_program: running = false; break;
					default:                            break;
				}
			}

			w.set_active();

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
			  ::cobalt::graphics::V4Float32{0.0f, 0.3125f, 0.312f, 1.0f});
			rp->SetAttachmentClearData(
			  ::cobalt::graphics::IFrameBuffer::AttachmentType::Depth,
			  0,
			  ::cobalt::graphics::V4Float32(1.0f, 1.0f, 1.0f, 1.0f));

			w.swap();
		}
		DEBUG("event loop finished");
	}

	DEBUG("platform quit");
	lak::platform_quit();

	return 0;
}
#	ifdef LAK_RUN_WINDOWING_TESTS
END_TEST()
#	endif
#endif
