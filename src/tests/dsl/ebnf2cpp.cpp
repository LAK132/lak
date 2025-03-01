#include "lak/test.hpp"

#include "lak/string_literals.hpp"

#include "ebnf2cpp_test.hpp"

BEGIN_TEST(ebnf2cpp)
{
	{
		auto str = u8"hello world!"_view;
		ASSERT_EQUAL(ebnf_test::hello_world.parse(str).UNWRAP().value,
		             u8"hello world"_view);
	}

	{
		auto str = u8"aaaaaaaaaa"_view;
		ASSERT_EQUAL(ebnf_test::aaaaa.parse(str).UNWRAP().value, u8"aaaaa"_view);
	}

	{
		auto str = u8"aaabbaaba"_view;
		ASSERT_EQUAL(ebnf_test::aaabb.parse(str).UNWRAP().value, u8"aaabb"_view);
	}

	{
		auto str = u8"        asdasd"_view;
		ASSERT_EQUAL(ebnf_test::whitespace.parse(str).UNWRAP().value,
		             u8"        "_view);
	}

	{
		auto str = u8"a"_view;
		ASSERT_EQUAL(ebnf_test::up_to_5_a.parse(str).UNWRAP().value, u8"a"_view);
		str = u8"aa"_view;
		ASSERT_EQUAL(ebnf_test::up_to_5_a.parse(str).UNWRAP().value, u8"aa"_view);
		str = u8"aaa"_view;
		ASSERT_EQUAL(ebnf_test::up_to_5_a.parse(str).UNWRAP().value, u8"aaa"_view);
		str = u8"aaaa"_view;
		ASSERT_EQUAL(ebnf_test::up_to_5_a.parse(str).UNWRAP().value,
		             u8"aaaa"_view);
		str = u8"aaaaa"_view;
		ASSERT_EQUAL(ebnf_test::up_to_5_a.parse(str).UNWRAP().value,
		             u8"aaaaa"_view);
		str = u8"aaaaaa"_view;
		ASSERT_EQUAL(ebnf_test::up_to_5_a.parse(str).UNWRAP().value,
		             u8"aaaaa"_view);
	}

	return 0;
}
END_TEST()
