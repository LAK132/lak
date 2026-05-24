#include "lak/file/ebnf.hpp"

static constexpr auto ws =
  lak::dsl::ascii_whitespace | lak::dsl::simple_bounded_str<u8"(*", u8"*)">;

static constexpr auto special_seq_parser =
  lak::dsl::capture_simple_bounded_str<u8"?", u8"?">;

static constexpr auto transform_seq_parser =
  lak::dsl::capture_simple_bounded_str<u8"$", u8"$">;

static constexpr auto terminal_string =
  lak::dsl::capture_simple_bounded_str<u8"\"", u8"\"">;

static constexpr auto terminal_char = lak::dsl::capture_2nd<
  lak::dsl::char_literal<U'\''>,
  (lak::dsl::capture_2nd<lak::dsl::str_literal<u8"\\x">,
                         lak::dsl::parsed_hex_uint<char32_t>> |
   lak::dsl::replace_str_literal<u8"\\t", U'\t'> |
   lak::dsl::replace_str_literal<u8"\\n", U'\n'> |
   lak::dsl::replace_str_literal<u8"\\r", U'\r'> |
   lak::dsl::replace_str_literal<u8"\\'", U'\''> |
   lak::dsl::replace_str_literal<u8"\\\"", U'"'> | lak::dsl::any_char32),
  lak::dsl::char_literal<U'\''>>;

static constexpr auto _identifier =
  lak::dsl::ascii_alphanumeric | lak::dsl::char_literal<U'_'>;

static constexpr auto identifier =
  (lak::dsl::ascii_alpha |
   lak::dsl::char_literal<U'_'>)+*(_identifier | ((+ws) + _identifier));

static constexpr auto define_rule =
  lak::dsl::capture_1st<identifier, (*ws) + lak::dsl::char_literal<U'='>>;

static constexpr auto repeat_n =
  lak::dsl::capture_1st<lak::dsl::parsed_dec_uint<size_t>,
                        (*ws) + lak::dsl::char_literal<U'*'>>;

static constexpr auto trailing_punct =
  (*ws) + (lak::dsl::str_literal<u8"->"> | lak::dsl::one_of_chars<U',',
                                                                  U'~',
                                                                  U'+',
                                                                  U'-',
                                                                  U'|',
                                                                  U'^',
                                                                  U';',
                                                                  U')',
                                                                  U']',
                                                                  U'}',
                                                                  U'>',
                                                                  U'$'>);

