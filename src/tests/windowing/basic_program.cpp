#define LAK_BASIC_PROGRAM_MAIN   lak_test_basic_program_main
#define LAK_BASIC_PROGRAM_PREFIX lak_test_basic_

#include "lak/test.hpp"

#include "lak/basic_program.inl"

lak::optional<int> lak_test_basic_program_preinit(lak::span<char *>)
{
	FUNCTION_CHECKPOINT();
	return lak::nullopt;
}

static bool running = true;
lak::optional<int> lak_test_basic_program_init()
{
	FUNCTION_CHECKPOINT();
	running = true;
	lak_test_basic_create_window().UNWRAP();
	return lak::nullopt;
}

bool lak_test_basic_program_loop(uint64_t counter_delta)
{
	LAK_UNUSED(counter_delta);
	FUNCTION_CHECKPOINT();
	return running;
}

int lak_test_basic_program_quit()
{
	FUNCTION_CHECKPOINT();
	return EXIT_SUCCESS;
}

void lak_test_basic_window_init(lak::window &window)
{
	FUNCTION_CHECKPOINT();
	window.set_title(L"Test Window");
	auto title = window.title();
	ASSERT_EQUAL(lak::wstring_view::from_c_str(title.c_str()),
	             L"Test Window"_view);

	if (lak_test_basic_window_force_software)
		ASSERT(window.graphics() == lak::graphics_mode::Software);
}

void lak_test_basic_window_handle_event(lak::window *, lak::event &)
{
	FUNCTION_CHECKPOINT();
}

void lak_test_basic_window_loop(lak::window &, uint64_t)
{
	FUNCTION_CHECKPOINT();
	running = false;
}

void lak_test_basic_window_quit(lak::window &) { FUNCTION_CHECKPOINT(); }

#if !(defined(LAK_ENABLE_OPENGL) || defined(LAK_ENABLE_VULKAN) ||             \
      defined(LAK_ENABLE_METAL) || defined(LAK_ENABLE_SOFTRENDER))
#	error At least one renderer must be enabled
#endif

#ifdef LAK_RUN_WINDOWING_TESTS
BEGIN_TEST(basic_program)
#else
int basic_program_compile_test()
#endif
{
#if defined(LAK_ENABLE_OPENGL) || defined(LAK_ENABLE_VULKAN) ||               \
  defined(LAK_ENABLE_METAL)
	DEBUG("attempting to launch hardware rendered window");
	lak_test_basic_window_force_software = false;
	if (int err = LAK_BASIC_PROGRAM_MAIN(0, nullptr); err != EXIT_SUCCESS)
		return err;
#endif

#ifdef LAK_ENABLE_SOFTRENDER
	DEBUG("attempting to launch software rendered window");
	lak_test_basic_window_force_software = true;
	if (int err = LAK_BASIC_PROGRAM_MAIN(0, nullptr); err != EXIT_SUCCESS)
		return err;
#endif

	return EXIT_SUCCESS;
}
#ifdef LAK_RUN_WINDOWING_TESTS
END_TEST()
#endif
