#include "lak/lifetime_view.hpp"

#include "lak/debug.hpp"
#include "lak/utility.hpp"

template<typename T>
lak::lifetime_view<T>::lifetime_view(lifetime_view &&other)
: _data(lak::exchange(other._data, lak::span<T>{})),
  _constructed(lak::exchange(other._constructed, 0))
{
}

template<typename T>
lak::lifetime_view<T> &lak::lifetime_view<T>::operator=(lifetime_view &&other)
{
  lak::swap(_data, other._data);
  lak::swap(_constructed, other._constructed);
  return *this;
}

template<typename T>
lak::lifetime_view<T> lak::lifetime_view<T>::make(
  lak::span<T> uninitialised_data)
{
  lak::lifetime_view<T> result;
  result._data = uninitialised_data;
  return result;
}

template<typename T>
void lak::lifetime_view<T>::resize(size_t new_size)
{
  ASSERT_LESS_OR_EQUAL(new_size, max_size());
  if (new_size > size())
    for (; size() < new_size; ++_constructed) lak::construct_at(end());
  else
    while (size() > new_size) pop_back();
}

template<typename T>
void lak::lifetime_view<T>::resize(size_t new_size, const T &t)
{
  ASSERT_LESS_OR_EQUAL(new_size, max_size());
  if (new_size > size())
    for (; size() < new_size; ++_constructed) lak::construct_at(end(), t);
  else
    while (size() > new_size) pop_back();
}

template<typename T>
void lak::lifetime_view<T>::clear()
{
  while (_constructed-- > 0) lak::destroy_at(end());
}

template<typename T>
void lak::lifetime_view<T>::reset()
{
  clear();
  _data        = lak::span<T>{};
  _constructed = 0;
}

template<typename T>
lak::span<T> lak::lifetime_view<T>::release()
{
  lak::span<T> result = _data;
  reset();
  return result;
}

template<typename T>
template<typename... ARGS>
T &lak::lifetime_view<T>::emplace_back(ARGS &&... args)
{
  ASSERT_LESS(size(), max_size());
  auto *result = end();
  lak::construct_at(result, lak::forward<ARGS>(args)...);
  ++_constructed;
  return *result;
}

template<typename T>
T &lak::lifetime_view<T>::push_back(const T &t)
{
  ASSERT_LESS(size(), max_size());
  auto *result = end();
  lak::construct_at(result, t);
  ++_constructed;
  return *result;
}

template<typename T>
T &lak::lifetime_view<T>::push_back(T &&t)
{
  ASSERT_LESS(size(), max_size());
  auto *result = end();
  lak::construct_at(result, lak::move(t));
  ++_constructed;
  return *result;
}

template<typename T>
void lak::lifetime_view<T>::pop_back()
{
  ASSERT_GREATER(_constructed, 0U);
  --_constructed;
  lak::destroy_at(end());
}

template<typename T>
T *lak::lifetime_view<T>::erase(const T *first, const T *last)
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
