#include "lak/utility.hpp"

#ifdef LAK_NO_STD
#	error Requires std::sort
#endif

#include <algorithm>

/* --- lak::bank<T> --- */

template<typename T>
void lak::bank<T>::internal_flush()
{
	if (_deleted.empty()) return;
	std::sort(_deleted.begin(), _deleted.end(), std::greater<size_t>{});
	size_t destroyed = 0;
	while (destroyed < _deleted.size() &&
	       _deleted[destroyed] == _container.size() - 1)
	{
		_container.pop_back();
		++destroyed;
	}
	_deleted.erase(_deleted.begin(), _deleted.begin() + destroyed);
}

template<typename T>
size_t lak::bank<T>::internal_find_index(T *ptr)
{
	if (!ptr) return std::numeric_limits<size_t>::max();

	auto it = std::lower_bound(_container.begin(),
	                           _container.end(),
	                           ptr,
	                           [](const T &a, const T *b) { return &a < b; });
	ASSERT(it != _container.end());
	return std::distance(_container.begin(), it);
}

template<typename T>
template<typename... ARGS>
size_t lak::bank<T>::internal_create(ARGS &&...args)
{
	if (_deleted.size() > 0)
	{
		auto index = _deleted.back();
		_deleted.pop_back();
		_container[index].~T();
		new (&_container[index]) T(lak::forward<ARGS>(args)...);
		return index;
	}
	else
	{
		_container.emplace_back(lak::forward<ARGS>(args)...);
		return _container.size() - 1;
	}
}

template<typename T>
void lak::bank<T>::internal_destroy(size_t index)
{
	ASSERT_GREATER(_container.size(), index);
	_deleted.push_back(index);
	internal_flush();
}

template<typename T>
template<typename FUNCTOR>
size_t lak::bank<T>::internal_find_if(FUNCTOR &&func)
{
	for (size_t del_index = 0, i = 0; i < _container.size(); ++i)
	{
		if (del_index < _deleted.size())
		{
			ASSERT_GREATER_OR_EQUAL(_deleted[del_index], i);
			if (i == _deleted[del_index])
			{
				++del_index;
				continue;
			}
		}

		if (func(_container[i])) return i;
	}

	return std::numeric_limits<size_t>::max();
}

template<typename T>
void lak::bank<T>::flush()
{
	std::lock_guard lock(_mutex);
	internal_flush();
}

template<typename T>
T *lak::bank<T>::create(const T &t)
{
	std::lock_guard lock(_mutex);
	auto result = internal_create(t);
	return result != std::numeric_limits<size_t>::max() ? &_container[result]
	                                                    : nullptr;
}

template<typename T>
T *lak::bank<T>::create(T &&t)
{
	std::lock_guard lock(_mutex);
	auto result = internal_create(lak::move(t));
	return result != std::numeric_limits<size_t>::max() ? &_container[result]
	                                                    : nullptr;
}

template<typename T>
template<typename... ARGS>
T *lak::bank<T>::create(ARGS &&...args)
{
	std::lock_guard lock(_mutex);
	auto result = internal_create(lak::forward<ARGS>(args)...);
	return result != std::numeric_limits<size_t>::max() ? &_container[result]
	                                                    : nullptr;
}

template<typename T>
void lak::bank<T>::destroy(T *t)
{
	std::lock_guard lock(_mutex);
	auto it = std::lower_bound(_container.begin(),
	                           _container.end(),
	                           t,
	                           [](const T &a, const T *b) { return &a < b; });
	ASSERT(it != _container.end());
	internal_destroy(std::distance(_container.begin(), it));
}

template<typename T>
template<typename FUNCTOR>
void lak::bank<T>::for_each(FUNCTOR &&func)
{
	std::lock_guard lock(_mutex);
	for (size_t del_index = 0, i = 0; i < _container.size(); ++i)
	{
		ASSERT_GREATER_OR_EQUAL(_deleted[del_index], i);
		if (i < _deleted[del_index])
		{
			func(_container[i]);
		}
		else
		{
			++del_index;
		}
	}
}

template<typename T>
template<typename FUNCTOR>
T *lak::bank<T>::find_if(FUNCTOR &&func)
{
	std::lock_guard lock(_mutex);
	auto result = internal_find_if(lak::forward<FUNCTOR>(func));
	return result != std::numeric_limits<size_t>::max() ? &_container[result]
	                                                    : nullptr;
}

