#ifndef LAK_OPTIONAL_HPP
#define LAK_OPTIONAL_HPP

#include "lak/type_pack.hpp"
#include "lak/type_utils.hpp"
#include "lak/utility.hpp"
#include "lak/variant.hpp"
#include "lak/visit.hpp"

namespace lak
{
  struct nullopt_t
  {
  };
  static constexpr lak::nullopt_t nullopt;

  /* --- optional<T> --- */

  template<typename T>
  struct optional
  {
    using value_type = T;

  private:
    lak::variant<lak::monostate, value_type> _value;

  public:
    optional() : _value() {}
    optional(const optional &) = default;
    optional(optional &&)      = default;

    optional(lak::nullopt_t) {}
    template<typename U>
    optional(U &&other)
    : _value(lak::in_place_index<1>, lak::forward<U>(other))
    {
    }
    template<typename... ARGS>
    optional(lak::in_place_t, ARGS &&... args)
    : _value(lak::in_place_index<1>, lak::forward<ARGS>(args)...)
    {
    }

    optional &operator=(const optional &) = default;
    optional &operator=(optional &&) = default;

    optional &operator=(lak::nullopt_t)
    {
      reset();
      return *this;
    }
    template<typename U>
    optional &operator=(U &&other)
    {
      emplace(lak::forward<U>(other));
      return *this;
    }

    constexpr inline bool has_value() const { return _valie.index() == 1; }

    constexpr inline operator bool() const { return _has_value; }

    template<typename U>
    value_type &emplace(U &&other)
    {
      return _value.emplace<1>(lak::forward<U>(other));
    }

    void reset() { _value.emplace<0>(); }

    T *get() { return _value.get<1>(); }
    const T *get() const { return _value.get<1>(); }

    T &operator*() { return *get(); }
    const T &operator*() const { return *get(); }

    T *operator->() { return get(); }
    const T *operator->() const { return get(); }
  };

  /* --- optional<T&> --- */

  template<typename T>
  struct optional<T &>
  {
    using value_type = T;

  private:
    T *_value;

  public:
    optional() : _value(nullptr) {}
    optional(const optional &) = default;
    optional(optional &&)      = default;

    optional(lak::nullopt_t) : _value(nullptr) {}
    optional(T &other) : _value(&other) {}
    template<typename ARGS>
    optional(lak::in_place_t, T &other) : _value(other)
    {
    }

    optional &operator=(const optional &) = default;
    optional &operator=(optional &&) = default;

    optional &operator=(lak::nullopt_t)
    {
      reset();
      return *this;
    }
    optional &operator=(T &other)
    {
      emplace(other);
      return *this;
    }

    constexpr inline bool has_value() const { return _value; }

    constexpr inline operator bool() const { return _value; }

    value_type &emplace(T &other)
    {
      _value = &other;
      return *_value;
    }

    void reset() { _value = nullptr; }

    T *get() { return _value; }
    const T *get() const { return _value; }

    T &operator*() { return *get(); }
    const T &operator*() const { return *get(); }

    T *operator->() { return get(); }
    const T *operator->() const { return get(); }
  };

  /* --- as_ptr --- */

  template<typename T>
  force_inline lak::remove_reference_t<T> *as_ptr(std::optional<T &> p)
  {
    return lak::as_ptr(p.operator->());
  }

  template<typename T>
  force_inline lak::remove_reference_t<T> *as_ptr(lak::optional<T> &p)
  {
    return lak::as_ptr(p.operator->());
  }

  template<typename T>
  force_inline const lak::remove_reference_t<T> *as_ptr(
    const lak::optional<T> &p)
  {
    return lak::as_ptr(p.operator->());
  }
}

#endif
