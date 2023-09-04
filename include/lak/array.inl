#include "lak/algorithm.hpp"
#include "lak/alloc.hpp"
#include "lak/compiler.hpp"
#include "lak/integer_range.hpp"
#include "lak/memmanip.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

/* --- uninit_array --- */

template<typename T>
size_t lak::uninit_array<T>::page_threshold()
{
	const static size_t _page_threshold = (lak::page_size() / 4U) / sizeof(T);
	return _page_threshold;
}

template<typename T>
lak::uninit_array<T>::uninit_array(uninit_array &&other)
: _data(lak::exchange(other._data, lak::span<T>{})),
  _committed(lak::exchange(other._committed, 0U)),
  _size(lak::exchange(other._size, 0U))
{
}

template<typename T>
lak::uninit_array<T> &lak::uninit_array<T>::operator=(uninit_array &&other)
{
	lak::swap(_data, other._data);
	lak::swap(_committed, other._committed);
	lak::swap(_size, other._size);
	return *this;
}

template<typename T>
lak::uninit_array<T>::~uninit_array()
{
	if (is_paged())
	{
		lak::page_free(
		  lak::span<void>(static_cast<void *>(_data.data()),
		                  lak::round_to_page_multiple(_data.size() * sizeof(T))))
		  .expect("page free failed");
	}
	else
	{
		lak::global_free(lak::span<byte_t>(_data));
	}
	_data      = {};
	_committed = 0U;
	_size      = 0U;
}

template<typename T>
void lak::uninit_array<T>::commit_impl(size_t new_capacity)
{
	if (!is_paged() || new_capacity <= _committed) return;

	const size_t committed_bytes{
	  lak::round_to_page_multiple(_committed * sizeof(T))};

	const size_t new_bytes =
	  lak::round_to_page_multiple(new_capacity * sizeof(T));

	lak::page_commit(
	  lak::span<void>(
	    reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(_data.data()) +
	                             committed_bytes),
	    new_bytes - committed_bytes))
	  .expect("commit failed");

	_committed = new_bytes / sizeof(T);
}

template<typename T>
lak::optional<lak::uninit_array<T>> lak::uninit_array<T>::reserve(
  size_t new_capacity)
{
	if (new_capacity <= capacity()) return lak::nullopt;

	new_capacity = std::max(new_capacity, capacity() * 2U);

	const auto old_committed{committed()};
	const auto old_size{_size};

	lak::optional<uninit_array> result{lak::exchange(*this, uninit_array{})};

	if (is_paged(new_capacity))
	{
		// use paged allocations

		const size_t reserve_bytes{
		  lak::round_to_page_multiple(new_capacity * sizeof(T))};

		_data =
		  lak::span<T>(lak::page_reserve(reserve_bytes).expect("reserve failed"));

		commit_impl(old_committed);
	}
	else
	{
		// use non-paged allocations

		_data = lak::span<T>(
		  lak::global_alloc(new_capacity * sizeof(T)).expect("alloc failed"));
	}

	_size = old_size;

	return result;
}

template<typename T>
lak::optional<lak::uninit_array<T>> lak::uninit_array<T>::commit(
  size_t new_capacity)
{
	auto result{reserve(new_capacity)};

	commit_impl(new_capacity);

	return result;
}

template<typename T>
lak::optional<lak::uninit_array<T>> lak::uninit_array<T>::resize(
  size_t new_size)
{
	auto result{commit(new_size)};
	_size = new_size;

	return result;
}

template<typename T>
lak::optional<lak::uninit_array<T>> lak::uninit_array<T>::push_back()
{
	return resize(size() + 1U);
}

/* --- fixed size --- */

template<typename T, size_t SIZE>
lak::array<T, SIZE>::array(std::initializer_list<T> list)
{
	ASSERT_EQUAL(list.size(), SIZE);
	lak::copy(list.begin(), list.end(), data(), data() + SIZE);
}

template<typename T, size_t SIZE>
T &lak::array<T, SIZE>::at(size_t index)
{
	ASSERT_GREATER(SIZE, index);
	return _data[index];
}

