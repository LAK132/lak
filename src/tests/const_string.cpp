#include "lak/test.hpp"

#include "lak/const_string.hpp"
#include "lak/string_view.hpp"

template<lak::u8const_string STR>
lak::u8string_view template_test_function()
{
	return lak::string_view(STR.begin(), STR.size());
}

template<lak::u8const_string... STRS>
lak::u8string template_test_function2()
{
	return ((lak::u8string(STRS)) + ...);
}

BEGIN_TEST(const_string)
{
	lak::u8string_view view = template_test_function<u8"Hello, World!">();
	lak::u8string str =
	  template_test_function2<u8"Hello", u8", ", u8"World", u8"!">();
	ASSERT_EQUAL(view, u8"Hello, World!");
	ASSERT_EQUAL(str, u8"Hello, World!");
	return 0;
}
END_TEST()
