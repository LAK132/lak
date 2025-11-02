#include "lak/dsl/ebnf.hpp"

static constexpr auto ws =
  lak::dsl::ascii_whitespace | lak::dsl::simple_bounded_str<u8"(*", u8"*)">;

static constexpr auto special_seq_parser =
  lak::dsl::capture_simple_bounded_str<u8"?", u8"?">;

static constexpr auto transform_seq_parser =
  lak::dsl::capture_simple_bounded_str<u8"$", u8"$">;

static constexpr auto terminal_string =
  lak::dsl::capture_simple_bounded_str<u8"\"", u8"\""> |
  lak::dsl::capture_simple_bounded_str<u8"'", u8"'">;

static constexpr auto _identifier =
  lak::dsl::ascii_alphanumeric | lak::dsl::char_literal<U'_'>;

static constexpr auto identifier =
  lak::dsl::ascii_alpha + *(_identifier | ((+ws) + _identifier));

static constexpr auto define_rule =
  lak::dsl::capture_1st<identifier, (*ws) + lak::dsl::char_literal<U'='>>;

static constexpr auto repeat_n =
  lak::dsl::capture_1st<lak::dsl::parsed_dec_uint<size_t>,
                        (*ws) + lak::dsl::char_literal<U'*'>>;

static constexpr auto trailing_punct =
  (*ws) +
  (lak::dsl::str_literal<u8"->"> |
   lak::dsl::one_of_chars<U',', U'|', U';', U')', U']', U'}', U'>', U'$'>);

