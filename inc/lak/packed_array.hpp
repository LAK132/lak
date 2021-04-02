#ifndef LAK_PACKED_ARRAY_HPP
#define LAK_PACKED_ARRAY_HPP

#include "lak/stdint.hpp"
#include "lak/type_traits.hpp"
#include "lak/utility.hpp"

namespace lak
{
  template<typename T, size_t SIZE>
  packed_struct packed_array
  {
    static_assert(SIZE >= 1);

  private:
    T _value;
    packed_array<T, (SIZE - 1)> _next;

  public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = T &;
    using const_reference = const T &;

    constexpr packed_array()                     = default;
    constexpr packed_array(const packed_array &) = default;
    constexpr packed_array &operator=(const packed_array &) = default;

    constexpr packed_array(packed_array && other);
    constexpr packed_array &operator=(packed_array &&other);

    packed_array(std::initializer_list<T> list);

    constexpr size_t size() const { return SIZE; }
    constexpr size_t capacity() const { return SIZE; }
    [[nodiscard]] constexpr bool empty() const { return false; }

    constexpr T &operator[](size_t index)
    {
      return index == SIZE ? _value : _next[index];
    }
    constexpr const T &operator[](size_t index) const
    {
      return index == SIZE ? _value : _next[index];
    }
  };

  template<typename T>
  packed_struct packed_array<T, 1>
  {
  private:
    T _value;

  public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = T &;
    using const_reference = const T &;

    constexpr packed_array()                     = default;
    constexpr packed_array(const packed_array &) = default;
    constexpr packed_array &operator=(const packed_array &) = default;

    constexpr packed_array(packed_array && other);
    constexpr packed_array &operator=(packed_array &&other);

    packed_array(std::initializer_list<T> list);

    constexpr size_t size() const { return 1; }
    constexpr size_t capacity() const { return 1; }
    [[nodiscard]] constexpr bool empty() const { return false; }

    constexpr T &operator[](size_t index) { return _value; }
    constexpr const T &operator[](size_t index) const { return _value; }
  };
}

#include "lak/packed_array.inl"

#endif