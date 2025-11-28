#ifndef LAK_STRING_LITERALS_FORMAT_HPP
#define LAK_STRING_LITERALS_FORMAT_HPP

#include "lak/format.hpp"

template<lak::const_string STR>
consteval auto operator""_fmt()
{
	return lak::format_string<STR>{};
}
static_assert(lak::is_same_v<char, typename decltype(""_fmt)::char_type>);
static_assert(lak::is_same_v<wchar_t, typename decltype(L""_fmt)::char_type>);
static_assert(lak::is_same_v<char8_t, typename decltype(u8""_fmt)::char_type>);
static_assert(lak::is_same_v<char16_t, typename decltype(u""_fmt)::char_type>);
static_assert(lak::is_same_v<char32_t, typename decltype(U""_fmt)::char_type>);

#endif
