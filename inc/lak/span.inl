#include "lak/debug.hpp"
#include "lak/stdint.hpp"

#include <array>
#include <vector>

namespace lak
{
  /* --- span<T, SIZE> --- */

  template<typename T, size_t SIZE>
  inline constexpr T &span<T, SIZE>::operator[](size_t index) const noexcept
  {
    ASSERTF(index < SIZE, index << " > " << SIZE);
    return _data[index];
  }

  template<typename T, size_t SIZE>
  inline constexpr T *span<T, SIZE>::data() const noexcept
  {
    return _data;
  }

  template<typename T, size_t SIZE>
  inline constexpr size_t span<T, SIZE>::size() const noexcept
  {
    return SIZE;
  }

  template<typename T, size_t SIZE>
  inline constexpr size_t span<T, SIZE>::size_bytes() const noexcept
  {
    return SIZE * sizeof(T);
  }

  template<typename T, size_t SIZE>
  inline constexpr T *span<T, SIZE>::begin() const noexcept
  {
    return _data;
  }

  template<typename T, size_t SIZE>
  inline constexpr T *span<T, SIZE>::end() const noexcept
  {
    return _data + SIZE;
  }

  template<typename T, size_t SIZE>
  template<size_t offset, size_t count>
  inline constexpr span<T, count> span<T, SIZE>::subspan() const
  {
    if constexpr (count == lak::dynamic_extent)
    {
      ASSERTF(offset + count <= size(), offset + count << " > " << size());
      return {begin() + offset, count};
    }
    else
    {
      static_assert(offset + count <= SIZE, "subspan too large");
      return {begin() + offset};
    }
  }

  template<typename T, size_t SIZE>
  inline constexpr span<T, lak::dynamic_extent> span<T, SIZE>::subspan(
    size_t offset, size_t count) const
  {
    if (count == lak::dynamic_extent)
    {
      ASSERTF(offset <= size(), offset << " > " << size());
      return {begin() + offset, size() - offset};
    }
    else
    {
      ASSERTF(offset + count <= size(), offset + count << " > " << size());
      return {begin() + offset, count};
    }
  }

  template<typename T, size_t SIZE>
  template<size_t count>
  inline constexpr span<T, count> span<T, SIZE>::first() const
  {
    static_assert(count <= SIZE, "subspan too large");
    lak::span<T, count> result;
    result._data = begin();
    return result;
  }

  template<typename T, size_t SIZE>
  inline constexpr span<T, lak::dynamic_extent> span<T, SIZE>::first(
    size_t count) const
  {
    ASSERTF(count <= size(), count << " > " << size());
    return {begin(), count};
  }

  template<typename T, size_t SIZE>
  template<size_t count>
  inline constexpr span<T, count> span<T, SIZE>::last() const
  {
    static_assert(count <= SIZE, "subspan too large");
    return {begin() + (size() - count)};
  }

  template<typename T, size_t SIZE>
  inline constexpr span<T, lak::dynamic_extent> span<T, SIZE>::last(
    size_t count) const
  {
    ASSERTF(count <= size(), count << " > " << size());
    return {begin() + (size() - count), count};
  }

  /* --- span<void, SIZE> --- */

  template<size_t SIZE>
  inline constexpr void *span<void, SIZE>::data() const noexcept
  {
    return _data;
  }

  template<size_t SIZE>
  inline constexpr size_t span<void, SIZE>::size() const noexcept
  {
    return SIZE;
  }

  template<size_t SIZE>
  inline constexpr size_t span<void, SIZE>::size_bytes() const noexcept
  {
    return SIZE;
  }

  /* --- span<const void, SIZE> --- */

  template<size_t SIZE>
  inline constexpr const void *span<const void, SIZE>::data() const noexcept
  {
    return _data;
  }

  template<size_t SIZE>
  inline constexpr size_t span<const void, SIZE>::size() const noexcept
  {
    return SIZE;
  }

  template<size_t SIZE>
  inline constexpr size_t span<const void, SIZE>::size_bytes() const noexcept
  {
    return SIZE;
  }

  /* --- span<T, dynamic_extent> --- */

  template<typename T>
  inline constexpr T &span<T, dynamic_extent>::operator[](
    size_t index) const noexcept
  {
    ASSERTF(index < size(), index << " >= " << size());
    return _data[index];
  }

  template<typename T>
  inline constexpr T *span<T, dynamic_extent>::data() const noexcept
  {
    return _data;
  }

