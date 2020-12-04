#ifndef LAK_TYPE_TRAITS
#define LAK_TYPE_TRAITS

#include "lak/type_utils.hpp"
#include "lak/utility.hpp"

namespace lak
{
  /* --- is_constructible --- */

  template<typename...>
  using void_t = void;

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