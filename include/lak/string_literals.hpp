#ifndef LAK_STRING_LITERALS_HPP
#define LAK_STRING_LITERALS_HPP

#include "lak/const_string.hpp"
#include "lak/span.hpp"
#include "lak/string_view.hpp"

#include <bit>

/* --- operator"" _view --- */

inline consteval lak::astring_view operator"" _view(const char *str,
                                                    size_t len)
{
	return lak::astring_view{str, len};
}

inline consteval lak::wstring_view operator"" _view(const wchar_t *str,
                                                    size_t len)
{
	return lak::wstring_view{str, len};
}

inline consteval lak::u8string_view operator"" _view(const char8_t *str,
                                                     size_t len)
{
	return lak::u8string_view{str, len};
}

inline consteval lak::u16string_view operator"" _view(const char16_t *str,
                                                      size_t len)
{
	return lak::u16string_view{str, len};
}

inline consteval lak::u32string_view operator"" _view(const char32_t *str,
                                                      size_t len)
{
	return lak::u32string_view{str, len};
}

/* --- operator"" _span --- */

inline consteval lak::span<const char> operator"" _span(const char *str,
                                                        size_t size)
{
	return lak::span<const char>{str, size};
}

inline consteval lak::span<const wchar_t> operator"" _span(const wchar_t *str,
                                                           size_t size)
{
	return lak::span<const wchar_t>{str, size};
}

inline consteval lak::span<const char8_t> operator"" _span(const char8_t *str,
                                                           size_t size)
{
	return lak::span<const char8_t>{str, size};
}

inline consteval lak::span<const char16_t> operator"" _span(
  const char16_t *str, size_t size)
{
	return lak::span<const char16_t>{str, size};
}

inline consteval lak::span<const char32_t> operator"" _span(
  const char32_t *str, size_t size)
{
	return lak::span<const char32_t>{str, size};
}

/* --- opeator"" _magic --- "*/

template<lak::u8const_string STR>
requires(STR.size() == 1U || STR.size() == 2U || STR.size() == 4U ||
         STR.size() == 8U)
inline consteval auto operator"" _magic()
{
	using result_type = lak::conditional_t<
	  STR.size() == 1U,
	  uint8_t,
	  lak::conditional_t<
	    STR.size() == 2U,
	    uint16_t,
	    lak::conditional_t<STR.size() == 4U, uint32_t, uint64_t>>>;

	static_assert(sizeof(STR) == sizeof(result_type));

	return std::bit_cast<result_type>(STR);
}

#endif