  template<typename T>
  inline constexpr size_t span<T, dynamic_extent>::size() const noexcept
  {
    return _size;
  }

  template<typename T>
  inline constexpr size_t span<T, dynamic_extent>::size_bytes() const noexcept
  {
    return _size * sizeof(T);
  }

  template<typename T>
  inline constexpr T *span<T, dynamic_extent>::begin() const noexcept
  {
    return _data;
  }

  template<typename T>
  inline constexpr T *span<T, dynamic_extent>::end() const noexcept
  {
    return _data + _size;
  }

  template<typename T>
  template<size_t offset, size_t count>
  inline constexpr span<T, count> span<T, dynamic_extent>::subspan() const
  {
    ASSERTF(offset + count <= size(), offset + count << " > " << size());
    if constexpr (count == lak::dynamic_extent)
    {
      return {begin() + offset, count};
    }
    else
    {
      return {begin() + offset};
    }
  }

  template<typename T>
  inline constexpr span<T, lak::dynamic_extent>
  span<T, dynamic_extent>::subspan(size_t offset, size_t count) const
  {
    if (count == lak::dynamic_extent)
    {
      ASSERTF(offset <= size(), offset << " > " << size());
      return {begin() + offset, size() - offset};
    }
    else
    {
      ASSERTF(offset + count <= size(), offset + count << " > " << size());
      return {begin() + offset, count};
    }
  }

  template<typename T>
  template<size_t count>
  inline constexpr span<T, count> span<T, dynamic_extent>::first() const
  {
    ASSERTF(count <= size(), count << " > " << size());
    return {begin()};
  }

  template<typename T>
  inline constexpr span<T, lak::dynamic_extent> span<T, dynamic_extent>::first(
    size_t count) const
  {
    ASSERTF(count <= size(), count << " > " << size());
    return {begin(), count};
  }

  template<typename T>
  template<size_t count>
  inline constexpr span<T, count> span<T, dynamic_extent>::last() const
  {
    ASSERTF(count <= size(), count << " > " << size());
    return {begin() + (size() - count)};
  }

  template<typename T>
  inline constexpr span<T, lak::dynamic_extent> span<T, dynamic_extent>::last(
    size_t count) const
  {
    ASSERTF(count <= size(), count << " > " << size());
    return {begin() + (size() - count), count};
  }

  /* --- span<void, dynamic_extent> --- */

  inline constexpr void *span<void, dynamic_extent>::data() const noexcept
  {
    return _data;
  }

  inline constexpr size_t span<void, dynamic_extent>::size() const noexcept
  {
    return _size;
  }

  inline constexpr size_t span<void, dynamic_extent>::size_bytes()
    const noexcept
  {
    return _size;
  }

  /* --- span<const void, dynamic_extent> --- */

  inline constexpr const void *span<const void, dynamic_extent>::data()
    const noexcept
  {
    return _data;
  }

  inline constexpr size_t span<const void, dynamic_extent>::size()
    const noexcept
  {
    return _size;
  }

  inline constexpr size_t span<const void, dynamic_extent>::size_bytes()
    const noexcept
  {
    return _size;
  }

  template<typename T>
  bool operator==(span<T> a, span<T> b)
  {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
      if (a[i] != b[i]) return false;
    return true;
  }

  template<typename T>
  bool operator!=(span<T> a, span<T> b)
  {
    return !(a == b);
  }

  template<typename T>
  span<T> find_subspan(span<T> source, span<T> subspan)
  {
    while (source.size() > subspan.size())
    {
      if (auto s = source.first(subspan.size()); subspan == s) return s;
      source = source.subspan(1);
    }
    return {};
  }

  template<typename T>
  void rotate_left(span<T> data, size_t distance)
  {
    if (data.size() == 0) return;

    const size_t offset = distance % data.size();

    if (offset == 0) return;

    const size_t count = data.size() - offset;

    for (size_t i = 0; i < count; ++i)
    {
      std::swap(data[i], data[i + offset]);
    }
  }

  template<typename T>
  void rotate_right(span<T> data, size_t distance)
  {
    if (data.size() == 0) return;

    const size_t offset = distance % data.size();

    if (offset == 0) return;

    const size_t count = data.size() - offset;

    for (size_t i = count; i-- > 0;)
    {
      std::swap(data[i], data[i + offset]);
    }
  }
}
