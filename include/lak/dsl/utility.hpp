#ifndef LAK_DSL_UTILITY_HPP
#define LAK_DSL_UTILITY_HPP

#include "lak/dsl/dsl.hpp"

#include "lak/char_utils.hpp"
#include "lak/numeric.hpp"

namespace lak
{
	namespace dsl
	{
		inline constexpr auto ascii_whitespace =
		  lak::dsl::one_of_chars_str<lak::char_utils_impl::ascii_spaces32>;
		inline constexpr auto whitespace =
		  lak::dsl::one_of_chars_str<lak::char_utils_impl::spaces>;

		inline constexpr auto number_sign = lak::dsl::one_of_chars<U'+', U'-'>;

		inline constexpr auto bin_digit       = lak::dsl::char_range<U'0', U'1'>;
		inline constexpr auto bin_number      = +lak::dsl::bin_digit;
		inline constexpr auto parsed_bin_uint = lak::dsl::transform<
		  lak::dsl::bin_number,
		  [](lak::u8string_view num)
		  { return lak::string_to_uintmax(num, lak::numeric_base::dec); }>;

		inline constexpr auto oct_digit       = lak::dsl::char_range<U'0', U'7'>;
		inline constexpr auto oct_number      = +lak::dsl::oct_digit;
		inline constexpr auto parsed_oct_uint = lak::dsl::transform<
		  lak::dsl::oct_number,
		  [](lak::u8string_view num)
		  { return lak::string_to_uintmax(num, lak::numeric_base::oct); }>;

		inline constexpr auto dec_digit  = lak::dsl::char_range<U'0', U'9'>;
		inline constexpr auto dec_number = +lak::dsl::dec_digit;
		inline constexpr auto signed_dec_number =
		  lak::dsl::optional<lak::dsl::number_sign> + lak::dsl::dec_number;
		inline constexpr auto parsed_dec_uint = lak::dsl::transform<
		  lak::dsl::dec_number,
		  [](lak::u8string_view num)
		  { return lak::string_to_uintmax(num, lak::numeric_base::dec); }>;
		inline constexpr auto parsed_dec_int = lak::dsl::transform<
		  lak::dsl::signed_dec_number,
		  [](lak::u8string_view num)
		  { return lak::string_to_intmax(num, lak::numeric_base::dec); }>;

		inline constexpr auto hex_digit = lak::dsl::char_range<U'0', U'9'> |
		                                  lak::dsl::char_range<U'a', U'f'> |
		                                  lak::dsl::char_range<U'A', U'F'>;
		inline constexpr auto hex_number      = +lak::dsl::hex_digit;
		inline constexpr auto parsed_hex_uint = lak::dsl::transform<
		  lak::dsl::hex_number,
		  [](lak::u8string_view num)
		  { return lak::string_to_uintmax(num, lak::numeric_base::hex); }>;

		template<lak::dsl::pure_match_parser auto frac_separator,
		         lak::dsl::pure_match_parser auto exp_separator>
		inline constexpr auto dec_float =
		  lak::dsl::capture<lak::dsl::signed_dec_number> +
		  lak::dsl::capture<
		    ~lak::dsl::capture_nth<1U, frac_separator, lak::dsl::dec_number>> +
		  lak::dsl::capture<~lak::dsl::capture_nth<1U,
		                                           exp_separator,
		                                           lak::dsl::signed_dec_number>>;

		template<lak::dsl::pure_match_parser auto frac_separator,
		         lak::dsl::pure_match_parser auto exp_separator>
		inline constexpr auto parsed_dec_float =
		  lak::dsl::transform<lak::dsl::dec_float<frac_separator, exp_separator>,
		                      [](const auto &result)
		                      { return result.apply(lak::dec_string_to_double); }>;

		template<char32_t chr>
		inline constexpr auto until_next_char =
		  *lak::dsl::negative_char_literal<chr>;

		template<char32_t chr>
		inline constexpr auto next_char =
		  lak::dsl::until_next_char<chr> + lak::dsl::char_literal<chr>;

		template<lak::u8const_string const_str>
		inline constexpr auto until_next_str =
		  *lak::dsl::negative_str_literal<const_str>;

		template<lak::u8const_string const_str>
		inline constexpr auto next_str =
		  lak::dsl::until_next_str<const_str> + lak::dsl::str_literal<const_str>;

		inline constexpr auto slash_line_comment =
		  u8"//"_dsl_str + lak::dsl::next_char<U'\n'>;

		inline constexpr auto block_comment =
		  u8"/*"_dsl_str + lak::dsl::next_str<u8"*/">;

		inline constexpr auto pound_line_comment =
		  U"#"_dsl_char + lak::dsl::next_char<U'\n'>;

	}
}

#endif
