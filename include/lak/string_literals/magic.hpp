#ifndef LAK_STRING_LITERALS_MAGIC_HPP
#define LAK_STRING_LITERALS_MAGIC_HPP

#include "lak/const_string.hpp"
#include "lak/math.hpp"

#include "lak/system/endian.hpp"

#include <bit>

template<lak::u8const_string_literal STR>
inline consteval auto operator"" _magic_ne()
{
	static_assert(STR.size == 1U || STR.size == 2U || STR.size == 4U ||
	              STR.size == 8U);

	using result_type = lak::conditional_t<
	  STR.size == 1U,
	  uint8_t,
	  lak::conditional_t<
	    STR.size == 2U,
	    uint16_t,
	    lak::conditional_t<STR.size == 4U, uint32_t, uint64_t>>>;

	static_assert((STR.size * sizeof(STR.data[0])) == sizeof(result_type));

	return std::bit_cast<result_type>(lak::u8const_string<STR.size>(STR)._value);
}

static_assert(u8" "_magic_ne == 0x20);
static_assert(u8"  "_magic_ne == 0x20'20);
static_assert(u8"    "_magic_ne == 0x20'20'20'20);
static_assert(u8"        "_magic_ne == 0x20'20'20'20'20'20'20'20);
#if defined(LAK_LITTLE_ENDIAN)
static_assert(u8"ABCD"_magic_ne == 0x44'43'42'41);
#elif defined(LAK_BIG_ENDIAN)
static_assert(u8"ABCD"_magic_ne == 0x41'42'43'44);
#endif

template<lak::u8const_string_literal STR>
inline consteval auto operator"" _magic_le()
{
	static_assert(STR.size == 1U || STR.size == 2U || STR.size == 4U ||
	              STR.size == 8U);

	using result_type = lak::conditional_t<
	  STR.size == 1U,
	  uint8_t,
	  lak::conditional_t<
	    STR.size == 2U,
	    uint16_t,
	    lak::conditional_t<STR.size == 4U, uint32_t, uint64_t>>>;

	static_assert((STR.size * sizeof(STR.data[0])) == sizeof(result_type));

	result_type result = 0U;
	for (size_t i = 0U; i < STR.size; ++i)
		result |= result_type(STR.data[i])
		          << (i * sizeof(STR.data[0U]) * CHAR_BIT);
	return result;
}

static_assert(u8"ABCD"_magic_le == 0x44'43'42'41);

template<lak::u8const_string_literal STR>
inline consteval auto operator"" _magic_be()
{
	static_assert(STR.size == 1U || STR.size == 2U || STR.size == 4U ||
	              STR.size == 8U);

	using result_type = lak::conditional_t<
	  STR.size == 1U,
	  uint8_t,
	  lak::conditional_t<
	    STR.size == 2U,
	    uint16_t,
	    lak::conditional_t<STR.size == 4U, uint32_t, uint64_t>>>;

	static_assert((STR.size * sizeof(STR.data[0])) == sizeof(result_type));

	result_type result = 0U;
	for (size_t i = 0U; i < STR.size; ++i)
		result |= result_type(STR.data[i])
		          << lak::integer_range_reverse<size_t>(i, STR.size) *
		               sizeof(STR.data[0U]) * CHAR_BIT;
	return result;
}

static_assert(u8"ABCD"_magic_be == 0x41'42'43'44);

#endif
