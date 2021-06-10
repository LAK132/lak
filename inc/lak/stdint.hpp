#ifndef LAK_STDINT_HPP
#define LAK_STDINT_HPP

#include <cstddef>
#include <cstdint>

namespace lak
{
  static const constexpr size_t dynamic_extent = SIZE_MAX;
}

using f32_t = float;
using f64_t = double;

static_assert(sizeof(f32_t) == 4);
static_assert(sizeof(f64_t) == 8);

#define LAK_FOREACH_INTEGER(MACRO, ...)                                       \
  MACRO(uint8_t, u8, __VA_ARGS__)                                             \
  MACRO(int8_t, s8, __VA_ARGS__)                                              \
  MACRO(uint16_t, u16, __VA_ARGS__)                                           \
  MACRO(int16_t, s16, __VA_ARGS__)                                            \
  MACRO(uint32_t, u32, __VA_ARGS__)                                           \
  MACRO(int32_t, s32, __VA_ARGS__)                                            \
  MACRO(uint64_t, u64, __VA_ARGS__)                                           \
  MACRO(int64_t, s64, __VA_ARGS__)

#define LAK_FOREACH_FLOAT(MACRO, ...)                                         \
  MACRO(f32_t, f32, __VA_ARGS__)                                              \
  MACRO(f64_t, f64, __VA_ARGS__)

#endif