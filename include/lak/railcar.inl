#include "lak/railcar.hpp"

#include "lak/debug.hpp"

/* --- railcar_iterator --- */

template<typename T>
lak::railcar_iterator<T>::railcar_iterator(container_type *container,
                                           size_t index)
: _container(container), _index(index)
{
}

template<typename T>
lak::railcar_iterator<T>::railcar_iterator(const railcar_iterator &other)
: _container(other._container), _index(other._index)
{
}

template<typename T>
lak::railcar_iterator<T> &lak::railcar_iterator<T>::operator=(
  const railcar_iterator &other)
{
	_container = other._container;
	_index     = other._index;
	return *this;
}

template<typename T>
lak::railcar_iterator<T> &lak::railcar_iterator<T>::operator=(
  railcar_iterator &&other)
{
	lak::swap(_container, other._container);
	lak::swap(_index, other._index);
	return *this;
}

template<typename T>
typename lak::railcar_iterator<T>::reference
lak::railcar_iterator<T>::operator*() const
{
	return _container->at(_index);
}

template<typename T>
typename lak::railcar_iterator<T>::pointer
lak::railcar_iterator<T>::operator->() const
{
	return &_container->at(_index);
}

template<typename T>
typename lak::railcar_iterator<T>::reference
lak::railcar_iterator<T>::operator[](difference_type index)
{
	return _container->at(_index + index);
}

template<typename T>
lak::railcar_iterator<T> &lak::railcar_iterator<T>::operator++()
{
	++_index;
	return *this;
}

template<typename T>
lak::railcar_iterator<T> lak::railcar_iterator<T>::operator++(int)
{
	return railcar_iterator{_container, _index++};
}

template<typename T>
lak::railcar_iterator<T> &lak::railcar_iterator<T>::operator+=(size_t n)
{
	_index += n;
	return *this;
}

template<typename T>
lak::railcar_iterator<T> lak::railcar_iterator<T>::operator+(size_t n)
{
	return railcar_iterator{_container, _index + n};
}

template<typename T>
lak::railcar_iterator<T> &lak::railcar_iterator<T>::operator--()
{
	--_index;
	return *this;
}

template<typename T>
lak::railcar_iterator<T> lak::railcar_iterator<T>::operator--(int)
{
	return railcar_iterator{_container, _index--};
}

template<typename T>
lak::railcar_iterator<T> &lak::railcar_iterator<T>::operator-=(size_t n)
{
	_index -= n;
	return *this;
}

template<typename T>
lak::railcar_iterator<T> lak::railcar_iterator<T>::operator-(size_t n)
{
	return railcar_iterator{_container, _index - n};
}

template<typename T>
typename lak::railcar_iterator<T>::difference_type
lak::railcar_iterator<T>::operator-(const railcar_iterator &other)
{
	return _index - other._index;
}

template<typename T>
bool lak::railcar_iterator<T>::operator==(const railcar_iterator &other) const
{
	ASSERT_EQUAL(_container, other._container);
	return _index == other._index;
}

template<typename T>
bool lak::railcar_iterator<T>::operator!=(const railcar_iterator &other) const
{
	ASSERT_EQUAL(_container, other._container);
	return _index != other._index;
}

template<typename T>
bool lak::railcar_iterator<T>::operator>(const railcar_iterator &other) const
{
	ASSERT_EQUAL(_container, other._container);
	return _index > other._index;
}

template<typename T>
bool lak::railcar_iterator<T>::operator<(const railcar_iterator &other) const
{
	ASSERT_EQUAL(_container, other._container);
	return _index < other._index;
}

template<typename T>
bool lak::railcar_iterator<T>::operator>=(const railcar_iterator &other) const
{
	ASSERT_EQUAL(_container, other._container);
	return _index >= other._index;
}

template<typename T>
bool lak::railcar_iterator<T>::operator<=(const railcar_iterator &other) const
{
	ASSERT_EQUAL(_container, other._container);
	return _index <= other._index;
}

/* --- railcar --- */

template<typename T>
void lak::railcar<T>::internal_init_bin_size()
{
	if (_bin_size == 0)
		_bin_size = std::max<size_t>(lak::page_size() / sizeof(T), size_t(1));
	ASSERT_GREATER(_bin_size, 1U);
}

template<typename T>
void lak::railcar<T>::internal_alloc_end()
{
	if (_data.empty() || _data.back().size() == _bin_size)
	{
		internal_init_bin_size();
		lak::array<T> new_bin;
		new_bin.reserve(_bin_size);
		_data.emplace_back(lak::move(new_bin));
	}
	ASSERT_NOT_EQUAL(_data.back().size(), _bin_size);
}

template<typename T>
lak::railcar<T>::railcar(const railcar &other)
requires lak::concepts::copy_constructible<T>
: _data(other._data)
{
}

template<typename T>
lak::railcar<T> &lak::railcar<T>::operator=(const railcar &other)
requires lak::concepts::copy_constructible<T>
{
	_data = other._data;
	return *this;
}

