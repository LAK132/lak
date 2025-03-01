#ifndef LAK_DSL_JSON_HPP
#define LAK_DSL_JSON_HPP

#include "lak/array.hpp"
#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"
#include "lak/result.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	namespace dsl
	{
		struct json_value
		{
			enum struct value_type
			{
				token,
				string,
				number,
				array,
				object,
			} type;
			size_t index;
		};

		struct json_array
		{
			size_t begin;
			size_t end;

			size_t size() const { return end - begin; }
		};

		struct json_object
		{
			size_t begin;
			size_t end;

			size_t size() const { return end - begin; }
		};

		struct json_block
		{
			lak::array<lak::u8string_view> tokens;
			lak::array<lak::u8string_view> strings;
			lak::array<lak::u8string_view> numbers;
			lak::array<json_value> values;   // indexes into tokens/strings/numbers
			lak::array<json_array> arrays;   // indexes into values
			lak::array<json_object> objects; // indexes into values (interlaced kvs)

			// if not empty, root is values.front()
		};

		struct json_t
		{
			static constexpr bool is_pure_match = false;

			using value_type = json_block;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				lak::u8string_view rem = str;

				json_block result;

				auto move_str =
				  [&](const lak::dsl::parse_result<lak::u8string_view> &res)
				{ rem = res.remaining; };

				constexpr auto lbrace        = lak::dsl::char_literal<U'{'>;
				constexpr auto rbrace        = lak::dsl::char_literal<U'}'>;
				constexpr auto lbracket      = lak::dsl::char_literal<U'['>;
				constexpr auto rbracket      = lak::dsl::char_literal<U']'>;
				constexpr auto colon         = lak::dsl::char_literal<U':'>;
				constexpr auto comma         = lak::dsl::char_literal<U','>;
				constexpr auto whitepsace    = lak::dsl::one_of_chars_str<U" \r\n\t">;
				constexpr auto number_parser = lak::dsl::as_pure<
				  lak::dsl::dec_float<lak::dsl::char_literal<U'.'>,
				                      lak::dsl::one_of_chars_str<U"eE">>>;
				constexpr auto escaped_string_character =
				  lak::dsl::str_literal<u8"\\\""> | lak::dsl::str_literal<u8"\\\\"> |
				  lak::dsl::str_literal<u8"\\/"> | lak::dsl::str_literal<u8"\\b"> |
				  lak::dsl::str_literal<u8"\\f"> | lak::dsl::str_literal<u8"\\n"> |
				  lak::dsl::str_literal<u8"\\r"> | lak::dsl::str_literal<u8"\\t"> |
				  (lak::dsl::str_literal<u8"\\u"> +
				   (lak::dsl::repeat_exact<lak::dsl::hex_digit, 4>));
				constexpr auto string_parser =
				  lak::dsl::char_literal<U'"'> +
				  *(escaped_string_character |
				    !lak::dsl::char_literal<U'"'>)+lak::dsl::char_literal<U'"'>;
				constexpr auto token_parser = lak::dsl::str_literal<u8"true"> |
				                              lak::dsl::str_literal<u8"false"> |
				                              lak::dsl::str_literal<u8"null">;

				RES_TRY((*whitespace).parse(rem).if_ok(move_str));

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
						array,
						object,
						kvpair
					} type;

					size_t begin;
					size_t size;
				};

				lak::array<json_value> working_values;
				lak::array<working_data> working_tree;

				auto pop_values = [&](size_t count) -> size_t
				{
					size_t begin = result.values.size();
					result.values.reserve(result.values.size() + count);
					for (size_t i = working_values.size() - count;
					     i < working_values.size();
					     ++i)
						result.values.push_back(working_values[i]);
					working_values.resize(working_values.size() - count);
					return begin;
				};

				auto pop_kvpair =
				  [&](bool allow_empty) -> lak::error_code<lak::dsl::parse_error>
				{
					if (working_tree.back().size == 2U)
					{
						working_tree.pop_back();
						working_tree.back().size += 2U;
					}
					else if (allow_empty && working_tree.back().size == 0U)
						working_tree.pop_back();
					else if (working_tree.back().size == 1U)
						return lak::err_t{
						  lak::dsl::parse_error{.message = u8"expected kvpair value"}};
					else
						return lak::err_t{
						  lak::dsl::parse_error{.message = u8"invalid kvpair length"}};

					return lak::ok_t{};
				};

				// this will be replaced with the last value in the working values once
				// parsing is complete
				result.values.emplace_back();

				do
				{
					RES_TRY((*whitespace).parse(rem).if_ok(move_str));

					if (rem.empty())
						return lak::err_t{
						  lak::dsl::parse_error{.message = u8"out of data"}};

					if (lbrace.parse(rem).if_ok(move_str).is_ok())
					{
						working_tree.push_back({
						  .type  = working_data::value_type::object,
						  .begin = working_values.size(),
						  .size  = 0U,
						});
						working_tree.push_back({
						  .type  = working_data::value_type::kvpair,
						  .begin = working_values.size(),
						  .size  = 0U,
						});
					}
					else if (colon.parse(rem).if_ok(move_str).is_ok())
					{
						if (working_tree.back().type != working_data::value_type::kvpair)
							return lak::err_t{
							  lak::dsl::parse_error{.message = u8"unexpected ':'"}};

						if (working_tree.back().begin + 1U > working_values.size())
							return lak::err_t{lak::dsl::parse_error{
							  .message = u8"expected kvpair key, got ':'"}};

						if (working_tree.back().size >= 1U)
							return lak::err_t{lak::dsl::parse_error{
							  .message = u8"expected kvpair value, got ':'"}};

						++working_tree.back().size;
					}
					else if (comma.parse(rem).if_ok(move_str).is_ok())
					{
						if (working_tree.back().type == working_data::value_type::array)
						{
							if (working_tree.back().begin + working_tree.back().size >=
							    working_values.size())
								return lak::err_t{lak::dsl::parse_error{
								  .message = u8"expected array value, got ','"}};

							++working_tree.back().size;
						}
						else if (working_tree.back().type ==
						         working_data::value_type::kvpair)
						{
							if (working_tree.back().size == 0U)
								return lak::err_t{lak::dsl::parse_error{
								  .message = u8"expected kvpair key, got ','"}};

							if (working_tree.back().begin + 2U > working_values.size())
								return lak::err_t{lak::dsl::parse_error{
								  .message = u8"expected kvpair value, got ','"}};

							++working_tree.back().size;

							RES_TRY(pop_kvpair(false));

							working_tree.push_back({
							  .type  = working_data::value_type::kvpair,
							  .begin = working_values.size(),
							  .size  = 0U,
							});
						}
						else
							return lak::err_t{
							  lak::dsl::parse_error{.message = u8"unexpected ','"}};
					}
					else if (rbrace.parse(rem).if_ok(move_str).is_ok())
					{
						if (working_tree.back().type != working_data::value_type::kvpair ||
						    working_tree.back().size >= 2U)
							return lak::err_t{
							  lak::dsl::parse_error{.message = u8"unexpected '}'"}};

						if (working_tree.back().size == 1U)
						{
							if (working_tree.back().begin + 2U != working_values.size())
								return lak::err_t{lak::dsl::parse_error{
								  .message = u8"expected kvpair value, got ','"}};
							else
								++working_tree.back().size;
						}

						RES_TRY(pop_kvpair(true));

						if (working_tree.back().type != working_data::value_type::object)
							return lak::err_t{
							  lak::dsl::parse_error{.message = u8"unexpected '}'"}};

						size_t begin = pop_values(working_tree.back().size);
						working_values.push_back({
						  .type  = json_value::value_type::object,
						  .index = result.objects.size(),
						});
						result.objects.push_back({
						  .begin = begin,
						  .end   = begin + working_tree.back().size,
						});
						working_tree.pop_back();
					}
					else if (lbracket.parse(rem).if_ok(move_str).is_ok())
					{
						working_tree.push_back({
						  .type  = working_data::value_type::array,
						  .begin = working_values.size(),
						  .size  = 0U,
						});
					}
					else if (rbracket.parse(rem).if_ok(move_str).is_ok())
					{
						if (working_tree.back().type != working_data::value_type::array)
							return lak::err_t{
							  lak::dsl::parse_error{.message = u8"unexpected ']'"}};

						if (working_tree.back().begin + working_tree.back().size + 1U ==
						    working_values.size())
							++working_tree.back().size;

						size_t begin = pop_values(working_tree.back().size);
						working_values.push_back({
						  .type  = json_value::value_type::array,
						  .index = result.arrays.size(),
						});
						result.arrays.push_back({
						  .begin = begin,
						  .end   = begin + working_tree.back().size,
						});
						working_tree.pop_back();
					}
					else if_let_ok (auto tok, token_parser.parse(rem).if_ok(move_str))
					{
						working_values.push_back({
						  .type  = json_value::value_type::token,
						  .index = result.tokens.size(),
						});
						result.tokens.push_back(tok.value);
					}
					else if_let_ok (auto num, number_parser.parse(rem).if_ok(move_str))
					{
						working_values.push_back({
						  .type  = json_value::value_type::number,
						  .index = result.numbers.size(),
						});
						result.numbers.push_back(num.value);
					}
					else if_let_ok (auto str, string_parser.parse(rem).if_ok(move_str))
					{
						working_values.push_back({
						  .type  = json_value::value_type::string,
						  .index = result.strings.size(),
						});
						result.strings.push_back(str.value);
					}
					else
						return lak::err_t{
						  lak::dsl::parse_error{.message = u8"unexpected data"}};
				} while (!working_tree.empty());

				if (working_values.size() == 0U)
					return lak::err_t{
					  lak::dsl::parse_error{.message = u8"missing root value"}};
				else if (working_values.size() > 1U)
					return lak::err_t{
					  lak::dsl::parse_error{.message = u8"unused working values"}};

				result.values.front() = working_values.back();

				return lak::ok_t<lak::dsl::parse_result<value_type>>{{
				  .consumed  = str.substr(str.size() - rem.size()),
				  .remaining = rem,
				  .value     = lak::move(result),
				}};
			}
		};

		inline constexpr json_t json;

		static_assert(lak::dsl::parser<json_t>);
	}
}

#endif
