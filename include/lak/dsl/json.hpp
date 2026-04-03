#ifndef LAK_DSL_JSON_HPP
#define LAK_DSL_JSON_HPP

#include "lak/array.hpp"
#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"
#include "lak/format.hpp"
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

		namespace err
		{
			struct json_unexpected_type
			{
				lak::dsl::json_value::value_type expected;
				lak::dsl::json_value::value_type got;
			};
		}

		struct json_array
		{
			size_t begin;
			size_t end;

			inline size_t size() const { return end - begin; }
		};

		struct json_object
		{
			size_t begin;
			size_t end;

			inline size_t size() const { return end - begin; }
		};

		struct json_value_proxy;
		struct json_array_proxy;
		struct json_object_proxy;

		struct json_block
		{
			lak::array<char8_t> _internal;
			lak::array<lak::u8string_view> tokens;
			lak::array<lak::u8string_view> strings;
			lak::array<lak::u8string_view> numbers;
			lak::array<json_value> values;   // indexes into tokens/strings/numbers
			lak::array<json_array> arrays;   // indexes into values
			lak::array<json_object> objects; // indexes into values (interlaced kvs)

			void intern(); // reallocate views internally to the json_block

			json_value_proxy root() const;
		};

		struct json_token_proxy : public lak::u8string_view
		{
		};
		struct json_string_proxy : public lak::u8string_view
		{
		};
		struct json_number_proxy : public lak::u8string_view
		{
		};

		struct json_array_proxy
		{
			const json_block &block;
			json_array array;

			inline size_t size() const { return array.size(); }

			json_value_proxy operator[](size_t index) const;
		};

		struct json_object_proxy
		{
			const json_block &block;
			json_object object;

			inline size_t size() const { return object.size() / 2U; }

			lak::pair<lak::u8string_view, json_value_proxy> operator[](
			  size_t index) const;

			json_value_proxy operator[](lak::u8string_view key) const;
		};

		struct json_value_proxy
		{
			template<typename T>
			using result_type = lak::result<T, lak::dsl::err::json_unexpected_type>;
			template<typename T>
			using num_result_type =
			  lak::result<T,
			              lak::variant<lak::dsl::err::json_unexpected_type,
			                           lak::err::string_to_numeric>>;

			const json_block &block;
			json_value value;

			bool is_none() const;
			bool is_token() const;
			bool is_string() const;
			bool is_number() const;
			bool is_array() const;
			bool is_object() const;

			result_type<lak::u8string_view> token() const;
			result_type<lak::u8string_view> string() const;
			result_type<lak::u8string_view> number_str() const;
			result_type<json_array_proxy> array() const;
			result_type<json_object_proxy> object() const;

			template<typename NUM>
			num_result_type<NUM> number() const
			{
				return number_str().and_then(
				  [](lak::u8string_view num) -> num_result_type<NUM>
				  {
					  if constexpr (!std::numeric_limits<NUM>::is_integer)
					  {
						  constexpr auto num_parser =
						    lak::dsl::dec_float<lak::dsl::char_literal<U'.'>,
						                        lak::dsl::one_of_chars_str<U"eE">>;
						  auto [intp, fracp, expp] = num_parser.parse(num).UNWRAP();
						  RES_TRY_ASSIGN(double v =,
						                 lak::dec_string_to_double(intp, fracp, expp));
						  if (v > std::numeric_limits<NUM>::max() ||
						      v < std::numeric_limits<NUM>::lowest())
							  return lak::err_t{lak::err::string_to_numeric::out_of_bounds};
						  else
							  return lak::ok_t{static_cast<NUM>(v)};
					  }
					  else
					  {
						  RES_TRY_ASSIGN(
						    NUM result =,
						    lak::string_to_int<NUM>(num, lak::numeric_base::dec));
						  return lak::ok_t{result};
					  }
				  });
			}

			inline auto visit(auto &&func) const
			{
				switch (value.type)
				{
					case json_value::value_type::token:
						return func(json_token_proxy{block.tokens[value.index]});
					case json_value::value_type::string:
						return func(json_string_proxy{block.strings[value.index]});
					case json_value::value_type::number:
						return func(json_number_proxy{block.numbers[value.index]});
					case json_value::value_type::array:
						return func(json_array_proxy{.block = block,
						                             .array = block.arrays[value.index]});
					case json_value::value_type::object:
						return func(json_object_proxy{
						  .block = block, .object = block.objects[value.index]});
					default: ASSERT_UNREACHABLE();
				}
			}

			inline explicit operator bool() const { return !is_none(); }
		};

		struct json_parser
		{
			static constexpr bool is_pure_match = false;

			using value_type = json_block;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const;
		};

		inline constexpr json_parser json;

		static_assert(lak::dsl::concepts::parser<json_parser>);
	}

	template<typename CHAR>
	struct format_traits<lak::dsl::json_value::value_type, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::dsl::json_value::value_type &type)
		{
			switch (type)
			{
				case lak::dsl::json_value::value_type::token:
					return lak::strconv<CHAR>("token"_view);
				case lak::dsl::json_value::value_type::string:
					return lak::strconv<CHAR>("string"_view);
				case lak::dsl::json_value::value_type::number:
					return lak::strconv<CHAR>("number"_view);
				case lak::dsl::json_value::value_type::array:
					return lak::strconv<CHAR>("array"_view);
				case lak::dsl::json_value::value_type::object:
					return lak::strconv<CHAR>("object"_view);
				default: return lak::fmt<CHAR, "{:#0X}">(static_cast<uintmax_t>(type));
			}
		}
	};

	template<typename CHAR>
	struct format_traits<lak::dsl::err::json_unexpected_type, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::dsl::err::json_unexpected_type &err)
		{
			return lak::fmt<CHAR, "expected {0}, got {1}">(err.expected, err.got);
		}
	};
}

#endif
