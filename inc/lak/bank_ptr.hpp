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

    bank_ptr(const bank_ptr &other);

    bank_ptr &operator=(const bank_ptr &);

    ~bank_ptr();

    T *operator->() { return &_container[_index]; }
    const T *operator->() const { return &_container[_index]; }

    T &operator*() { return _container[_index]; }
    const T &operator*() const { return _container[_index]; }
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