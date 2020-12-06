#include "lak/variant.hpp"

#include "lak/visit.hpp"

template<typename T, typename... U>
template<size_t I>
auto &lak::pack_union<T, U...>::get()
{
  if constexpr (I == 0)
    return value;
  else
    return next.template get<I - 1>();
}

template<typename T, typename... U>
template<size_t I>
const auto &lak::pack_union<T, U...>::get() const
{
  if constexpr (I == 0)
    return value;
  else
    return next.template get<I - 1>();
}

template<typename T, typename... U>
template<size_t I, typename... ARGS>
void lak::pack_union<T, U...>::emplace(ARGS &&... args)
{
  if constexpr (I == 0)
    new (&value) T(lak::forward<ARGS>(args)...);
  else
    next.template emplace<I - 1>(lak::forward<ARGS>(args)...);
}

template<typename T, typename... U>
template<typename... ARGS>
bool lak::pack_union<T, U...>::emplace_dynamic(size_t i, ARGS &&... args)
{
  if (i == 0)
  {
    new (&value) T(lak::forward<ARGS>(args)...);
    return true;
  }
  else
  {
    return next.emplace_dynamic(i, lak::forward<ARGS>(args)...);
  }
}

template<typename T, typename... U>
template<size_t I>
void lak::pack_union<T, U...>::reset()
{
  if constexpr (I == 0)
    value.~T();
  else
    next.template reset<I - 1>();
}

template<typename T, typename... U>
bool lak::pack_union<T, U...>::reset_dynamic(size_t i)
{
  if (i == 0)
  {
    value.~T();
    return true;
  }
  else
  {
    return next.reset_dynamic(i - 1);
  }
}

template<typename T>
lak::pack_union<T>::pack_union(lak::uninitialised_union_flag_t) : _()
{
  _.~uninitialised_union_flag_t();
}

template<typename T>
template<size_t I>
auto &lak::pack_union<T>::get()
{
  static_assert(I == 0);
  return value;
}

template<typename T>
template<size_t I>
const auto &lak::pack_union<T>::get() const
{
  static_assert(I == 0);
  return value;
}

template<typename T>
template<size_t I, typename... ARGS>
void lak::pack_union<T>::emplace(ARGS &&... args)
{
  static_assert(I == 0);
  new (&value) T(lak::forward<ARGS>(args)...);
}

template<typename T>
template<typename... ARGS>
bool lak::pack_union<T>::emplace_dynamic(size_t i, ARGS &&... args)
{
  if (i == 0)
  {
    new (&value) T(lak::forward<ARGS>(args)...);
    return true;
  }
  else
  {
    return false;
  }
}

template<typename T>
template<size_t I>
void lak::pack_union<T>::reset()
{
  static_assert(I == 0);
  value.~T();
}

template<typename T>
bool lak::pack_union<T>::reset_dynamic(size_t i)
{
  if (i == 0)
  {
    value.~T();
    return true;
  }
  else
  {
    return false;
  }
}

template<typename... T>
lak::variant<T...>::variant(const variant &other)
: _index(other._index), _value(lak::uninitialised_union_flag)
{
  ASSERT(lak::visit_switch(
    lak::make_index_sequence<_size>{}, _index, [&, this](auto index) {
      using I = lak::remove_cvref_t<decltype(index)>;
      if constexpr (_is_ref<I::value>)
        _value.template emplace<I::value>(other.template get<I::value>());
      else
        _value.template emplace<I::value>(*other.template get<I::value>());
    }));
}

template<typename... T>
lak::variant<T...>::variant(variant &&other)
: _index(other._index), _value(lak::uninitialised_union_flag)
{
  _index = other._index;
  ASSERT(lak::visit_switch(
    lak::make_index_sequence<_size>{}, _index, [&, this](auto index) {
      using I = lak::remove_cvref_t<decltype(index)>;
      if constexpr (_is_ref<I::value>)
        _value.template emplace<I::value>(other.template get<I::value>());
      else
        _value.template emplace<I::value>(
          lak::forward<value_type<I::value>>(*other.template get<I::value>()));
    }));
}

template<typename... T>
lak::variant<T...> &lak::variant<T...>::operator=(const variant &other)
{
  ASSERT(lak::visit_switch(
    lak::make_index_sequence<_size>{}, other._index, [&, this](auto index) {
      using I = lak::remove_cvref_t<decltype(index)>;
      emplace<I::value>(*other.template get<I::value>());
    }));
  return *this;
}

template<typename... T>
lak::variant<T...> &lak::variant<T...>::operator=(variant &&other)
{
  ASSERT(lak::visit_switch(
    lak::make_index_sequence<_size>{}, other._index, [&, this](auto index) {
      using I = lak::remove_cvref_t<decltype(index)>;
      emplace<I::value>(
        lak::forward<value_type<I::value>>(*other.template get<I::value>()));
    }));
  return *this;
}

template<typename... T>
template<size_t I, typename... ARGS>
auto &lak::variant<T...>::emplace(ARGS &&... args)
{
  ASSERT(_value.reset_dynamic(_index));
  _index = I;
  _value.template emplace<I>(lak::forward<ARGS>(args)...);
  return _value.template get<I>();
}

template<typename... T>
template<size_t I, typename... ARGS>
lak::variant<T...> lak::variant<T...>::make(ARGS &&... args)
{
  static_assert(I < _size);
  return variant(lak::in_place_index<I>, lak::forward<ARGS>(args)...);
}

template<typename... T>
lak::variant<T...>::~variant()
{
  ASSERT(lak::visit_switch(
    lak::make_index_sequence<_size>{}, _index, [this](auto index) {
      using I = lak::remove_cvref_t<decltype(index)>;
      _value.template reset<I::value>();
    }));
}

template<typename... T>
template<size_t I>
auto *lak::variant<T...>::get()
{
  // we store references as pointers.
  if constexpr (_is_ref<I>)
    return I == _index ? _value.template get<I>() : nullptr;
  else
    return I == _index ? &_value.template get<I>() : nullptr;
}

template<typename... T>
template<size_t I>
const auto *lak::variant<T...>::get() const
{
  // we store references as pointers.
  if constexpr (_is_ref<I>)
    return I == _index ? _value.template get<I>() : nullptr;
  else
    return I == _index ? &_value.template get<I>() : nullptr;
}

// template<typename FUNCTOR>
// auto visit(FUNCTOR&& functor)
// {
//   return lak::visit_switch()
// }

template<typename... T>
struct lak::is_variant<lak::variant<T...>> : public lak::true_type
{
};
