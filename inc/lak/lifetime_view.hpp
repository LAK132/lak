#ifndef LAK_LIFETIME_VIEW_HPP
#define LAK_LIFETIME_VIEW_HPP

#include "lak/span.hpp"

namespace lak
{
  template<typename T>
  struct lifetime_view
  {
  private:
    lak::span<T> _data  = {};
    size_t _constructed = 0;

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

    lifetime_view() = default;

    lifetime_view(const lifetime_view &) = delete;
    lifetime_view &operator=(const lifetime_view &) = delete;

    lifetime_view(lifetime_view &&other);
    lifetime_view &operator=(lifetime_view &&other);

    ~lifetime_view() { clear(); }

    static lifetime_view make(lak::span<T> uninitialised_data);

    size_t size() const { return _constructed; }
    size_t max_size() const { return _data.size(); }
    void resize(size_t new_size);
    void resize(size_t new_size, const T &t);
    void clear();
    void reset();
    lak::span<T> release();

    [[nodiscard]] bool empty() const { return _constructed == 0; }

    lak::span<T> get() { return _data; }
    lak::span<const T> get() const { return lak::span<const T>(_data); }

    T *data() { return _data.data(); }
    const T *data() const { return _data.data(); }

    T *begin() { return _data.data(); }
    T *end() { return _data.data() + _constructed; }

    const T *begin() const { return _data.data(); }
    const T *end() const { return _data.data() + _constructed; }

    const T *cbegin() const { return _data.data(); }
    const T *cend() const { return _data.data() + _constructed; }

    T &at(size_t index) { return _data[index]; }
    const T &at(size_t index) const { return _data[index]; }

    T &operator[](size_t index) { return _data[index]; }
    const T &operator[](size_t index) const { return _data[index]; }

    T &front() { return _data[0]; }
    const T &front() const { return _data[0]; }

    T &back() { return _data[_constructed - 1]; }
    const T &back() const { return _data[_constructed - 1]; }

    template<typename... ARGS>
    T &emplace_back(ARGS &&... args);

    T &push_back(const T &t);
    T &push_back(T &&t);

    void pop_back();

    T *erase(const T *first, const T *last);

    T *erase(const T *element) { return erase(element, element + 1); }
  };

}

#include "lak/lifetime_view.inl"

#endif