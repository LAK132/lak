#ifndef LAK_ARRAY_HPP
#define LAK_ARRAY_HPP

#include "lak/compiler.hpp"
#include "lak/stdint.hpp"
#include "lak/type_utils.hpp"
#include "lak/utility.hpp"

namespace lak
{
  template<typename T, size_t SIZE = lak::dynamic_extent>
  packed_struct array
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

    constexpr array(array && other);
    constexpr array &operator=(array &&other);

    array(std::initializer_list<T> list);

    constexpr size_t size() const { return SIZE; }
    constexpr size_t capacity() const { return SIZE; }
    [[nodiscard]] constexpr bool empty() const { return SIZE == 0; }

    T *data() { return _data; }
    const T *data() const { return _data; }

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
    T *_data          = nullptr;
    size_t _size      = 0; // Ts
    size_t _committed = 0; // bytes
    size_t _reserved  = 0; // bytes

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

    array()              = default;
    array(const array &) = default;
    array &operator=(const array &) = default;

    array(array &&other);
    array &operator=(array &&other);

    array(std::initializer_list<T> list);

    template<typename ITER>
    array(ITER &&begin, ITER &&end);

    ~array();

    size_t size() const { return _size; }
    void resize(size_t new_size);
    void clear();

    size_t capacity() const { return _committed / sizeof(T); }
    size_t reserved() const { return _reserved / sizeof(T); }
    void reserve(size_t new_capacity);
    void force_clear();

    [[nodiscard]] bool empty() const { return _size == 0; }

    T *data() { return _data; }
    const T *data() const { return _data; }

    T *begin() { return _data; }
    T *end() { return _data + _size; }

    const T *begin() const { return _data; }
    const T *end() const { return _data + _size; }

    const T *cbegin() const { return _data; }
    const T *cend() const { return _data + _size; }

    T &at(size_t index);
    const T &at(size_t index) const;

    T &operator[](size_t index);
    const T &operator[](size_t index) const;

    T &front();
    const T &front() const;

    T &back();
    const T &back() const;

    template<typename... ARGS>
    T &emplace_back(ARGS &&... args);

    T &push_back(const T &t);
    T &push_back(T &&t);

    void pop_back();

    T *erase(T *element);
  };
}

#include "lak/array.inl"

#endif