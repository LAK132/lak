#ifndef LAK_SPAN_HPP
#define LAK_SPAN_HPP

#include "lak/char.hpp"
#include "lak/stdint.hpp"
#include "lak/type_utils.hpp"

#include <array>
#include <vector>

namespace lak
{
  template<typename T, size_t SIZE>
  struct array;

  /* --- Compile time sized spans --- */

  template<typename T, size_t SIZE = lak::dynamic_extent>
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

    template<typename U, std::enable_if_t<std::is_same_v<const U, T>, int> = 0>
    inline constexpr span(const span<U, SIZE> &other) noexcept
    : _data(other.data())
    {
    }

    template<
      typename U,
      std::enable_if_t<!std::is_void_v<std::remove_const_t<U>>, int> = 0>
    explicit inline constexpr span(
      const span<U, (SIZE * sizeof(T)) / sizeof(U)> &other) noexcept
    : _data(reinterpret_cast<T *>(other.data()))
    {
    }

    template<typename U,
             std::enable_if_t<std::is_void_v<std::remove_const_t<U>>, int> = 0>
    explicit inline constexpr span(
      const span<U, SIZE * sizeof(T)> &other) noexcept
    : _data(reinterpret_cast<T *>(other.data()))
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

    template<size_t N>
    inline constexpr span(
      const lak::array<std::remove_const_t<T>, SIZE> &array) noexcept
    : _data(array.data())
    {
    }

    template<size_t N>
    inline constexpr span(
      lak::array<std::remove_const_t<T>, SIZE> &array) noexcept
    : _data(array.data())
    {
    }

    template<
      typename U            = T,
      std::enable_if_t<std::is_same_v<U, T> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, wchar_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char8_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char16_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char32_t>,
                       int> = 0>
    inline constexpr span(U (&data)[SIZE]) noexcept : _data(data)
    {
    }

    explicit inline constexpr span(T *data) noexcept : _data(data) {}

    inline constexpr T &operator[](size_t index) const noexcept;

    inline constexpr T *data() const noexcept;

    [[nodiscard]] inline constexpr bool empty() const noexcept;

    inline constexpr size_t size() const noexcept;

    inline constexpr size_t size_bytes() const noexcept;

    inline constexpr T *begin() const noexcept;

    inline constexpr T *end() const noexcept;

    template<size_t offset, size_t count = lak::dynamic_extent>
    inline constexpr span<T, count> subspan() const;

    inline constexpr span<T, lak::dynamic_extent> subspan(
      size_t offset, size_t count = lak::dynamic_extent) const;

    template<size_t count>
    inline constexpr span<T, count> first() const;

    inline constexpr span<T, lak::dynamic_extent> first(size_t count) const;

    template<size_t count>
    inline constexpr span<T, count> last() const;

    inline constexpr span<T, lak::dynamic_extent> last(size_t count) const;

    inline constexpr auto to_string() const;
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

    template<
      typename T,
      std::enable_if_t<!std::is_same_v<lak::remove_cvref_t<T>, char> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, wchar_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char8_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char16_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char32_t>,
                       int> = 0>
    inline constexpr span(T (&data)[SIZE / sizeof(T)]) noexcept : _data(data)
    {
    }

    inline constexpr void *data() const noexcept;

    [[nodiscard]] inline constexpr bool empty() const noexcept;

    inline constexpr size_t size() const noexcept;

    inline constexpr size_t size_bytes() const noexcept;
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
    : _data(other.data())
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

    inline constexpr const void *data() const noexcept;

    [[nodiscard]] inline constexpr bool empty() const noexcept;

    inline constexpr size_t size() const noexcept;