template<typename T>
lak::railcar<T>::railcar(railcar &&other)
: _data(lak::exchange(other._data, lak::array<lak::array<T>>{})),
  _bin_size(other._bin_size)
{
}

template<typename T>
lak::railcar<T> &lak::railcar<T>::operator=(railcar &&other)
{
	lak::swap(_data, other._data);
	lak::swap(_bin_size, other._bin_size);
	return *this;
}

template<typename T>
lak::railcar<T>::railcar(std::initializer_list<T> list)
requires lak::concepts::copy_constructible<T>
: railcar(list.begin(), list.end())
{
}

template<typename T>
template<typename ITER>
requires lak::concepts::copy_constructible<T>
lak::railcar<T>::railcar(ITER &&begin, ITER &&end)
{
	for (; begin != end; ++begin) push_back(*begin);
}

template<typename T>
size_t lak::railcar<T>::size() const
{
	size_t result = _data.size() * _bin_size;
	if (!_data.empty()) result -= (_bin_size - _data.back().size());
	return result;
}

template<typename T>
void lak::railcar<T>::resize(size_t new_size)
{
	if (size_t current_size = size(); new_size > current_size)
	{
		reserve(new_size);
		for (size_t i = new_size - current_size; i-- > 0;) emplace_back();
	}
	else if (new_size < current_size)
	{
		for (size_t i = current_size - new_size; i-- > 0;) pop_back();
	}
}

template<typename T>
void lak::railcar<T>::clear()
{
	resize(0);
}

template<typename T>
void lak::railcar<T>::reserve(size_t new_capacity)
{
	internal_init_bin_size();
	_data.reserve(new_capacity / _bin_size);
}

template<typename T>
void lak::railcar<T>::force_clear()
{
	_data.force_clear();
	_bin_size = 0;
}

template<typename T>
typename lak::railcar<T>::iterator lak::railcar<T>::begin()
{
	return iterator(this, 0);
}

template<typename T>
typename lak::railcar<T>::iterator lak::railcar<T>::end()
{
	return iterator(this, size());
}

template<typename T>
typename lak::railcar<T>::const_iterator lak::railcar<T>::cbegin() const
{
	return const_iterator(this, 0);
}

template<typename T>
typename lak::railcar<T>::const_iterator lak::railcar<T>::cend() const
{
	return const_iterator(this, size());
}

template<typename T>
T &lak::railcar<T>::at(size_t index)
{
	return _data[index / _bin_size][index % _bin_size];
}

template<typename T>
const T &lak::railcar<T>::at(size_t index) const
{
	return _data[index / _bin_size][index % _bin_size];
}

template<typename T>
T &lak::railcar<T>::operator[](size_t index)
{
	return _data[index / _bin_size][index % _bin_size];
}

template<typename T>
const T &lak::railcar<T>::operator[](size_t index) const
{
	return _data[index / _bin_size][index % _bin_size];
}

template<typename T>
T &lak::railcar<T>::front()
{
	return _data.front().front();
}

template<typename T>
const T &lak::railcar<T>::front() const
{
	return _data.front().front();
}

template<typename T>
T &lak::railcar<T>::back()
{
	return _data.back().back();
}

template<typename T>
const T &lak::railcar<T>::back() const
{
	return _data.back().back();
}

template<typename T>
template<typename... ARGS>
T &lak::railcar<T>::emplace_back(ARGS &&...args)
{
	internal_alloc_end();

	return _data.back().emplace_back(lak::forward<ARGS>(args)...);
}

template<typename T>
T &lak::railcar<T>::push_back(const T &t)
requires lak::concepts::copy_constructible<T>
{
	internal_alloc_end();

	return _data.back().push_back(t);
}

template<typename T>
T &lak::railcar<T>::push_back(T &&t)
{
	internal_alloc_end();

	return _data.back().push_back(lak::move(t));
}

template<typename T>
void lak::railcar<T>::pop_back()
{
	_data.back().pop_back();
	if (_data.back().empty()) _data.pop_back();
}

template<typename T>
T *lak::railcar<T>::erase(const T *element)
{
	auto contains = [](lak::span<const T> span, const T *elem) -> bool
	{ return elem >= span.begin() && elem < span.end(); };

	size_t page = 0;

	for (; page < _data.size() && !contains(lak::span(_data[page]), element);
	     ++page)
		;

	ASSERT(contains(lak::span(_data[page]), element));

	T *result = _data[page].erase(element);

	for (; page + 1 < _data.size(); ++page)
	{
		// Move the first element of the next page to the end of this page.
		auto &next_page = _data[page + 1];
		_data[page].emplace_back(lak::move(next_page.front()));
		next_page.erase(next_page.begin());
	}

	// If the last page only had 1 element in it it will now be empty, remove the
	// page.
	if (_data.back().empty()) _data.pop_back();

	return result;
}