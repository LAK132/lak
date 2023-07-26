#ifndef LAK_DSL_UTILITY_HPP
#define LAK_DSL_UTILITY_HPP

#include "lak/char_utils.hpp"
#include "lak/dsl/dsl.hpp"

namespace lak
{
	namespace dsl
	{
		inline constexpr auto ascii_whitespace =
		  lak::dsl::one_of_chars_str<lak::char_utils_impl::ascii_spaces32>;
		inline constexpr auto whitespace =
		  lak::dsl::one_of_chars_str<lak::char_utils_impl::spaces>;

		inline constexpr auto bin_digit  = lak::dsl::char_range<U'0', U'1'>;
		inline constexpr auto bin_number = +lak::dsl::bin_digit;

		inline constexpr auto oct_digit  = lak::dsl::char_range<U'0', U'7'>;
		inline constexpr auto oct_number = +lak::dsl::oct_digit;

		inline constexpr auto dec_digit  = lak::dsl::char_range<U'0', U'9'>;
		inline constexpr auto dec_number = +lak::dsl::dec_digit;

		inline constexpr auto hex_digit = lak::dsl::char_range<U'0', U'9'> |
		                                  lak::dsl::char_range<U'a', U'f'> |
		                                  lak::dsl::char_range<U'A', U'F'>;
		inline constexpr auto hex_number = +lak::dsl::hex_digit;
	}
}

#endif
