#ifndef LAK_STRING_LITERALS_CONST_HPP
#define LAK_STRING_LITERALS_CONST_HPP

#include "lak/const_string.hpp"

template<lak::aconst_string_literal STR>
constexpr auto operator"" _aconst()
{
	return lak::aconst_string<STR.size>::from_ptr(STR.data);
}

template<lak::wconst_string_literal STR>
constexpr auto operator"" _wconst()
{
	return lak::wconst_string<STR.size>::from_ptr(STR.data);
}

template<lak::u8const_string_literal STR>
constexpr auto operator"" _u8const()
{
	return lak::u8const_string<STR.size>::from_ptr(STR.data);
}

template<lak::u16const_string_literal STR>
constexpr auto operator"" _16const()
{
	return lak::u16const_string<STR.size>::from_ptr(STR.data);
}

template<lak::u32const_string_literal STR>
constexpr auto operator"" _32const()
{
	return lak::u32const_string<STR.size>::from_ptr(STR.data);
}

#endif
