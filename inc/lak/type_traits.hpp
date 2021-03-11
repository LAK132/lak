#ifndef LAK_TYPE_TRAITS
#define LAK_TYPE_TRAITS

#include "lak/type_utils.hpp"
#include "lak/utility.hpp"

namespace lak
{
  template<typename...>
  using void_t = void;

  struct nonesuch
  {
    ~nonesuch()                = delete;
    nonesuch(const nonesuch &) = delete;
    void operator=(const nonesuch &) = delete;
  };

  /* --- detected_or --- */

  template<typename DEFAULT,
           typename V,
           template<typename...>
           typename OP,
           typename... ARGS>
  struct _detected
  {
    using value_t = lak::false_type;
    using type    = DEFAULT;
  };

  template<typename DEFAULT,
           template<typename...>
           typename OP,
           typename... ARGS>
  struct _detected<DEFAULT, void_t<OP<ARGS...>>, OP, ARGS...>
  {
    using value_t = lak::true_type;
    using type    = OP<ARGS...>;
  };

  template<typename DEFAULT,
           template<typename...>
           typename OP,
           typename... ARGS>
  using detected_or = lak::_detected<DEFAULT, void, OP, ARGS...>;

  template<typename DEFAULT,
           template<typename...>
           typename OP,
           typename... ARGS>
  using detected_or_t = typename lak::detected_or<DEFAULT, OP, ARGS...>::type;

  template<template<typename...> typename OP, typename... ARGS>
  using detected_t =
    typename lak::detected_or<lak::nonesuch, OP, ARGS...>::type;

  /* --- is_detected --- */

  template<template<typename...> typename OP, typename... ARGS>
  using is_detected =
    typename lak::detected_or<lak::nonesuch, OP, ARGS...>::value_t;

  template<template<typename...> typename OP, typename... ARGS>
  constexpr inline bool is_detected_v = lak::is_detected<OP, ARGS...>::value;

  /* --- is_detected_exact --- */

  template<typename EXPECTED,
           template<typename...>
           typename OP,
           typename... ARGS>
  using is_detected_exact =
    lak::is_same<EXPECTED, lak::detected_t<OP, ARGS...>>;

  template<typename EXPECTED,
           template<typename...>
           typename OP,
           typename... ARGS>
  constexpr inline bool is_detected_exact_v =
    lak::is_deteccted_exact<EXPECTED, OP, ARGS...>::value;

  /* --- is_constructible --- */

  template<typename, typename T, typename... ARGS>
  struct is_constructible_ : lak::false_type
  {
  };

  template<typename T, typename... ARGS>
  struct is_constructible_<void_t<decltype(T(lak::declval<ARGS>()...))>,
                           T,
                           ARGS...> : std::true_type
  {
  };

  template<typename T, typename... ARGS>
  using is_constructible = is_constructible_<void_t<>, T, ARGS...>;

  template<typename T, typename... ARGS>
  static constexpr bool is_constructible_v =
    is_constructible<T, ARGS...>::value;

  template<typename T>
  struct is_default_constructible : public is_constructible<T>
  {
  };

  template<typename T>
  static constexpr bool is_default_constructible_v =
    is_default_constructible<T>::value;
}

#endif