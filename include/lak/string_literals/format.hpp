#ifndef LAK_STRING_LITERALS_FORMAT_HPP
#define LAK_STRING_LITERALS_FORMAT_HPP

#include "lak/format.hpp"

template<lak::aconst_string_literal STR>
consteval auto operator"" _afmt()
{
	return lak::format_string<lak::aconst_string<STR.size>(STR)>{};
}

template<lak::wconst_string_literal STR>
consteval auto operator"" _wfmt()
{
	return lak::format_string<lak::wconst_string<STR.size>(STR)>{};
}

template<lak::u8const_string_literal STR>
consteval auto operator"" _u8fmt()
{
	return lak::format_string<lak::u8const_string<STR.size>(STR)>{};
}

template<lak::u16const_string_literal STR>
consteval auto operator"" _u16fmt()
{
	return lak::format_string<lak::u16const_string<STR.size>(STR)>{};
}

template<lak::u32const_string_literal STR>
consteval auto operator"" _u32fmt()
{
	return lak::format_string<lak::u32const_string<STR.size>(STR)>{};
}

#endif
