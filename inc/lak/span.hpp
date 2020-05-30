#ifndef LAK_SPAN_HPP
#define LAK_SPAN_HPP

#include "lak/debug.hpp"
#include "lak/stdint.hpp"

#include <array>
#include <vector>

namespace lak
{
  /* --- Compile time sized spans --- */

  template<typename T, size_t SIZE = dynamic_extent>
  struct span
  {
  private:
    T *_data = nullptr;

    template<typename U, size_t S>
    friend struct span;

  public:
    constexpr span()             = default;
    constexpr span(const span &) = default;
    constexpr span &operator=(const span &) = default;

    template<typename U>
    inline constexpr span(
      const span<U, (SIZE * sizeof(T)) / sizeof(U)> &other) noexcept
    : _data(static_cast<T>(other._data))
    {
    }

    template<size_t N>
    inline constexpr span(
      const std::array<std::remove_const_t<T>, SIZE> &array) noexcept
    : _data(array.data())
    {
    }
    template<size_t N>
    inline constexpr span(
      std::array<std::remove_const_t<T>, SIZE> &array) noexcept
    : _data(array.data())
    {
    }

    inline constexpr span(T (&data)[SIZE]) noexcept : _data(data) {}
    explicit inline constexpr span(T *data) noexcept : _data(data) {}

    inline constexpr T &operator[](size_t index) const noexcept
    {
      ASSERT(index < SIZE);
      return _data[index];
    }

    inline constexpr T *data() const noexcept { return _data; }
    inline constexpr size_t size() const noexcept { return SIZE; }
    inline constexpr size_t size_bytes() const noexcept
    {
      return SIZE * sizeof(T);
    }
    inline constexpr T *begin() const noexcept { return _data; }
    inline constexpr T *end() const noexcept { return _data + SIZE; }

    template<size_t offset, size_t count = lak::dynamic_extent>
    inline constexpr span<T, count> subspan() const
    {
      if constexpr (count == lak::dynamic_extent)
      {
        ASSERT(offset + count <= size());
        return {begin() + offset, count};
      }
      else
      {
        static_assert(offset + count <= SIZE, "subspan too large");
        return {begin() + offset};
      }
    }
    inline constexpr span<T, lak::dynamic_extent> subspan(
      size_t offset, size_t count = lak::dynamic_extent) const
    {
      if (count == lak::dynamic_extent)
      {
        ASSERT(offset <= size());
        return {begin() + offset, size() - offset};
      }
      else
      {
        ASSERT(offset + count <= size());
        return {begin() + offset, count};
      }
    }

    template<size_t count>
    inline constexpr span<T, count> first() const
    {
      static_assert(count <= SIZE, "subspan too large");
      return {begin()};
    }
    inline constexpr span<T, lak::dynamic_extent> first(size_t count) const
    {
      ASSERT(count <= size());
      return {begin(), count};
    }

    template<size_t count>
    inline constexpr span<T, count> last() const
    {
      static_assert(count <= SIZE, "subspan too large");
      return {begin() + (size() - count)};
    }
    inline constexpr span<T, lak::dynamic_extent> last(size_t count) const
    {
      ASSERT(count <= size());
      return {begin() + (size() - count), count};
    }
  };

  template<size_t SIZE>
  struct span<void, SIZE>
  {
  private:
    void *_data = nullptr;

    template<typename U, size_t S>
    friend struct span;

  public:
    constexpr span()             = default;
    constexpr span(const span &) = default;
    constexpr span &operator=(const span &) = default;

    template<typename T, std::enable_if_t<!std::is_void_v<T>, int> = 0>
    inline constexpr span(const span<T, SIZE / sizeof(T)> &other) noexcept
    : _data(other.data())
    {
    }

    template<typename T>
    inline constexpr span(std::array<T, SIZE / sizeof(T)> &array) noexcept
    : _data(array.data())
    {
    }

    template<typename T>
    inline constexpr span(T (&data)[SIZE / sizeof(T)]) noexcept : _data(data)
    {
    }
    explicit inline constexpr span(void *data) noexcept : _data(data) {}

    inline constexpr void *data() const noexcept { return _data; }
    inline constexpr size_t size() const noexcept { return SIZE; }
    inline constexpr size_t size_bytes() const noexcept { return SIZE; }
  };

