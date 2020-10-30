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

#endif