    inline constexpr size_t size_bytes() const noexcept;
  };

  /* --- Runtime sized spans --- */

  template<typename T>
  struct span<T, lak::dynamic_extent>
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

    template<size_t S>
    inline constexpr span(const span<T, S> &other) noexcept
    : _data(other.data()), _size(other.size())
    {
    }

    template<typename U,
             size_t S,
             std::enable_if_t<std::is_same_v<const U, T>, int> = 0>
    inline constexpr span(const span<U, S> &other) noexcept
    : _data(reinterpret_cast<T *>(other.data()))
    {
      if constexpr (std::is_void_v<std::remove_const_t<U>>)
        _size = other.size() / sizeof(T);
      else
        _size = other.size();
    }

    template<typename U,
             size_t S,
             std::enable_if_t<!std::is_same_v<const U, T>, int> = 0>
    explicit inline constexpr span(const span<U, S> &other) noexcept
    : _data(reinterpret_cast<T *>(other.data()))
    {
      if constexpr (std::is_void_v<std::remove_const_t<U>>)
        _size = other.size() / sizeof(T);
      else
        _size = other.size();
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

    template<size_t N>
    inline constexpr span(
      const lak::array<std::remove_const_t<T>, N> &array) noexcept
    : _data(array.data()), _size(array.size())
    {
    }

    template<size_t N>
    inline constexpr span(
      lak::array<std::remove_const_t<T>, N> &array) noexcept
    : _data(array.data()), _size(array.size())
    {
    }

    template<
      typename U = T,
      size_t SIZE,
      std::enable_if_t<std::is_same_v<U, T> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, wchar_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char8_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char16_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char32_t>,
                       int> = 0>
    inline constexpr span(U (&data)[SIZE]) noexcept : _data(data), _size(SIZE)
    {
    }

    inline constexpr span(T *data, size_t size) noexcept
    : _data(data), _size(size)
    {
    }

    explicit inline constexpr span(T *begin, T *end) noexcept;

    inline constexpr T &operator[](size_t index) const noexcept;

    inline constexpr T *data() const noexcept;

    [[nodiscard]] inline constexpr bool empty() const noexcept;

    inline constexpr size_t size() const noexcept;

    inline constexpr size_t size_bytes() const noexcept;

    inline constexpr T *begin() const noexcept;

    inline constexpr T *end() const noexcept;

    template<size_t offset, size_t count = lak::dynamic_extent>
    inline constexpr span<T, count> subspan() const;

    inline constexpr span<T, lak::dynamic_extent> subspan(
      size_t offset, size_t count = lak::dynamic_extent) const;

    template<size_t count>
    inline constexpr span<T, count> first() const;

    inline constexpr span<T, lak::dynamic_extent> first(size_t count) const;

    template<size_t count>
    inline constexpr span<T, count> last() const;

    inline constexpr span<T, lak::dynamic_extent> last(size_t count) const;

    inline constexpr auto to_string() const;
  };

  template<>
  struct span<void, lak::dynamic_extent>
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

    template<
      typename T,
      size_t SIZE,
      std::enable_if_t<!std::is_same_v<lak::remove_cvref_t<T>, char> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, wchar_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char8_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char16_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char32_t>,
                       int> = 0>
    inline constexpr span(T (&data)[SIZE]) noexcept
    : _data(data), _size(SIZE * sizeof(T))
    {
    }

    inline constexpr span(void *data, size_t size) noexcept
    : _data(data), _size(size)
    {
    }

    inline constexpr void *data() const noexcept;

    [[nodiscard]] inline constexpr bool empty() const noexcept;

    inline constexpr size_t size() const noexcept;

    inline constexpr size_t size_bytes() const noexcept;
  };

  template<>
  struct span<const void, lak::dynamic_extent>
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
    : _data(other.data()), _size(other.size())
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

    template<
      typename T,
      size_t SIZE,
      std::enable_if_t<!std::is_same_v<lak::remove_cvref_t<T>, char> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, wchar_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char8_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char16_t> &&
                         !std::is_same_v<lak::remove_cvref_t<T>, char32_t>,
                       int> = 0>
    inline constexpr span(T (&data)[SIZE]) noexcept
    : _data(data), _size(SIZE * sizeof(T))
    {
    }

    inline constexpr span(const void *data, size_t size) noexcept
    : _data(data), _size(size)
    {
    }

    inline constexpr const void *data() const noexcept;

    [[nodiscard]] inline constexpr bool empty() const noexcept;

    inline constexpr size_t size() const noexcept;

    inline constexpr size_t size_bytes() const noexcept;
  };

  template<typename T>
  span(const std::vector<T> &) -> span<const T, lak::dynamic_extent>;

  template<typename T>
  span(std::vector<T> &) -> span<T, lak::dynamic_extent>;

  template<typename T, size_t N>
  span(const std::array<T, N> &) -> span<const T, N>;

  template<typename T, size_t N>
  span(std::array<T, N> &) -> span<T, N>;

  template<typename T, size_t N>
  span(const lak::array<T, N> &) -> span<const T, N>;

  template<typename T, size_t N>
  span(lak::array<T, N> &) -> span<T, N>;

  template<typename T, size_t N>
  span(T (&)[N]) -> span<T, N>;

  template<typename T>
  span(T *, size_t) -> span<T, lak::dynamic_extent>;

  template<typename T>
  span(T *, T *) -> span<T, lak::dynamic_extent>;

  template<typename T>
  struct is_span
  {
    static constexpr bool value = false;
  };

  template<typename T, size_t S>
  struct is_span<lak::span<T, S>>
  {
    static constexpr bool value = true;
  };

  template<typename T>
  inline constexpr bool is_span_v = is_span<T>::value;

  /* --- helper functions --- */

  // Find the subspan of source that is equal to sub.
  template<typename T>
  lak::span<T> find_subspan(lak::span<T> source, lak::span<T> sub);

  template<typename T>
  lak::span<T> common_initial_sequence(lak::span<T> a, lak::span<T> b);

  template<typename T>
  void rotate_left(lak::span<T> data, size_t distance = 1);

  template<typename T>
  void rotate_right(lak::span<T> data, size_t distance = 1);

  template<typename T>
  size_t compare(lak::span<const T> a, lak::span<const T> b);
}

template<typename T>
bool operator==(lak::span<T> a, lak::span<T> b);

template<typename T>
bool operator!=(lak::span<T> a, lak::span<T> b);

#include "lak/span.inl"

#endif