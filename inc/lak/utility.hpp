#ifndef LAK_UTILITY_HPP
#define LAK_UTILITY_HPP

#include "lak/compiler.hpp"
#include "lak/type_utils.hpp"

namespace lak
{
  /* --- declval --- */

  template<typename T>
  lak::add_rvalue_reference_t<T> declval() noexcept;

  /* --- in_place_index --- */

  template<size_t I>
  struct in_place_index_t : public lak::size_type<I>
  {
  };
  template<size_t I>
  inline constexpr in_place_index_t<I> in_place_index{};

  struct in_place_t
  {
  };
  inline constexpr in_place_t in_place{};

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

  /* --- as_ptr --- */

  // Converts pointers and pointer-like types to pointers.

  template<typename T>
  force_inline constexpr T *as_ptr(T *p)
  {
    return p;
  }

  // Converts references and reference-like types to pointers.

  template<typename T>
  force_inline constexpr T *as_ptr(lak::remove_refs_ptrs<T> &p)
  {
    std::addressof(p);
  }
}

#endif