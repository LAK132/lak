#include "lak/debug.hpp"
#include "lak/stdint.hpp"
#include "lak/strconv.hpp"
#include "lak/string.hpp"

#include <array>
#include <vector>

/* --- lak::span<T, SIZE> --- */

template<typename T, size_t SIZE>
inline constexpr T &lak::span<T, SIZE>::operator[](size_t index) const noexcept
{
  ASSERTF(index < SIZE, index << " > " << SIZE);
  return _data[index];
}

template<typename T, size_t SIZE>
inline constexpr T *lak::span<T, SIZE>::data() const noexcept
{
  return _data;
}

template<typename T, size_t SIZE>
[[nodiscard]] inline constexpr bool lak::span<T, SIZE>::empty() const noexcept
{
  return size() == 0;
}

template<typename T, size_t SIZE>
inline constexpr size_t lak::span<T, SIZE>::size() const noexcept
{
  return SIZE;
}

template<typename T, size_t SIZE>
inline constexpr size_t lak::span<T, SIZE>::size_bytes() const noexcept
{
  return SIZE * sizeof(T);
}

template<typename T, size_t SIZE>
inline constexpr T *lak::span<T, SIZE>::begin() const noexcept
{
  return _data;
}

template<typename T, size_t SIZE>
inline constexpr T *lak::span<T, SIZE>::end() const noexcept
{
  return _data + SIZE;
}

