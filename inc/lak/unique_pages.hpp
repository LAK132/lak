#ifndef LAK_UNIQUE_PAGES_HPP
#define LAK_UNIQUE_PAGES_HPP

#include "lak/lifetime_view.hpp"

namespace lak
{
  template<typename T>
  struct unique_pages
  {
  private:
    lak::lifetime_view<T> _data;

    unique_pages(lak::span<void> data);

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

    unique_pages() = default;

    unique_pages(unique_pages &&other);

    unique_pages &operator=(unique_pages &&other);

    ~unique_pages();

    static unique_pages make(size_t min_size = 1);

    size_t size() const { return _data.size(); }
    size_t max_size() const { return _data.max_size(); }
    void resize(size_t new_size) { _data.resize(new_size); }
    void resize(size_t new_size, const T &t) { _data.resize(new_size, t); }
    void clear() { _data.clear(); }

    [[nodiscard]] bool empty() const { return _data.empty(); }

    lak::span<T> get() { return _data.get(); }
    lak::span<const T> get() const { return _data.get(); }

    T *data() { return _data.data(); }
    const T *data() const { return _data.data(); }

    T *begin() { return _data.begin(); }
    T *end() { return _data.end(); }

    const T *begin() const { return _data.data(); }
    const T *end() const { return _data.end(); }

    const T *cbegin() const { return _data.cbegin(); }
    const T *cend() const { return _data.cend(); }

    T &at(size_t index) { return _data.at(index); }
    const T &at(size_t index) const { return _data.at(index); }

    T &operator[](size_t index) { return _data[index]; }
    const T &operator[](size_t index) const { return _data[index]; }

    T &front() { return _data.front(); }
    const T &front() const { return _data.front(); }

    T &back() { return _data.back(); }
    const T &back() const { return _data.back(); }

    template<typename... ARGS>
    T &emplace_back(ARGS &&... args);

    template<typename... ARGS>
    T &push_back(const T &t);

    template<typename... ARGS>
    T &push_back(T &&t);

    void pop_back();

    T *erase(const T *first, const T *last);

    T *erase(const T *element);
  };

#if 0
  struct unique_pages : private lak::span<void>
  {
  protected:
    unique_pages(lak::span<void> pages) : lak::span<void>(pages) {}

    void clear()
    {
      if (!empty()) ASSERT(lak::page_free(*this));
      static_cast<lak::span<void> &>(*this) = {};
    }

  public:
    static unique_pages make(size_t min_size)
    {
      ASSERT_GREATER(min_size, 0);
      unique_pages pages(lak::page_reserve(min_size));
      if (pages.size() < min_size || !lak::page_commit(pages))
        throw std::bad_alloc();
      return pages;
    }

    ~unique_pages() { clear(); }

    unique_pages() = default;

    unique_pages(unique_pages &&other)
    : unique_pages(static_cast<lak::span<void> &>(other))
    {
      other.clear();
    }

    unique_pages &operator=(unique_pages &&other)
    {
      lak::swap<lak::span<void>>(*this, other);
      return *this;
    }

    // -Werror=class-conversion bug
    // operator lak::span<void>()
    // {
    //   return static_cast<lak::span<void> &>(*this);
    // }

    using lak::span<void>::data;
    using lak::span<void>::empty;
    using lak::span<void>::size;
    using lak::span<void>::size_bytes;
  };

  struct shared_pages : private lak::span<void>
  {
  private:
    lak::reference_count *_count = nullptr;

    shared_pages(lak::span<void> pages)
    {
      ASSERT_GREATER(pages.size(), sizeof(lak::reference_count));
      auto &span = static_cast<lak::span<void> &>(*this);

      span =
        lak::span<void>(static_cast<lak::reference_count *>(pages.data()) + 1,
                        pages.size_bytes() - sizeof(lak::reference_count));

      _count = static_cast<lak::reference_count *>(pages.data());
      new (_count) lak::reference_count();

      ++*_count;
    }

    void clear()
    {
      if (!_count) return;

      static_cast<lak::span<void> &>(*this) = {};

      if (auto &count = *lak::exchange(_count, nullptr); --count)
        ASSERT(lak::page_free(*this));
    }

  public:
    static shared_pages make(size_t min_size)
    {
      ASSERT_GREATER(min_size, 0);
      min_size += sizeof(lak::reference_count);
      auto pages = lak::page_reserve(min_size);
      if (!pages.empty() && pages.size() < min_size)
      {
        ASSERT(lak::page_free(pages));
        pages = {};
      }
      if (pages.empty() || !lak::page_commit(pages)) throw std::bad_alloc();
      return shared_pages(pages);
    }

    ~shared_pages() { clear(); }

    shared_pages() = default;

    shared_pages(const shared_pages &other)
    : lak::span<void>(static_cast<lak::span<void>>(other))
    {
      _count = other._count;
      if (_count) ++*_count;
    }

    shared_pages(shared_pages &&other)
    : lak::span<void>(static_cast<lak::span<void>>(other))
    {
      _count = lak::exchange(other._count, nullptr);
      other.clear();
    }

    shared_pages &operator=(const shared_pages &other)
    {
      static_cast<lak::span<void> &>(*this) = other;

      _count = other._count;
      if (_count) ++*_count;

      return *this;
    }

    shared_pages &operator=(shared_pages &&other)
    {
      lak::swap<lak::span<void>>(*this, other);
      lak::swap(_count, other._count);

      return *this;
    }

    // -Werror=class-conversion bug
    // operator lak::span<void>()
    // {
    //   return static_cast<lak::span<void> &>(*this);
    // }

    using lak::span<void>::data;
    using lak::span<void>::empty;
    using lak::span<void>::size;
    using lak::span<void>::size_bytes;
  };
#endif
}

#include "lak/unique_pages.inl"

#endif