/* --- lak::unique_bank_ptr<T> --- */

template<typename T>
lak::unique_bank_ptr<T> lak::unique_bank_ptr<T>::create(const T &t)
{
	std::lock_guard lock(bank<T>::_mutex);
	return {bank<T>::internal_create(t)};
}

template<typename T>
lak::unique_bank_ptr<T> lak::unique_bank_ptr<T>::create(T &&t)
{
	std::lock_guard lock(bank<T>::_mutex);
	return {bank<T>::internal_create(lak::move(t))};
}

template<typename T>
template<typename... ARGS>
lak::unique_bank_ptr<T> lak::unique_bank_ptr<T>::create(ARGS &&...args)
{
	std::lock_guard lock(bank<T>::_mutex);
	return {bank<T>::internal_create(lak::forward<ARGS>(args)...)};
}

template<typename T>
lak::unique_bank_ptr<T> lak::unique_bank_ptr<T>::from_raw_bank_ptr(T *ptr)
{
	if (!ptr) return {};
	std::lock_guard lock(bank<T>::_mutex);
	ASSERT_GREATER_OR_EQUAL(ptr, &bank<T>::_container.front());
	ASSERT_GREATER_OR_EQUAL(&bank<T>::_container.back(), ptr);
	return {bank<T>::internal_find_index(ptr)};
}

template<typename T>
lak::unique_bank_ptr<T>::unique_bank_ptr()
: unique_bank_ptr(std::numeric_limits<size_t>::max())
{
}

template<typename T>
lak::unique_bank_ptr<T> &lak::unique_bank_ptr<T>::operator=(std::nullptr_t)
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
lak::unique_bank_ptr<T>::~unique_bank_ptr()
{
	reset();
}

template<typename T>
void lak::unique_bank_ptr<T>::reset()
{
	if (!*this) return;
	std::lock_guard lock(bank<T>::_mutex);
	if (_index == bank<T>::_container.size() - 1)
	{
		bank<T>::_container.pop_back();
	}
	else
	{
		bank<T>::internal_destroy(_index);
	}
	_index = std::numeric_limits<size_t>::max();
	_value = nullptr;
}