  template<size_t SIZE>
  struct span<const void, SIZE>
  {
  private:
    const void *_data = nullptr;

    template<typename U, size_t S>
    friend struct span;

  public:
    constexpr span()             = default;
    constexpr span(const span &) = default;
    constexpr span &operator=(const span &) = default;

    inline constexpr span(const span<void, SIZE> &other) noexcept
    : _data(other._data)
    {
    }

    template<typename T, std::enable_if_t<!std::is_void_v<T>, int> = 0>
    inline constexpr span(const span<T, SIZE / sizeof(T)> &other) noexcept
    : _data(other.data())
    {
    }

    template<typename T>
    inline constexpr span(std::array<T, SIZE / sizeof(T)> &array) noexcept
    : _data(array.data())
    {
    }
    template<typename T>
    inline constexpr span(
      const std::array<T, SIZE / sizeof(T)> &array) noexcept
    : _data(array.data())
    {
    }

    template<typename T>
    inline constexpr span(T (&data)[SIZE / sizeof(T)]) noexcept : _data(data)
    {
    }
    explicit inline constexpr span(const void *data) noexcept : _data(data) {}

    inline constexpr const void *data() const noexcept { return _data; }
    inline constexpr size_t size() const noexcept { return SIZE; }
    inline constexpr size_t size_bytes() const noexcept { return SIZE; }
  };

  /* --- Runtime sized spans --- */

  template<typename T>
  struct span<T, dynamic_extent>
  {
  private:
    T *_data     = nullptr;
    size_t _size = 0;

    template<typename U, size_t S>
    friend struct span;

  public:
    constexpr span()             = default;
    constexpr span(const span &) = default;
    constexpr span &operator=(const span &) = default;

    template<typename U, size_t S>
    inline constexpr span(const span<U, S> &other) noexcept
    : _data(static_cast<T>(other._data)), _size(other.size())
    {
    }

    inline constexpr span(
      const std::vector<std::remove_const_t<T>> &vector) noexcept
    : _data(vector.data()), _size(vector.size())
    {
    }
    inline constexpr span(std::vector<std::remove_const_t<T>> &vector) noexcept
    : _data(vector.data()), _size(vector.size())
    {
    }

    template<size_t N>
    inline constexpr span(
      const std::array<std::remove_const_t<T>, N> &array) noexcept
    : _data(array.data()), _size(N)
    {
    }
    template<size_t N>
    inline constexpr span(
      std::array<std::remove_const_t<T>, N> &array) noexcept
    : _data(array.data()), _size(N)
    {
    }

    inline constexpr span(
      std::initializer_list<std::remove_const_t<T>> list) noexcept
    : _data(list.begin()), _size(list.size())
    {
    }

    template<size_t SIZE>
    inline constexpr span(T (&data)[SIZE]) noexcept : _data(data), _size(SIZE)
    {
    }
    inline constexpr span(T *data, size_t size) noexcept
    : _data(data), _size(size)
    {
    }

    inline constexpr T &operator[](size_t index) const noexcept
    {
      ASSERT(index < _size);
      return _data[index];
    }

    inline constexpr T *data() const noexcept { return _data; }
    inline constexpr size_t size() const noexcept { return _size; }
    inline constexpr size_t size_bytes() const noexcept
    {
      return _size * sizeof(T);
    }
    inline constexpr T *begin() const noexcept { return _data; }
    inline constexpr T *end() const noexcept { return _data + _size; }

    template<size_t offset, size_t count = lak::dynamic_extent>
    inline constexpr span<T, count> subspan() const
    {
      ASSERT(offset + count <= size());
      if constexpr (count == lak::dynamic_extent)
      {
        return {begin() + offset, count};
      }
      else
      {
        return {begin() + offset};
      }
    }
    inline constexpr span<T, lak::dynamic_extent> subspan(
      size_t offset, size_t count = lak::dynamic_extent) const
    {
      if (count == lak::dynamic_extent)
      {
        ASSERT(offset <= size());
        return {begin() + offset, size() - offset};
      }
      else
      {
        ASSERT(offset + count <= size());
        return {begin() + offset, count};
      }
    }

    template<size_t count>
    inline constexpr span<T, count> first() const
    {
      ASSERT(count <= size());
      return {begin()};
    }
    inline constexpr span<T, lak::dynamic_extent> first(size_t count) const
    {
      ASSERT(count <= size());
      return {begin(), count};
    }