lak::dsl::result<lak::dsl::ebnf_t::value_type> lak::dsl::ebnf_t::parse(
  lak::u8string_view str) const
{
	lak::u8string_view rem = str;

	ebnf_block result;

	auto move_str = [&]<typename T>(const lak::dsl::parse_result<T> &res)
	{ rem = res.remaining; };

#define EBNF_PARSE() parse(rem).if_ok(move_str)

	RES_TRY((*ws).EBNF_PARSE());

	if (rem.empty())
		return lak::ok_t<lak::dsl::parse_result<value_type>>{{
		  .consumed  = str.substr(str.size() - rem.size()),
		  .remaining = rem,
		  .value     = {},
		}};

	struct working_data
	{
		enum struct value_type
		{
			rule,
			concat,
			altern,
			option,
			repeat,
			repeat_n,
			match_case,
			group,
			capture,
			except,
			pos_lookahead,
			transform,
		} type;

		size_t begin;
		size_t size;
	};

	lak::array<ebnf_rule_value> working_values;
	lak::array<working_data> working_tree;

	auto pop_values = [&](size_t count) -> size_t
	{
		size_t begin = result.rule_values.size();
		result.rule_values.reserve(result.rule_values.size() + count);
		for (size_t i = working_values.size() - count; i < working_values.size();
		     ++i)
			result.rule_values.push_back(working_values[i]);
		working_values.resize(working_values.size() - count);
		return begin;
	};

	auto pop_repeat_n = [&]() -> lak::error_code<lak::dsl::parse_error>
	{
		if (working_tree.back().size != 1U)
			return lak::err_t{lak::dsl::parse_error{.message = u8"invalid repeat"}};

		const size_t index = pop_values(1U);
		working_values.push_back({
		  .type  = lak::dsl::ebnf_rule_value::value_type::repetition,
		  .index = result.repetitions.size(),
		});
		result.repetitions.push_back({
		  .count = working_tree.back().begin,
		  .index = index,
		});
		working_tree.pop_back();
		++working_tree.back().size;

		return lak::ok_t{};
	};

	auto pop_positive_lookahead = [&]() -> lak::error_code<lak::dsl::parse_error>
	{
		if (working_tree.back().size != 1U)
			return lak::err_t{
			  lak::dsl::parse_error{.message = u8"invalid positive lookahead"}};

		const size_t index = pop_values(1U);
		working_values.push_back({
		  .type  = lak::dsl::ebnf_rule_value::value_type::positive_lookahead,
		  .index = result.positive_lookaheads.size(),
		});
		result.positive_lookaheads.push_back({
		  .index = index,
		});
		working_tree.pop_back();
		++working_tree.back().size;

		return lak::ok_t{};
	};

	auto inc_tree = [&]() -> lak::error_code<lak::dsl::parse_error>
	{
		++working_tree.back().size;

		while (!working_tree.empty())
		{
			if (working_tree.back().type == working_data::value_type::repeat_n)
			{
				RES_TRY(pop_repeat_n());
			}
			else if (working_tree.back().type ==
			         working_data::value_type::pos_lookahead)
			{
				RES_TRY(pop_positive_lookahead());
			}
			else
				break;
		}

		return lak::ok_t{};
	};

	auto pop_tree = [&]() -> lak::error_code<lak::dsl::parse_error>
	{
		working_tree.pop_back();
		return inc_tree();
	};

	auto pop_except = [&]() -> lak::error_code<lak::dsl::parse_error>
	{
		if (working_tree.back().size != 2U)
			return lak::err_t{
			  lak::dsl::parse_error{.message = u8"invalid exception"}};

		const size_t begin = pop_values(2U);
		working_values.push_back({
		  .type  = lak::dsl::ebnf_rule_value::value_type::exception,
		  .index = result.exceptions.size(),
		});
		result.exceptions.push_back({
		  .rule        = begin,
		  .except_rule = begin + 1U,
		});
		return pop_tree();
	};

	auto pop_match_case = [&]() -> lak::error_code<lak::dsl::parse_error>
	{
		if (working_tree.back().size != 2U)
			return lak::err_t{
			  lak::dsl::parse_error{.message = u8"invalid match case"}};

		// abuse .index to hold subvalue indices until it can be patched in
		// pop_altern
		working_values.push_back({
		  .type  = lak::dsl::ebnf_rule_value::value_type::match_case,
		  .index = pop_values(2U),
		});
		return pop_tree();
	};

	auto pop_altern = [&]() -> lak::error_code<lak::dsl::parse_error>
	{
		if (working_tree.back().size <= working_tree.back().begin)
			return lak::err_t{lak::dsl::parse_error{
			  .message = u8"missing final value of alternation"}};

		// subsequences of match cases should be treated as if they were grouped

		auto first_match_case_subsequence =
		  [](lak::span<lak::dsl::ebnf_rule_value> values)
		  -> lak::span<lak::dsl::ebnf_rule_value>
		{
			auto first_match_case = lak::find_if(
			  values.begin(),
			  values.end(),
			  [](const lak::dsl::ebnf_rule_value &v)
			  {
				  return v.type == lak::dsl::ebnf_rule_value::value_type::match_case;
			  });
			if (first_match_case == values.end()) return {};
			auto first_non_match_case = lak::find_if(
			  first_match_case,
			  values.end(),
			  [](const lak::dsl::ebnf_rule_value &v)
			  {
				  return v.type != lak::dsl::ebnf_rule_value::value_type::match_case;
			  });
			return lak::span(first_match_case, first_non_match_case);
		};

		for (lak::span<lak::dsl::ebnf_rule_value> match_subseq;
		     !(match_subseq = first_match_case_subsequence(
		         lak::span(working_values).last(working_tree.back().size)))
		        .empty();)
		{
			ASSERT_GREATER_OR_EQUAL(working_tree.back().size, match_subseq.size());
			working_tree.back().size -= (match_subseq.size() - 1U);

			// fix up match case indexing
			for (auto &mc : match_subseq)
			{
				const size_t index = result.match_cases.size();
				result.match_cases.push_back({
				  .condition = mc.index,
				  .matched   = mc.index + 1U,
				});
				mc.index = index;
			}

			const size_t ret_idx = match_subseq.begin() - working_values.begin();
			const size_t sz      = match_subseq.size();
			const size_t rem_sz  = working_values.end() - match_subseq.begin();

			lak::rotate_left(
			  match_subseq.begin(), working_values.end(), match_subseq.size());
			const size_t begin = pop_values(sz);

			working_values.insert(
			  working_values.begin() + ret_idx,
			  {
			    .type  = lak::dsl::ebnf_rule_value::value_type::match_sequence,
			    .index = result.match_sequences.size(),
			  });
			result.match_sequences.push_back({
			  .begin = begin,
			  .end   = begin + sz,
			});
		}

		const size_t sz = working_tree.back().size;

		if (sz == 1U && working_values.back().type ==
		                  lak::dsl::ebnf_rule_value::value_type::match_sequence)
			return pop_tree();

		if (sz < 2U)
			return lak::err_t{
			  lak::dsl::parse_error{.message = u8"invalid alternation length"}};

		const size_t begin = pop_values(sz);
		working_values.push_back({
		  .type  = lak::dsl::ebnf_rule_value::value_type::alternation,
		  .index = result.alternations.size(),
		});
		result.alternations.push_back({
		  .begin = begin,
		  .end   = begin + sz,
		});
		return pop_tree();
	};

	auto pop_concat = [&]() -> lak::error_code<lak::dsl::parse_error>
	{
		const size_t sz = working_tree.back().size;

		if (sz <= working_tree.back().begin)
			return lak::err_t{lak::dsl::parse_error{
			  .message = u8"missing final value of concatenation"}};

		if (sz < 2U)
			return lak::err_t{
			  lak::dsl::parse_error{.message = u8"invalid concatenation length"}};

		const size_t begin = pop_values(sz);
		working_values.push_back({
		  .type  = lak::dsl::ebnf_rule_value::value_type::concatenation,
		  .index = result.concatenations.size(),
		});
		result.concatenations.push_back({
		  .begin = begin,
		  .end   = begin + sz,
		});
		return pop_tree();
	};

	while (!rem.empty())
	{
		// DEBUG(rem.begin() - str.begin());
		if (working_tree.empty())
		{
			// start of a new rule
			RES_TRY_ASSIGN(auto rule_name =, define_rule.EBNF_PARSE());
			ASSERT_EQUAL(working_values.size(), 0U);
			working_tree.push_back({
			  .type  = working_data::value_type::rule,
			  .begin = result.rules.size(),
			  .size  = 0U,
			});
			result.rules.push_back({
			  .name       = rule_name.value,
			  .definition = {},
			});
		}
		else if_let_ok (auto iden, identifier.EBNF_PARSE())
		{
			RES_TRY(trailing_punct.parse(rem));
			working_values.push_back({
			  .type  = lak::dsl::ebnf_rule_value::value_type::rule,
			  .index = result.identifiers.size(),
			});
			result.identifiers.push_back(iden.value);
			RES_TRY(inc_tree());
		}
		else if_let_ok (auto str, terminal_string.EBNF_PARSE())
		{
			RES_TRY(trailing_punct.parse(rem));
			working_values.push_back({
			  .type  = lak::dsl::ebnf_rule_value::value_type::string,
			  .index = result.strings.size(),
			});
			result.strings.push_back(str.value);
			RES_TRY(inc_tree());
		}
		else if_let_ok (auto spec, special_seq_parser.EBNF_PARSE())
		{
			RES_TRY(trailing_punct.parse(rem));
			working_values.push_back({
			  .type  = lak::dsl::ebnf_rule_value::value_type::special,
			  .index = result.specials.size(),
			});
			result.specials.push_back(spec.value);
			RES_TRY(inc_tree());
		}
		else if_let_ok (auto rn, repeat_n.EBNF_PARSE())
		{
			working_tree.push_back({
			  .type  = working_data::value_type::repeat_n,
			  .begin = rn.value,
			  .size  = 0U,
			});
		}
		else if (lak::dsl::char_literal<U'['>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type  = working_data::value_type::option,
			  .begin = 0U,
			  .size  = 0U,
			});
		}
		else if (lak::dsl::char_literal<U']'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::pos_lookahead)
				{
					RES_TRY(pop_positive_lookahead());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::match_case)
				{
					RES_TRY(pop_match_case());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (working_tree.back().type != working_data::value_type::option)
				return lak::err_t{
				  lak::dsl::parse_error{.message = u8"unexpected end of optional"}};

			if (working_tree.back().size != 1U)
				return lak::err_t{
				  lak::dsl::parse_error{.message = u8"invalid optional length"}};

			const size_t index = pop_values(1U);
			working_values.push_back({
			  .type  = lak::dsl::ebnf_rule_value::value_type::optional,
			  .index = result.optionals.size(),
			});
			result.optionals.push_back({
			  .index = index,
			});
			RES_TRY(pop_tree());
		}
		else if (lak::dsl::char_literal<U'{'>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type  = working_data::value_type::repeat,
			  .begin = 0U,
			  .size  = 0U,
			});
		}
		else if (lak::dsl::char_literal<U'}'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::pos_lookahead)
				{
					RES_TRY(pop_positive_lookahead());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::match_case)
				{
					RES_TRY(pop_match_case());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (working_tree.back().type != working_data::value_type::repeat)
				return lak::err_t{
				  lak::dsl::parse_error{.message = u8"unexpected end of repetition"}};

			if (working_tree.back().size != 1U)
				return lak::err_t{
				  lak::dsl::parse_error{.message = u8"invalid repetition length"}};

			const size_t index = pop_values(1U);
			working_values.push_back({
			  .type  = lak::dsl::ebnf_rule_value::value_type::repetition,
			  .index = result.repetitions.size(),
			});
			result.repetitions.push_back({
			  .count = lak::nullopt,
			  .index = index,
			});
			RES_TRY(pop_tree());
		}
		else if (lak::dsl::char_literal<U'('>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type  = working_data::value_type::group,
			  .begin = 0U,
			  .size  = 0U,
			});
		}
		else if (lak::dsl::char_literal<U')'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::pos_lookahead)
				{
					RES_TRY(pop_positive_lookahead());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::match_case)
				{
					RES_TRY(pop_match_case());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (working_tree.back().type != working_data::value_type::group)
				return lak::err_t{
				  lak::dsl::parse_error{.message = u8"unexpected end of grouping"}};

			if (working_tree.back().size != 1U)
				return lak::err_t{
				  lak::dsl::parse_error{.message = u8"invalid grouping length"}};

			const size_t index = pop_values(1U);
			working_values.push_back({
			  .type  = lak::dsl::ebnf_rule_value::value_type::grouping,
			  .index = result.groupings.size(),
			});
			result.groupings.push_back({
			  .index = index,
			});
			RES_TRY(pop_tree());
		}
		else if (lak::dsl::char_literal<U'<'>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type  = working_data::value_type::capture,
			  .begin = 0U,
			  .size  = 0U,
			});
		}
		else if (lak::dsl::char_literal<U'>'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::pos_lookahead)
				{
					RES_TRY(pop_positive_lookahead());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::match_case)
				{
					RES_TRY(pop_match_case());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (working_tree.back().type != working_data::value_type::capture)
				return lak::err_t{lak::dsl::parse_error{
				  .message = u8"unexpected end of capture group"}};

			if (working_tree.back().size != 1U)
				return lak::err_t{
				  lak::dsl::parse_error{.message = u8"invalid capture group length"}};

			const size_t index = pop_values(1U);
			working_values.push_back({
			  .type  = lak::dsl::ebnf_rule_value::value_type::capture,
			  .index = result.captures.size(),
			});
			result.captures.push_back({
			  .index = index,
			});
			RES_TRY(pop_tree());
		}
		else if (lak::dsl::str_literal<u8"->">.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::pos_lookahead)
				{
					RES_TRY(pop_positive_lookahead());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (working_values.size() < 1U)
				return lak::err_t{lak::dsl::parse_error{
				  .message = u8"missing beginning of match case"}};

			--working_tree.back().size;

			if (working_tree.back().type != working_data::value_type::altern)
			{
				working_tree.push_back({
				  .type  = working_data::value_type::altern,
				  .begin = 0U,
				  .size  = 0U,
				});
			}

			working_tree.push_back({
			  .type  = working_data::value_type::match_case,
			  .begin = 0U,
			  .size  = 1U,
			});
		}
		else if (lak::dsl::char_literal<U'-'>.EBNF_PARSE().is_ok())
		{
			if (working_values.size() < 1U)
				return lak::err_t{lak::dsl::parse_error{
				  .message = u8"missing beginning of exception"}};

			--working_tree.back().size;
			working_tree.push_back({
			  .type  = working_data::value_type::except,
			  .begin = 0U,
			  .size  = 1U,
			});
		}
		else if (lak::dsl::char_literal<U'+'>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type  = working_data::value_type::pos_lookahead,
			  .begin = 0U,
			  .size  = 0U,
			});
		}
		else if (lak::dsl::char_literal<U','>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::pos_lookahead)
				{
					RES_TRY(pop_positive_lookahead());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::match_case)
				{
					RES_TRY(pop_match_case());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (working_tree.back().type != working_data::value_type::concat)
			{
				if (working_values.size() < 1U)
					return lak::err_t{lak::dsl::parse_error{
					  .message = u8"missing beginning of concatenation"}};

				--working_tree.back().size;
				working_tree.push_back({
				  .type  = working_data::value_type::concat,
				  .begin = 1U,
				  .size  = 1U,
				});
			}
			else
				working_tree.back().begin = working_tree.back().size;
		}
		else if (lak::dsl::char_literal<U'|'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::pos_lookahead)
				{
					RES_TRY(pop_positive_lookahead());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::match_case)
				{
					RES_TRY(pop_match_case());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (working_tree.back().type != working_data::value_type::altern)
			{
				if (working_values.size() < 1U)
					return lak::err_t{lak::dsl::parse_error{
					  .message = u8"missing beginning of alternation"}};

				--working_tree.back().size;
				working_tree.push_back({
				  .type  = working_data::value_type::altern,
				  .begin = 1U,
				  .size  = 1U,
				});
			}
			else
				working_tree.back().begin = working_tree.back().size;
		}
		else if_let_ok (auto trans, transform_seq_parser.EBNF_PARSE())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::pos_lookahead)
				{
					RES_TRY(pop_positive_lookahead());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::match_case)
				{
					RES_TRY(pop_match_case());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::repeat_n)
				{
					RES_TRY(pop_repeat_n());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (working_tree.back().type != working_data::value_type::rule)
				return lak::err_t{lak::dsl::parse_error{
				  .message =
				    u8"transform sequences are only valid at the end of a rule"}};
			if (working_tree.back().size != 1U)
				return lak::err_t{lak::dsl::parse_error{
				  .message = u8"unexpected overlong transformed rule"}};

			working_values.push_back({
			  .type  = lak::dsl::ebnf_rule_value::value_type::transform,
			  .index = result.transforms.size(),
			});
			result.transforms.push_back(trans.value);
			++working_tree.back().size;
		}
		else if (lak::dsl::char_literal<U';'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::pos_lookahead)
				{
					RES_TRY(pop_positive_lookahead());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::match_case)
				{
					RES_TRY(pop_match_case());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::repeat_n)
				{
					RES_TRY(pop_repeat_n());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (working_tree.back().type != working_data::value_type::rule)
				return lak::err_t{
				  lak::dsl::parse_error{.message = u8"unexpected end of rule"}};
			if (working_tree.back().size != 1U &&
			    !(working_tree.back().size == 2U &&
			      working_values.back().type ==
			        ebnf_rule_value::value_type::transform))
				return lak::err_t{
				  lak::dsl::parse_error{.message = u8"unexpected overlong rule"}};

			const size_t begin = pop_values(working_tree.back().size);
			result.rules[working_tree.back().begin].definition = begin;
			if (working_tree.back().size == 2U)
				result.rules[working_tree.back().begin].transform = begin + 1U;
			else
				result.rules[working_tree.back().begin].transform = lak::nullopt;
			working_tree.pop_back();
		}
		else
			return lak::err_t{lak::dsl::parse_error{.message = u8"bad state"}};

		RES_TRY((*ws).EBNF_PARSE());
	}
#undef EBNF_PARSE

	if (working_tree.size() > 0U)
		return lak::err_t{lak::dsl::parse_error{.message = u8"out of data"}};

	ASSERT_EQUAL(working_tree.size(), 0U);

	return lak::ok_t<lak::dsl::parse_result<value_type>>{{
	  .consumed  = str.substr(str.size() - rem.size()),
	  .remaining = rem,
	  .value     = lak::move(result),
	}};
}
