#include "lak/compiler.hpp"
#include "lak/memmanip.hpp"
#include "lak/span.hpp"

/* --- fixed size --- */

template<typename T, size_t SIZE>
constexpr lak::array<T, SIZE>::array(array<T, SIZE> &&other)
{
  for (size_t i = 0; i < SIZE; ++i) std::swap(_data[i], other._data[i]);
}

template<typename T, size_t SIZE>
constexpr lak::array<T, SIZE> &lak::array<T, SIZE>::operator=(
  array<T, SIZE> &&other)
{
  for (size_t i = 0; i < SIZE; ++i) std::swap(_data[i], other._data[i]);
  return *this;
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

/* --- dynamic size --- */

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
  std::swap(_data, other._data);
  std::swap(_size, other._size);
  std::swap(_committed, other._committed);
  std::swap(_reserved, other._reserved);
  return *this;
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
    size_t new_size_bytes = new_size * sizeof(T);
    if (new_size_bytes > _committed)
    {
      if (new_size_bytes > _reserved) reserve(new_size);
      _committed = new_size_bytes;
      ASSERT(lak::page_commit(lak::span<void>(_data, _committed)));
    }
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
  while (_size-- > 0) _data[_size].~T();
}

template<typename T>
void lak::array<T, lak::dynamic_extent>::reserve(size_t new_capacity)
{
  size_t new_capacity_bytes = new_capacity * sizeof(T);

  if (new_capacity_bytes <= _reserved) return;

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
      new (static_cast<T *>(new_buffer.data()) + i) T(std::move(_data[i]));

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
T &lak::array<T, lak::dynamic_extent>::operator[](size_t index)
{
  ASSERT_GREATER(_size, index);
  return _data[index];
}

template<typename T>
const T &lak::array<T, lak::dynamic_extent>::operator[](size_t index) const
{
  ASSERT_GREATER(_size, index);
  return _data[index];
}
