#ifndef LAK_TEST_HPP
#define LAK_TEST_HPP

#include "lak/debug.hpp"
#include "lak/string.hpp"

#include <unordered_map>

namespace lak
{
	int run_tests(lak::u8string_view tests = {});

	[[nodiscard]] bool register_test(lak::u8string_view test_name,
	                                 int (*test_function)());

	const std::unordered_map<lak::u8string, int (*)()> &registered_tests();

#define BEGIN_TEST(NAME)                                                      \
	const static bool UNIQUIFY(TEST_##NAME) = lak::register_test(u8"" #NAME, []()->int
#define END_TEST() );
}

#endif
