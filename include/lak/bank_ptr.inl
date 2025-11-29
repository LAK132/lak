#include "lak/ptr_intrin.hpp"
#include "lak/utility.hpp"

#include <algorithm>

/* --- lak::bank<T> --- */

template<typename T>
void lak::bank<T>::internal_sort()
{
	std::sort(_deleted.begin(), _deleted.end(), std::less<size_t>{});
	ASSERT(std::adjacent_find(_deleted.begin(), _deleted.end()) ==
	       _deleted.end());
}

template<typename T>
void lak::bank<T>::internal_flush()
{
	if (_deleted.empty()) return;
	internal_sort();
	size_t destroyed = 0;
	while (destroyed < _deleted.size() &&
	       _deleted[_deleted.size() - (destroyed + 1U)] == _container.size() - 1)
	{
		_container.pop_back();
		++destroyed;
	}
	_deleted.erase(_deleted.end() - destroyed, _deleted.end());
}

template<typename T>
size_t lak::bank<T>::internal_find_index(T *ptr)
{
	if (!ptr) return lak::dynamic_extent;
	size_t it = _container.find(ptr);
	ASSERT_NOT_EQUAL(it, _container.size());
	return it;
}

template<typename T>
template<typename... ARGS>
size_t lak::bank<T>::internal_create(ARGS &&...args)
{
	if (_deleted.size() > 0)
	{
		size_t index = _deleted.popped_back();
		new (&_container[index]) T(lak::forward<ARGS>(args)...);
		return index;
	}
	else
	{
		_container.push_back();
		new (&_container.back()) T(lak::forward<ARGS>(args)...);
		return _container.size() - 1;
	}
}

template<typename T>
void lak::bank<T>::internal_destroy(size_t index)
{
	ASSERT_GREATER(_container.size(), index);
	_container[index].~T();
	_deleted.push_back(index);
	internal_flush();
}

template<typename T>
template<typename FUNCTOR>
size_t lak::bank<T>::internal_find_if(FUNCTOR &&func)
{
	internal_sort();

	size_t i = 0U;
	for (size_t del_index = 0;
	     del_index < _deleted.size() && i < _container.size();
	     ++i)
	{
		ASSERT_LESS_OR_EQUAL(i, _deleted[del_index]);
		if (i == _deleted[del_index])
		{
			++del_index;
			continue;
		}

		if (func(_container[i])) return i;
	}

	for (; i < _container.size(); ++i)
		if (func(_container[i])) return i;

	return lak::dynamic_extent;
}

template<typename T>
void lak::bank<T>::flush()
{
	std::lock_guard lock(_mutex);
	internal_flush();
}

template<typename T>
bool lak::bank<T>::empty()
{
	std::lock_guard lock(_mutex);
	return _container.empty();
}

template<typename T>
T *lak::bank<T>::create(const T &t)
{
	std::lock_guard lock(_mutex);
	auto result = internal_create(t);
	return result != lak::dynamic_extent ? &_container[result] : nullptr;
}

template<typename T>
T *lak::bank<T>::create(T &&t)
{
	std::lock_guard lock(_mutex);
	auto result = internal_create(lak::move(t));
	return result != lak::dynamic_extent ? &_container[result] : nullptr;
}

template<typename T>
template<typename... ARGS>
T *lak::bank<T>::create(ARGS &&...args)
{
	std::lock_guard lock(_mutex);
	auto result = internal_create(lak::forward<ARGS>(args)...);
	return result != lak::dynamic_extent ? &_container[result] : nullptr;
}

template<typename T>
void lak::bank<T>::destroy(T *t)
{
	std::lock_guard lock(_mutex);
	internal_destroy(internal_find_index(t));
}

