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
lak::uninit_array<T>::uninit_array::~uninit_array()
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
requires lak::concepts::copy_constructible<T>
{
	ASSERT_EQUAL(list.size(), SIZE);
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
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
	if (count == 0) return;

	const size_t old_size = size();

	if (auto old{_data.resize(old_size + count)}; old)
	{
		for (size_t i : lak::size_range_count(before))
			lak::destructive_move_construct(old->data() + i, _data.data() + i);
		for (size_t i : lak::size_range::from_to(before, old_size))
			lak::destructive_move_construct(old->data() + i,
			                                _data.data() + i + count);
	}
	else
	{
		for (size_t i = old_size; i-- > before;)
			lak::destructive_move_construct(_data.data() + i,
			                                _data.data() + i + count);
	}
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::resize_impl(size_t new_size)
{
	if (auto old{_data.resize(new_size)}; old)
		for (size_t i : lak::size_range_count(old->size()))
			lak::destructive_move_construct(old->data() + i, _data.data() + i);
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(size_t initial_size) : array()
{
	resize(initial_size);
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(
  const array<T, lak::dynamic_extent> &other)
requires lak::concepts::copy_constructible<T>
{
	_data.resize(other.size());
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
		for (size_t i : lak::size_range_count(other.size()))
			new (data() + i) T(other[i]);
}

template<typename T>
lak::array<T, lak::dynamic_extent> &lak::array<T, lak::dynamic_extent>::
operator=(const array<T, lak::dynamic_extent> &other)
requires lak::concepts::copy_constructible<T>
{
	clear();
	_data.resize(other.size());
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
		for (size_t i : lak::size_range_count(other.size()))
			new (data() + i) T(other[i]);
	return *this;
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(
  array<T, lak::dynamic_extent> &&other)
: _data(lak::move(other._data))
{
}

template<typename T>
lak::array<T, lak::dynamic_extent> &lak::array<T, lak::dynamic_extent>::
operator=(array<T, lak::dynamic_extent> &&other)
{
	lak::swap(_data, other._data);
	return *this;
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(std::initializer_list<T> list)
requires lak::concepts::copy_constructible<T>
{
	_data.resize(list.size());
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
		for (size_t i : lak::size_range_count(list.size()))
			new (data() + i) T(list.begin()[i]);
}

template<typename T>
template<typename ITER>
requires lak::concepts::copy_constructible<T>
lak::array<T, lak::dynamic_extent>::array(ITER &&begin, ITER &&end)
{
	_data.resize(end - begin);
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
		for (size_t i = 0; begin != end; ++begin, ++i) new (data() + i) T(*begin);
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array::~array()
{
	force_clear();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::resize(size_t new_size)
{
	if (const size_t old_size{size()}; new_size > old_size)
	{
		resize_impl(new_size);
		if constexpr (!std::is_trivially_default_constructible_v<T>)
			for (T *it : lak::pointer_range(begin() + old_size, end())) new (it) T();
	}
	else if (new_size < old_size)
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
			for (T *it : lak::pointer_range(begin() + new_size, end())) it->~T();

		if (_data.resize(new_size)) ASSERT_UNREACHABLE();
	}
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::resize(size_t new_size,
                                                const T &default_value)
requires lak::concepts::copy_constructible<T>
{
	if (const size_t old_size{size()}; new_size > old_size)
	{
		resize_impl(new_size);
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
		if constexpr (!lak::concepts::copy_constructible<T>)
			ASSERT_UNREACHABLE();
		else
#endif
			for (T *it : lak::pointer_range(begin() + old_size, end()))
				new (it) T(default_value);
	}
	else if (new_size < old_size)
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
			for (T *it : lak::pointer_range(begin() + new_size, end())) it->~T();

		if (_data.resize(new_size)) ASSERT_UNREACHABLE();
	}
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::reserve(size_t new_capacity)
{
	if (auto old{_data.reserve(new_capacity)}; old)
		for (size_t i : lak::size_range_count(old->size()))
			lak::destructive_move_construct(old->data() + i, _data.data() + i);
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::clear()
{
	if constexpr (!std::is_trivially_destructible_v<T>)
		for (auto &e : *this) e.~T();
	_data.clear();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::force_clear()
{
	if constexpr (!std::is_trivially_destructible_v<T>)
		for (auto &e : *this) e.~T();
	[[maybe_unused]] auto old{_data.force_clear()};
}

template<typename T>
template<typename... ARGS>
typename lak::array<T, lak::dynamic_extent>::reference
lak::array<T, lak::dynamic_extent>::emplace_front(ARGS &&...args)
{
	right_shift(1U);
	new (data()) T(lak::forward<ARGS>(args)...);
	return front();
}

template<typename T>
typename lak::array<T, lak::dynamic_extent>::reference
lak::array<T, lak::dynamic_extent>::push_front(const T &t)
requires lak::concepts::copy_constructible<T>
{
	right_shift(1U);
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
		new (data()) T(t);
	return front();
}

template<typename T>
typename lak::array<T, lak::dynamic_extent>::reference
lak::array<T, lak::dynamic_extent>::push_front(T &&t)
{
	right_shift(1U);
	new (data()) T(lak::move(t));
	return front();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::pop_front()
{
	ASSERT_GREATER(size(), 0U);
	lak::shift_left(lak::span(_data), 1U);
	if constexpr (!std::is_trivially_destructible_v<T>) back().~T();
	if (_data.resize(size() - 1U)) ASSERT_UNREACHABLE();
}

template<typename T>
T lak::array<T, lak::dynamic_extent>::popped_front()
{
	ASSERT_GREATER(size(), 0U);
	T result = lak::move(front());
	lak::shift_left(lak::span(_data), 1U);
	if constexpr (!std::is_trivially_destructible_v<T>) back().~T();
	if (_data.resize(size() - 1U)) ASSERT_UNREACHABLE();
	return result;
}

template<typename T>
template<typename... ARGS>
typename lak::array<T, lak::dynamic_extent>::reference
lak::array<T, lak::dynamic_extent>::emplace_back(ARGS &&...args)
{
	resize_impl(size() + 1U);
	new (data() + size() - 1U) T(lak::forward<ARGS>(args)...);
	return back();
}

template<typename T>
typename lak::array<T, lak::dynamic_extent>::reference
lak::array<T, lak::dynamic_extent>::push_back(const T &t)
requires lak::concepts::copy_constructible<T>
{
	resize_impl(size() + 1U);
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
		new (data() + size() - 1U) T(t);
	return back();
}

template<typename T>
typename lak::array<T, lak::dynamic_extent>::reference
lak::array<T, lak::dynamic_extent>::push_back(T &&t)
{
	resize_impl(size() + 1U);
	new (data() + size() - 1U) T(lak::move(t));
	return back();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::pop_back()
{
	ASSERT_GREATER(size(), 0U);
	if constexpr (!std::is_trivially_destructible_v<T>) back().~T();
	if (_data.resize(size() - 1U)) ASSERT_UNREACHABLE();
}

template<typename T>
T lak::array<T, lak::dynamic_extent>::popped_back()
{
	ASSERT_GREATER(size(), 0U);
	T result = lak::move(back());
	if constexpr (!std::is_trivially_destructible_v<T>) back().~T();
	if (_data.resize(size() - 1U)) ASSERT_UNREACHABLE();
	return result;
}

template<typename T>
typename lak::array<T, lak::dynamic_extent>::iterator
lak::array<T, lak::dynamic_extent>::insert(const_iterator before,
                                           const T &value)
requires lak::concepts::copy_constructible<T>
{
	ASSERT_GREATER_OR_EQUAL(before, cbegin());
	ASSERT_LESS_OR_EQUAL(before, cend());

	const size_t index = before - data();

	right_shift(1U, index);

#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
		new (data() + index) T(value);

	return data() + index;
}

template<typename T>
typename lak::array<T, lak::dynamic_extent>::iterator
lak::array<T, lak::dynamic_extent>::insert(const_iterator before, T &&value)
{
	ASSERT_GREATER_OR_EQUAL(before, cbegin());
	ASSERT_LESS_OR_EQUAL(before, cend());

	const size_t index = before - data();

	right_shift(1U, index);

	new (data() + index) T(lak::move(value));

	return data() + index;
}

template<typename T>
typename lak::array<T, lak::dynamic_extent>::iterator
lak::array<T, lak::dynamic_extent>::insert(const_iterator before,
                                           std::initializer_list<T> values)
requires lak::concepts::copy_constructible<T>
{
	ASSERT_GREATER_OR_EQUAL(before, cbegin());
	ASSERT_LESS_OR_EQUAL(before, cend());

	const size_t index = before - data();

	right_shift(values.size(), index);

#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
		for (T *it = data() + index; const auto &value : values)
			new (it++) T(value);

	return data() + index;
}

template<typename T>
typename lak::array<T, lak::dynamic_extent>::iterator
lak::array<T, lak::dynamic_extent>::erase(const_iterator first,
                                          const_iterator last)
{
	ASSERT_GREATER_OR_EQUAL(first, cbegin());
	ASSERT_LESS_OR_EQUAL(first, cend());
	ASSERT_GREATER_OR_EQUAL(last, cbegin());
	ASSERT_LESS_OR_EQUAL(last, cend());

	const size_t index  = first - cbegin();
	const size_t length = last - first;

	ASSERT_LESS_OR_EQUAL(index + length, size());

	if (length == 0) return begin() + index;

	lak::shift_left(lak::split(lak::span(_data), first).second, length);

	resize(size() - length);

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

/* --- stack_array --- */

template<typename T, size_t MAX_SIZE>
lak::stack_array<T, MAX_SIZE>::stack_array(std::initializer_list<T> list)
requires lak::concepts::copy_constructible<T>
{
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
		_size =
		  lak::copy(list.begin(), list.end(), data(), data() + MAX_SIZE) - data();
}

template<typename T, size_t MAX_SIZE>
template<typename ITER>
lak::stack_array<T, MAX_SIZE>::stack_array(ITER &&begin, ITER &&end)
requires lak::concepts::copy_constructible<T>
{
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
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
requires lak::concepts::copy_constructible<T>
{
	ASSERT_LESS(_size, MAX_SIZE);
#if defined(LAK_COMPILER_CLANG) && defined(LAK_OS_APPLE)
	if constexpr (!lak::concepts::copy_constructible<T>)
		ASSERT_UNREACHABLE();
	else
#endif
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

template<typename T, size_t MAX_SIZE>
T lak::stack_array<T, MAX_SIZE>::popped_back()
{
	ASSERT(!empty());
	T result = lak::move(back());
	--_size;
	return result;
}
