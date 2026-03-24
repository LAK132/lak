#ifndef LAK_STRING_LITERALS_STRING_HPP
#define LAK_STRING_LITERALS_STRING_HPP

#include "lak/string.hpp"

inline lak::astring operator""_str(const char *str, size_t size)
{
	return lak::astring{str, size};
}

inline lak::wstring operator""_str(const wchar_t *str, size_t size)
{
	return lak::wstring{str, size};
}

inline lak::u8string operator""_str(const char8_t *str, size_t size)
{
	return lak::u8string{str, size};
}

inline lak::u16string operator""_str(const char16_t *str, size_t size)
{
	return lak::u16string{str, size};
}

inline lak::u32string operator""_str(const char32_t *str, size_t size)
{
	return lak::u32string{str, size};
}

#endif
