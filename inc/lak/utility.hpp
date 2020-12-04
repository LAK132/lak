#ifndef LAK_UTILITY_HPP
#define LAK_UTILITY_HPP

#include "lak/compiler.hpp"
#include "lak/type_utils.hpp"

namespace lak
{
  /* --- move --- */

  template<typename T>
  force_inline constexpr lak::remove_reference_t<T> &&move(T &&t) noexcept
  {
    return static_cast<lak::remove_reference_t<T> &&>(t);
  }

  /* --- forward --- */

  template<typename T>
  force_inline constexpr T &&forward(lak::remove_reference_t<T> &t) noexcept
  {
    return static_cast<T &&>(t);
  }

  template<typename T>
  force_inline constexpr T &&forward(lak::remove_reference_t<T> &&t) noexcept
  {
    return static_cast<T &&>(t);
  }

  /* --- swap --- */

  template<typename T>
  force_inline constexpr void swap(T &a, T &b)
  {
    auto temp = lak::move(a);
    a         = lak::move(b);
    b         = lak::move(temp);
  }

  template<typename T, size_t SIZE>
  force_inline constexpr void swap(T (&a)[SIZE], T (&b)[SIZE])
  {
    for (size_t i = 0; i < SIZE; ++i) lak::swap(a[i], b[i]);
  }

  /* --- exchange --- */

  template<typename T>
  force_inline constexpr T exchange(T &a, T &&n)
  {
    auto result = lak::move(a);
    a           = lak::move(n);
    return lak::move(result);
  }

  template<typename T, typename U>
  force_inline constexpr T exchange(T &a, U &&n)
  {
    auto result = lak::move(a);
    a           = T(lak::forward<U>(n));
    return lak::move(result);
  }
}

#endif