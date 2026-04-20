#ifndef LAK_FILE_EBNF_HPP
#define LAK_FILE_EBNF_HPP

#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"

#include "lak/array.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	namespace ebnf
	{
		struct concatenation_sequence
		{
			size_t begin;
			size_t end;

			size_t size() const { return end - begin; }
		};

		struct alternation_sequence
		{
			size_t begin;
			size_t end;

			size_t size() const { return end - begin; }
		};

		struct optional_sequence
		{
			size_t index;
		};

		struct repetition_sequence
		{
			lak::optional<size_t> count;
			size_t index;
		};

		struct grouping_sequence
		{
			size_t index;
		};

		struct capture_sequence
		{
			size_t index;
		};

		struct replace_sequence
		{
			size_t index;
			size_t special;
		};

		struct exception_sequence
		{
			size_t rule;
			size_t except_rule;
		};

		struct negative_lookahead_sequence
		{
			size_t index;
		};

		struct positive_lookahead_sequence
		{
			size_t index;
		};

		struct match_case
		{
			size_t condition;
			size_t matched;
		};

		struct match_sequence
		{
			size_t begin;
			size_t end;

			size_t size() const { return end - begin; }
		};

		struct rule_value
		{
			enum struct value_type
			{
				string,
				rule,
				concatenation,
				alternation,
				optional,
				repetition,
				match_case,
				match_sequence,
				grouping,
				capture,
				special,
				replace,
				exception,
				negative_lookahead,
				positive_lookahead,
				transform,
			} type;
			size_t index;
		};

		struct rule
		{
			lak::u8string_view name;
			size_t definition;
			lak::optional<size_t> transform;
		};

		struct block
		{
			lak::array<lak::u8string_view> strings;
			lak::array<lak::u8string_view> identifiers;
			lak::array<lak::u8string_view> specials;
			lak::array<lak::u8string_view> transforms;

			lak::array<lak::ebnf::concatenation_sequence> concatenations;
			lak::array<lak::ebnf::alternation_sequence> alternations;
			lak::array<lak::ebnf::optional_sequence> optionals;
			lak::array<lak::ebnf::repetition_sequence> repetitions;
			lak::array<lak::ebnf::match_case> match_cases;
			lak::array<lak::ebnf::match_sequence> match_sequences;
			lak::array<lak::ebnf::grouping_sequence> groupings;
			lak::array<lak::ebnf::capture_sequence> captures;
			lak::array<lak::ebnf::replace_sequence> replaces;

			lak::array<lak::ebnf::exception_sequence> exceptions;
			lak::array<lak::ebnf::negative_lookahead_sequence> negative_lookaheads;
			lak::array<lak::ebnf::positive_lookahead_sequence> positive_lookaheads;

			lak::array<lak::ebnf::rule_value> rule_values;

			lak::array<lak::ebnf::rule> rules;
		};
	}

	namespace dsl
	{
		struct ebnf_t
		{
			static constexpr auto is_pure_match = false;

			using value_type = lak::ebnf::block;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const;
		};

		constexpr ebnf_t ebnf;

		static_assert(lak::dsl::concepts::parser<lak::dsl::ebnf_t>);
	}
}

#endif
