#ifndef LAK_STDINT_HPP
#define LAK_STDINT_HPP

#include <climits>
#include <cstddef>
#include <cstdint>

namespace lak
{
	static const constexpr size_t dynamic_extent = SIZE_MAX;

	using nullptr_t = decltype(nullptr);
}

using f32_t = float;
using f64_t = double;

enum struct byte_t : uint8_t
{
};

static_assert(sizeof(f32_t) == 4);
static_assert(sizeof(f64_t) == 8);

#define LAK_FOREACH_SIGNED_INTEGER(MACRO, ...)                                \
	MACRO(int8_t, s8, __VA_ARGS__)                                              \
	MACRO(int16_t, s16, __VA_ARGS__)                                            \
	MACRO(int32_t, s32, __VA_ARGS__)                                            \
	MACRO(int64_t, s64, __VA_ARGS__)

#define LAK_ALL_SIGNED_INTEGERS int8_t, int16_t, int32_t, int64_t

#define LAK_FOREACH_UNSIGNED_INTEGER(MACRO, ...)                              \
	MACRO(uint8_t, u8, __VA_ARGS__)                                             \
	MACRO(uint16_t, u16, __VA_ARGS__)                                           \
	MACRO(uint32_t, u32, __VA_ARGS__)                                           \
	MACRO(uint64_t, u64, __VA_ARGS__)

#define LAK_ALL_UNSIGNED_INTEGERS uint8_t, uint16_t, uint32_t, uint64_t

#define LAK_FOREACH_INTEGER(MACRO, ...)                                       \
	LAK_FOREACH_SIGNED_INTEGER(MACRO, __VA_ARGS__)                              \
	LAK_FOREACH_UNSIGNED_INTEGER(MACRO, __VA_ARGS__)

#define LAK_ALL_INTEGERS LAK_ALL_SIGNED_INTEGERS, LAK_ALL_UNSIGNED_INTEGERS

#define LAK_FOREACH_FLOAT(MACRO, ...)                                         \
	MACRO(f32_t, f32, __VA_ARGS__)                                              \
	MACRO(f64_t, f64, __VA_ARGS__)

#define LAK_ALL_FLOATS f32_t, f64_t

#endif