template<typename T>
template<typename FUNCTOR>
void lak::bank<T>::for_each(FUNCTOR &&func)
{
	std::unique_lock lock(_mutex);
	internal_sort();
	for (size_t del_index = 0, i = 0; i < _container.size(); ++i)
	{
		// we want _deleted[del_index] to either refer to the current index or the
		// next deleted index. if _deleted[del_index] is less than the current
		// index, increment until it is greater or equal. if _deleted[del_index-1]
		// is greater or equal to the current index, then we need to roll back
		// del_index until it isn't.
		if (del_index > _deleted.size()) del_index = _deleted.size();
		while (del_index != 0U && (del_index - 1U) < _deleted.size() &&
		       _deleted[del_index - 1U] >= i)
			--del_index;
		while (del_index < _deleted.size() && _deleted[del_index] < i) ++del_index;

		if (del_index < _deleted.size() && _deleted[del_index] == i)
			++del_index;
		else
		{
			auto &it = _container[i];
			lock.unlock();
			func(it);
			lock.lock();
		}
	}
}

template<typename T>
template<typename FUNCTOR>
T *lak::bank<T>::find_if(FUNCTOR &&func)
{
	std::lock_guard lock(_mutex);
	auto result = internal_find_if(lak::forward<FUNCTOR>(func));
	return result != lak::dynamic_extent ? &_container[result] : nullptr;
}

/* --- lak::unique_bank_ptr<T> --- */

template<typename T>
lak::unique_bank_ptr<T> lak::unique_bank_ptr<T>::create(const T &t)
{
	std::lock_guard lock(lak::bank<T>::_mutex);
	return {lak::bank<T>::internal_create(t)};
}

template<typename T>
lak::unique_bank_ptr<T> lak::unique_bank_ptr<T>::create(T &&t)
{
	std::lock_guard lock(lak::bank<T>::_mutex);
	return {lak::bank<T>::internal_create(lak::move(t))};
}

template<typename T>
template<typename... ARGS>
lak::unique_bank_ptr<T> lak::unique_bank_ptr<T>::create(ARGS &&...args)
{
	std::lock_guard lock(lak::bank<T>::_mutex);
	return {lak::bank<T>::internal_create(lak::forward<ARGS>(args)...)};
}

template<typename T>
lak::unique_bank_ptr<T> lak::unique_bank_ptr<T>::from_raw_bank_ptr(T *ptr)
{
	if (!ptr) return {};
	std::lock_guard lock(lak::bank<T>::_mutex);
	return {lak::bank<T>::internal_find_index(ptr)};
}

template<typename T>
lak::unique_bank_ptr<T>::unique_bank_ptr()
: unique_bank_ptr(lak::dynamic_extent)
{
}

template<typename T>
lak::unique_bank_ptr<T> &lak::unique_bank_ptr<T>::operator=(lak::nullptr_t)
{
	reset();
	return *this;
}

template<typename T>
lak::unique_bank_ptr<T>::unique_bank_ptr(unique_bank_ptr &&other)
: unique_bank_ptr()
{
	std::swap(_index, other._index);
	std::swap(_value, other._value);
};

template<typename T>
lak::unique_bank_ptr<T> &lak::unique_bank_ptr<T>::operator=(
  unique_bank_ptr &&other)
{
	std::swap(_index, other._index);
	std::swap(_value, other._value);
	return *this;
};

template<typename T>
lak::unique_bank_ptr<T>::unique_bank_ptr::~unique_bank_ptr()
{
	reset();
}

template<typename T>
void lak::unique_bank_ptr<T>::reset()
{
	if (!*this) return;
	std::lock_guard lock(lak::bank<T>::_mutex);
	lak::bank<T>::internal_destroy(_index);
	_index = lak::dynamic_extent;
	_value = nullptr;
}

template<typename T>
T *lak::unique_bank_ptr<T>::release()
{
	auto result = _value;
	_index      = lak::dynamic_extent;
	_value      = nullptr;
	return result;
}

template<typename T>
T *lak::unique_bank_ptr<T>::get()
{
	return _value;
}

template<typename T>
const T *lak::unique_bank_ptr<T>::get() const
{
	return _value;
}

template<typename T>
T *lak::unique_bank_ptr<T>::operator->()
{
	return _value;
}

template<typename T>
const T *lak::unique_bank_ptr<T>::operator->() const
{
	return _value;
}

template<typename T>
T &lak::unique_bank_ptr<T>::operator*()
{
	return *_value;
}

template<typename T>
const T &lak::unique_bank_ptr<T>::operator*() const
{
	return *_value;
}

