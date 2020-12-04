#ifndef LAK_VISIT_HPP
#define LAK_VISIT_HPP

#include "lak/compiler.hpp"
#include "lak/stdint.hpp"
#include "lak/type_utils.hpp"

namespace lak
{
  template<typename T, T... I>
  struct integer_sequence
  {
  };

  template<size_t... I>
  using index_sequence = integer_sequence<size_t, I...>;

  template<typename T, T I, T... Is>
  struct _make_integer_sequence
  {
    struct cont
    {
      template<typename U, U J, U... Js>
      using type =
        typename lak::_make_integer_sequence<U, J - 1, J - 1, Js...>::type;
    };

    struct done
    {
      template<typename U, U J, U... Js>
      using type = lak::integer_sequence<U, Js...>;
    };

    template<typename U, U... Js>
    using next = typename lak::conditional_t<I == 0, done, cont>::
      template type<U, I, Js...>;

    using type = next<T, Is...>;
  };

  template<typename T, T I>
  using make_integer_sequence =
    typename lak::_make_integer_sequence<T, I>::type;

  template<size_t I>
  using make_index_sequence =
    typename lak::_make_integer_sequence<size_t, I>::type;

  static_assert(
    lak::is_same_v<lak::make_index_sequence<0>, lak::index_sequence<>>);
  static_assert(
    lak::is_same_v<lak::make_index_sequence<1>, lak::index_sequence<0>>);
  static_assert(
    lak::is_same_v<lak::make_index_sequence<10>,
                   lak::index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>>);

  template<typename... T>
  using index_sequence_for = lak::make_index_sequence<sizeof...(T)>;

  /*

  Example:

  template<size_t S>
  void some_function();

  ASSERT(lak::visit_switch(lak::index_sequence<0, 1, 2>{}, 1, [](auto index) {
    using I = lak::remove_cvref_t<decltype(index)>;
    static_assert(lak::is_integral_constant<I>);
    some_function<I::value>();
  }));

  */
  template<typename FUNCTOR, typename T, T I, T... J>
  force_inline bool visit_switch(lak::integer_sequence<T, I, J...>,
                                 T i,
                                 FUNCTOR &&functor)
  {
    if (I == i)
    {
      functor(lak::integral_constant<T, I>{});
      return true;
    }
    else if constexpr (sizeof...(J) > 0)
    {
      return visit_switch(
        lak::integer_sequence<T, J...>{}, i, lak::forward<FUNCTOR>(functor));
    }
    else
    {
      return false;
    }
  }
}

#endif