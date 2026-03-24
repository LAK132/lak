#ifndef LAK_STRING_LITERALS_VIEW_HPP
#define LAK_STRING_LITERALS_VIEW_HPP

#ifndef LAK_STRING_VIEW_FORWARD_ONLY
#	define LAK_STRING_VIEW_FORWARD_ONLY
#endif
#include "lak/string_view.hpp"

inline consteval lak::astring_view operator""_view(const char *str, size_t len)
{
	return lak::astring_view{str, len};
}

inline consteval lak::wstring_view operator""_view(const wchar_t *str,
                                                   size_t len)
{
	return lak::wstring_view{str, len};
}

inline consteval lak::u8string_view operator""_view(const char8_t *str,
                                                    size_t len)
{
	return lak::u8string_view{str, len};
}

inline consteval lak::u16string_view operator""_view(const char16_t *str,
                                                     size_t len)
{
	return lak::u16string_view{str, len};
}

inline consteval lak::u32string_view operator""_view(const char32_t *str,
                                                     size_t len)
{
	return lak::u32string_view{str, len};
}

#endif
