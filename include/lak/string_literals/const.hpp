#ifndef LAK_STRING_LITERALS_CONST_HPP
#define LAK_STRING_LITERALS_CONST_HPP

#include "lak/const_string.hpp"

template<lak::const_string STR>
constexpr auto operator""_const()
{
	return STR;
}
static_assert(("hello"_const).size() == 5U);
static_assert((L"world"_const).size() == 5U);
static_assert((u8"how"_const).size() == 3U);
static_assert((u"are"_const).size() == 3U);
static_assert((U"you"_const).size() == 3U);

#endif