    template<size_t count>
    inline constexpr span<T, count> last() const
    {
      ASSERT(count <= size());
      return {begin() + (size() - count)};
    }
    inline constexpr span<T, lak::dynamic_extent> last(size_t count) const
    {
      ASSERT(count <= size());
      return {begin() + (size() - count), count};
    }
  };

  template<>
  struct span<void, dynamic_extent>
  {
  private:
    void *_data  = nullptr;
    size_t _size = 0;

    template<typename U, size_t S>
    friend struct span;

  public:
    constexpr span()             = default;
    constexpr span(const span &) = default;
    constexpr span &operator=(const span &) = default;

    template<typename T, std::enable_if_t<!std::is_void_v<T>, int> = 0>
    inline constexpr span(const span<T> &other) noexcept
    : _data(other.data()), _size(sizeof(T) * other.size())
    {
    }

    template<typename T>
    inline constexpr span(std::vector<T> &vector) noexcept
    : _data(vector.data()), _size(sizeof(T) * vector.size())
    {
    }

    template<typename T, size_t N>
    inline constexpr span(std::array<T, N> &array) noexcept
    : _data(array.data()), _size(sizeof(T) * N)
    {
    }

    template<typename T, size_t SIZE>
    inline constexpr span(T (&data)[SIZE]) noexcept
    : _data(data), _size(SIZE * sizeof(T))
    {
    }
    inline constexpr span(void *data, size_t size) noexcept
    : _data(data), _size(size)
    {
    }

    inline constexpr void *data() const noexcept { return _data; }
    inline constexpr size_t size() const noexcept { return _size; }
    inline constexpr size_t size_bytes() const noexcept { return _size; }
  };

  template<>
  struct span<const void, dynamic_extent>
  {
  private:
    const void *_data = nullptr;
    size_t _size      = 0;

    template<typename U, size_t S>
    friend struct span;

  public:
    constexpr span()             = default;
    constexpr span(const span &) = default;
    constexpr span &operator=(const span &) = default;

    inline constexpr span(const span<void> &other) noexcept
    : _data(other._data), _size(other._size)
    {
    }

    template<typename T, std::enable_if_t<!std::is_void_v<T>, int> = 0>
    inline constexpr span(const span<T> &other) noexcept
    : _data(other.data()), _size(sizeof(T) * other.size())
    {
    }

    template<typename T>
    inline constexpr span(std::vector<T> &vector) noexcept
    : _data(vector.data()), _size(sizeof(T) * vector.size())
    {
    }
    template<typename T>
    inline constexpr span(const std::vector<T> &vector) noexcept
    : _data(vector.data()), _size(sizeof(T) * vector.size())
    {
    }

    template<typename T, size_t N>
    inline constexpr span(std::array<T, N> &array) noexcept
    : _data(array.data()), _size(sizeof(T) * N)
    {
    }
    template<typename T, size_t N>
    inline constexpr span(const std::array<T, N> &array) noexcept
    : _data(array.data()), _size(sizeof(T) * N)
    {
    }

    template<typename T>
    inline constexpr span(std::initializer_list<T> list) noexcept
    : _data(list.begin()), _size(sizeof(T) * list.size())
    {
    }

    template<typename T, size_t SIZE>
    inline constexpr span(T (&data)[SIZE]) noexcept
    : _data(data), _size(SIZE * sizeof(T))
    {
    }
    inline constexpr span(const void *data, size_t size) noexcept
    : _data(data), _size(size)
    {
    }

    inline constexpr const void *data() const noexcept { return _data; }
    inline constexpr size_t size() const noexcept { return _size; }
    inline constexpr size_t size_bytes() const noexcept { return _size; }
  };

  template<typename T>
  span(const std::vector<T> &)
    -> span<const std::remove_const_t<T>, dynamic_extent>;

  template<typename T>
  span(std::vector<T> &) -> span<T, dynamic_extent>;

  template<typename T, size_t N>
  span(const std::array<T, N> &) -> span<const std::remove_const_t<T>, N>;

  template<typename T, size_t N>
  span(std::array<T, N> &) -> span<T, N>;

  template<typename T, size_t N>
  span(T (&)[N]) -> span<T, N>;

  template<typename T>
  span(T *, size_t) -> span<T, dynamic_extent>;
}

#endif