lak::dsl::result<lak::dsl::ebnf_t::value_type> lak::dsl::ebnf_t::parse(
  lak::u8string_view str) const
{
	lak::u8string_view rem = str;

	lak::ebnf::block result;

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
			range,
			concat,
			altern,
			option,
			repeat,
			repeat_n,
			match_case,
			group,
			capture,
			replace,
			except,
			neg_lookahead,
			pos_lookahead,
			transform,
		} type;

		lak::array<lak::ebnf::rule_value> values;

		size_t count = 0U;

		inline size_t size() const { return values.size(); }
	};

	lak::array<working_data> working_tree;
	lak::optional<lak::ebnf::rule_value> unused_value;

	auto pop_unused = [&]() -> bool
	{
		if (!unused_value) return false;
		working_tree.back().values.push_back(lak::move(*unused_value));
		unused_value.reset();
		return true;
	};

	auto pop_specific_unused =
	  [&](lak::ebnf::rule_value::value_type type) -> bool
	{
		if (!unused_value) return false;
		if (unused_value->type != type) return false;
		working_tree.back().values.push_back(lak::move(*unused_value));
		unused_value.reset();
		return true;
	};

	auto pop_values = [&]() -> size_t
	{
		size_t begin = result.rule_values.size();
		result.rule_values.insert(result.rule_values.end(),
		                          lak::move(working_tree.back().values));
		working_tree.pop_back();
		return begin;
	};

	auto pop_repeat_n = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		if (working_tree.back().size() != 0U)
			return lak::err_t{u8"invalid repeat"_str};

		if (!pop_unused()) return lak::err_t{u8"missing contents of repeat"_str};

		const size_t count = working_tree.back().count;
		const size_t index = pop_values();
		unused_value.emplace(lak::ebnf::rule_value{
		  .type  = lak::ebnf::rule_value::value_type::repetition,
		  .index = result.repetitions.size(),
		});
		result.repetitions.push_back({
		  .count = count,
		  .index = index,
		});

		return lak::ok_t{};
	};

	auto pop_negative_lookahead = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		if (working_tree.back().size() != 0U)
			return lak::err_t{u8"invalid negative lookahead"_str};

		if (!pop_unused())
			return lak::err_t{u8"missing contents of negative lookahead"_str};

		const size_t index = pop_values();
		unused_value.emplace(lak::ebnf::rule_value{
		  .type  = lak::ebnf::rule_value::value_type::negative_lookahead,
		  .index = result.negative_lookaheads.size(),
		});
		result.negative_lookaheads.push_back({
		  .index = index,
		});

		return lak::ok_t{};
	};

	auto pop_positive_lookahead = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		if (working_tree.back().size() != 0U)
			return lak::err_t{u8"invalid positive lookahead"_str};

		if (!pop_unused())
			return lak::err_t{u8"missing contents of positive lookahead"_str};

		const size_t index = pop_values();
		unused_value.emplace(lak::ebnf::rule_value{
		  .type  = lak::ebnf::rule_value::value_type::positive_lookahead,
		  .index = result.positive_lookaheads.size(),
		});
		result.positive_lookaheads.push_back({
		  .index = index,
		});

		return lak::ok_t{};
	};

	auto pop_range = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		if (!pop_specific_unused(lak::ebnf::rule_value::value_type::character))
			return lak::err_t{u8"missing range character"_str};

		if (working_tree.back().size() != 2U)
			return lak::err_t{u8"invalid range"_str};

		const size_t begin = pop_values();
		unused_value.emplace(lak::ebnf::rule_value{
		  .type  = lak::ebnf::rule_value::value_type::range,
		  .index = result.ranges.size(),
		});
		result.ranges.push_back({
		  .begin = begin,
		  .end   = begin + 1U,
		});

		return lak::ok_t{};
	};

	auto pop_trailing = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		while (!working_tree.empty())
		{
			if (working_tree.back().type == working_data::value_type::range)
			{
				RES_TRY(pop_range());
			}
			else if (working_tree.back().type == working_data::value_type::repeat_n)
			{
				RES_TRY(pop_repeat_n());
			}
			else if (working_tree.back().type ==
			         working_data::value_type::neg_lookahead)
			{
				RES_TRY(pop_negative_lookahead());
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

	auto pop_replace = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		if (!pop_specific_unused(lak::ebnf::rule_value::value_type::special))
			return lak::err_t{u8"missing replacement"_str};

		if (working_tree.back().size() != 2U)
			return lak::err_t{u8"invalid replace"_str};

		const size_t begin = pop_values();
		unused_value.emplace(lak::ebnf::rule_value{
		  .type  = lak::ebnf::rule_value::value_type::replace,
		  .index = result.replaces.size(),
		});
		result.replaces.push_back({
		  .index   = begin,
		  .special = begin + 1U,
		});

		return pop_trailing();
	};

	auto pop_except = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		if (!pop_unused()) return lak::err_t{u8"missing exception rule"_str};

		if (working_tree.back().size() != 2U)
			return lak::err_t{u8"invalid exception"_str};

		const size_t begin = pop_values();
		unused_value.emplace(lak::ebnf::rule_value{
		  .type  = lak::ebnf::rule_value::value_type::exception,
		  .index = result.exceptions.size(),
		});
		result.exceptions.push_back({
		  .rule        = begin,
		  .except_rule = begin + 1U,
		});

		return pop_trailing();
	};

	auto pop_match_case = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		if (!pop_unused()) return lak::err_t{u8"missing on-matched rule"_str};

		if (working_tree.back().size() != 2U)
			return lak::err_t{u8"invalid match case"_str};

		// abuse .index to hold subvalue indices until it can be patched in
		// pop_altern
		const size_t index = pop_values();
		unused_value.emplace(lak::ebnf::rule_value{
		  .type  = lak::ebnf::rule_value::value_type::match_case,
		  .index = index,
		});

		return pop_trailing();
	};

	auto pop_altern = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		if (!pop_unused())
			return lak::err_t{u8"missing final value of alternation"_str};

		if (working_tree.back().size() <= working_tree.back().count)
			return lak::err_t{u8"invalid alternation"_str};

		// subsequences of match cases should be treated as if they were grouped

		auto first_match_case_subsequence =
		  [](lak::span<lak::ebnf::rule_value> values)
		  -> lak::span<lak::ebnf::rule_value>
		{
			auto first_match_case = lak::find_if(
			  values.begin(),
			  values.end(),
			  [](const lak::ebnf::rule_value &v)
			  { return v.type == lak::ebnf::rule_value::value_type::match_case; });

			if (first_match_case == values.end()) return {};

			auto first_non_match_case = lak::find_if(
			  first_match_case,
			  values.end(),
			  [](const lak::ebnf::rule_value &v)
			  { return v.type != lak::ebnf::rule_value::value_type::match_case; });

			return lak::span(first_match_case, first_non_match_case);
		};

		for (lak::span<lak::ebnf::rule_value> match_subseq;
		     !(match_subseq =
		         first_match_case_subsequence(working_tree.back().values))
		        .empty();)
		{
			ASSERT_GREATER_OR_EQUAL(working_tree.back().size(), match_subseq.size());

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

			lak::ebnf::rule_value match_seq{
			  .type  = lak::ebnf::rule_value::value_type::match_sequence,
			  .index = result.match_sequences.size(),
			};
			result.match_sequences.push_back({
			  .begin = result.rule_values.size(),
			  .end   = result.rule_values.size() + match_subseq.size(),
			});
			result.rule_values.insert(
			  result.rule_values.end(), match_subseq.begin(), match_subseq.end());

			match_subseq[0] = match_seq;

			working_tree.back().values.erase(match_subseq.begin() + 1U,
			                                 match_subseq.end());
		}

		const size_t sz = working_tree.back().size();

		if (sz == 1U && working_tree.back().values.back().type ==
		                  lak::ebnf::rule_value::value_type::match_sequence)
		{
			// entire sequence was a match sequence
			auto seq = lak::move(working_tree.back().values.back());
			working_tree.pop_back();
			unused_value.emplace(lak::move(seq));
		}
		else
		{
			if (sz < 2U) return lak::err_t{u8"invalid alternation length"_str};

			const size_t begin = pop_values();
			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::alternation,
			  .index = result.alternations.size(),
			});
			result.alternations.push_back({
			  .begin = begin,
			  .end   = begin + sz,
			});
		}

		return pop_trailing();
	};

	auto pop_concat = [&]() -> lak::error_code<lak::dsl::err::parse>
	{
		if (!pop_unused())
			return lak::err_t{u8"missing final value of concatenation"_str};

		const size_t sz = working_tree.back().size();

		if (sz <= working_tree.back().count)
			return lak::err_t{u8"invalid concatenation"_str};

		if (sz < 2U) return lak::err_t{u8"invalid concatenation length"_str};

		const size_t begin = pop_values();
		unused_value.emplace(lak::ebnf::rule_value{
		  .type  = lak::ebnf::rule_value::value_type::concatenation,
		  .index = result.concatenations.size(),
		});
		result.concatenations.push_back({
		  .begin = begin,
		  .end   = begin + sz,
		});

		return pop_trailing();
	};

	while (!rem.empty())
	{
		if (working_tree.empty())
		{
			// start of a new rule
			RES_TRY_ASSIGN(auto rule_name =, define_rule.EBNF_PARSE());
			working_tree.push_back({
			  .type  = working_data::value_type::rule,
			  .count = result.rules.size(),
			});
			result.rules.push_back({
			  .name       = rule_name.value,
			  .definition = {},
			});
		}
		else if_let_ok (auto iden, identifier.EBNF_PARSE())
		{
			if (unused_value) return lak::err_t{u8"unexpected identifier"_str};
			RES_TRY(trailing_punct.parse(rem));
			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::rule,
			  .index = result.identifiers.size(),
			});
			result.identifiers.push_back(iden.value);
			RES_TRY(pop_trailing());
		}
		else if_let_ok (auto str, terminal_string.EBNF_PARSE())
		{
			if (unused_value) return lak::err_t{u8"unexpected string literal"_str};
			RES_TRY(trailing_punct.parse(rem));
			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::string,
			  .index = result.strings.size(),
			});
			result.strings.push_back(str.value);
			RES_TRY(pop_trailing());
		}
		else if_let_ok (auto chr, terminal_char.EBNF_PARSE())
		{
			if (unused_value) return lak::err_t{u8"unexpected char literal"_str};
			RES_TRY(trailing_punct.parse(rem));
			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::character,
			  .index = result.characters.size(),
			});
			result.characters.push_back(chr.value);
			RES_TRY(pop_trailing());
		}
		else if_let_ok (auto spec, special_seq_parser.EBNF_PARSE())
		{
			if (unused_value) return lak::err_t{u8"unexpected special sequence"_str};
			RES_TRY(trailing_punct.parse(rem));
			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::special,
			  .index = result.specials.size(),
			});
			result.specials.push_back(spec.value);
			RES_TRY(pop_trailing());
		}
		else if_let_ok (auto rn, repeat_n.EBNF_PARSE())
		{
			working_tree.push_back({
			  .type  = working_data::value_type::repeat_n,
			  .count = rn.value,
			});
		}
		else if (lak::dsl::char_literal<U'['>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type = working_data::value_type::option,
			});
		}
		else if (lak::dsl::char_literal<U']'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::replace)
				{
					RES_TRY(pop_replace());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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
				return lak::err_t{u8"unexpected end of optional"_str};

			if (!pop_unused())
				return lak::err_t{u8"missing contents of optional"_str};

			if (working_tree.back().size() != 1U)
				return lak::err_t{u8"invalid optional length"_str};

			const size_t index = pop_values();
			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::optional,
			  .index = result.optionals.size(),
			});
			result.optionals.push_back({
			  .index = index,
			});

			RES_TRY(pop_trailing());
		}
		else if (lak::dsl::char_literal<U'{'>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type = working_data::value_type::repeat,
			});
		}
		else if (lak::dsl::char_literal<U'}'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::replace)
				{
					RES_TRY(pop_replace());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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
				return lak::err_t{u8"unexpected end of repetition"_str};

			if (!pop_unused())
				return lak::err_t{u8"missing contents of repetition"_str};

			if (working_tree.back().size() != 1U)
				return lak::err_t{u8"invalid repetition length"_str};

			const size_t index = pop_values();
			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::repetition,
			  .index = result.repetitions.size(),
			});
			result.repetitions.push_back({
			  .count = lak::nullopt,
			  .index = index,
			});

			RES_TRY(pop_trailing());
		}
		else if (lak::dsl::char_literal<U'('>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type = working_data::value_type::group,
			});
		}
		else if (lak::dsl::char_literal<U')'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::replace)
				{
					RES_TRY(pop_replace());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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
				return lak::err_t{u8"unexpected end of grouping"_str};

			if (!pop_unused())
				return lak::err_t{u8"missing contents of grouping"_str};

			if (working_tree.back().size() != 1U)
				return lak::err_t{u8"invalid grouping length"_str};

			const size_t index = pop_values();
			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::grouping,
			  .index = result.groupings.size(),
			});
			result.groupings.push_back({
			  .index = index,
			});

			RES_TRY(pop_trailing());
		}
		else if (lak::dsl::char_literal<U'<'>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type = working_data::value_type::capture,
			});
		}
		else if (lak::dsl::char_literal<U'>'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::replace)
				{
					RES_TRY(pop_replace());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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
				return lak::err_t{u8"unexpected end of capture group"_str};

			if (!pop_unused())
				return lak::err_t{u8"missing contents of capture group"_str};

			if (working_tree.back().size() != 1U)
				return lak::err_t{u8"invalid capture group length"_str};

			const size_t index = pop_values();
			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::capture,
			  .index = result.captures.size(),
			});
			result.captures.push_back({
			  .index = index,
			});

			RES_TRY(pop_trailing());
		}
		else if (lak::dsl::char_literal<U'~'>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type = working_data::value_type::range,
			});

			if (!pop_specific_unused(lak::ebnf::rule_value::value_type::character))
				return lak::err_t{u8"missing beginning of range sequence"_str};
		}
		else if (lak::dsl::str_literal<u8"->">.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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

			if (working_tree.back().type != working_data::value_type::altern)
			{
				working_tree.push_back({
				  .type = working_data::value_type::altern,
				});
			}

			working_tree.push_back({
			  .type = working_data::value_type::match_case,
			});

			if (!pop_unused())
				return lak::err_t{u8"missing beginning of match case"_str};
		}
		else if (lak::dsl::char_literal<U'-'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				else
					break;
				ASSERT_GREATER(s, working_tree.size());
			}

			if (unused_value)
			{
				working_tree.push_back({
				  .type = working_data::value_type::except,
				});

				working_tree.back().values.push_back(lak::move(*unused_value));
				unused_value.reset();
			}
			else
			{
				working_tree.push_back({
				  .type = working_data::value_type::neg_lookahead,
				});
			}
		}
		else if (lak::dsl::char_literal<U'^'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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

			working_tree.push_back({
			  .type = working_data::value_type::replace,
			});

			if (!pop_unused())
				return lak::err_t{u8"missing beginning of replace sequence"_str};
		}
		else if (lak::dsl::char_literal<U'+'>.EBNF_PARSE().is_ok())
		{
			working_tree.push_back({
			  .type = working_data::value_type::pos_lookahead,
			});
		}
		else if (lak::dsl::char_literal<U','>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::replace)
				{
					RES_TRY(pop_replace());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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
				working_tree.push_back({
				  .type = working_data::value_type::concat,
				});
			}
			else
				++working_tree.back().count;

			if (!pop_unused())
				return lak::err_t{u8"unexpected concatenation delimiter"_str};
		}
		else if (lak::dsl::char_literal<U'|'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::replace)
				{
					RES_TRY(pop_replace());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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
				working_tree.push_back({
				  .type = working_data::value_type::altern,
				});
			}
			else
				++working_tree.back().count;

			if (!pop_unused())
				return lak::err_t{u8"unexpected alternation delimiter"_str};
		}
		else if_let_ok (auto trans, transform_seq_parser.EBNF_PARSE())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::replace)
				{
					RES_TRY(pop_replace());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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
				  u8"transform sequences are only valid at the end of a rule"_str};

			if (working_tree.back().size() != 0U)
				return lak::err_t{u8"invalid transform sequence"_str};

			if (!pop_unused())
				return lak::err_t{u8"missing contents of transform sequence"_str};

			unused_value.emplace(lak::ebnf::rule_value{
			  .type  = lak::ebnf::rule_value::value_type::transform,
			  .index = result.transforms.size(),
			});
			result.transforms.push_back(trans.value);
		}
		else if (lak::dsl::char_literal<U';'>.EBNF_PARSE().is_ok())
		{
			while (true)
			{
				size_t s = working_tree.size();
				if (working_tree.back().type == working_data::value_type::range)
				{
					RES_TRY(pop_range());
				}
				if (working_tree.back().type == working_data::value_type::concat)
				{
					RES_TRY(pop_concat());
				}
				else if (working_tree.back().type == working_data::value_type::altern)
				{
					RES_TRY(pop_altern());
				}
				else if (working_tree.back().type == working_data::value_type::replace)
				{
					RES_TRY(pop_replace());
				}
				else if (working_tree.back().type == working_data::value_type::except)
				{
					RES_TRY(pop_except());
				}
				else if (working_tree.back().type ==
				         working_data::value_type::neg_lookahead)
				{
					RES_TRY(pop_negative_lookahead());
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
				return lak::err_t{u8"unexpected end of rule"_str};

			if (unused_value)
			{
				working_tree.back().values.push_back(lak::move(*unused_value));
				unused_value.reset();
			}

			if (working_tree.back().size() != 1U &&
			    !(working_tree.back().size() == 2U &&
			      working_tree.back().values.back().type ==
			        lak::ebnf::rule_value::value_type::transform))
				return lak::err_t{u8"unexpected overlong rule"_str};

			const size_t count = working_tree.back().count;
			const size_t size  = working_tree.back().size();
			const size_t begin = pop_values();

			auto &rule = result.rules[count];

			rule.definition = begin;
			if (size == 2U)
				rule.transform = begin + 1U;
			else
				rule.transform = lak::nullopt;
		}
		else
			return lak::err_t{u8"bad state"_str};

		RES_TRY((*ws).EBNF_PARSE());
	}
#undef EBNF_PARSE

	if (working_tree.size() > 0U) return lak::err_t{lak::err::out_of_data{}};

	ASSERT_EQUAL(working_tree.size(), 0U);

	return lak::ok_t<lak::dsl::parse_result<value_type>>{{
	  .consumed  = str.substr(str.size() - rem.size()),
	  .remaining = rem,
	  .value     = lak::move(result),
	}};
}
