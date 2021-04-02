#include "lak/algorithm.hpp"
#include "lak/compiler.hpp"
#include "lak/memmanip.hpp"
#include "lak/span.hpp"

/* --- fixed size --- */

template<typename T, size_t SIZE>
constexpr lak::packed_array<T, SIZE>::packed_array(
  packed_array<T, SIZE> &&other)
{
  lak::swap(_value, other._value);
  lak::swap(_next, other._next);
}

template<typename T, size_t SIZE>
constexpr lak::packed_array<T, SIZE> &lak::packed_array<T, SIZE>::operator=(
  packed_array<T, SIZE> &&other)
{
  lak::swap(_value, other._value);
  lak::swap(_next, other._next);
  return *this;
}

template<typename T, size_t SIZE>
lak::packed_array<T, SIZE>::packed_array(std::initializer_list<T> list)
{
  ASSERT_EQUAL(list.size(), SIZE);
  for (size_t i = 0; i < list.size() && i < SIZE; ++i)
    operator[](i) = list.begin()[i];
}

template<typename T>
constexpr lak::packed_array<T, 1>::packed_array(packed_array<T, 1> &&other)
{
  lak::swap(_value, other._value);
}

template<typename T>
constexpr lak::packed_array<T, 1> &lak::packed_array<T, 1>::operator=(
  packed_array<T, 1> &&other)
{
  lak::swap(_value, other._value);
  return *this;
}

template<typename T>
lak::packed_array<T, 1>::packed_array(std::initializer_list<T> list)
{
  ASSERT_EQUAL(list.size(), 1);
  _value = *list.begin();
}
