#ifndef LAK_MEMMANIP_HPP
#define LAK_MEMMANIP_HPP

#include "lak/span.hpp"

#include <cstring>

namespace lak
{
  template<typename T, typename U>
  force_inline void memcpy(T *dst, const U *src)
  {
    static_assert(std::is_same_v<T, U>, "Cannot memcpy different types");
    std::memcpy(dst, src, sizeof(T));
  }

  template<typename T, typename U>
  force_inline void memmove(T *dst, const U *src)
  {
    static_assert(std::is_same_v<T, U>, "Cannot memmove different types");
    std::memmove(dst, src, sizeof(T));
  }

  template<typename T>
  force_inline void memset(T *dst, int val)
  {
    std::memset(dst, val, sizeof(T));
  }

  size_t page_size();

  lak::span<void> page_reserve(size_t size, size_t *page_size_out = nullptr);

  bool page_commit(lak::span<void> pages);

  bool page_decommit(lak::span<void> pages);

  bool page_free(lak::span<void> pages);
}

#endif