template<typename T>
T *lak::unique_bank_ptr<T>::release()
{
	auto result = _value;
	_index      = std::numeric_limits<size_t>::max();
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
bool lak::unique_bank_ptr<T>::operator==(std::nullptr_t) const
{
	return !*this;
}

template<typename T>
bool lak::unique_bank_ptr<T>::operator!=(std::nullptr_t) const
{
	return !!*this;
}

template<typename T>
lak::unique_bank_ptr<T>::operator bool() const
{
	ASSERT((_index == std::numeric_limits<size_t>::max()) ==
	       (_value == nullptr));
	return _index != std::numeric_limits<size_t>::max();
}

/* --- lak::shared_bank_ptr<T> --- */

template<typename T>
void lak::shared_bank_ptr<T>::flush()
{
	std::lock_guard lock(bank<T>::_mutex);
	bank<T>::internal_flush();
	_reference_count.resize(bank<T>::_container.size());
}

template<typename T>
lak::shared_bank_ptr<T> lak::shared_bank_ptr<T>::create(const T &t)
{
	std::lock_guard lock(bank<T>::_mutex);
	auto index = bank<T>::internal_create(t);
	_reference_count.resize(bank<T>::_container.size());
	++_reference_count[index];
	return {index};
}

template<typename T>
lak::shared_bank_ptr<T> lak::shared_bank_ptr<T>::create(T &&t)
{
	std::lock_guard lock(bank<T>::_mutex);
	auto index = bank<T>::internal_create(lak::move(t));
	_reference_count.resize(bank<T>::_container.size());
	++_reference_count[index];
	return {index};
}

template<typename T>
template<typename... ARGS>
lak::shared_bank_ptr<T> lak::shared_bank_ptr<T>::create(ARGS &&...args)
{
	std::lock_guard lock(bank<T>::_mutex);
	auto index = bank<T>::internal_create(lak::forward<ARGS>(args)...);
	_reference_count.resize(bank<T>::_container.size());
	++_reference_count[index];
	return {index};
}

template<typename T>
template<typename FUNCTOR>
lak::shared_bank_ptr<T> lak::shared_bank_ptr<T>::find_if(FUNCTOR &&func)
{
	std::lock_guard lock(bank<T>::_mutex);
	auto result = internal_find_if(lak::forward<FUNCTOR>(func));
	_reference_count.resize(bank<T>::_container.size());
	if (result != std::numeric_limits<size_t>::max()) ++_reference_count[result];
	return {result};
}

template<typename T>
lak::shared_bank_ptr<T> &lak::shared_bank_ptr<T>::operator=(std::nullptr_t)
{
	reset();
	return *this;
}

template<typename T>
lak::shared_bank_ptr<T>::shared_bank_ptr(const shared_bank_ptr &other)
{
	if (other)
	{
		std::lock_guard lock(bank<T>::_mutex);
		unique_bank_ptr<T>::_index = other._index;
		unique_bank_ptr<T>::_value = other._value;
		++_reference_count[unique_bank_ptr<T>::_index];
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
		std::lock_guard lock(bank<T>::_mutex);
		unique_bank_ptr<T>::_index = other._index;
		unique_bank_ptr<T>::_value = other._value;
		++_reference_count[unique_bank_ptr<T>::_index];
	}
	else
		reset();
	return *this;
};

template<typename T>
lak::shared_bank_ptr<T>::shared_bank_ptr(unique_bank_ptr<T> &&other)
: unique_bank_ptr<T>()
{
	if (other)
	{
		std::lock_guard lock(bank<T>::_mutex);
		std::swap(unique_bank_ptr<T>::_index, other._index);
		std::swap(unique_bank_ptr<T>::_value, other._value);
		++_reference_count[unique_bank_ptr<T>::_index];
	}
	else
		reset();
};

template<typename T>
lak::shared_bank_ptr<T> &lak::shared_bank_ptr<T>::operator=(
  unique_bank_ptr<T> &&other)
{
	if (other)
	{
		std::lock_guard lock(bank<T>::_mutex);
		std::swap(unique_bank_ptr<T>::_index, other._index);
		std::swap(unique_bank_ptr<T>::_value, other._value);
		++_reference_count[unique_bank_ptr<T>::_index];
	}
	else
		reset();
	return *this;
};

template<typename T>
lak::shared_bank_ptr<T>::~shared_bank_ptr()
{
	reset();
}

template<typename T>
void lak::shared_bank_ptr<T>::reset()
{
	if (!*this) return;
	std::lock_guard lock(bank<T>::_mutex);
	if (--_reference_count[unique_bank_ptr<T>::_index] == 0)
	{
		if (unique_bank_ptr<T>::_index == bank<T>::_container.size() - 1)
		{
			bank<T>::_container.pop_back();
			_reference_count.pop_back();
		}
		else
		{
			bank<T>::internal_destroy(unique_bank_ptr<T>::_index);
			_reference_count.resize(bank<T>::_container.size());
		}
	}
	unique_bank_ptr<T>::_index = std::numeric_limits<size_t>::max();
	unique_bank_ptr<T>::_value = nullptr;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator<(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((unique_bank_ptr<T>::_index == rhs._index) ==
	       (unique_bank_ptr<T>::_value == rhs._value));
	return unique_bank_ptr<T>::_index < rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator<=(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((unique_bank_ptr<T>::_index == rhs._index) ==
	       (unique_bank_ptr<T>::_value == rhs._value));
	return unique_bank_ptr<T>::_index <= rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator>(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((unique_bank_ptr<T>::_index == rhs._index) ==
	       (unique_bank_ptr<T>::_value == rhs._value));
	return unique_bank_ptr<T>::_index > rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator>=(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((unique_bank_ptr<T>::_index == rhs._index) ==
	       (unique_bank_ptr<T>::_value == rhs._value));
	return unique_bank_ptr<T>::_index >= rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator==(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((unique_bank_ptr<T>::_index == rhs._index) ==
	       (unique_bank_ptr<T>::_value == rhs._value));
	return unique_bank_ptr<T>::_index == rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator!=(
  const lak::shared_bank_ptr<T> &rhs) const
{
	ASSERT((unique_bank_ptr<T>::_index == rhs._index) ==
	       (unique_bank_ptr<T>::_value == rhs._value));
	return unique_bank_ptr<T>::_index != rhs._index;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator==(std::nullptr_t) const
{
	return !*this;
}

template<typename T>
bool lak::shared_bank_ptr<T>::operator!=(std::nullptr_t) const
{
	return !!*this;
}
