#ifndef LAK_BANK_PTR_HPP
#define LAK_BANK_PTR_HPP

#include <deque>
#include <mutex>
#include <vector>

namespace lak
{
  template<typename T>
  struct bank_ptr
  {
  private:
    static std::mutex _mutex;
    static std::deque<T> _container;
    static std::deque<size_t> _reference_count;
    static std::vector<size_t> _deleted;

    size_t _index;

    bank_ptr(size_t index) : _index(index){};

    static void internal_flush();

    template<typename... ARGS>
    static [[nodiscard]] bank_ptr internal_create(ARGS &&... args);

  public:
    static void flush();

    static [[nodiscard]] bank_ptr create(const T &t);

    static [[nodiscard]] bank_ptr create(T &&t);

    template<typename... ARGS>
    static [[nodiscard]] bank_ptr create(ARGS &&... args);

    template<typename FUNCTOR>
    static void for_each(FUNCTOR &&func);

    template<typename FUNCTOR>
    static bank_ptr find_if(FUNCTOR &&func);

    bank_ptr();

    bank_ptr(std::nullptr_t);
    bank_ptr &operator=(std::nullptr_t);

    bank_ptr(const bank_ptr &other);
    bank_ptr &operator=(const bank_ptr &other);

    ~bank_ptr();

    void reset();

    T *operator->();
    const T *operator->() const;

    T &operator*();
    const T &operator*() const;

    bool operator==(const bank_ptr &rhs) const;
    bool operator!=(const bank_ptr &rhs) const;

    bool operator==(std::nullptr_t) const;
    bool operator!=(std::nullptr_t) const;

    operator bool() const;
  };

  template<typename T>
  std::mutex bank_ptr<T>::_mutex;
  template<typename T>
  std::deque<T> bank_ptr<T>::_container;
  template<typename T>
  std::deque<size_t> bank_ptr<T>::_reference_count;
  template<typename T>
  std::vector<size_t> bank_ptr<T>::_deleted;
}

#include "bank_ptr.inl"

#endif