template<typename T, size_t SIZE>
const T &lak::array<T, SIZE>::at(size_t index) const
{
	ASSERT_GREATER(SIZE, index);
	return _data[index];
}

template<typename T, size_t SIZE>
constexpr T &lak::array<T, SIZE>::operator[](size_t index)
{
	return _data[index];
}

template<typename T, size_t SIZE>
constexpr const T &lak::array<T, SIZE>::operator[](size_t index) const
{
	return _data[index];
}

template<typename T, size_t SIZE>
constexpr T &lak::array<T, SIZE>::front()
{
	return _data[0];
}

template<typename T, size_t SIZE>
constexpr const T &lak::array<T, SIZE>::front() const
{
	return _data[0];
}

template<typename T, size_t SIZE>
constexpr T &lak::array<T, SIZE>::back()
{
	return _data[SIZE - 1];
}

template<typename T, size_t SIZE>
constexpr const T &lak::array<T, SIZE>::back() const
{
	return _data[SIZE - 1];
}

/* --- dynamic size --- */

template<typename T>
void lak::array<T, lak::dynamic_extent>::right_shift(size_t count,
                                                     size_t before)
{
	if (_size == 0 || count == 0) return;

	ASSERT_GREATER_OR_EQUAL(_committed * sizeof(T), _size + count);
	ASSERT_GREATER_OR_EQUAL(_size, before);

	const size_t moved_count   = _size - before;
	const size_t new_moved     = count > moved_count ? moved_count : count;
	const size_t replace_moved = moved_count - new_moved;

	for (size_t i : lak::size_range::from_count(before, replace_moved))
		data()[i + count] = lak::move(data()[i]);

	for (size_t i :
	     lak::size_range::from_count(before + replace_moved, new_moved))
		new (data() + count + i) T(lak::move(data()[i]));

	if constexpr (!std::is_trivially_destructible_v<T>)
		for (size_t i : lak::size_range::from_count(before, count)) data()[i].~T();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::reserve_bytes(
  size_t new_capacity_bytes, size_t right_shift_count)
{
	if (new_capacity_bytes <= _data.size_bytes())
		return right_shift(right_shift_count);

	ASSERT_GREATER_OR_EQUAL(new_capacity_bytes - _data.size_bytes(),
	                        right_shift_count * sizeof(T));

	new_capacity_bytes = std::max(new_capacity_bytes, _data.size_bytes() * 2);

	lak::unique_pages new_buffer =
	  lak::unique_pages::make(new_capacity_bytes, &new_capacity_bytes);

	if (_data.size_bytes() > 0)
	{
		_committed = lak::round_to_page_multiple(_size * sizeof(T));
		lak::page_commit(lak::span<void>(new_buffer.data(), _committed))
		  .expect("commit failed");

		// move all the Ts from the old memory block to the new memory block
		for (size_t i = 0; i < _size; ++i)
			new (static_cast<T *>(new_buffer.data()) + i + right_shift_count)
			  T(lak::move(data()[i]));

		// destroy the Ts in the old memory block
		if constexpr (!std::is_trivially_destructible_v<T>)
			for (size_t i = 0; i < _size; ++i) data()[i].~T();
	}
	else
	{
		_committed = 0;
	}

	lak::swap(_data, new_buffer);

	ASSERT_GREATER_OR_EQUAL(_data.size_bytes(), _committed);
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::commit(size_t new_size,
                                                size_t right_shift_count)
{
	size_t new_size_bytes = lak::round_to_page_multiple(new_size * sizeof(T));
	if (new_size_bytes > _committed)
	{
		reserve_bytes(new_size_bytes, right_shift_count);
		_committed = new_size_bytes;
		lak::page_commit(lak::span<void>(_data.data(), _committed))
		  .expect("commit failed");
	}
	else
	{
		right_shift(right_shift_count);
	}
	if (new_size != 0) ASSERT(_data.size_bytes() > 0);
	ASSERT_GREATER_OR_EQUAL(_committed, new_size);
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(size_t initial_size) : array()
{
	resize(initial_size);
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(
  const array<T, lak::dynamic_extent> &other)
{
	operator=(other);
}

template<typename T>
lak::array<T, lak::dynamic_extent> &lak::array<T, lak::dynamic_extent>::
operator=(const array<T, lak::dynamic_extent> &other)
{
	clear();
	reserve(other.size());
	for (const auto &e : other) push_back(e);
	return *this;
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(
  array<T, lak::dynamic_extent> &&other)
: _data(lak::exchange(other._data, lak::unique_pages{})),
  _size(lak::exchange(other._size, 0)),
  _committed(lak::exchange(other._committed, 0))
{
}

template<typename T>
lak::array<T, lak::dynamic_extent> &lak::array<T, lak::dynamic_extent>::
operator=(array<T, lak::dynamic_extent> &&other)
{
	lak::swap(_data, other._data);
	lak::swap(_size, other._size);
	lak::swap(_committed, other._committed);
	return *this;
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(std::initializer_list<T> list)
{
	commit(list.size());
	ASSERT_LESS_OR_EQUAL(list.size(), _committed);
	for (; _size < list.size(); ++_size)
		new (data() + _size) T(list.begin()[_size]);
}

template<typename T>
template<typename ITER>
lak::array<T, lak::dynamic_extent>::array(ITER &&begin, ITER &&end)
{
	const size_t sz = end - begin;
	commit(sz);
	ASSERT_LESS_OR_EQUAL(sz, _committed);
	for (; _size < sz; ++_size, ++begin) new (data() + _size) T(*begin);
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array::~array()
{
	force_clear();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::resize(size_t new_size)
{
	if (new_size > _size)
	{
		commit(new_size);
		for (; _size < new_size; ++_size) new (data() + _size) T();
	}
	else if (new_size < _size)
	{
		if constexpr (std::is_trivially_destructible_v<T>)
			_size = new_size;
		else
			for (; _size > new_size; --_size) data()[_size - 1].~T();
	}
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::resize(size_t new_size,
                                                const T &default_value)
{
	if (new_size > _size)
	{
		commit(new_size);
		for (; _size < new_size; ++_size) new (data() + _size) T(default_value);
	}
	else if (new_size < _size)
	{
		if constexpr (std::is_trivially_destructible_v<T>)
			_size = new_size;
		else
			for (; _size > new_size; --_size) data()[_size - 1].~T();
	}
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::reserve(size_t new_capacity)
{
	reserve_bytes(new_capacity * sizeof(T));
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::clear()
{
	if constexpr (!std::is_trivially_destructible_v<T>)
		for (auto &e : *this) e.~T();
	_size = 0;
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::force_clear()
{
	clear();
	_committed = 0;
	_data      = {};
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::at(size_t index)
{
	ASSERT_GREATER(_size, index);
	return data()[index];
}

template<typename T>
const T &lak::array<T, lak::dynamic_extent>::at(size_t index) const
{
	ASSERT_GREATER(_size, index);
	return data()[index];
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::front()
{
	ASSERT_GREATER(_size, 0U);
	return data()[0];
}

template<typename T>
const T &lak::array<T, lak::dynamic_extent>::front() const
{
	ASSERT_GREATER(_size, 0U);
	return data()[0];
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::back()
{
	ASSERT_GREATER(_size, 0U);
	return data()[_size - 1];
}

template<typename T>
const T &lak::array<T, lak::dynamic_extent>::back() const
{
	ASSERT_GREATER(_size, 0U);
	return data()[_size - 1];
}

template<typename T>
template<typename... ARGS>
T &lak::array<T, lak::dynamic_extent>::emplace_front(ARGS &&...args)
{
	commit(_size + 1, 1U);
	new (data()) T(lak::forward<ARGS>(args)...);
	++_size;
	return back();
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::push_front(const T &t)
{
	commit(_size + 1, 1U);
	new (data()) T(t);
	++_size;
	return back();
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::push_front(T &&t)
{
	commit(_size + 1, 1U);
	new (data()) T(lak::move(t));
	++_size;
	return back();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::pop_front()
{
	ASSERT_GREATER(_size, 0U);
	lak::rotate_left(begin(), end());
	--_size;
	if constexpr (!std::is_trivially_destructible_v<T>) data()[_size].~T();
}

template<typename T>
template<typename... ARGS>
T &lak::array<T, lak::dynamic_extent>::emplace_back(ARGS &&...args)
{
	commit(_size + 1);
	new (data() + _size) T(lak::forward<ARGS>(args)...);
	++_size;
	return back();
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::push_back(const T &t)
{
	commit(_size + 1);
	new (data() + _size) T(t);
	++_size;
	return back();
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::push_back(T &&t)
{
	commit(_size + 1);
	new (data() + _size) T(lak::move(t));
	++_size;
	return back();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::pop_back()
{
	ASSERT_GREATER(_size, 0U);
	--_size;
	if constexpr (!std::is_trivially_destructible_v<T>) data()[_size].~T();
}

template<typename T>
T *lak::array<T, lak::dynamic_extent>::insert(const T *before, const T &value)
{
	ASSERT_GREATER_OR_EQUAL(before, cbegin());
	ASSERT_LESS_OR_EQUAL(before, cend());

	const size_t index = before - data();

	commit(_size + 1U);

	right_shift(1U, index);

	new (data() + index) T(value);

	++_size;

	return data() + index;
}

template<typename T>
T *lak::array<T, lak::dynamic_extent>::insert(const T *before, T &&value)
{
	ASSERT_GREATER_OR_EQUAL(before, cbegin());
	ASSERT_LESS_OR_EQUAL(before, cend());

	const size_t index = before - data();

	commit(_size + 1U);

	right_shift(1U, index);

	new (data() + index) T(lak::move(value));

	++_size;

	return data() + index;
}

template<typename T>
T *lak::array<T, lak::dynamic_extent>::insert(const T *before,
                                              std::initializer_list<T> values)
{
	ASSERT_GREATER_OR_EQUAL(before, cbegin());
	ASSERT_LESS_OR_EQUAL(before, cend());

	const size_t index = before - data();

	commit(_size + values.size());

	right_shift(values.size(), index);

	for (size_t i = index; const auto &value : values)
		new (data() + i++) T(value);

	++_size;

	return data() + index;
}

template<typename T>
T *lak::array<T, lak::dynamic_extent>::erase(const T *first, const T *last)
{
	ASSERT_GREATER_OR_EQUAL(first, cbegin());
	ASSERT_LESS_OR_EQUAL(first, cend());
	ASSERT_GREATER_OR_EQUAL(last, cbegin());
	ASSERT_LESS_OR_EQUAL(last, cend());

	const size_t index  = first - cbegin();
	const size_t length = last - first;

	ASSERT_LESS_OR_EQUAL(index + length, size());

	if (length == 0) return begin() + index;

	lak::shift_left(lak::split(lak::span(*this), first).second, length);

	for (size_t i = length; i-- > 0;) pop_back();

	return begin() + index;
}

template<typename T, size_t S>
bool operator==(const lak::array<T, S> &a, const lak::array<T, S> &b)
{
	if (a.size() != b.size()) return false;
	for (size_t i = 0; i < a.size(); ++i)
		if (lak::not_equal_to<T>{}(a[i], b[i])) return false;
	return true;
}

template<typename T, size_t S>
bool operator!=(const lak::array<T, S> &a, const lak::array<T, S> &b)
{
	return !(a == b);
}

/* --- small_array --- */

template<typename T, lak::alloc::locality LOC>
void lak::small_array<T, LOC>::right_shift(size_t count, size_t before)
{
	if (_size == 0 || count == 0) return;

	ASSERT_GREATER_OR_EQUAL(_data.size(), _size + count);
	ASSERT_GREATER_OR_EQUAL(_size, before);

	const size_t moved_count   = _size - before;
	const size_t new_moved     = count > moved_count ? moved_count : count;
	const size_t replace_moved = moved_count - new_moved;

	for (size_t i : lak::size_range::from_count(before, replace_moved))
		data()[i + count] = lak::move(data()[i]);

	for (size_t i :
	     lak::size_range::from_count(before + replace_moved, new_moved))
		new (data() + count + i) T(lak::move(data()[i]));

	if constexpr (!std::is_trivially_destructible_v<T>)
		for (size_t i : lak::size_range::from_count(before, count)) data()[i].~T();
}

template<typename T, lak::alloc::locality LOC>
lak::small_array<T, LOC>::small_array(size_t initial_size) : small_array()
{
	resize(initial_size);
}

template<typename T, lak::alloc::locality LOC>
lak::small_array<T, LOC>::small_array(const small_array<T, LOC> &other)
{
	operator=(other);
}

template<typename T, lak::alloc::locality LOC>
lak::small_array<T, LOC> &lak::small_array<T, LOC>::operator=(
  const small_array<T, LOC> &other)
{
	clear();
	reserve(other.size());
	for (const auto &e : other) push_back(e);
	return *this;
}

template<typename T, lak::alloc::locality LOC>
lak::small_array<T, LOC>::small_array(small_array<T, LOC> &&other)
: _data(lak::exchange(other._data, {})), _size(lak::exchange(other._size, 0))
{
}

template<typename T, lak::alloc::locality LOC>
lak::small_array<T, LOC> &lak::small_array<T, LOC>::operator=(
  small_array<T, LOC> &&other)
{
	lak::swap(_data, other._data);
	lak::swap(_size, other._size);
	return *this;
}

template<typename T, lak::alloc::locality LOC>
lak::small_array<T, LOC>::small_array(std::initializer_list<T> list)
{
	reserve(list.size());
	ASSERT_LESS_OR_EQUAL(list.size(), _data.size());
	for (; _size < list.size(); ++_size)
		new (data() + _size) T(list.begin()[_size]);
}

template<typename T, lak::alloc::locality LOC>
template<typename ITER>
lak::small_array<T, LOC>::small_array(ITER &&begin, ITER &&end)
{
	const size_t sz = end - begin;
	reserve(sz);
	ASSERT_LESS_OR_EQUAL(sz, _data.size());
	for (; _size < sz; ++_size, ++begin) new (data() + _size) T(*begin);
}

template<typename T, lak::alloc::locality LOC>
lak::small_array<T, LOC>::small_array::~small_array()
{
	force_clear();
}

template<typename T, lak::alloc::locality LOC>
void lak::small_array<T, LOC>::resize(size_t new_size)
{
	if (new_size > _size)
	{
		reserve(new_size);
		for (; _size < new_size; ++_size) new (data() + _size) T();
	}
	else if (new_size < _size)
	{
		for (; _size > new_size; --_size) data()[_size - 1].~T();
	}
}

template<typename T, lak::alloc::locality LOC>
void lak::small_array<T, LOC>::resize(size_t new_size, const T &default_value)
{
	if (new_size > _size)
	{
		reserve(new_size);
		for (; _size < new_size; ++_size) new (data() + _size) T(default_value);
	}
	else if (new_size < _size)
	{
		for (; _size > new_size; --_size) data()[_size - 1].~T();
	}
}

template<typename T, lak::alloc::locality LOC>
void lak::small_array<T, LOC>::reserve(size_t new_capacity)
{
	if (new_capacity > _data.size())
	{
		auto new_data = lak::span<T>(
		  lak::malloc<LOC>(new_capacity * sizeof(T), alignof(T)).UNWRAP());

		for (size_t i = 0; i < _size; ++i)
		{
			new (new_data.data() + i) T(lak::move(_data[i]));
			_data[i].~T();
		}

		lak::free<LOC>(lak::span<byte_t>(_data));

		_data = new_data;
	}
}

template<typename T, lak::alloc::locality LOC>
void lak::small_array<T, LOC>::clear()
{
	for (auto &e : *this) e.~T();
	_size = 0;
}

template<typename T, lak::alloc::locality LOC>
void lak::small_array<T, LOC>::force_clear()
{
	clear();
	lak::free<LOC>(lak::span<byte_t>(_data));
	_data = {};
}

template<typename T, lak::alloc::locality LOC>
T &lak::small_array<T, LOC>::at(size_t index)
{
	ASSERT_GREATER(_size, index);
	return data()[index];
}

template<typename T, lak::alloc::locality LOC>
const T &lak::small_array<T, LOC>::at(size_t index) const
{
	ASSERT_GREATER(_size, index);
	return data()[index];
}

template<typename T, lak::alloc::locality LOC>
T &lak::small_array<T, LOC>::front()
{
	ASSERT_GREATER(_size, 0U);
	return data()[0];
}

template<typename T, lak::alloc::locality LOC>
const T &lak::small_array<T, LOC>::front() const
{
	ASSERT_GREATER(_size, 0U);
	return data()[0];
}

template<typename T, lak::alloc::locality LOC>
T &lak::small_array<T, LOC>::back()
{
	ASSERT_GREATER(_size, 0U);
	return data()[_size - 1];
}

template<typename T, lak::alloc::locality LOC>
const T &lak::small_array<T, LOC>::back() const
{
	ASSERT_GREATER(_size, 0U);
	return data()[_size - 1];
}

template<typename T, lak::alloc::locality LOC>
template<typename... ARGS>
T &lak::small_array<T, LOC>::emplace_front(ARGS &&...args)
{
	reserve(_size + 1, 1U);
	new (data()) T(lak::forward<ARGS>(args)...);
	++_size;
	return back();
}

template<typename T, lak::alloc::locality LOC>
T &lak::small_array<T, LOC>::push_front(const T &t)
{
	reserve(_size + 1, 1U);
	new (data()) T(t);
	++_size;
	return back();
}

template<typename T, lak::alloc::locality LOC>
T &lak::small_array<T, LOC>::push_front(T &&t)
{
	reserve(_size + 1, 1U);
	new (data()) T(lak::move(t));
	++_size;
	return back();
}

template<typename T, lak::alloc::locality LOC>
void lak::small_array<T, LOC>::pop_front()
{
	ASSERT_GREATER(_size, 0U);
	lak::rotate_left(begin(), end());
	--_size;
	if constexpr (!std::is_trivially_destructible_v<T>) data()[_size].~T();
}

template<typename T, lak::alloc::locality LOC>
template<typename... ARGS>
T &lak::small_array<T, LOC>::emplace_back(ARGS &&...args)
{
	reserve(_size + 1);
	new (data() + _size) T(lak::forward<ARGS>(args)...);
	++_size;
	return back();
}

template<typename T, lak::alloc::locality LOC>
T &lak::small_array<T, LOC>::push_back(const T &t)
{
	reserve(_size + 1);
	new (data() + _size) T(t);
	++_size;
	return back();
}

template<typename T, lak::alloc::locality LOC>
T &lak::small_array<T, LOC>::push_back(T &&t)
{
	reserve(_size + 1);
	new (data() + _size) T(lak::move(t));
	++_size;
	return back();
}

template<typename T, lak::alloc::locality LOC>
void lak::small_array<T, LOC>::pop_back()
{
	ASSERT_GREATER(_size, 0U);
	--_size;
	if constexpr (!std::is_trivially_destructible_v<T>) data()[_size].~T();
}

template<typename T, lak::alloc::locality LOC>
T *lak::small_array<T, LOC>::insert(const T *before, const T &value)
{
	ASSERT_GREATER_OR_EQUAL(before, cbegin());
	ASSERT_LESS_OR_EQUAL(before, cend());

	const size_t index = before - data();

	reserve(_size + 1U);

	right_shift(1U, index);

	new (data() + index) T(value);

	++_size;

	return data() + index;
}

template<typename T, lak::alloc::locality LOC>
T *lak::small_array<T, LOC>::insert(const T *before, T &&value)
{
	ASSERT_GREATER_OR_EQUAL(before, cbegin());
	ASSERT_LESS_OR_EQUAL(before, cend());

	const size_t index = before - data();

	reserve(_size + 1U);

	right_shift(1U, index);

	new (data() + index) T(lak::move(value));

	++_size;

	return data() + index;
}

template<typename T, lak::alloc::locality LOC>
T *lak::small_array<T, LOC>::insert(const T *before,
                                    std::initializer_list<T> values)
{
	ASSERT_GREATER_OR_EQUAL(before, cbegin());
	ASSERT_LESS_OR_EQUAL(before, cend());

	const size_t index = before - data();

	reserve(_size + values.size());

	right_shift(values.size(), index);

	for (size_t i = index; const auto &value : values)
		new (data() + i++) T(value);

	++_size;

	return data() + index;
}

template<typename T, lak::alloc::locality LOC>
T *lak::small_array<T, LOC>::erase(const T *first, const T *last)
{
	ASSERT_GREATER_OR_EQUAL(first, cbegin());
	ASSERT_LESS_OR_EQUAL(first, cend());
	ASSERT_GREATER_OR_EQUAL(last, cbegin());
	ASSERT_LESS_OR_EQUAL(last, cend());

	const size_t index  = first - cbegin();
	const size_t length = last - first;

	ASSERT_LESS_OR_EQUAL(index + length, size());

	if (length == 0) return begin() + index;

	lak::shift_left(lak::split(lak::span(*this), first).second, length);

	for (size_t i = length; i-- > 0;) pop_back();

	return begin() + index;
}

template<typename T, lak::alloc::locality LOC>
bool operator==(const lak::small_array<T, LOC> &a,
                const lak::small_array<T, LOC> &b)
{
	if (a.size() != b.size()) return false;
	for (size_t i = 0; i < a.size(); ++i)
		if (lak::not_equal_to<T>{}(a[i], b[i])) return false;
	return true;
}

template<typename T, lak::alloc::locality LOC>
bool operator!=(const lak::small_array<T, LOC> &a,
                const lak::small_array<T, LOC> &b)
{
	return !(a == b);
}

template<typename T, size_t MAX_SIZE>
lak::stack_array<T, MAX_SIZE>::stack_array(std::initializer_list<T> list)
{
	_size =
	  lak::copy(list.begin(), list.end(), data(), data() + MAX_SIZE) - data();
}

template<typename T, size_t MAX_SIZE>
template<typename ITER>
lak::stack_array<T, MAX_SIZE>::stack_array(ITER &&begin, ITER &&end)
{
	_size = lak::copy(begin, end, data(), data() + MAX_SIZE) - data();
}

template<typename T, size_t MAX_SIZE>
T &lak::stack_array<T, MAX_SIZE>::at(size_t index)
{
	ASSERT_LESS(index, _size);
	return _data[index];
}

template<typename T, size_t MAX_SIZE>
const T &lak::stack_array<T, MAX_SIZE>::at(size_t index) const
{
	ASSERT_LESS(index, _size);
	return _data[index];
}

template<typename T, size_t MAX_SIZE>
constexpr T &lak::stack_array<T, MAX_SIZE>::operator[](size_t index)
{
	return _data[index];
}

template<typename T, size_t MAX_SIZE>
constexpr const T &lak::stack_array<T, MAX_SIZE>::operator[](
  size_t index) const
{
	return _data[index];
}

template<typename T, size_t MAX_SIZE>
constexpr T &lak::stack_array<T, MAX_SIZE>::front()
{
	ASSERT(!empty());
	return _data[0U];
}

template<typename T, size_t MAX_SIZE>
constexpr const T &lak::stack_array<T, MAX_SIZE>::front() const
{
	ASSERT(!empty());
	return _data[0U];
}

template<typename T, size_t MAX_SIZE>
constexpr T &lak::stack_array<T, MAX_SIZE>::back()
{
	ASSERT(!empty());
	return _data[_size - 1U];
}

template<typename T, size_t MAX_SIZE>
constexpr const T &lak::stack_array<T, MAX_SIZE>::back() const
{
	ASSERT(!empty());
	return _data[_size - 1U];
}

template<typename T, size_t MAX_SIZE>
T &lak::stack_array<T, MAX_SIZE>::push_back(const T &t)
{
	ASSERT_LESS(_size, MAX_SIZE);
	_data[_size] = t;
	return _data[_size++];
}

template<typename T, size_t MAX_SIZE>
T &lak::stack_array<T, MAX_SIZE>::push_back(T &&t)
{
	ASSERT_LESS(_size, MAX_SIZE);
	_data[_size] = lak::move(t);
	return _data[_size++];
}

template<typename T, size_t MAX_SIZE>
void lak::stack_array<T, MAX_SIZE>::pop_back()
{
	ASSERT(!empty());
	--_size;
}