template<typename T, size_t SIZE>
template<size_t offset, size_t count>
inline constexpr lak::span<T, count> lak::span<T, SIZE>::subspan() const
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
inline constexpr lak::span<T, lak::dynamic_extent> lak::span<T, SIZE>::subspan(
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
inline constexpr lak::span<T, count> lak::span<T, SIZE>::first() const
{
  static_assert(count <= SIZE, "subspan too large");
  lak::span<T, count> result;
  result._data = begin();
  return result;
}

template<typename T, size_t SIZE>
inline constexpr lak::span<T, lak::dynamic_extent> lak::span<T, SIZE>::first(
  size_t count) const
{
  ASSERTF(count <= size(), count << " > " << size());
  return {begin(), count};
}

template<typename T, size_t SIZE>
template<size_t count>
inline constexpr lak::span<T, count> lak::span<T, SIZE>::last() const
{
  static_assert(count <= SIZE, "subspan too large");
  return {begin() + (size() - count)};
}

template<typename T, size_t SIZE>
inline constexpr lak::span<T, lak::dynamic_extent> lak::span<T, SIZE>::last(
  size_t count) const
{
  ASSERTF(count <= size(), count << " > " << size());
  return {begin() + (size() - count), count};
}

template<typename T, size_t SIZE>
inline constexpr auto lak::span<T, SIZE>::to_string() const
{
  return lak::string<lak::remove_const_t<T>>(begin(), end());
}

/* --- lak::span<void, SIZE> --- */

template<size_t SIZE>
inline constexpr void *lak::span<void, SIZE>::data() const noexcept
{
  return _data;
}

template<size_t SIZE>
[[nodiscard]] inline constexpr bool lak::span<void, SIZE>::empty()
  const noexcept
{
  return size() == 0;
}

template<size_t SIZE>
inline constexpr size_t lak::span<void, SIZE>::size() const noexcept
{
  return SIZE;
}

template<size_t SIZE>
inline constexpr size_t lak::span<void, SIZE>::size_bytes() const noexcept
{
  return SIZE;
}

/* --- lak::span<const void, SIZE> --- */

template<size_t SIZE>
inline constexpr const void *lak::span<const void, SIZE>::data() const noexcept
{
  return _data;
}

template<size_t SIZE>
[[nodiscard]] inline constexpr bool lak::span<const void, SIZE>::empty()
  const noexcept
{
  return size() == 0;
}

template<size_t SIZE>
inline constexpr size_t lak::span<const void, SIZE>::size() const noexcept
{
  return SIZE;
}

template<size_t SIZE>
inline constexpr size_t lak::span<const void, SIZE>::size_bytes()
  const noexcept
{
  return SIZE;
}

/* --- lak::span<T, lak::dynamic_extent> --- */

template<typename T>
inline constexpr lak::span<T, lak::dynamic_extent>::span(T *begin,
                                                         T *end) noexcept
: _data(begin), _size(static_cast<size_t>(end - begin))
{
  ASSERT(end >= begin);
}

template<typename T>
inline constexpr T &lak::span<T, lak::dynamic_extent>::operator[](
  size_t index) const noexcept
{
  ASSERTF(index < size(), index << " >= " << size());
  return _data[index];
}

template<typename T>
inline constexpr T *lak::span<T, lak::dynamic_extent>::data() const noexcept
{
  return _data;
}

template<typename T>
[[nodiscard]] inline constexpr bool lak::span<T, lak::dynamic_extent>::empty()
  const noexcept
{
  return size() == 0;
}

template<typename T>
inline constexpr size_t lak::span<T, lak::dynamic_extent>::size()
  const noexcept
{
  return _size;
}

template<typename T>
inline constexpr size_t lak::span<T, lak::dynamic_extent>::size_bytes()
  const noexcept
{
  return _size * sizeof(T);
}

template<typename T>
inline constexpr T *lak::span<T, lak::dynamic_extent>::begin() const noexcept
{
  return _data;
}

template<typename T>
inline constexpr T *lak::span<T, lak::dynamic_extent>::end() const noexcept
{
  return _data + _size;
}

template<typename T>
template<size_t offset, size_t count>
inline constexpr lak::span<T, count>
lak::span<T, lak::dynamic_extent>::subspan() const
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
inline constexpr lak::span<T, lak::dynamic_extent>
lak::span<T, lak::dynamic_extent>::subspan(size_t offset, size_t count) const
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
inline constexpr lak::span<T, count> lak::span<T, lak::dynamic_extent>::first()
  const
{
  ASSERTF(count <= size(), count << " > " << size());
  return {begin()};
}

template<typename T>
inline constexpr lak::span<T, lak::dynamic_extent>
lak::span<T, lak::dynamic_extent>::first(size_t count) const
{
  ASSERTF(count <= size(), count << " > " << size());
  return {begin(), count};
}

template<typename T>
template<size_t count>
inline constexpr lak::span<T, count> lak::span<T, lak::dynamic_extent>::last()
  const
{
  ASSERTF(count <= size(), count << " > " << size());
  return {begin() + (size() - count)};
}

template<typename T>
inline constexpr lak::span<T, lak::dynamic_extent>
lak::span<T, lak::dynamic_extent>::last(size_t count) const
{
  ASSERTF(count <= size(), count << " > " << size());
  return {begin() + (size() - count), count};
}

template<typename T>
inline constexpr auto lak::span<T, lak::dynamic_extent>::to_string() const
{
  return lak::string<lak::remove_const_t<T>>(begin(), end());
}

/* --- lak::span<void, lak::dynamic_extent> --- */

inline constexpr void *lak::span<void, lak::dynamic_extent>::data()
  const noexcept
{
  return _data;
}

[[nodiscard]] inline constexpr bool
lak::span<void, lak::dynamic_extent>::empty() const noexcept
{
  return size() == 0;
}

inline constexpr size_t lak::span<void, lak::dynamic_extent>::size()
  const noexcept
{
  return _size;
}

inline constexpr size_t lak::span<void, lak::dynamic_extent>::size_bytes()
  const noexcept
{
  return _size;
}

/* --- lak::span<const void, lak::dynamic_extent> --- */

inline constexpr const void *lak::span<const void, lak::dynamic_extent>::data()
  const noexcept
{
  return _data;
}

[[nodiscard]] inline constexpr bool
lak::span<const void, lak::dynamic_extent>::empty() const noexcept
{
  return size() == 0;
}

inline constexpr size_t lak::span<const void, lak::dynamic_extent>::size()
  const noexcept
{
  return _size;
}

inline constexpr size_t
lak::span<const void, lak::dynamic_extent>::size_bytes() const noexcept
{
  return _size;
}

template<typename T>
lak::span<T> lak::find_subspan(lak::span<T> source, lak::span<T> subspan)
{
  while (source.size() > subspan.size())
  {
    if (auto s = source.first(subspan.size()); subspan == s) return s;
    source = source.subspan(1);
  }
  return {};
}

template<typename T>
lak::span<T> lak::common_initial_sequence(lak::span<T> a, lak::span<T> b)
{
  size_t count = 0;
  while (count < a.size() && count < b.size() && a[count] == b[count]) ++count;
  return a.first(count);
}

template<typename T>
void lak::rotate_left(lak::span<T> data, size_t distance)
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
void lak::rotate_right(lak::span<T> data, size_t distance)
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

template<typename T>
size_t lak::compare(lak::span<const T> a, lak::span<const T> b)
{
  if (a == b) return a.size();
  size_t result = 0;
  while (result < a.size() && result < b.size() && a[result] == b[result])
    ++result;
  return result;
}

template<typename T>
bool operator==(lak::span<T> a, lak::span<T> b)
{
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i)
    if (a[i] != b[i]) return false;
  return true;
}

template<typename T>
bool operator!=(lak::span<T> a, lak::span<T> b)
{
  return !(a == b);
}
