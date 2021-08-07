#ifndef LAK_CHAR_HPP
#define LAK_CHAR_HPP

#include "lak/compiler.hpp"
#include "lak/stdint.hpp"

// char8_t typedef for C++ < 20
#ifndef LAK_COMPILER_CPP20
using char8_t = uint_least8_t;
#endif

#if WCHAR_MAX == 0x7FFFFFFF || WCHAR_MAX == 0xFFFFFFFF
using wchar_unicode_t = char32_t;
#elif WCHAR_MAX == 0x7FFF || WCHAR_MAX == 0xFFFF
using wchar_unicode_t = char16_t;
#elif WCHAR_MAX == 0x7F || WCHAR_MAX == 0xFF
using wchar_unicode_t = char8_t;
#else
#	error "Unsupported WCHAR_MAX"
#endif

// (char_t, prefix, arguments)
#define LAK_FOREACH_CHAR(MACRO, ...)                                          \
	MACRO(char, a, __VA_ARGS__)                                                 \
	MACRO(wchar_t, w, __VA_ARGS__)                                              \
	MACRO(char8_t, u8, __VA_ARGS__)                                             \
	MACRO(char16_t, u16, __VA_ARGS__)                                           \
	MACRO(char32_t, u32, __VA_ARGS__)

#define LAK_TEMPLATE_FOREACH_CHAR_EX(CHAR, PREFIX, TYPE)                      \
	template struct TYPE<CHAR>;
#define LAK_TEMPLATE_FOREACH_CHAR(TYPE)                                       \
	LAK_FOREACH_CHAR(LAK_TEMPLATE_FOREACH_CHAR_EX, TYPE)

#define LAK_EXTERN_TEMPLATE_FOREACH_CHAR_EX(CHAR, PREFIX, TYPE)               \
	extern template struct TYPE<CHAR>;
#define LAK_EXTERN_TEMPLATE_FOREACH_CHAR(TYPE)                                \
	LAK_FOREACH_CHAR(LAK_EXTERN_TEMPLATE_FOREACH_CHAR_EX, TYPE)

#define LAK_TYPEDEF_FOREACH_CHAR_EX(CHAR, PREFIX, TYPE)                       \
	using TOKEN_CONCAT(PREFIX, TYPE) = TYPE<CHAR>;
#define LAK_TYPEDEF_FOREACH_CHAR(TYPE)                                        \
	LAK_FOREACH_CHAR(LAK_TYPEDEF_FOREACH_CHAR_EX, TYPE)

namespace lak
{
	template<typename CHAR>
	struct chars_per_codepoint;

	template<>
	struct chars_per_codepoint<char>
	{
		static constexpr size_t value = 1;
	};

	template<>
	struct chars_per_codepoint<char8_t>
	{
		static constexpr size_t value = 4;
	};

	template<>
	struct chars_per_codepoint<char16_t>
	{
		static constexpr size_t value = 2;
	};

	template<>
	struct chars_per_codepoint<char32_t>
	{
		static constexpr size_t value = 1;
	};

	template<>
	struct chars_per_codepoint<wchar_t>
	{
		static constexpr size_t value =
		  chars_per_codepoint<wchar_unicode_t>::value;
	};

	template<typename CHAR>
	inline constexpr size_t chars_per_codepoint_v =
	  chars_per_codepoint<CHAR>::value;
}

#endif