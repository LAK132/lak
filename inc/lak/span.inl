#include "lak/span_forward.hpp"

#include "lak/ptr_intrin.hpp"

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

template<typename T, size_t SIZE>
inline constexpr bool lak::span<T, SIZE>::contains(const T *ptr) const noexcept
{
	return lak::ptr_in_range(ptr, _data, SIZE);
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
requires(!lak::concepts::same_as<U, T> && !lak::is_void_v<U>) //
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
requires(lak::is_reinterpret_castable_v<T *, V *>) //
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

template<size_t SIZE>
inline constexpr bool lak::span<void, SIZE>::contains(
  const void *ptr) const noexcept
{
	return __lakc_ptr_in_range(ptr, _data, SIZE);
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
requires(!lak::is_void_v<T>) //
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

template<size_t SIZE>
inline constexpr bool lak::span<const void, SIZE>::contains(
  const void *ptr) const noexcept
{
	return __lakc_ptr_in_range(ptr, _data, SIZE);
}

// reinterpret cast to T
template<size_t SIZE>
template<lak::concepts::ptr_reinterpret_castable_from<const void> T>
requires(!lak::is_void_v<T>) //
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

template<typename T>
inline constexpr bool lak::span<T, lak::dynamic_extent>::contains(
  const T *ptr) const noexcept
{
	return lak::ptr_in_range(ptr, _data, _size);
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
requires(!lak::concepts::same_as<U, T> && !lak::is_void_v<U>) //
  constexpr lak::span<T, lak::dynamic_extent>::
  operator span<U, lak::dynamic_extent>() const noexcept
{
	return lak::span<U, lak::dynamic_extent>(reinterpret_cast<U *>(_data),
	                                         (_size * sizeof(T)) / sizeof(U));
}

// reinterpret cast to void
template<typename T>
template<lak::concepts::void_type V>
requires(lak::is_reinterpret_castable_v<T *, V *>) //
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

inline constexpr bool lak::span<void, lak::dynamic_extent>::contains(
  const void *ptr) const noexcept
{
	return __lakc_ptr_in_range(ptr, _data, _size);
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
requires(!lak::is_void_v<U>) //
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

inline constexpr bool lak::span<const void, lak::dynamic_extent>::contains(
  const void *ptr) const noexcept
{
	return __lakc_ptr_in_range(ptr, _data, _size);
}

// reinterpret cast to U
template<lak::concepts::ptr_reinterpret_castable_from<const void> U>
requires(!lak::is_void_v<U>) //
  constexpr lak::span<const void, lak::dynamic_extent>::
  operator span<U, lak::dynamic_extent>() const noexcept
{
	return lak::span<U, lak::dynamic_extent>(reinterpret_cast<U *>(_data),
	                                         _size / sizeof(U));
}

template<typename T, size_t S1, typename U, size_t S2>
constexpr bool operator==(lak::span<T, S1> a, lak::span<U, S2> b)
{
	if (a.size() != b.size()) return false;
	for (size_t i = 0; i < a.size(); ++i)
		if (a[i] != b[i]) return false;
	return true;
}

template<typename T, size_t S1, typename U, size_t S2>
constexpr bool operator!=(lak::span<T, S1> a, lak::span<U, S2> b)
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
		result += lak::streamify(element, ", ");
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

/* --- lak::span<T, SIZE> --- */

template<typename T, size_t SIZE>
inline constexpr T &lak::span<T, SIZE>::operator[](size_t index) const noexcept
{
	ASSERT_LESS_OR_EQUAL(index, SIZE);
	return _data[index];
}

template<typename T, size_t SIZE>
template<size_t offset, size_t count>
inline constexpr lak::span<T, count> lak::span<T, SIZE>::subspan() const
{
	ASSERT_LESS_OR_EQUAL(offset + count, size());
	if constexpr (count == lak::dynamic_extent)
		return lak::span<T, count>{begin() + offset, count};
	else
		return lak::span<T, count>::from_ptr(begin() + offset);
}

template<typename T, size_t SIZE>
inline constexpr lak::span<T, lak::dynamic_extent> lak::span<T, SIZE>::subspan(
  size_t offset, size_t count) const
{
	if (count == lak::dynamic_extent)
	{
		ASSERT_LESS_OR_EQUAL(offset, size());
		return lak::span<T>{begin() + offset, size() - offset};
	}
	else
	{
		ASSERT_LESS_OR_EQUAL(offset + count, size());
		return lak::span<T>{begin() + offset, count};
	}
}

template<typename T, size_t SIZE>
inline constexpr lak::span<T, lak::dynamic_extent> lak::span<T, SIZE>::first(
  size_t count) const
{
	ASSERT_LESS_OR_EQUAL(count, size());
	return lak::span<T>{begin(), count};
}

template<typename T, size_t SIZE>
inline constexpr lak::span<T, lak::dynamic_extent> lak::span<T, SIZE>::last(
  size_t count) const
{
	ASSERT_LESS_OR_EQUAL(count, size());
	return lak::span<T>{begin() + (size() - count), count};
}

/* --- lak::span<T, lak::dynamic_extent> --- */

template<typename T>
inline constexpr lak::span<T, lak::dynamic_extent>::span(T *begin,
                                                         T *end) noexcept
: _data(begin), _size(static_cast<size_t>(end - begin))
{
	ASSERT_LESS_OR_EQUAL(begin, end);
}

template<typename T>
inline constexpr T &lak::span<T, lak::dynamic_extent>::operator[](
  size_t index) const noexcept
{
	ASSERT_LESS(index, size());
	return _data[index];
}

template<typename T>
template<size_t offset, size_t count>
inline constexpr lak::span<T, count>
lak::span<T, lak::dynamic_extent>::subspan() const
{
	ASSERT_LESS_OR_EQUAL(offset + count, size());
	if constexpr (count == lak::dynamic_extent)
		return lak::span<T, count>{begin() + offset, count};
	else
		return lak::span<T, count>::from_ptr(begin() + offset);
}

template<typename T>
inline constexpr lak::span<T, lak::dynamic_extent>
lak::span<T, lak::dynamic_extent>::subspan(size_t offset, size_t count) const
{
	if (count == lak::dynamic_extent)
	{
		ASSERT_LESS_OR_EQUAL(offset, size());
		return lak::span<T>{begin() + offset, size() - offset};
	}
	else
	{
		ASSERT_LESS_OR_EQUAL(offset + count, size());
		return lak::span<T>{begin() + offset, count};
	}
}

template<typename T>
template<size_t count>
inline constexpr lak::span<T, count> lak::span<T, lak::dynamic_extent>::first()
  const
{
	ASSERT_LESS_OR_EQUAL(count, size());
	return lak::span<T, count>::from_ptr(begin());
}

template<typename T>
inline constexpr lak::span<T, lak::dynamic_extent>
lak::span<T, lak::dynamic_extent>::first(size_t count) const
{
	ASSERT_LESS_OR_EQUAL(count, size());
	return lak::span<T>{begin(), count};
}

template<typename T>
template<size_t count>
inline constexpr lak::span<T, count> lak::span<T, lak::dynamic_extent>::last()
  const
{
	ASSERT_LESS_OR_EQUAL(count, size());
	return lak::span<T, count>::from_ptr(begin() + (size() - count));
}

template<typename T>
inline constexpr lak::span<T, lak::dynamic_extent>
lak::span<T, lak::dynamic_extent>::last(size_t count) const
{
	ASSERT_LESS_OR_EQUAL(count, size());
	return lak::span<T>{begin() + (size() - count), count};
}
