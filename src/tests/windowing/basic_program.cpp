#define LAK_BASIC_PROGRAM_MAIN   lak_test_basic_program_main
#define LAK_BASIC_PROGRAM_PREFIX lak_test_basic_

#include "lak/test.hpp"

#include "lak/basic_program.inl"

lak::optional<int> lak_test_basic_window_preinit(int, char **)
{
	FUNCTION_CHECKPOINT();
	return lak::nullopt;
}

void lak_test_basic_window_init(lak::window &window)
{
	FUNCTION_CHECKPOINT();
	window.set_title(L"Test Window");
	ASSERT_EQUAL(window.title(), L"Test Window");

	if (lak_test_basic_window_force_software)
		ASSERT(window.graphics() == lak::graphics_mode::Software);
}

void lak_test_basic_window_handle_event(lak::window &, lak::event &)
{
	//
}

void lak_test_basic_window_loop(lak::window &, uint64_t)
{
	//
}

int lak_test_basic_window_quit(lak::window &)
{
	FUNCTION_CHECKPOINT();
	return EXIT_SUCCESS;
}

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