template<typename T>
bool lak::unique_bank_ptr<T>::operator<(
  const lak::unique_bank_ptr<T> &rhs) const
{
	ASSERT((_index == rhs._index) == (_value == rhs._value));
	return _index < rhs._index;
}

template<typename T>
bool lak::unique_bank_ptr<T>::operator<=(
  const lak::unique_bank_ptr<T> &rhs) const
{
	ASSERT((_index == rhs._index) == (_value == rhs._value));
	return _index <= rhs._index;
}

template<typename T>
bool lak::unique_bank_ptr<T>::operator>(
  const lak::unique_bank_ptr<T> &rhs) const
{
	ASSERT((_index == rhs._index) == (_value == rhs._value));
	return _index > rhs._index;
}

template<typename T>
bool lak::unique_bank_ptr<T>::operator>=(
  const lak::unique_bank_ptr<T> &rhs) const
{
	ASSERT((_index == rhs._index) == (_value == rhs._value));
	return _index >= rhs._index;
}

template<typename T>
bool lak::unique_bank_ptr<T>::operator==(
  const lak::unique_bank_ptr<T> &rhs) const
{
	ASSERT((_index == rhs._index) == (_value == rhs._value));
	return _index == rhs._index;
}

template<typename T>
bool lak::unique_bank_ptr<T>::operator!=(
  const lak::unique_bank_ptr<T> &rhs) const
{
	ASSERT((_index == rhs._index) == (_value == rhs._value));
	return _index != rhs._index;
}

template<typename T>
bool lak::unique_bank_ptr<T>::operator==(lak::nullptr_t) const
{
	return !*this;
}

template<typename T>
bool lak::unique_bank_ptr<T>::operator!=(lak::nullptr_t) const
{
	return !!*this;
}

template<typename T>
lak::unique_bank_ptr<T>::operator bool() const
{
	ASSERT((_index == lak::dynamic_extent) == (_value == nullptr));
	return _index != lak::dynamic_extent;
}

/* --- lak::shared_bank_ptr<T> --- */

template<typename T>
void lak::shared_bank_ptr<T>::flush()
{
	std::lock_guard lock(lak::bank<T>::_mutex);
	lak::bank<T>::internal_flush();
	_reference_count.resize(lak::bank<T>::_container.size());
}

template<typename T>
lak::shared_bank_ptr<T> lak::shared_bank_ptr<T>::create(const T &t)
{
	std::lock_guard lock(lak::bank<T>::_mutex);
	auto index = lak::bank<T>::internal_create(t);
	_reference_count.resize(lak::bank<T>::_container.size());
	++_reference_count[index];
	return {index};
}

template<typename T>
lak::shared_bank_ptr<T> lak::shared_bank_ptr<T>::create(T &&t)
{
	std::lock_guard lock(lak::bank<T>::_mutex);
	auto index = lak::bank<T>::internal_create(lak::move(t));
	_reference_count.resize(lak::bank<T>::_container.size());
	++_reference_count[index];
	return {index};
}

template<typename T>
template<typename... ARGS>
lak::shared_bank_ptr<T> lak::shared_bank_ptr<T>::create(ARGS &&...args)
{
	std::lock_guard lock(lak::bank<T>::_mutex);
	auto index = lak::bank<T>::internal_create(lak::forward<ARGS>(args)...);
	_reference_count.resize(lak::bank<T>::_container.size());
	++_reference_count[index];
	return {index};
}

template<typename T>
template<typename FUNCTOR>
lak::shared_bank_ptr<T> lak::shared_bank_ptr<T>::find_if(FUNCTOR &&func)
{
	std::lock_guard lock(lak::bank<T>::_mutex);
	auto result = internal_find_if(lak::forward<FUNCTOR>(func));
	_reference_count.resize(lak::bank<T>::_container.size());
	if (result != lak::dynamic_extent) ++_reference_count[result];
	return {result};
}

template<typename T>
lak::shared_bank_ptr<T> &lak::shared_bank_ptr<T>::operator=(lak::nullptr_t)
{
	reset();
	return *this;
}

