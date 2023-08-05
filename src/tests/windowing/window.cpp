#include "lak/test.hpp"

#include "lak/events.hpp"
#include "lak/platform.hpp"
#include "lak/window.hpp"

#ifndef LAK_ENABLE_SOFTRENDER
#	error Test requires softrender to be enabled
#endif

#ifdef LAK_RUN_WINDOWING_TESTS
BEGIN_TEST(window)
#else
int window_compile_test()
#endif
{
	DEBUG("platform init");
	ASSERT(lak::platform_init());

	{
		DEBUG("create window");
		lak::window w = lak::window::make(lak::software_settings{}).UNWRAP();

		w.set_title(L"Test Window");
		ASSERT_EQUAL(w.title(), L"Test Window");

		DEBUG("starting event loop");
		for (bool running = true; running;)
		{
			for (lak::event e; lak::next_event(&e);)
			{
				switch (e.type)
				{
					case lak::event_type::close_window:
						[[fallthrough]];
					case lak::event_type::quit_program:
						running = false;
						break;
					default:
						break;
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
#ifdef LAK_RUN_WINDOWING_TESTS
END_TEST()
#endif
