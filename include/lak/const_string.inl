#include "lak/const_string.hpp"

template<typename CHAR>
constexpr size_t lak::const_strlen(const CHAR *str)
{
	size_t result = 0;
	while (*(str++) != CHAR(0)) ++result;
	return result;
}

#define LAK_BASIC_CONST_STRING(CHAR, PREFIX, ...)                             \
	constexpr size_t lak::PREFIX##const_strlen(const CHAR *str)                 \
	{                                                                           \
		size_t result = 0;                                                        \
		while (*(str++) != CHAR(0)) ++result;                                     \
		return result;                                                            \
	}
LAK_FOREACH_CHAR(LAK_BASIC_CONST_STRING)
#undef LAK_BASIC_CONST_STRING

template<typename CHAR, size_t N>
inline constexpr lak::const_string<CHAR, N>::const_string(
  const lak::const_string_literal<CHAR> &other)
{
	for (size_t i = 0; i < N; ++i) _value[i] = other.data[i];
}

template<typename CHAR, size_t N>
inline constexpr lak::const_string<CHAR, N>::const_string(
  const const_string &other)
{
	for (size_t i = 0; i < N; ++i) _value[i] = other._value[i];
}

template<typename CHAR, size_t N>
inline constexpr lak::const_string<CHAR, N> &
lak::const_string<CHAR, N>::operator=(const const_string &other)
{
	for (size_t i = 0; i < N; ++i) _value[i] = other._value[i];
}

template<typename CHAR, size_t N>
inline constexpr lak::const_string<CHAR, N>::const_string(
  const CHAR (&str)[N + 1])
{
	if (!std::is_constant_evaluated())
	{
		ASSERT_EQUAL(str[N], CHAR(0));
	}
	else if (str[N] != CHAR(0))
		throw "expected null terminator";
	for (size_t i = 0; i < N; ++i) _value[i] = str[i];
}

template<typename CHAR, size_t N>
inline constexpr lak::const_string<CHAR, N>::operator lak::c_array<CHAR, N>()
  const
{
	lak::c_array<CHAR, N> result;
	for (size_t i = 0; i < N; ++i) result[i] = _value[i];
	return result;
}

template<typename CHAR, size_t N>
inline lak::const_string<CHAR, N>::operator std::basic_string<CHAR>() const
{
	return std::basic_string<CHAR>(begin(), end());
}

template<typename CHAR, size_t N>
inline constexpr uint32_t lak::const_string<CHAR, N>::crc32() const
{
	uint32_t result = ~uint32_t(0);
	for (size_t i = 0U; i < N; ++i)
		result = lak::crc32_table[static_cast<uint8_t>(result ^ _value[i])] ^
		         (result >> 8U);
	return ~result;
}

template<typename CHAR, size_t N>
constexpr lak::const_string<CHAR, N> lak::strconv(
  const lak::aconst_string<N> &str)
{
	CHAR result[N + 1];
	result[N] = CHAR(0);
	for (size_t i = 0U; i < N; ++i) result[i] = CHAR(str[i]);
	return lak::const_string<CHAR, N>(result);
}
