#ifndef LAK_CHAR_UTILS_HPP
#define LAK_CHAR_UTILS_HPP

#include "lak/char.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

namespace lak
{
	/* --- alphanumeric --- */

	// '0'-'9' -> 0-9
	inline lak::result<uint8_t> from_alphanumeric(char8_t c);

	/* --- hex_alphanumeric --- */

	inline constexpr lak::span<const char8_t> hex_alphanumerics();
	inline constexpr bool is_hex_alphanumeric(char8_t c);
	// '0'-'9' -> 0x0-0x9, 'A'-'F'/'a'-'f' -> 0xA-0xF
	inline lak::result<uint8_t> from_hex_alphanumeric(char8_t c);

	/* --- control_codes --- */

	inline constexpr lak::span<const char32_t> control_codes();
	inline constexpr bool is_control_code(char32_t c);

	/* --- whitespace --- */

	inline constexpr lak::span<const char32_t> whitespaces();
	inline constexpr bool is_whitespace(char32_t c);

	/* --- ascii_whitespace --- */

	inline constexpr lak::span<const char8_t> ascii_whitespaces();
	inline constexpr bool is_ascii_whitespace(char8_t c);
}

#include "lak/char_utils.inl"

#endif
