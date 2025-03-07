#ifndef LAK_DSL_UTILITY_HPP
#define LAK_DSL_UTILITY_HPP

#include "lak/dsl/dsl.hpp"

#include "lak/char_utils.hpp"
#include "lak/numeric.hpp"

namespace lak
{
	namespace dsl
	{
		inline constexpr auto newline = lak::dsl::char_literal<U'\n'>;
		inline constexpr auto ascii_whitespace =
		  lak::dsl::one_of_chars_str<lak::char_utils_impl::ascii_spaces32>;
		inline constexpr auto ascii_nonnewline_whitespace =
		  (!lak::dsl::newline) & lak::dsl::ascii_whitespace;
		inline constexpr auto whitespace =
		  lak::dsl::one_of_chars_str<lak::char_utils_impl::spaces>;
		inline constexpr auto nonnewline_whitespace =
		  (!lak::dsl::newline) & lak::dsl::whitespace;

		inline constexpr auto ascii_alpha =
		  lak::dsl::char_range<U'a', U'z'> | lak::dsl::char_range<U'A', U'Z'>;
		inline constexpr auto ascii_numeric = lak::dsl::char_range<U'0', U'9'>;
		inline constexpr auto ascii_alphanumeric =
		  lak::dsl::ascii_alpha | lak::dsl::ascii_numeric;

		inline constexpr auto number_sign = lak::dsl::one_of_chars<U'+', U'-'>;

		template<lak::dsl::parser auto PAR, lak::numeric_base BASE, typename UINT>
		inline constexpr auto parsed_uint = lak::dsl::transform<
		  PAR,
		  [](lak::u8string_view num)
		  {
			  return lak::string_to_uintmax(num, BASE)
			    .map_err(
			      [](lak::string_to_numeric_error)
			      {
				      return lak::dsl::parse_error{
				        .message = u8"error converting string to uintmax_t"};
			      })
			    .and_then(
			      [](uintmax_t v) -> lak::result<UINT, lak::dsl::parse_error>
			      {
				      if (v > std::numeric_limits<UINT>::max())
					      return lak::err_t{lak::dsl::parse_error{
					        .message = u8"parsed value out of range"}};
				      else
					      return lak::ok_t{UINT(v)};
			      });
		  }>;

		inline constexpr auto bin_digit  = lak::dsl::char_range<U'0', U'1'>;
		inline constexpr auto bin_number = +lak::dsl::bin_digit;
		template<typename UINT>
		inline constexpr auto parsed_bin_uint = lak::dsl::
		  parsed_uint<lak::dsl::bin_number, lak::numeric_base::bin, UINT>;
		inline constexpr auto c_bin_prefix =
		  lak::dsl::str_literal<u8"0b"> | lak::dsl::str_literal<u8"0B">;

		inline constexpr auto oct_digit  = lak::dsl::char_range<U'0', U'7'>;
		inline constexpr auto oct_number = +lak::dsl::oct_digit;
		template<typename UINT>
		inline constexpr auto parsed_oct_uint = lak::dsl::
		  parsed_uint<lak::dsl::oct_number, lak::numeric_base::oct, UINT>;
		inline constexpr auto c_oct_prefix = lak::dsl::char_literal<U'0'>;

		inline constexpr auto dec_digit  = lak::dsl::char_range<U'0', U'9'>;
		inline constexpr auto dec_number = +lak::dsl::dec_digit;
		inline constexpr auto signed_dec_number =
		  lak::dsl::optional<lak::dsl::number_sign> + lak::dsl::dec_number;
		template<typename UINT>
		inline constexpr auto parsed_dec_uint = lak::dsl::
		  parsed_uint<lak::dsl::dec_number, lak::numeric_base::dec, UINT>;
		template<typename SINT>
		inline constexpr auto parsed_dec_int = lak::dsl::transform<
		  lak::dsl::signed_dec_number,
		  [](lak::u8string_view num)
		  {
			  return lak::string_to_intmax(num, lak::numeric_base::dec)
			    .map_err(
			      [](lak::string_to_numeric_error)
			      {
				      return lak::dsl::parse_error{
				        .message = u8"error converting dec string to intmax_t"};
			      })
			    .and_then(
			      [](intmax_t v) -> lak::result<SINT, lak::dsl::parse_error>
			      {
				      if (v > std::numeric_limits<SINT>::max() ||
				          v < std::numeric_limits<SINT>::lowest())
					      return lak::err_t{lak::dsl::parse_error{
					        .message = u8"parsed value out of range"}};
				      else
					      return lak::ok_t{SINT(v)};
			      });
		  }>;