template<typename T>
lak::shared_bank_ptr<T>::shared_bank_ptr(const shared_bank_ptr &other)
{
	if (other)
	{
		std::lock_guard lock(lak::bank<T>::_mutex);
		lak::unique_bank_ptr<T>::_index = other._index;
		lak::unique_bank_ptr<T>::_value = other._value;
		++_reference_count[lak::unique_bank_ptr<T>::_index];
	}
	else
		reset();
};

template<typename T>
lak::shared_bank_ptr<T> &lak::shared_bank_ptr<T>::operator=(
  const shared_bank_ptr &other)
{
	if (other)
	{
		std::lock_guard lock(lak::bank<T>::_mutex);
		lak::unique_bank_ptr<T>::_index = other._index;
		lak::unique_bank_ptr<T>::_value = other._value;
		++_reference_count[lak::unique_bank_ptr<T>::_index];
	}
	else
		reset();
	return *this;
};

template<typename T>
lak::shared_bank_ptr<T>::shared_bank_ptr(lak::unique_bank_ptr<T> &&other)
: lak::unique_bank_ptr<T>()
{
	if (other)
	{
		std::lock_guard lock(lak::bank<T>::_mutex);
		std::swap(lak::unique_bank_ptr<T>::_index, other._index);
		std::swap(lak::unique_bank_ptr<T>::_value, other._value);
		++_reference_count[lak::unique_bank_ptr<T>::_index];
	}
	else
		reset();
};

template<typename T>
lak::shared_bank_ptr<T> &lak::shared_bank_ptr<T>::operator=(
  lak::unique_bank_ptr<T> &&other)
{
	if (other)
	{
		std::lock_guard lock(lak::bank<T>::_mutex);
		std::swap(lak::unique_bank_ptr<T>::_index, other._index);
		std::swap(lak::unique_bank_ptr<T>::_value, other._value);
		++_reference_count[lak::unique_bank_ptr<T>::_index];
	}
	else
		reset();
	return *this;
};

template<typename T>
lak::shared_bank_ptr<T>::shared_bank_ptr::~shared_bank_ptr()
{
	reset();
}

template<typename T>
void lak::shared_bank_ptr<T>::reset()
{
	if (!*this) return;
	std::lock_guard lock(lak::bank<T>::_mutex);
	if (--_reference_count[lak::unique_bank_ptr<T>::_index] == 0)
	{
		if (lak::unique_bank_ptr<T>::_index == lak::bank<T>::_container.size() - 1)
		{
			lak::bank<T>::_container.pop_back();
			_reference_count.pop_back();
		}
		else
		{
			lak::bank<T>::internal_destroy(lak::unique_bank_ptr<T>::_index);
			_reference_count.resize(lak::bank<T>::_container.size());
		}
	}
	lak::unique_bank_ptr<T>::_index = lak::dynamic_extent;
	lak::unique_bank_ptr<T>::_value = nullptr;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator<(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((lak::unique_bank_ptr<T>::_index == rhs._index) ==
	       (lak::unique_bank_ptr<T>::_value == rhs._value));
	return lak::unique_bank_ptr<T>::_index < rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator<=(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((lak::unique_bank_ptr<T>::_index == rhs._index) ==
	       (lak::unique_bank_ptr<T>::_value == rhs._value));
	return lak::unique_bank_ptr<T>::_index <= rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator>(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((lak::unique_bank_ptr<T>::_index == rhs._index) ==
	       (lak::unique_bank_ptr<T>::_value == rhs._value));
	return lak::unique_bank_ptr<T>::_index > rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator>=(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((lak::unique_bank_ptr<T>::_index == rhs._index) ==
	       (lak::unique_bank_ptr<T>::_value == rhs._value));
	return lak::unique_bank_ptr<T>::_index >= rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator==(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((lak::unique_bank_ptr<T>::_index == rhs._index) ==
	       (lak::unique_bank_ptr<T>::_value == rhs._value));
	return lak::unique_bank_ptr<T>::_index == rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator!=(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((lak::unique_bank_ptr<T>::_index == rhs._index) ==
	       (lak::unique_bank_ptr<T>::_value == rhs._value));
	return lak::unique_bank_ptr<T>::_index != rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator==(lak::nullptr_t) const
{
	return !*this;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator!=(lak::nullptr_t) const
{
	return !!*this;
}
