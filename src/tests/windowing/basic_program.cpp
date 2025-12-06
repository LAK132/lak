#define LAK_BASIC_PROGRAM_MAIN   lak_test_basic_program_main
#define LAK_BASIC_PROGRAM_PREFIX lak_test_basic_

#include "lak/test.hpp"

#include "lak/basic_program.inl"

#include "lak/string_literals/view.hpp"

struct lak_test_basic_window : virtual public lak_test_basic_window_api
{
	lak_test_basic_window() : lak_test_basic_window_api() {}

	virtual ~lak_test_basic_window() { MEMBER_FUNCTION_CHECKPOINT(); }

	virtual void init() override final
	{
		MEMBER_FUNCTION_CHECKPOINT();
		window().set_title(L"Test Window");
		auto title = window().title();
		ASSERT_EQUAL(lak::wstring_view::from_c_str(title.c_str()),
		             L"Test Window"_view);

		if (lak_test_basic_window_force_software)
			ASSERT(window().graphics() == lak::graphics_mode::Software);
	}

	virtual void handle_event(lak::event &event) override final
	{
		LAK_UNUSED(event);
		MEMBER_FUNCTION_CHECKPOINT();
	}

	virtual void loop(uint64_t counter_delta) override final
	{
		LAK_UNUSED(counter_delta);
		MEMBER_FUNCTION_CHECKPOINT();
		destroy();
	}
};

lak::error_code<int> lak_test_basic_program_preinit(lak::span<char *>)
{
	FUNCTION_CHECKPOINT();
	return lak::ok_t{};
}

static_assert(lak::is_of_template_v<lak::u8string, std::basic_string>);

lak::weak_ptr<lak_test_basic_window_instance<lak_test_basic_window>> wnd_ptr;

lak::error_code<int> lak_test_basic_program_init()
{
	FUNCTION_CHECKPOINT();
	RES_TRY_ASSIGN(wnd_ptr =,
	               lak_test_basic_create_window<lak_test_basic_window>().map_err(
	                 [](lak::u8string err)
	                 {
		                 ERROR(err);
		                 return EXIT_FAILURE;
	                 }));
	return lak::ok_t{};
}

bool lak_test_basic_program_loop(uint64_t counter_delta)
{
	LAK_UNUSED(counter_delta);
	FUNCTION_CHECKPOINT();
	return !lak_test_basic_window_instances().empty();
}

int lak_test_basic_program_quit()
{
	FUNCTION_CHECKPOINT();
	return EXIT_SUCCESS;
}

void lak_test_basic_program_handle_event(lak::event &event)
{
	LAK_UNUSED(event);
	FUNCTION_CHECKPOINT();
}

#if !(defined(LAK_ENABLE_SOFTRENDER) || defined(LAK_ENABLE_OPENGL))
#	error At least one renderer must be enabled
#endif

#ifdef LAK_RUN_WINDOWING_TESTS
BEGIN_TEST(basic_program)
#else
int basic_program_compile_test()
#endif
{
#ifdef LAK_ENABLE_OPENGL
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
