// This library assumes that char only holds 7bit ASCII characters, and that
// wchar_t will hold Unicode characters. wchar_t encoding is determined based
// on its size.

#ifndef LAK_STRCAST_HPP
#define LAK_STRCAST_HPP

#include "lak/compiler.hpp"

#define LAK_STRING_VIEW_FORWARD_ONLY
#include "lak/string_view.hpp"

namespace lak
{
	// Reinterpret UTF-8 string as an ASCII string.
	force_inline lak::astring_view as_astring(lak::astring_view str);
	force_inline lak::astring_view as_astring(lak::u8string_view str);

	// Reinterpret ASCII string as a UTF-8 string.
	force_inline lak::u8string_view as_u8string(lak::astring_view str);
	force_inline lak::u8string_view as_u8string(lak::u8string_view str);

	// Reinterpret UTF-8 string as an ASCII string.
	force_inline const char *as_astring(const char *str);
	force_inline const char *as_astring(const char8_t *str);
	// Reinterpret ASCII string as a UTF-8 string.
	force_inline const char8_t *as_u8string(const char *str);
	force_inline const char8_t *as_u8string(const char8_t *str);
}

#include "lak/strcast.inl"

#endif
