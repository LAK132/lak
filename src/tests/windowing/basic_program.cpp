#define LAK_BASIC_PROGRAM_MAIN basic_program_main

#include "lak/test.hpp"

#include "lak/basic_program.inl"

lak::optional<int> basic_window_preinit(int, char **)
{
	FUNCTION_CHECKPOINT();
	return lak::nullopt;
}

void basic_window_init(lak::window &window)
{
	FUNCTION_CHECKPOINT();
	window.set_title(L"Test Window");
	ASSERT_EQUAL(window.title(), L"Test Window");

	if (basic_window_force_software)
		ASSERT(window.graphics() == lak::graphics_mode::Software);
}

void basic_window_handle_event(lak::window &, lak::event &)
{
	//
}

void basic_window_loop(lak::window &, uint64_t)
{
	//
}

int basic_window_quit(lak::window &)
{
	FUNCTION_CHECKPOINT();
	return EXIT_SUCCESS;
}

BEGIN_TEST(basic_program)
{
	DEBUG("attempting to launch hardware rendered window");
	basic_window_force_software = false;
	if (int err = LAK_BASIC_PROGRAM_MAIN(0, nullptr); err != EXIT_SUCCESS)
		return err;

	DEBUG("attempting to launch software rendered window");
	basic_window_force_software = true;
	if (int err = LAK_BASIC_PROGRAM_MAIN(0, nullptr); err != EXIT_SUCCESS)
		return err;

	return EXIT_SUCCESS;
}
END_TEST()
