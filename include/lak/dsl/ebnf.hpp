#ifndef LAK_DSL_EBNF_HPP
#define LAK_DSL_EBNF_HPP

#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"

#include "lak/array.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	namespace dsl
	{
		struct ebnf_concatenation_sequence
		{
			size_t begin;
			size_t end;

			size_t size() const { return end - begin; }
		};

		struct ebnf_alternation_sequence
		{
			size_t begin;
			size_t end;

			size_t size() const { return end - begin; }
		};

		struct ebnf_optional_sequence
		{
			size_t index;
		};

		struct ebnf_repetition_sequence
		{
			lak::optional<size_t> count;
			size_t index;
		};

		struct ebnf_grouping_sequence
		{
			size_t index;
		};

		struct ebnf_exception_sequence
		{
			size_t rule;
			size_t except_rule;
		};

		struct ebnf_rule_value
		{
			enum struct value_type
			{
				string,
				rule,
				concatenation,
				alternation,
				optional,
				repetition,
				grouping,
				special,
				exception,
			} type;
			size_t index;
		};

		struct ebnf_rule
		{
			lak::u8string_view name;
			size_t definition;
		};

		struct ebnf_block
		{
			lak::array<lak::u8string_view> strings;
			lak::array<lak::u8string_view> identifiers;
			lak::array<lak::u8string_view> specials;

			lak::array<lak::dsl::ebnf_concatenation_sequence> concatenations;
			lak::array<lak::dsl::ebnf_alternation_sequence> alternations;
			lak::array<lak::dsl::ebnf_optional_sequence> optionals;
			lak::array<lak::dsl::ebnf_repetition_sequence> repetitions;
			lak::array<lak::dsl::ebnf_grouping_sequence> groupings;

			lak::array<lak::dsl::ebnf_exception_sequence> exceptions;

			lak::array<lak::dsl::ebnf_rule_value> rule_values;

			lak::array<lak::dsl::ebnf_rule> rules;
		};

		struct ebnf_t
		{
			static constexpr auto is_pure_match = false;

			using value_type = ebnf_block;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const;
		};

		constexpr ebnf_t ebnf;

		static_assert(lak::dsl::parser<lak::dsl::ebnf_t>);
	}
}

#endif
