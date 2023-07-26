#ifndef LAK_CONST_STRING
#define LAK_CONST_STRING

#define LAK_ARRAY_FORWARD_ONLY
#include "lak/array.hpp"
#include "lak/crc.hpp"
#include "lak/string.hpp"
#include "lak/string_view.hpp"
#include "lak/type_traits.hpp"

namespace lak
{
#define LAK_BASIC_CONST_STRING(CHAR, PREFIX, ...)                             \
	constexpr size_t PREFIX##const_strlen(const CHAR *str)                      \
	{                                                                           \
		size_t result = 0;                                                        \
		while (*(str++) != CHAR(0)) ++result;                                     \
		return result;                                                            \
	}                                                                           \
	struct PREFIX##const_string_literal                                         \
	{                                                                           \
		const CHAR *data;                                                         \
		const size_t size;                                                        \
		constexpr PREFIX##const_string_literal(const CHAR *str)                   \
		: data(str), size(PREFIX##const_strlen(str))                              \
		{                                                                         \
		}                                                                         \
	};                                                                          \
	template<size_t N>                                                          \
	struct PREFIX##const_string                                                 \
	{                                                                           \
		CHAR _value[N];                                                           \
		inline constexpr PREFIX##const_string(const CHAR (&str)[N + 1])           \
		{                                                                         \
			for (size_t i = 0; i < N; ++i) _value[i] = str[i];                      \
		}                                                                         \
		inline constexpr PREFIX##const_string from_ptr(const CHAR *str)           \
		{                                                                         \
			ASSERT_EQUAL(PREFIX##const_strlen(str), N);                             \
			PREFIX##const_string result;                                            \
			for (size_t i = 0; i < N; ++i) result._value[i] = str[i];               \
			return result;                                                          \
		}                                                                         \
		inline constexpr CHAR &operator[](size_t index) { return _value[index]; } \
		inline constexpr const CHAR &operator[](size_t index) const               \
		{                                                                         \
			return _value[index];                                                   \
		}                                                                         \
		inline constexpr CHAR *begin() { return _value; }                         \
		inline constexpr CHAR *end() { return _value + N; }                       \
                                                                              \
		inline constexpr const CHAR *begin() const { return _value; }             \
		inline constexpr const CHAR *end() const { return _value + N; }           \
                                                                              \
		inline constexpr const CHAR *cbegin() const { return _value; }            \
		inline constexpr const CHAR *cend() const { return _value + N; }          \
                                                                              \
		inline constexpr CHAR *data() { return _value; }                          \
		inline constexpr const CHAR *data() const { return _value; }              \
                                                                              \
		inline constexpr size_t size() const { return N; }                        \
                                                                              \
		operator std::basic_string<CHAR>() const                                  \
		{                                                                         \
			return std::basic_string<CHAR>(begin(), end());                         \
		}                                                                         \
                                                                              \
		constexpr uint32_t crc32() const                                          \
		{                                                                         \
			uint32_t result = ~uint32_t(0);                                         \
			for (size_t i = 0U; i < N; ++i)                                         \
				result = lak::crc32_table[static_cast<uint8_t>(result ^ _value[i])] ^ \
				         (result >> 8U);                                              \
			return ~result;                                                         \
		}                                                                         \
	};                                                                          \
	template<size_t N>                                                          \
	PREFIX##const_string(const CHAR(&)[N])->PREFIX##const_string<N - 1>;        \
                                                                              \
	template<template<CHAR...> typename T, PREFIX##const_string str, typename>  \
	struct _##PREFIX##apply_const_string;                                       \
	template<template<CHAR...> typename T,                                      \
	         PREFIX##const_string str,                                          \
	         size_t... I>                                                       \
	struct _##PREFIX##apply_const_string<T, str, lak::index_sequence<I...>>     \
	{                                                                           \
		using type = T<str[I]...>;                                                \
	};                                                                          \
	template<template<CHAR...> typename T, PREFIX##const_string str>            \
	struct PREFIX##apply_const_string                                           \
	{                                                                           \
		using type = typename _##PREFIX##apply_const_string<                      \
		  T,                                                                      \
		  str,                                                                    \
		  lak::make_index_sequence<str.size()>>::type;                            \
	};                                                                          \
	template<template<CHAR...> typename T, PREFIX##const_string str>            \
	using PREFIX##apply_const_string_t =                                        \
	  typename PREFIX##apply_const_string<T, str>::type;

	// aconst_string, wconst_string, u8const_string, u16const_string and
	// u32const_string
	LAK_FOREACH_CHAR(LAK_BASIC_CONST_STRING)
}

#define LAK_BASIC_CONST_STRING_LITERAL_OPERATOR(CHAR, PREFIX, ...)            \
	template<lak::PREFIX##const_string_literal STR>                             \
	constexpr auto operator"" _##PREFIX##const()                                \
	{                                                                           \
		return lak::PREFIX##const_string<STR.size>::from_ptr(STR.data);           \
	}
LAK_FOREACH_CHAR(LAK_BASIC_CONST_STRING_LITERAL_OPERATOR)

#endif
