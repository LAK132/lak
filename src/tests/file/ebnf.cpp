#include "lak/test.hpp"

#include "lak/file/ebnf.hpp"

#include "lak/string_literals/view.hpp"

#include "test.ebnf.hpp"

BEGIN_TEST(ebnf)
{
	{
		auto str = u8""_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.characters.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 0U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.ranges.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 0U);
		ASSERT_EQUAL(res.alternations.size(), 0U);
		ASSERT_EQUAL(res.unordereds.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.match_cases.size(), 0U);
		ASSERT_EQUAL(res.match_sequences.size(), 0U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.positive_lookaheads.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 0U);
		ASSERT_EQUAL(res.rules.size(), 0U);
	}

	{
		auto str = u8"rule1 = rule2 ;"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.characters.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 1U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.ranges.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 0U);
		ASSERT_EQUAL(res.alternations.size(), 0U);
		ASSERT_EQUAL(res.unordereds.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.match_cases.size(), 0U);
		ASSERT_EQUAL(res.match_sequences.size(), 0U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.positive_lookaheads.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 1U);
		ASSERT_EQUAL(res.rules.size(), 1U);
	}

	{
		auto str = u8"a_to_z = 'a'~'z'|'A'~'Z' ;"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.characters.size(), 4U);
		ASSERT_EQUAL(res.identifiers.size(), 0U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.ranges.size(), 2U);
		ASSERT_EQUAL(res.concatenations.size(), 0U);
		ASSERT_EQUAL(res.alternations.size(), 1U);
		ASSERT_EQUAL(res.unordereds.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.match_cases.size(), 0U);
		ASSERT_EQUAL(res.match_sequences.size(), 0U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.positive_lookaheads.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 7U);
		ASSERT_EQUAL(res.rules.size(), 1U);
	}

	{
		auto str = u8"rule1 = rule2 | ( \"asdf\" , { \"meow tbh\" } );"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 2U);
		ASSERT_EQUAL(res.characters.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 1U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.ranges.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 1U);
		ASSERT_EQUAL(res.alternations.size(), 1U);
		ASSERT_EQUAL(res.unordereds.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 1U);
		ASSERT_EQUAL(res.match_cases.size(), 0U);
		ASSERT_EQUAL(res.match_sequences.size(), 0U);
		ASSERT_EQUAL(res.groupings.size(), 1U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.positive_lookaheads.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 7U);
		ASSERT_EQUAL(res.rules.size(), 1U);

		ASSERT_EQUAL(res.rules[0U].name, u8"rule1"_view);

		ASSERT_EQUAL(res.rule_values[res.rules[0U].definition].type,
		             lak::ebnf::rule_value::value_type::alternation);

		ASSERT_EQUAL(res.rule_values[res.alternations[0U].begin].type,
		             lak::ebnf::rule_value::value_type::rule);
		ASSERT_EQUAL(res.rule_values[res.alternations[0U].begin + 1U].type,
		             lak::ebnf::rule_value::value_type::grouping);

		ASSERT_EQUAL(res.rule_values[res.groupings[0U].index].type,
		             lak::ebnf::rule_value::value_type::concatenation);

		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin].type,
		             lak::ebnf::rule_value::value_type::string);
		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin + 1U].type,
		             lak::ebnf::rule_value::value_type::repetition);

		ASSERT_EQUAL(res.rule_values[res.repetitions[0U].index].type,
		             lak::ebnf::rule_value::value_type::string);
	}

	{
		auto str = u8"rule = \"hello\"->\"world\";"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 2U);
		ASSERT_EQUAL(res.characters.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 0U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.ranges.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 0U);
		ASSERT_EQUAL(res.alternations.size(), 0U);
		ASSERT_EQUAL(res.unordereds.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.match_cases.size(), 1U);
		ASSERT_EQUAL(res.match_sequences.size(), 1U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.positive_lookaheads.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 4U);
		ASSERT_EQUAL(res.rules.size(), 1U);
	}

	{
		auto str =
		  u8"rule = \"hello\"->\" world\"|\"how\"->\" are you today?\";"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 4U);
		ASSERT_EQUAL(res.characters.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 0U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.ranges.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 0U);
		ASSERT_EQUAL(res.alternations.size(), 0U);
		ASSERT_EQUAL(res.unordereds.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.match_cases.size(), 2U);
		ASSERT_EQUAL(res.match_sequences.size(), 1U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.positive_lookaheads.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 7U);
		ASSERT_EQUAL(res.rules.size(), 1U);
	}

	{
		auto str =
		  u8"rule = \"hello\"->\" world\""
		  "|not_a_match_case"
		  "|\"how\"->\" are you today?\";"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 4U);
		ASSERT_EQUAL(res.characters.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 1U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.ranges.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 0U);
		ASSERT_EQUAL(res.alternations.size(), 1U);
		ASSERT_EQUAL(res.unordereds.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.match_cases.size(), 2U);
		ASSERT_EQUAL(res.match_sequences.size(), 2U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.positive_lookaheads.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 10U);
		ASSERT_EQUAL(res.rules.size(), 1U);
	}

	{
		auto str = u8"rule1 = rule2,<rule3>,rule4;"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.characters.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 3U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.ranges.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 1U);
		ASSERT_EQUAL(res.alternations.size(), 0U);
		ASSERT_EQUAL(res.unordereds.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.match_cases.size(), 0U);
		ASSERT_EQUAL(res.match_sequences.size(), 0U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.positive_lookaheads.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 1U);
		ASSERT_EQUAL(res.rule_values.size(), 5U);
		ASSERT_EQUAL(res.rules.size(), 1U);

		ASSERT_EQUAL(res.rules[0U].name, u8"rule1"_view);

		ASSERT_EQUAL(res.rule_values[res.rules[0U].definition].type,
		             lak::ebnf::rule_value::value_type::concatenation);

		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin].type,
		             lak::ebnf::rule_value::value_type::rule);
		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin + 1U].type,
		             lak::ebnf::rule_value::value_type::capture);
		ASSERT_EQUAL(res.rule_values[res.captures[0U].index].type,
		             lak::ebnf::rule_value::value_type::rule);
		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin + 2U].type,
		             lak::ebnf::rule_value::value_type::rule);
	}

	{
		auto str =
		  u8"string = '\"',{ ? lak::dsl::negative_char_literal<u8'\"'> ? },'\"';\n"
		  "hello_world = string | \"hello_world\";"_view;

		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 1U);
		ASSERT_EQUAL(res.characters.size(), 2U);
		ASSERT_EQUAL(res.identifiers.size(), 1U);
		ASSERT_EQUAL(res.specials.size(), 1U);
		ASSERT_EQUAL(res.ranges.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 1U);
		ASSERT_EQUAL(res.alternations.size(), 1U);
		ASSERT_EQUAL(res.unordereds.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 1U);
		ASSERT_EQUAL(res.match_cases.size(), 0U);
		ASSERT_EQUAL(res.match_sequences.size(), 0U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.positive_lookaheads.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 8U);
		ASSERT_EQUAL(res.rules.size(), 2U);

		ASSERT_EQUAL(res.characters[0], U'"');
		ASSERT_EQUAL(res.characters[1], U'"');
		ASSERT_EQUAL(res.strings[0], u8"hello_world"_view);
	}

	{
		auto str = u8"string match"_view;
		auto res = lak::ebnf_test::string_match.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		auto str = u8"C"_view;
		auto res = lak::ebnf_test::character_match.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		char32_t _cstr[2] = {char32_t(0x132), char32_t(0)};
		auto _str         = lak::to_u8string(_cstr);
		auto str          = lak::string_view(_str);
		auto res = lak::ebnf_test::hex_lit_match.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		char32_t _cstr[2] = {char32_t('\n'), char32_t(0)};
		auto _str         = lak::to_u8string(_cstr);
		auto str          = lak::string_view(_str);
		auto res = lak::ebnf_test::newline_lit_match.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		auto str = u8"meow"_view;
		auto res = lak::ebnf_test::meow.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		auto str = u8"woof"_view;
		auto res = lak::ebnf_test::woof.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		auto str = u8"\"meow\""_view;
		auto res = lak::ebnf_test::sequence.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		auto str1 = u8"meowwoof"_view;
		auto str2 = u8"woofmeow"_view;
		auto res1 = lak::ebnf_test::unordered.parse(str1).UNWRAP().value;
		auto res2 = lak::ebnf_test::unordered.parse(str2).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str2, res2);
	}

	{
		auto str1 = u8"a"_view;
		auto str2 = u8"b"_view;
		auto str3 = u8"meow"_view;
		auto res1 = lak::ebnf_test::alternation.parse(str1).UNWRAP().value;
		auto res2 = lak::ebnf_test::alternation.parse(str2).UNWRAP().value;
		auto res3 = lak::ebnf_test::alternation.parse(str3).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str2, res2);
		ASSERT_EQUAL(str3, res3);
	}

	{
		auto str = u8"'meow'"_view;
		auto exp = u8"meow"_view;
		auto res = lak::ebnf_test::capture_group.parse(str).UNWRAP().value;
		ASSERT_EQUAL(exp, res);
	}

	{
		auto str = u8"abc"_view;
		auto res = lak::ebnf_test::non_capture_group_abc.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		auto str = u8"aab"_view;
		auto res = lak::ebnf_test::non_capture_group_aab.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		auto str = u8"abab"_view;
		auto res =
		  lak::ebnf_test::non_capture_group_abab.parse(str).UNWRAP().value;
		ASSERT_EQUAL(str, res);
	}

	{
		auto str1 = u8"a"_view;
		auto str2 = u8"aa"_view;
		auto str3 = u8"aaaaa"_view;
		auto res1 = lak::ebnf_test::any_a.parse(str1).UNWRAP().value;
		auto res2 = lak::ebnf_test::any_a.parse(str2).UNWRAP().value;
		auto res3 = lak::ebnf_test::any_a.parse(str3).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str2, res2);
		ASSERT_EQUAL(str3, res3);
	}

	{
		auto str1 = u8"nyaaa"_view;
		auto str2 = u8"nyaaaa"_view;
		auto str3 = u8"nyaaaaaaa"_view;
		auto res1 = lak::ebnf_test::at_least_nyaaa.parse(str1).UNWRAP().value;
		auto res2 = lak::ebnf_test::at_least_nyaaa.parse(str2).UNWRAP().value;
		auto res3 = lak::ebnf_test::at_least_nyaaa.parse(str3).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str2, res2);
		ASSERT_EQUAL(str3, res3);
	}

	{
		auto str1 = u8"hello"_view;
		auto str2 = u8"hello!"_view;
		auto res1 = lak::ebnf_test::hello_maybe_bang.parse(str1).UNWRAP().value;
		auto res2 = lak::ebnf_test::hello_maybe_bang.parse(str2).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str2, res2);
	}

	{
		auto str1 = u8"a"_view;
		auto str2 = u8"b"_view;
		auto str3 = u8"c"_view;
		auto str4 = u8"z"_view;
		auto res1 = lak::ebnf_test::a_to_z.parse(str1).UNWRAP().value;
		auto res2 = lak::ebnf_test::a_to_z.parse(str2).UNWRAP().value;
		auto res3 = lak::ebnf_test::a_to_z.parse(str3).UNWRAP().value;
		auto res4 = lak::ebnf_test::a_to_z.parse(str4).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str2, res2);
		ASSERT_EQUAL(str3, res3);
		ASSERT_EQUAL(str4, res4);
	}

	{
		auto str1 = u8"A"_view;
		auto str2 = u8"B"_view;
		auto str3 = u8"C"_view;
		auto str4 = u8"Z"_view;
		auto res1 = lak::ebnf_test::A_to_Z.parse(str1).UNWRAP().value;
		auto res2 = lak::ebnf_test::A_to_Z.parse(str2).UNWRAP().value;
		auto res3 = lak::ebnf_test::A_to_Z.parse(str3).UNWRAP().value;
		auto res4 = lak::ebnf_test::A_to_Z.parse(str4).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str2, res2);
		ASSERT_EQUAL(str3, res3);
		ASSERT_EQUAL(str4, res4);
	}

	{
		auto str1 = u8"0"_view;
		auto str2 = u8"1"_view;
		auto str3 = u8"3"_view;
		auto str4 = u8"9"_view;
		auto res1 = lak::ebnf_test::_0_to_9.parse(str1).UNWRAP().value;
		auto res2 = lak::ebnf_test::_0_to_9.parse(str2).UNWRAP().value;
		auto res3 = lak::ebnf_test::_0_to_9.parse(str3).UNWRAP().value;
		auto res4 = lak::ebnf_test::_0_to_9.parse(str4).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str2, res2);
		ASSERT_EQUAL(str3, res3);
		ASSERT_EQUAL(str4, res4);
	}

	{
		auto str1 = u8"a"_view;
		auto str2 = u8"b"_view;
		auto str3 = u8"c"_view;
		auto str4 = u8"z"_view;
		auto res1 = lak::ebnf_test::a_to_z_except_b.parse(str1).UNWRAP().value;
		lak::ebnf_test::a_to_z_except_b.parse(str2).UNWRAP_ERR();
		auto res3 = lak::ebnf_test::a_to_z_except_b.parse(str3).UNWRAP().value;
		auto res4 = lak::ebnf_test::a_to_z_except_b.parse(str4).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str3, res3);
		ASSERT_EQUAL(str4, res4);
	}

	{
		auto str1 = u8"dog says: woof"_view;
		auto str2 = u8"cat says: meow"_view;
		auto res1 = lak::ebnf_test::dogs_cats.parse(str1).UNWRAP().value;
		auto res2 = lak::ebnf_test::dogs_cats.parse(str2).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str2, res2);
	}

	{
		auto str1 = u8"a"_view;
		auto str2 = u8"b"_view;
		auto str3 = u8"c"_view;
		auto str4 = u8"z"_view;
		auto res1 = lak::ebnf_test::not_b.parse(str1).UNWRAP().value;
		lak::ebnf_test::not_b.parse(str2).UNWRAP_ERR();
		auto res3 = lak::ebnf_test::not_b.parse(str3).UNWRAP().value;
		auto res4 = lak::ebnf_test::not_b.parse(str4).UNWRAP().value;
		ASSERT_EQUAL(str1, res1);
		ASSERT_EQUAL(str3, res3);
		ASSERT_EQUAL(str4, res4);
	}

	{
		auto str = u8"ab"_view;
		auto exp = u8"a"_view;
		auto res = lak::ebnf_test::ab_a.parse(str).UNWRAP().value;
		ASSERT_EQUAL(exp, res);
	}

	{
		auto str1 = u8"true"_view;
		auto str2 = u8"trUE"_view;
		auto str3 = u8"TRUE"_view;
		auto str4 = u8"False"_view;
		auto res1 = lak::ebnf_test::true_false.parse(str1).UNWRAP().value;
		lak::ebnf_test::true_false.parse(str2).UNWRAP_ERR();
		auto res3 = lak::ebnf_test::true_false.parse(str3).UNWRAP().value;
		auto res4 = lak::ebnf_test::true_false.parse(str4).UNWRAP().value;
		ASSERT(res1);
		ASSERT(res3);
		ASSERT(!res4);
	}

	return 0;
}
END_TEST()
