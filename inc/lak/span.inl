#include "lak/stdint.hpp"

#include <array>
#include <vector>

/* --- lak::span<T, SIZE> --- */

template<typename T, size_t SIZE>
inline constexpr lak::span<T, SIZE> lak::span<T, SIZE>::from_ptr(
  T *data) noexcept
{
  lak::span<T, SIZE> result;
  result._data = data;
  return result;
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
template<size_t count>
inline constexpr lak::span<T, count> lak::span<T, SIZE>::first() const
{
  static_assert(count <= SIZE, "subspan too large");
  return lak::span<T, count>::from_ptr(begin());
}

template<typename T, size_t SIZE>
template<size_t count>
inline constexpr lak::span<T, count> lak::span<T, SIZE>::last() const
{
  static_assert(count <= SIZE, "subspan too large");
  return lak::span<T, count>::from_ptr(begin() + (size() - count));
}

// add const
template<typename T, size_t SIZE>
constexpr lak::span<T, SIZE>::operator span<const T, SIZE>() const noexcept
{
  return lak::span<const T, SIZE>::from_ptr(_data);
}

// reinterpret cast to U
template<typename T, size_t SIZE>
template<lak::concepts::ptr_reinterpret_castable_from<T> U>
requires !lak::concepts::same_as<U, T> &&
  !lak::is_void_v<U> //
  constexpr lak::span<T, SIZE>::operator span<U,
                                              (SIZE * sizeof(T)) / sizeof(U)>()
    const noexcept
{
  return lak::span<U, (SIZE * sizeof(T)) / sizeof(U)>::from_ptr(
    reinterpret_cast<U *>(_data));
}

// reinterpret cast to void
template<typename T, size_t SIZE>
template<lak::concepts::void_type V>
requires lak::is_reinterpret_castable_v<T *, V *> //
constexpr lak::span<T, SIZE>::operator span<V, SIZE * sizeof(T)>()
  const noexcept
{
  return lak::span<V, SIZE * sizeof(T)>::from_ptr(reintpret_cast<V *>(_data));
}

// static cast to dynamic size
template<typename T, size_t SIZE>
template<lak::concepts::ptr_static_castable_from<T> U>
constexpr lak::span<T, SIZE>::operator span<U, lak::dynamic_extent>()
  const noexcept
{
  return lak::span<U, lak::dynamic_extent>(static_cast<U *>(data()), size());
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

// add const
template<size_t SIZE>
constexpr lak::span<void, SIZE>::operator span<const void, SIZE>()
  const noexcept
{
  lak::span<const void, SIZE> result;
  result._data = _data;
  return result;
}

// reinterpret cast to T
template<size_t SIZE>
template<lak::concepts::ptr_reinterpret_castable_from<void> T>
requires !lak::is_void_v<T> //
  constexpr lak::span<void, SIZE>::operator span<T, (SIZE / sizeof(T))>()
    const noexcept
{
  return lak::span<T, (SIZE / sizeof(T))>::from_ptr(
    reinterpret_cast<T *>(_data));
}

// static cast to dynamic size
template<size_t SIZE>
template<lak::concepts::void_type V>
constexpr lak::span<void, SIZE>::operator span<V, lak::dynamic_extent>()
  const noexcept
{
  return lak::span<V, lak::dynamic_extent>(static_cast<V *>(data()), size());
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

// reinterpret cast to T
template<size_t SIZE>
template<lak::concepts::ptr_reinterpret_castable_from<const void> T>
requires !lak::is_void_v<T> //
  constexpr lak::span<const void, SIZE>::operator span<T, SIZE / sizeof(T)>()
    const noexcept
{
  return lak::span<T, SIZE / sizeof(T)>::from_ptr(
    reinterpret_cast<T *>(_data));
}

// static cast to dynamic size
template<size_t SIZE>
constexpr lak::span<const void, SIZE>::operator span<const void,
                                                     lak::dynamic_extent>()
  const noexcept
{
  return lak::span<const void, lak::dynamic_extent>(data(), size());
}

/* --- lak::span<T, lak::dynamic_extent> --- */

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

// add const
template<typename T>
constexpr lak::span<T, lak::dynamic_extent>::
operator span<const T, lak::dynamic_extent>() const noexcept
{
  return lak::span<const T, lak::dynamic_extent>(_data, _size);
}

// reinterpret cast to U
template<typename T>
template<lak::concepts::ptr_reinterpret_castable_from<T> U>
requires !lak::concepts::same_as<U, T> &&
  !lak::is_void_v<U> //
  constexpr lak::span<T, lak::dynamic_extent>::
  operator span<U, lak::dynamic_extent>() const noexcept
{
  return lak::span<U, lak::dynamic_extent>(reinterpret_cast<U *>(_data),
                                           (_size * sizeof(T)) / sizeof(U));
}

// reinterpret cast to void
template<typename T>
template<lak::concepts::void_type V>
requires lak::is_reinterpret_castable_v<T *, V *> //
constexpr lak::span<T, lak::dynamic_extent>::
operator span<V, lak::dynamic_extent>() const noexcept
{
  return lak::span<V, lak::dynamic_extent>(reinterpret_cast<V *>(_data),
                                           _size * sizeof(T));
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

// add const
constexpr lak::span<void, lak::dynamic_extent>::
operator span<const void, lak::dynamic_extent>() const noexcept
{
  return lak::span<const void, lak::dynamic_extent>(
    static_cast<const void *>(_data), _size);
}

// reinterpret cast to U
template<lak::concepts::ptr_reinterpret_castable_from<void> U>
requires !lak::is_void_v<U> //
  constexpr lak::span<void, lak::dynamic_extent>::
  operator span<U, lak::dynamic_extent>() const noexcept
{
  return lak::span<U, lak::dynamic_extent>(reinterpret_cast<U *>(_data),
                                           _size / sizeof(U));
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

// reinterpret cast to U
template<lak::concepts::ptr_reinterpret_castable_from<const void> U>
requires !lak::is_void_v<U> //
  constexpr lak::span<const void, lak::dynamic_extent>::
  operator span<U, lak::dynamic_extent>() const noexcept
{
  return lak::span<U, lak::dynamic_extent>(reinterpret_cast<U *>(_data),
                                           _size / sizeof(U));
}

/* --- helper functions --- */

template<typename T>
constexpr T *lak::find_first(lak::span<T> s, const T &v)
{
  for (auto it = s.begin(); it != s.end(); ++it)
    if (*it == v) return it;
  return s.end();
}

template<typename T>
constexpr bool lak::contains(lak::span<const T> s, const T &v)
{
  return lak::find_first<const T>(s, v) != s.end();
}

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split_before(lak::span<T> s,
                                                        const T &v)
{
  return lak::split(s, lak::find_first<T>(s, v));
}

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split_after(lak::span<T> s,
                                                       const T &v)
{
  if (auto at = lak::find_first(s, v); at != s.end())
    return lak::split(s, at + 1);
  else
    return {s.first(0), s};
}

template<typename T>
void lak::fill(lak::span<T> span, const T &value)
{
  for (auto &v : span) v = value;
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
lak::span<T> lak::rotate_left(lak::span<T> data, size_t distance)
{
  if (data.size() == 0 || (distance % data.size()) == 0) return data;

  for (size_t i = 0; i < data.size(); ++i)
  {
    lak::swap(data[i], data[(i + distance) % data.size()]);
  }

  return data;
}

template<typename T>
lak::span<T> lak::rotate_right(lak::span<T> data, size_t distance)
{
  if (data.size() == 0 || (distance % data.size()) == 0) return data;

  for (size_t i = data.size(); i-- > 0;)
  {
    lak::swap(data[i], data[(i + distance) % data.size()]);
  }

  return data;
}

template<typename T>
lak::shift_result<T> lak::shift_left(lak::span<T> data, size_t distance)
{
  lak::shift_result<T> result;

  if (distance > data.size())
  {
    result.moved_to   = data.first(0);
    result.unmodified = data;
    result.moved_from = data.last(0);
    return result;
  }

  for (size_t i = distance; i < data.size(); ++i)
  {
    data[i - distance] = lak::move(data[i]);
  }

  const size_t moved_count = data.size() - distance;

  lak::tie(result.moved_from, data) = lak::split(data, moved_count);

  lak::tie(result.unmodified, result.moved_from) = lak::split(
    data, (moved_count > data.size()) ? 0 : (data.size() - moved_count));

  return result;
}

template<typename T>
lak::shift_result<T> lak::shift_right(lak::span<T> data, size_t distance)
{
  lak::shift_result<T> result;

  if (distance > data.size())
  {
    result.moved_to   = data.last(0);
    result.unmodified = data;
    result.moved_from = data.first(0);
    return result;
  }

  for (size_t i = data.size(); i < distance; --i)
  {
    data[i] = lak::move(data[i - distance]);
  }

  const size_t moved_count = data.size() - distance;

  lak::tie(data, result.moved_from) =
    lak::split(data, data.size() - moved_count);

  lak::tie(result.moved_from, result.unmodified) =
    lak::split(data, (moved_count > data.size()) ? 0 : moved_count);

  return result;
}

template<typename T>
size_t lak::compare(lak::span<const T> a, lak::span<const T> b)
{
  if (lak::same_span(a, b)) return a.size();
  size_t result = 0;
  while (result < a.size() && result < b.size() && a[result] == b[result])
    ++result;
  return result;
}

template<typename T>
bool lak::same_span(lak::span<const T> a, lak::span<const T> b)
{
  return a.data() == b.data() && a.size() == b.size();
}

template<typename T, size_t S1, typename U, size_t S2>
bool operator==(lak::span<T, S1> a, lak::span<U, S2> b)
{
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i)
    if (a[i] != b[i]) return false;
  return true;
}

template<typename T, size_t S1, typename U, size_t S2>
bool operator!=(lak::span<T, S1> a, lak::span<U, S2> b)
{
  return !(a == b);
}

///////////////////////////////////////////////////////////////////////////////
// Requires string

#include "lak/string.hpp"

/* --- lak::span<T, SIZE> --- */

template<typename T, size_t SIZE>
inline auto lak::span<T, SIZE>::to_string() const
{
  return lak::string<lak::remove_const_t<T>>(begin(), end());
}

/* --- lak::span<T, lak::dynamic_extent> --- */

template<typename T>
inline auto lak::span<T, lak::dynamic_extent>::to_string() const
{
  return lak::string<lak::remove_const_t<T>>(begin(), end());
}

///////////////////////////////////////////////////////////////////////////////
// Requires strcast, streamify

#include "lak/strcast.hpp"
#include "lak/streamify.hpp"

/* --- lak::span<T, SIZE> --- */

template<typename T, size_t SIZE>
inline auto lak::span<T, SIZE>::stringify() const
{
  lak::u8string result = lak::as_u8string("{").to_string();
  for (const auto &element : *this)
  {
    result += lak::streamify(result, ", ");
  }
  if (result.size() > 2) result.resize(result.size() - 2);
  return result + lak::as_u8string("}").to_string();
}

/* --- lak::span<T, lak::dynamic_extent> --- */

template<typename T>
inline auto lak::span<T, lak::dynamic_extent>::stringify() const
{
  lak::u8string result = lak::as_u8string("{").to_string();
  for (const auto &element : *this)
  {
    result += lak::streamify(element, ", ");
  }
  if (result.size() > 2) result.resize(result.size() - 2);
  return result + lak::as_u8string("}").to_string();
}

///////////////////////////////////////////////////////////////////////////////
// Requires debug, strconv

#include "lak/debug.hpp"
#include "lak/strconv.hpp"

#ifndef LAK_COMPILER_CPP20
#  define ASSERT_CONSTEXPR
#else
#  define ASSERT_CONSTEXPR constexpr
#endif

/* --- lak::span<T, SIZE> --- */

template<typename T, size_t SIZE>
inline ASSERT_CONSTEXPR T &lak::span<T, SIZE>::operator[](
  size_t index) const noexcept
{
  ASSERTF(index < SIZE, index << " > " << SIZE);
  return _data[index];
}

template<typename T, size_t SIZE>
template<size_t offset, size_t count>
inline ASSERT_CONSTEXPR lak::span<T, count> lak::span<T, SIZE>::subspan() const
{
  if constexpr (count == lak::dynamic_extent)
  {
    ASSERTF(offset + count <= size(), offset + count << " > " << size());
    return lak::span<T, count>{begin() + offset, count};
  }
  else
  {
    static_assert(offset + count <= SIZE, "subspan too large");
    return lak::span<T, count>::from_ptr(begin() + offset);
  }
}

template<typename T, size_t SIZE>
inline ASSERT_CONSTEXPR lak::span<T, lak::dynamic_extent>
lak::span<T, SIZE>::subspan(size_t offset, size_t count) const
{
  if (count == lak::dynamic_extent)
  {
    ASSERTF(offset <= size(), offset << " > " << size());
    return lak::span<T>{begin() + offset, size() - offset};
  }
  else
  {
    ASSERTF(offset + count <= size(), offset + count << " > " << size());
    return lak::span<T>{begin() + offset, count};
  }
}

template<typename T, size_t SIZE>
inline ASSERT_CONSTEXPR lak::span<T, lak::dynamic_extent>
lak::span<T, SIZE>::first(size_t count) const
{
  ASSERTF(count <= size(), count << " > " << size());
  return lak::span<T>{begin(), count};
}

template<typename T, size_t SIZE>
inline ASSERT_CONSTEXPR lak::span<T, lak::dynamic_extent>
lak::span<T, SIZE>::last(size_t count) const
{
  ASSERTF(count <= size(), count << " > " << size());
  return lak::span<T>{begin() + (size() - count), count};
}

/* --- lak::span<T, lak::dynamic_extent> --- */

template<typename T>
inline ASSERT_CONSTEXPR lak::span<T, lak::dynamic_extent>::span(
  T *begin, T *end) noexcept
: _data(begin), _size(static_cast<size_t>(end - begin))
{
  ASSERT(end >= begin);
}

template<typename T>
inline ASSERT_CONSTEXPR T &lak::span<T, lak::dynamic_extent>::operator[](
  size_t index) const noexcept
{
  ASSERTF(index < size(), index << " >= " << size());
  return _data[index];
}

template<typename T>
template<size_t offset, size_t count>
inline ASSERT_CONSTEXPR lak::span<T, count>
lak::span<T, lak::dynamic_extent>::subspan() const
{
  ASSERTF(offset + count <= size(), offset + count << " > " << size());
  if constexpr (count == lak::dynamic_extent)
  {
    return lak::span<T, count>{begin() + offset, count};
  }
  else
  {
    return lak::span<T, count>::from_ptr(begin() + offset);
  }
}

template<typename T>
inline ASSERT_CONSTEXPR lak::span<T, lak::dynamic_extent>
lak::span<T, lak::dynamic_extent>::subspan(size_t offset, size_t count) const
{
  if (count == lak::dynamic_extent)
  {
    ASSERTF(offset <= size(), offset << " > " << size());
    return lak::span<T>{begin() + offset, size() - offset};
  }
  else
  {
    ASSERTF(offset + count <= size(), offset + count << " > " << size());
    return lak::span<T>{begin() + offset, count};
  }
}

template<typename T>
template<size_t count>
inline ASSERT_CONSTEXPR lak::span<T, count>
lak::span<T, lak::dynamic_extent>::first() const
{
  ASSERTF(count <= size(), count << " > " << size());
  return lak::span<T, count>::from_ptr(begin());
}

template<typename T>
inline ASSERT_CONSTEXPR lak::span<T, lak::dynamic_extent>
lak::span<T, lak::dynamic_extent>::first(size_t count) const
{
  ASSERTF(count <= size(), count << " > " << size());
  return lak::span<T>{begin(), count};
}

template<typename T>
template<size_t count>
inline ASSERT_CONSTEXPR lak::span<T, count>
lak::span<T, lak::dynamic_extent>::last() const
{
  ASSERTF(count <= size(), count << " > " << size());
  return lak::span<T, count>::from_ptr(begin() + (size() - count));
}

template<typename T>
inline ASSERT_CONSTEXPR lak::span<T, lak::dynamic_extent>
lak::span<T, lak::dynamic_extent>::last(size_t count) const
{
  ASSERTF(count <= size(), count << " > " << size());
  return lak::span<T>{begin() + (size() - count), count};
}

/* --- helper functions --- */

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split(lak::span<T> s, T *at)
{
  ASSERT_GREATER_OR_EQUAL(at, s.begin());
  ASSERT_LESS_OR_EQUAL(at, s.end());
  const size_t index = at - s.begin();
  return lak::split(s, index);
}

template<typename T, lak::enable_if_i<!lak::is_const_v<T>>>
lak::pair<lak::span<T>, lak::span<T>> lak::split(lak::span<T> s, const T *at)
{
  ASSERT_GREATER_OR_EQUAL(at, s.begin());
  ASSERT_LESS_OR_EQUAL(at, s.end());
  const size_t index = at - s.begin();
  return lak::split(s, index);
}

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split(lak::span<T> s, size_t at)
{
  ASSERT_LESS_OR_EQUAL(at, s.size());
  return {lak::span(s.begin(), s.begin() + at),
          lak::span(s.begin() + at, s.end())};
}

#undef ASSERT_CONSTEXPR