		inline constexpr auto hex_digit = lak::dsl::char_range<U'0', U'9'> |
		                                  lak::dsl::char_range<U'a', U'f'> |
		                                  lak::dsl::char_range<U'A', U'F'>;
		inline constexpr auto hex_number = +lak::dsl::hex_digit;
		template<typename UINT>
		inline constexpr auto parsed_hex_uint = lak::dsl::
		  parsed_uint<lak::dsl::hex_number, lak::numeric_base::hex, UINT>;
		inline constexpr auto c_hex_prefix =
		  lak::dsl::str_literal<u8"0x"> | lak::dsl::str_literal<u8"0X">;

		inline constexpr auto simple_c_number =
		  (lak::dsl::c_hex_prefix + lak::dsl::hex_number) |
		  (lak::dsl::c_bin_prefix + lak::dsl::bin_number) |
		  (lak::dsl::c_oct_prefix + lak::dsl::oct_number + -lak::dsl::dec_digit) |
		  lak::dsl::dec_number;
		template<typename UINT>
		inline constexpr auto parsed_simple_c_uint =
		  (lak::dsl::c_hex_prefix + lak::dsl::parsed_hex_uint<UINT>) |
		  (lak::dsl::c_bin_prefix + lak::dsl::parsed_bin_uint<UINT>) |
		  (lak::dsl::c_oct_prefix + lak::dsl::parsed_oct_uint<UINT> +
		   -lak::dsl::dec_digit) |
		  lak::dsl::parsed_dec_uint<UINT>;

		template<lak::dsl::pure_match_parser auto frac_separator,
		         lak::dsl::pure_match_parser auto exp_separator>
		inline constexpr auto dec_float =
		  lak::dsl::capture<lak::dsl::signed_dec_number> +
		  lak::dsl::capture<lak::dsl::conditional<frac_separator,
		                                          lak::dsl::dec_number,
		                                          lak::dsl::top>> +
		  lak::dsl::capture<lak::dsl::conditional<exp_separator,
		                                          lak::dsl::signed_dec_number,
		                                          lak::dsl::top>>;

		template<typename FLOAT,
		         lak::dsl::pure_match_parser auto frac_separator,
		         lak::dsl::pure_match_parser auto exp_separator>
		inline constexpr auto parsed_dec_float = lak::dsl::transform<
		  lak::dsl::dec_float<frac_separator, exp_separator>,
		  [](const lak::tuple<lak::u8string_view,
		                      lak::u8string_view,
		                      lak::u8string_view> &result)
		  {
			  return result.apply(lak::dec_string_to_double)
			    .map_err(
			      [&](lak::string_to_numeric_error)
			      {
				      ERROR("string to numeric error: ",
				            result.template get<0>(),
				            ".",
				            result.template get<1>(),
				            "e",
				            result.template get<2>());
				      return lak::dsl::parse_error{
				        .message = u8"error converting dec string to double"};
			      })
			    .and_then(
			      [](double v) -> lak::result<FLOAT, lak::dsl::parse_error>
			      {
				      if (v > std::numeric_limits<FLOAT>::max() ||
				          v < std::numeric_limits<FLOAT>::lowest())
					      return lak::err_t{lak::dsl::parse_error{
					        .message = u8"parsed value out of range"}};
				      else
					      return lak::ok_t{FLOAT(v)};
			      });
		  }>;

		template<char32_t chr>
		inline constexpr auto until_next_char =
		  lak::dsl::until<lak::dsl::char_literal<chr>>;

		template<char32_t chr>
		inline constexpr auto next_char =
		  lak::dsl::until_next_char<chr> + lak::dsl::char_literal<chr>;

		template<lak::u8const_string const_str>
		inline constexpr auto until_next_str =
		  lak::dsl::until<lak::dsl::str_literal<const_str>>;

		template<lak::u8const_string const_str>
		inline constexpr auto next_str =
		  lak::dsl::until_next_str<const_str> + lak::dsl::str_literal<const_str>;

		template<lak::u8const_string begin, lak::u8const_string end>
		inline constexpr auto simple_bounded_str =
		  lak::dsl::str_literal<begin> + lak::dsl::next_str<end>;

		template<lak::u8const_string begin, lak::u8const_string end>
		inline constexpr auto capture_simple_bounded_str =
		  lak::dsl::capture_2nd<lak::dsl::str_literal<begin>,
		                        lak::dsl::until_next_str<end>,
		                        lak::dsl::str_literal<end>>;

		inline constexpr auto slash_line_comment =
		  lak::dsl::simple_bounded_str<u8"//", u8"\n">;

		inline constexpr auto block_comment =
		  lak::dsl::simple_bounded_str<u8"/*", u8"*/">;

		inline constexpr auto pound_line_comment =
		  lak::dsl::simple_bounded_str<u8"#", u8"\n">;

	}
}

#endif
