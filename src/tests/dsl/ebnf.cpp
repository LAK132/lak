#include "lak/test.hpp"

#include "lak/dsl/ebnf.hpp"
#include "lak/string_literals.hpp"

BEGIN_TEST(ebnf)
{
	{
		auto str = u8""_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 0U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 0U);
		ASSERT_EQUAL(res.alternations.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 0U);
		ASSERT_EQUAL(res.rules.size(), 0U);
	}

	{
		auto str = u8"rule1 = rule2 ;"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 1U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 0U);
		ASSERT_EQUAL(res.alternations.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 1U);
		ASSERT_EQUAL(res.rules.size(), 1U);
	}

	{
		auto str = u8"rule1 = rule2 | ( 'asdf' , { 'meow tbh' } );"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 2U);
		ASSERT_EQUAL(res.identifiers.size(), 1U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 1U);
		ASSERT_EQUAL(res.alternations.size(), 1U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 1U);
		ASSERT_EQUAL(res.groupings.size(), 1U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 7U);
		ASSERT_EQUAL(res.rules.size(), 1U);

		ASSERT_EQUAL(res.rules[0U].name, u8"rule1"_view);

		ASSERT_EQUAL(res.rule_values[res.rules[0U].definition].type,
		             lak::dsl::ebnf_rule_value::value_type::alternation);

		ASSERT_EQUAL(res.rule_values[res.alternations[0U].begin].type,
		             lak::dsl::ebnf_rule_value::value_type::rule);
		ASSERT_EQUAL(res.rule_values[res.alternations[0U].begin + 1U].type,
		             lak::dsl::ebnf_rule_value::value_type::grouping);

		ASSERT_EQUAL(res.rule_values[res.groupings[0U].index].type,
		             lak::dsl::ebnf_rule_value::value_type::concatenation);

		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin].type,
		             lak::dsl::ebnf_rule_value::value_type::string);
		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin + 1U].type,
		             lak::dsl::ebnf_rule_value::value_type::repetition);

		ASSERT_EQUAL(res.rule_values[res.repetitions[0U].index].type,
		             lak::dsl::ebnf_rule_value::value_type::string);
	}

	{
		auto str = u8"rule1 = rule2,<rule3>,rule4;"_view;
		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.identifiers.size(), 3U);
		ASSERT_EQUAL(res.specials.size(), 0U);
		ASSERT_EQUAL(res.concatenations.size(), 1U);
		ASSERT_EQUAL(res.alternations.size(), 0U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 0U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 1U);
		ASSERT_EQUAL(res.rule_values.size(), 5U);
		ASSERT_EQUAL(res.rules.size(), 1U);

		ASSERT_EQUAL(res.rules[0U].name, u8"rule1"_view);

		ASSERT_EQUAL(res.rule_values[res.rules[0U].definition].type,
		             lak::dsl::ebnf_rule_value::value_type::concatenation);

		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin].type,
		             lak::dsl::ebnf_rule_value::value_type::rule);
		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin + 1U].type,
		             lak::dsl::ebnf_rule_value::value_type::capture);
		ASSERT_EQUAL(res.rule_values[res.captures[0U].index].type,
		             lak::dsl::ebnf_rule_value::value_type::rule);
		ASSERT_EQUAL(res.rule_values[res.concatenations[0U].begin + 2U].type,
		             lak::dsl::ebnf_rule_value::value_type::rule);
	}

	{
		auto str =
		  u8"string = '\"',{ ? lak::dsl::negative_char_literal<u8'\"'> ? },'\"';\n"
		  "hello_world = string | \"hello_world\";"_view;

		auto res = lak::dsl::ebnf.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.strings.size(), 3U);
		ASSERT_EQUAL(res.identifiers.size(), 1U);
		ASSERT_EQUAL(res.specials.size(), 1U);
		ASSERT_EQUAL(res.concatenations.size(), 1U);
		ASSERT_EQUAL(res.alternations.size(), 1U);
		ASSERT_EQUAL(res.optionals.size(), 0U);
		ASSERT_EQUAL(res.repetitions.size(), 1U);
		ASSERT_EQUAL(res.groupings.size(), 0U);
		ASSERT_EQUAL(res.exceptions.size(), 0U);
		ASSERT_EQUAL(res.transforms.size(), 0U);
		ASSERT_EQUAL(res.captures.size(), 0U);
		ASSERT_EQUAL(res.rule_values.size(), 8U);
		ASSERT_EQUAL(res.rules.size(), 2U);

		ASSERT_EQUAL(res.strings[0], u8"\""_view);
		ASSERT_EQUAL(res.strings[1], u8"\""_view);
		ASSERT_EQUAL(res.strings[2], u8"hello_world"_view);
	}

	return 0;
}
END_TEST()
