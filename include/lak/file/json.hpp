#ifndef LAK_FILE_JSON_HPP
#define LAK_FILE_JSON_HPP

#include "lak/array.hpp"
#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"
#include "lak/format.hpp"
#include "lak/result.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	namespace json
	{
		struct value
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
			struct unexpected_type
			{
				lak::json::value::value_type expected;
				lak::json::value::value_type got;
			};
		}

		struct array
		{
			size_t begin;
			size_t end;

			inline size_t size() const { return end - begin; }
		};

		struct object
		{
			size_t begin;
			size_t end;

			inline size_t size() const { return end - begin; }
		};

		struct value_proxy;
		struct array_proxy;
		struct object_proxy;

		struct block
		{
			lak::array<char8_t> _internal;
			lak::array<lak::u8string_view> tokens;
			lak::array<lak::u8string_view> strings;
			lak::array<lak::u8string_view> numbers;

			// indexes into tokens/strings/numbers
			lak::array<lak::json::value> values;

			// indexes into values
			lak::array<lak::json::array> arrays;

			// indexes into values (interlaced kvs)
			lak::array<lak::json::object> objects;

			void intern(); // reallocate views internally to the block

			lak::json::value_proxy root() const;
		};

		struct token_proxy : public lak::u8string_view
		{
		};
		struct string_proxy : public lak::u8string_view
		{
		};
		struct number_proxy : public lak::u8string_view
		{
		};

		struct array_proxy
		{
			const lak::json::block &block;
			lak::json::array array;

			inline size_t size() const { return array.size(); }

			lak::json::value_proxy operator[](size_t index) const;
		};

		struct object_proxy
		{
			const lak::json::block &block;
			lak::json::object object;

			inline size_t size() const { return object.size() / 2U; }

			lak::pair<lak::u8string_view, lak::json::value_proxy> operator[](
			  size_t index) const;

			lak::json::value_proxy operator[](lak::u8string_view key) const;
		};

		struct value_proxy
		{
			template<typename T>
			using result_type = lak::result<T, lak::json::err::unexpected_type>;
			template<typename T>
			using num_result_type =
			  lak::result<T,
			              lak::variant<lak::json::err::unexpected_type,
			                           lak::err::string_to_numeric>>;

			const lak::json::block &block;
			lak::json::value value;

			bool is_none() const;
			bool is_token() const;
			bool is_string() const;
			bool is_number() const;
			bool is_array() const;
			bool is_object() const;

			result_type<lak::u8string_view> token() const;
			result_type<lak::u8string_view> string() const;
			result_type<lak::u8string_view> number_str() const;
			result_type<lak::json::array_proxy> array() const;
			result_type<lak::json::object_proxy> object() const;

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
					case lak::json::value::value_type::token:
						return func(lak::json::token_proxy{block.tokens[value.index]});
					case lak::json::value::value_type::string:
						return func(lak::json::string_proxy{block.strings[value.index]});
					case lak::json::value::value_type::number:
						return func(lak::json::number_proxy{block.numbers[value.index]});
					case lak::json::value::value_type::array:
						return func(lak::json::array_proxy{
						  .block = block, .array = block.arrays[value.index]});
					case lak::json::value::value_type::object:
						return func(lak::json::object_proxy{
						  .block = block, .object = block.objects[value.index]});
					default: ASSERT_UNREACHABLE();
				}
			}

			inline explicit operator bool() const { return !is_none(); }
		};
	}

	namespace dsl
	{
		struct json_t
		{
			static constexpr bool is_pure_match = false;

			using value_type = lak::json::block;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const;
		};

		inline constexpr json_t json;

		static_assert(lak::dsl::concepts::parser<json_t>);
	}

	template<typename CHAR>
	struct format_traits<lak::json::value::value_type, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::json::value::value_type &type)
		{
			switch (type)
			{
				case lak::json::value::value_type::token:
					return lak::strconv<CHAR>("token"_view);
				case lak::json::value::value_type::string:
					return lak::strconv<CHAR>("string"_view);
				case lak::json::value::value_type::number:
					return lak::strconv<CHAR>("number"_view);
				case lak::json::value::value_type::array:
					return lak::strconv<CHAR>("array"_view);
				case lak::json::value::value_type::object:
					return lak::strconv<CHAR>("object"_view);
				default: return lak::fmt<CHAR, "{:#0X}">(static_cast<uintmax_t>(type));
			}
		}
	};

	template<typename CHAR>
	struct format_traits<lak::json::err::unexpected_type, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::json::err::unexpected_type &err)
		{
			return lak::fmt<CHAR, "expected {0}, got {1}">(err.expected, err.got);
		}
	};
}

#endif
