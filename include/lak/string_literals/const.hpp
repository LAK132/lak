#ifndef LAK_STRING_LITERALS_CONST_HPP
#define LAK_STRING_LITERALS_CONST_HPP

#include "lak/const_string.hpp"

template<lak::aconst_string_literal STR>
constexpr auto operator"" _aconst()
{
	return lak::const_string<char, STR.size>(STR);
}

template<lak::wconst_string_literal STR>
constexpr auto operator"" _wconst()
{
	return lak::const_string<wchar_t, STR.size>(STR);
}

template<lak::u8const_string_literal STR>
constexpr auto operator"" _u8const()
{
	return lak::const_string<char8_t, STR.size>(STR);
}

template<lak::u16const_string_literal STR>
constexpr auto operator"" _16const()
{
	return lak::const_string<char16_t, STR.size>(STR);
}

template<lak::u32const_string_literal STR>
constexpr auto operator"" _32const()
{
	return lak::const_string<char32_t, STR.size>(STR);
}

#endif
