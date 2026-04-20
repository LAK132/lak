#include "lak/test.hpp"

#include "lak/string_literals/view.hpp"

#include "ebnf2cpp.ebnf.hpp"

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
		static_assert(
		  lak::is_same_v<
		    decltype(lak::dsl::capture_2nd<
		             lak::dsl::repeat_exact<lak::dsl::str_literal<u8"a">, 2U>,
		             lak::dsl::repeat_exact<lak::dsl::str_literal<u8"b">, 2U>,
		             lak::dsl::repeat_exact<lak::dsl::str_literal<u8"a">, 2U>>),
		    decltype(ebnf_test::aabbaa)>);
		auto str = u8"aabbaa"_view;
		ASSERT_EQUAL(ebnf_test::aabbaa.parse(str).UNWRAP().value, u8"bb"_view);
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

	static_assert(
	  lak::is_same_v<int, typename decltype(ebnf_test::transform)::value_type>);

	{
		auto str = u8"132"_view;
		ASSERT_EQUAL(ebnf_test::onethreetwo.parse(str).UNWRAP().value, int(132));
	}

	return 0;
}
END_TEST()
