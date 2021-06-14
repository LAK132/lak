#include "lak/algorithm.hpp"
#include "lak/compiler.hpp"
#include "lak/memmanip.hpp"
#include "lak/span.hpp"

/* --- fixed size --- */

template<typename T, size_t SIZE>
constexpr lak::array<T, SIZE>::array(array<T, SIZE> &&other)
{
  lak::swap(_data, other._data);
}

template<typename T, size_t SIZE>
constexpr lak::array<T, SIZE> &lak::array<T, SIZE>::operator=(
  array<T, SIZE> &&other)
{
  lak::swap(_data, other._data);
  return *this;
}

template<typename T, size_t SIZE>
lak::array<T, SIZE>::array(std::initializer_list<T> list)
{
  ASSERT_EQUAL(list.size(), SIZE);
  lak::copy(list.begin(), list.end(), static_cast<T *>(_data));
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
void lak::array<T, lak::dynamic_extent>::commit(size_t new_size)
{
  size_t new_size_bytes = new_size * sizeof(T);
  if (new_size_bytes > _committed)
  {
    if (new_size_bytes > _reserved) reserve(new_size);
    _committed = new_size_bytes;
    ASSERT(lak::page_commit(lak::span<void>(_data, _committed)));
  }
  if (new_size != 0) ASSERT(_data != nullptr);
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
lak::array<T, lak::dynamic_extent>
  &lak::array<T, lak::dynamic_extent>::operator=(
    const array<T, lak::dynamic_extent> &other)
{
  clear();
  reserve(other.size());
  for (const auto &e : other) push_back(e);
  return *this;
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(
  array<T, lak::dynamic_extent> &&other)
: _data(other._data),
  _size(other._size),
  _committed(other._committed),
  _reserved(other._reserved)
{
  other._data      = nullptr;
  other._size      = 0;
  other._committed = 0;
  other._reserved  = 0;
}

template<typename T>
lak::array<T, lak::dynamic_extent>
  &lak::array<T, lak::dynamic_extent>::operator=(
    array<T, lak::dynamic_extent> &&other)
{
  lak::swap(_data, other._data);
  lak::swap(_size, other._size);
  lak::swap(_committed, other._committed);
  lak::swap(_reserved, other._reserved);
  return *this;
}

template<typename T>
lak::array<T, lak::dynamic_extent>::array(std::initializer_list<T> list)
{
  commit(list.size());
  ASSERT_LESS_OR_EQUAL(list.size(), _committed);
  for (; _size < list.size(); ++_size)
    new (_data + _size) T(list.begin()[_size]);
}

template<typename T>
template<typename ITER>
lak::array<T, lak::dynamic_extent>::array(ITER &&begin, ITER &&end)
{
  const size_t sz = end - begin;
  commit(sz);
  ASSERT_LESS_OR_EQUAL(sz, _committed);
  for (; _size < sz; ++_size, ++begin) new (_data + _size) T(*begin);
}

template<typename T>
lak::array<T, lak::dynamic_extent>::~array()
{
  force_clear();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::resize(size_t new_size)
{
  if (new_size > _size)
  {
    commit(new_size);
    for (; _size < new_size; ++_size) new (_data + _size) T();
  }
  else if (new_size < _size)
  {
    for (; _size > new_size; --_size) _data[_size - 1].~T();
  }
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::clear()
{
  for (auto &e : *this) e.~T();
  _size = 0;
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::reserve(size_t new_capacity)
{
  size_t new_capacity_bytes = new_capacity * sizeof(T);

  if (new_capacity_bytes <= _reserved)
  {
    if (new_capacity_bytes != 0) ASSERT(_data != nullptr);
    return;
  }

  if (new_capacity_bytes < _reserved * 2)
  {
    new_capacity       = (_reserved * 2) / sizeof(T);
    new_capacity_bytes = new_capacity * sizeof(T);
  }

  lak::span<void> new_buffer = lak::page_reserve(new_capacity_bytes);

  if (_size > 0)
  {
    _committed = _size * sizeof(T);
    ASSERT(lak::page_commit(lak::span<void>(new_buffer.data(), _committed)));

    // move all the Ts from the old memory block to the new memory block
    for (size_t i = 0; i < _size; ++i)
      new (static_cast<T *>(new_buffer.data()) + i) T(lak::move(_data[i]));

    // destroy the Ts in the old memory block and free memory
    for (size_t i = 0; i < _size; ++i) _data[i].~T();
    ASSERT(lak::page_free(lak::span<void>(_data, _reserved)));
  }
  else
  {
    _committed = 0;
  }

  _data     = static_cast<T *>(new_buffer.data());
  _reserved = new_buffer.size();
  ASSERT(_data);
  ASSERT_GREATER_OR_EQUAL(_reserved, _committed);
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::force_clear()
{
  if (_data)
  {
    clear();
    lak::page_free(lak::span<void>(_data, _reserved));
    _committed = 0;
    _reserved  = 0;
    _data      = nullptr;
  }
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::at(size_t index)
{
  ASSERT_GREATER(_size, index);
  return _data[index];
}

template<typename T>
const T &lak::array<T, lak::dynamic_extent>::at(size_t index) const
{
  ASSERT_GREATER(_size, index);
  return _data[index];
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::operator[](size_t index)
{
  return _data[index];
}

template<typename T>
const T &lak::array<T, lak::dynamic_extent>::operator[](size_t index) const
{
  return _data[index];
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::front()
{
  ASSERT_GREATER(_size, 0U);
  return _data[0];
}

template<typename T>
const T &lak::array<T, lak::dynamic_extent>::front() const
{
  ASSERT_GREATER(_size, 0U);
  return _data[0];
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::back()
{
  ASSERT_GREATER(_size, 0U);
  return _data[_size - 1];
}

template<typename T>
const T &lak::array<T, lak::dynamic_extent>::back() const
{
  ASSERT_GREATER(_size, 0U);
  return _data[_size - 1];
}

template<typename T>
template<typename... ARGS>
T &lak::array<T, lak::dynamic_extent>::emplace_back(ARGS &&... args)
{
  commit(_size + 1);
  new (_data + _size) T(lak::forward<ARGS>(args)...);
  ++_size;
  return back();
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::push_back(const T &t)
{
  commit(_size + 1);
  new (_data + _size) T(t);
  ++_size;
  return back();
}

template<typename T>
T &lak::array<T, lak::dynamic_extent>::push_back(T &&t)
{
  commit(_size + 1);
  new (_data + _size) T(lak::move(t));
  ++_size;
  return back();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::pop_back()
{
  ASSERT_GREATER(_size, 0U);
  _data[--_size].~T();
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
    if (a[i] != b[i]) return false;
  return true;
}

template<typename T, size_t S>
bool operator!=(const lak::array<T, S> &a, const lak::array<T, S> &b)
{
  return !(a == b);
}
