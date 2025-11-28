#ifndef LAK_CONST_STRING_HPP
#define LAK_CONST_STRING_HPP

#include "lak/c_array.hpp"
#include "lak/crc.hpp"
#include "lak/string.hpp"
#include "lak/string_view.hpp"
#include "lak/type_traits.hpp"

namespace lak
{
	template<typename CHAR>
	constexpr size_t const_strlen(const CHAR *str);

	template<typename CHAR>
	struct const_string_literal
	{
		const CHAR *data;
		const size_t size;
		constexpr const_string_literal(const CHAR *str)
		: data(str), size(const_strlen(str))
		{
		}
	};

#define LAK_BASIC_CONST_STRING(CHAR, PREFIX, ...)                             \
	constexpr size_t PREFIX##const_strlen(const CHAR *str);                     \
	using PREFIX##const_string_literal = lak::const_string_literal<CHAR>;
	LAK_FOREACH_CHAR(LAK_BASIC_CONST_STRING)
#undef LAK_BASIC_CONST_STRING

	template<typename CHAR, size_t N>
	struct const_string
	{
		using char_type = CHAR;
		CHAR _value[N];
		const_string() = delete;
		inline constexpr const_string(const const_string_literal<CHAR> &other);
		inline constexpr const_string(const const_string &other);
		inline constexpr const_string &operator=(const const_string &other);
		inline constexpr const_string(const CHAR (&str)[N + 1]);
		inline consteval static const_string from_ptr(const CHAR *str);
		inline constexpr CHAR &operator[](size_t index) { return _value[index]; }
		inline constexpr const CHAR &operator[](size_t index) const
		{
			return _value[index];
		}
		inline constexpr CHAR *begin() { return _value; }
		inline constexpr CHAR *end() { return _value + N; }

		inline constexpr const CHAR *begin() const { return _value; }
		inline constexpr const CHAR *end() const { return _value + N; }

		inline constexpr const CHAR *cbegin() const { return _value; }
		inline constexpr const CHAR *cend() const { return _value + N; }

		inline constexpr CHAR *data() { return _value; }
		inline constexpr const CHAR *data() const { return _value; }

		inline constexpr size_t size() const { return N; }

		inline constexpr operator lak::c_array<CHAR, N>() const;

		inline operator std::basic_string<CHAR>() const;

		inline constexpr uint32_t crc32() const;
	};
	template<typename CHAR, size_t N>
	const_string(const CHAR (&)[N]) -> const_string<CHAR, N - 1>;

#define LAK_BASIC_CONST_STRING(CHAR, PREFIX, ...)                             \
	template<size_t N>                                                          \
	using PREFIX##const_string = lak::const_string<CHAR, N>;                    \
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
#undef LAK_BASIC_CONST_STRING

	template<typename CHAR, size_t N>
	constexpr lak::const_string<CHAR, N> strconv(
	  const lak::aconst_string<N> &str);
}

#include "lak/const_string.inl"

#endif
