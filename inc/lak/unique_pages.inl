#include "lak/unique_pages.hpp"

#include "lak/debug.hpp"

template<typename T>
lak::unique_pages<T>::unique_pages(lak::span<void> data)
: _data(lak::lifetime_view<T>::make(lak::span<T>(data)))
{
}

template<typename T>
lak::unique_pages<T>::unique_pages(unique_pages &&other)
: _data(lak::exchange(other._data, lak::lifetime_view<T>{}))
{
}

template<typename T>
lak::unique_pages<T> &lak::unique_pages<T>::operator=(unique_pages &&other)
{
  lak::swap(_data, other._data);
  return *this;
}

template<typename T>
lak::unique_pages<T>::~unique_pages()
{
  if (auto span = _data.release(); span.data()) lak::page_free(span);
}

template<typename T>
lak::unique_pages<T> lak::unique_pages<T>::make(size_t min_size)
{
  lak::span<void> new_page = lak::page_reserve(min_size * sizeof(T));
  ASSERT_GREATER(new_page.size(), 0);
  ASSERT(lak::page_commit(new_page));
  return lak::unique_pages<T>(new_page);
}

template<typename T>
template<typename... ARGS>
T &lak::unique_pages<T>::emplace_back(ARGS &&... args)
{
  return _data.emplace_back(lak::forward<ARGS>(args)...);
}

template<typename T>
template<typename... ARGS>
T &lak::unique_pages<T>::push_back(const T &t)
{
  return _data.push_back(t);
}

template<typename T>
template<typename... ARGS>
T &lak::unique_pages<T>::push_back(T &&t)
{
  return _data.push_back(lak::move(t));
}

template<typename T>
void lak::unique_pages<T>::pop_back()
{
  _data.pop_back();
}

template<typename T>
T *lak::unique_pages<T>::erase(const T *first, const T *last)
{
  return _data.erase(first, last);
}

template<typename T>
T *lak::unique_pages<T>::erase(const T *element)
{
  return _data.erase(element);
}