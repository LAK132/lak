#ifndef LAK_UTILITY_HPP
#define LAK_UTILITY_HPP

#include "lak/compiler.hpp"
#include "lak/type_traits.hpp"

#ifndef LAK_NO_STD
#  include <memory>
#  include <utility>
#endif

namespace lak
{
  /* --- monostate --- */

  struct monostate
  {
  };
  static_assert(lak::is_default_constructible_v<lak::monostate>);

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
  inline constexpr T exchange(T &a, T &&n)
  {
    T result = lak::move(a);
    a        = lak::move(n);
    return result;
  }

  template<typename T, typename U>
  inline constexpr T exchange(T &a, U &&n)
  {
    T result = lak::move(a);
    a        = T(lak::forward<U>(n));
    return result;
  }

  /* --- addressof --- */

  template<typename T>
  auto addressof(T &&t)
  {
    return std::addressof(lak::forward<T>(t));
  }

  /* --- construct_at --- */

  template<typename T, typename... ARGS>
  constexpr T *construct_at(T *p, ARGS &&...args)
  {
    return ::new (const_cast<void *>(static_cast<const volatile void *>(p)))
      T(lak::forward<ARGS>(args)...);
  }

  /* --- destroy_at --- */

  template<typename T>
  constexpr void destroy_at(T *p)
  {
    if constexpr (lak::is_array_v<T>)
      for (auto &elem : *p) lak::destroy_at(lak::addressof(elem));
    else
      p->~T();
  }

  /* --- as_ptr --- */

  // Converts pointers and pointer-like types to pointers.

  template<typename T>
  force_inline constexpr T *as_ptr(T *p)
  {
    return p;
  }

#ifndef LAK_NO_STD
  template<typename T>
  force_inline T *as_ptr(std::reference_wrapper<T> *p)
  {
    return p ? &p->get() : nullptr;
  }

  template<typename T>
  force_inline const T *as_ptr(const std::reference_wrapper<T> *p)
  {
    return p ? &p->get() : nullptr;
  }
#endif

  // Converts references and reference-like types to pointers.

  template<typename T>
  force_inline constexpr T *as_ptr(lak::remove_refs_ptrs<T> &p)
  {
    lak::addressof(p);
  }

#ifndef LAK_NO_STD
  template<typename T>
  force_inline T *as_ptr(std::reference_wrapper<T> &p)
  {
    return &p->get();
  }

  template<typename T>
  force_inline const T *as_ptr(const std::reference_wrapper<T> &p)
  {
    return &p->get();
  }
#endif
}

#endif