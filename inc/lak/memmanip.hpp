#ifndef LAK_MEMMANIP_HPP
#define LAK_MEMMANIP_HPP

#include "lak/span.hpp"

#include <cstring>

namespace lak
{
  // see lak::binary_reader and lak::binary_writer for type safe manipulation
  // of types as bytes
  template<typename T>
  lak::span<lak::copy_const_t<T, char>, sizeof(T)> as_bytes(T *v)
  {
    return lak::span<lak::copy_const_t<T, char>, sizeof(T)>::from_ptr(
      reinterpret_cast<lak::copy_const_t<T, char> *>(v));
  }

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

  size_t round_to_page_multiple(size_t size, size_t *page_size_out = nullptr);

  lak::span<void> page_reserve(size_t size, size_t *page_size_out = nullptr);

  bool page_commit(lak::span<void> pages);

  bool page_decommit(lak::span<void> pages);

  bool page_free(lak::span<void> pages);
}

#endif