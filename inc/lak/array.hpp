#ifndef LAK_ARRAY_HPP
#define LAK_ARRAY_HPP

#include "lak/stdint.hpp"
#include "lak/type_traits.hpp"
#include "lak/unique_pages.hpp"
#include "lak/utility.hpp"

namespace lak
{
  template<typename T, size_t SIZE = lak::dynamic_extent>
  struct array
  {
  private:
    T _data[SIZE] = {};

  public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = T &;
    using const_reference = const T &;
    using pointer         = T *;
    using const_pointer   = const T *;
    using iterator        = T *;
    using const_iterator  = const T *;

    constexpr array()              = default;
    constexpr array(const array &) = default;
    constexpr array &operator=(const array &) = default;

    constexpr array(array &&other);
    constexpr array &operator=(array &&other);

    array(std::initializer_list<T> list);

    constexpr size_t size() const { return SIZE; }
    constexpr size_t capacity() const { return SIZE; }
    [[nodiscard]] constexpr bool empty() const { return SIZE == 0; }

    constexpr T *data() { return begin(); }
    constexpr const T *data() const { return begin(); }

    constexpr T *begin() { return &_data[0]; }
    constexpr T *end() { return &_data[SIZE]; }

    constexpr const T *begin() const { return &_data[0]; }
    constexpr const T *end() const { return &_data[SIZE]; }

    constexpr const T *cbegin() const { return &_data[0]; }
    constexpr const T *cend() const { return &_data[SIZE]; }

    T &at(size_t index);
    const T &at(size_t index) const;

    constexpr T &operator[](size_t index);
    constexpr const T &operator[](size_t index) const;

    constexpr T &front();
    constexpr const T &front() const;

    constexpr T &back();
    constexpr const T &back() const;
  };

  template<typename T>
  struct array<T, lak::dynamic_extent>
  {
  private:
    lak::unique_pages _data = {};
    size_t _size            = 0; // Ts
    size_t _committed       = 0; // bytes

    void reserve_bytes(size_t new_capacity_bytes);

    void commit(size_t new_size);

  public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = T &;
    using const_reference = const T &;
    using pointer         = T *;
    using const_pointer   = const T *;
    using iterator        = T *;
    using const_iterator  = const T *;

    array() = default;
    array(const array &);
    array &operator=(const array &);

    array(array &&other);
    array &operator=(array &&other);

    array(size_t initial_size);

    array(std::initializer_list<T> list);

    template<typename ITER>
    array(ITER &&begin, ITER &&end);

    ~array();

    size_t size() const { return _size; }
    size_t capacity() const { return _committed / sizeof(T); }
    size_t reserved() const { return _data.size_bytes() / sizeof(T); }

    void resize(size_t new_size);
    void resize(size_t new_size, const T &default_value);
    void reserve(size_t new_capacity);

    void clear();
    void force_clear();

    [[nodiscard]] bool empty() const { return _size == 0; }

    T *data() { return static_cast<T *>(_data.data()); }
    const T *data() const { return static_cast<const T *>(_data.data()); }

    T *begin() { return data(); }
    T *end() { return data() + _size; }

    const T *begin() const { return data(); }
    const T *end() const { return data() + _size; }

    const T *cbegin() const { return data(); }
    const T *cend() const { return data() + _size; }

    T &at(size_t index);
    const T &at(size_t index) const;

    T &operator[](size_t index) { return data()[index]; }
    const T &operator[](size_t index) const { return data()[index]; }

    T &front();
    const T &front() const;

    T &back();
    const T &back() const;

    template<typename... ARGS>
    T &emplace_back(ARGS &&...args);

    T &push_back(const T &t);
    T &push_back(T &&t);

    void pop_back();

    T *erase(const T *first, const T *last);

    T *erase(const T *element) { return erase(element, element + 1); }
  };

  template<typename T>
  using dynamic_array = lak::array<T, lak::dynamic_extent>;
}

template<typename T, size_t S>
bool operator==(const lak::array<T, S> &a, const lak::array<T, S> &b);

template<typename T, size_t S>
bool operator!=(const lak::array<T, S> &a, const lak::array<T, S> &b);

#include "lak/array.inl"

#endif