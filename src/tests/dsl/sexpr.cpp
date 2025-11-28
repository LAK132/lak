#include "lak/test.hpp"

#include "lak/dsl/sexpr.hpp"
#include "lak/dsl/utility.hpp"

#include "lak/string_literals/view.hpp"

BEGIN_TEST(sexpr)
{
	constexpr auto identifier_parser =
	  +(lak::dsl::char_range<U'a', U'z'> | lak::dsl::char_range<U'A', U'Z'> |
	    lak::dsl::char_range<U'0', U'9'>);

	constexpr auto character_parser =
	  lak::dsl::char_literal<U'\''> +
	  *(lak::dsl::str_literal<u8"\\'"> |
	    !lak::dsl::char_literal<U'\''>)+lak::dsl::char_literal<U'\''>;

	constexpr auto string_parser =
	  lak::dsl::char_literal<U'"'> +
	  *(lak::dsl::str_literal<u8"\\\""> |
	    !lak::dsl::char_literal<U'"'>)+lak::dsl::char_literal<U'"'>;

	constexpr auto token_parser =
	  string_parser | character_parser | identifier_parser;

	static_assert(lak::dsl::concepts::pure_match_parser<decltype(token_parser)>);

	constexpr auto sexpr_parser =
	  lak::dsl::sexpr<token_parser, lak::dsl::whitespace>;

	{
		auto str = u8"(hello (world))"_str;

		auto res = sexpr_parser.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 2U);
		ASSERT_EQUAL(res.cells.size(), 3U);
		ASSERT_EQUAL(res.lists.size(), 2U);

		auto list0 = res.list_cells(0);
		ASSERT_EQUAL(list0.size(), 2U);

		ASSERT(list0[0].type == lak::dsl::sexpr_cell::value_type::token);
		ASSERT_EQUAL(res.tokens[list0[0].index], u8"hello"_view);

		auto tok0 = res.as_token(list0[0]).UNWRAP();
		ASSERT_EQUAL(tok0, u8"hello"_view);

		ASSERT(list0[1].type == lak::dsl::sexpr_cell::value_type::list);
		ASSERT_EQUAL(list0[1].index, 1U);

		{
			auto list1 = res.list_cells(1);
			ASSERT_EQUAL(list1.size(), 1U);
		}

		auto list1 = res.list_cells(res.as_list(list0[1]).UNWRAP());

		ASSERT(list1[0].type == lak::dsl::sexpr_cell::value_type::token);
		ASSERT_EQUAL(res.tokens[list1[0].index], u8"world"_view);

		auto tok1 = res.as_token(list1[0]).UNWRAP();
		ASSERT_EQUAL(tok1, u8"world"_view);
	}

	{
		auto str = u8"(\"meow\" \"meow\" \"meow\")"_str;

		auto res = sexpr_parser.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 3U);
		ASSERT_EQUAL(res.cells.size(), 3U);
		ASSERT_EQUAL(res.lists.size(), 1U);

		ASSERT_EQUAL(res.tokens[0], u8"\"meow\""_view);
		ASSERT_EQUAL(res.tokens[1], u8"\"meow\""_view);
		ASSERT_EQUAL(res.tokens[2], u8"\"meow\""_view);
	}

	{
		auto str = u8"((())(()()())(()()))"_str;

		auto res = sexpr_parser.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 0U);
		ASSERT_EQUAL(res.cells.size(), 9U);
		ASSERT_EQUAL(res.lists.size(), 10U);
	}
	return 0;
}
END_TEST()
