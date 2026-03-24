#ifndef LAK_STRING_LITERALS_SPAN_HPP
#define LAK_STRING_LITERALS_SPAN_HPP

#ifndef LAK_SPAN_FORWARD_ONLY
#	define LAK_SPAN_FORWARD_ONLY
#endif
#include "lak/span.hpp"

inline consteval lak::span<const char> operator""_span(const char *str,
                                                       size_t size)
{
	return lak::span<const char>{str, size};
}

inline consteval lak::span<const wchar_t> operator""_span(const wchar_t *str,
                                                          size_t size)
{
	return lak::span<const wchar_t>{str, size};
}

inline consteval lak::span<const char8_t> operator""_span(const char8_t *str,
                                                          size_t size)
{
	return lak::span<const char8_t>{str, size};
}

inline consteval lak::span<const char16_t> operator""_span(const char16_t *str,
                                                           size_t size)
{
	return lak::span<const char16_t>{str, size};
}

inline consteval lak::span<const char32_t> operator""_span(const char32_t *str,
                                                           size_t size)
{
	return lak::span<const char32_t>{str, size};
}

inline consteval lak::span<const char> operator""_null_span(const char *str,
                                                            size_t size)
{
	return lak::span<const char>{str, size + 1U};
}

inline consteval lak::span<const wchar_t> operator""_null_span(
  const wchar_t *str, size_t size)
{
	return lak::span<const wchar_t>{str, size + 1U};
}

inline consteval lak::span<const char8_t> operator""_null_span(
  const char8_t *str, size_t size)
{
	return lak::span<const char8_t>{str, size + 1U};
}

inline consteval lak::span<const char16_t> operator""_null_span(
  const char16_t *str, size_t size)
{
	return lak::span<const char16_t>{str, size + 1U};
}

inline consteval lak::span<const char32_t> operator""_null_span(
  const char32_t *str, size_t size)
{
	return lak::span<const char32_t>{str, size + 1U};
}

#endif
