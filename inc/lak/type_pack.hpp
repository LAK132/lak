#ifndef LAK_TYPE_PACK_HPP
#define LAK_TYPE_PACK_HPP

#include "lak/optional.hpp"
#include "lak/type_utils.hpp"

namespace lak
{
  /* --- type_pack --- */

  template<typename... TYPES>
  struct type_pack
  {
  };

  /* --- prepend_to_pack --- */

  template<typename T, typename PACK>
  struct prepend_to_pack;
  template<typename T, typename... TYPES>
  struct prepend_to_pack<T, type_pack<TYPES...>>
  {
    using type = type_pack<T, TYPES...>;
  };
  template<typename T, typename PACK>
  using prepend_to_pack_t = typename prepend_to_pack<T, PACK>::type;

  /* --- append_to_pack --- */

  template<typename PACK, typename T>
  struct append_to_pack;
  template<typename... TYPES, typename T>
  struct append_to_pack<type_pack<TYPES...>, T>
  {
    using type = type_pack<TYPES..., T>;
  };
  template<typename PACK, typename T>
  using append_to_pack_t = typename append_to_pack<PACK, T>::type;

  /* --- concat_packs --- */

  template<typename... PACKS>
  struct concat_packs;
  template<>
  struct concat_packs<>
  {
    using type = type_pack<>;
  };
  template<typename... PACK>
  struct concat_packs<type_pack<PACK...>>
  {
    using type = type_pack<PACK...>;
  };
  template<typename... PACK1, typename... PACK2>
  struct concat_packs<type_pack<PACK1...>, type_pack<PACK2...>>
  {
    using type = type_pack<PACK1..., PACK2...>;
  };
  template<typename... PACK1, typename... PACK2, typename... PACKS>
  struct concat_packs<type_pack<PACK1...>, type_pack<PACK2...>, PACKS...>
  {
    using type =
      typename concat_packs<type_pack<PACK1..., PACK2...>, PACKS...>::type;
  };
  template<typename... PACKS>
  using concat_packs_t = typename concat_packs<PACKS...>::type;

  /* --- remove_from_pack --- */

  template<typename T, typename PACK>
  struct remove_from_pack;
  template<typename T>
  struct remove_from_pack<T, type_pack<>>
  {
    using type = type_pack<>;
  };
  template<typename T, typename T1, typename... TYPES>
  struct remove_from_pack<T, type_pack<T1, TYPES...>>
  {
    using next = typename remove_from_pack<T, type_pack<TYPES...>>::type;
    using type = std::
      conditional_t<std::is_same_v<T, T1>, next, prepend_to_pack_t<T1, next>>;
  };
  template<typename T, typename PACK>
  using remove_from_pack_t = typename remove_from_pack<T, PACK>::type;

  /* --- unique_pack --- */

  template<typename PACK>
  struct unique_pack;
  template<>
  struct unique_pack<type_pack<>>
  {
    using type = type_pack<>;
  };
  template<typename T>
  struct unique_pack<type_pack<T>>
  {
    using type = type_pack<T>;
  };
  template<typename T1, typename... TYPES>
  struct unique_pack<type_pack<T1, TYPES...>>
  {
    using type = prepend_to_pack_t<
      T1,
      remove_from_pack_t<T1, typename unique_pack<type_pack<TYPES...>>::type>>;
  };
  template<typename PACK>
  using unique_pack_t = typename unique_pack<PACK>::type;

  /* --- pack_from_function --- */

  template<typename FUNC>
  struct pack_from_function;
  template<typename FUNC>
  struct pack_from_function<FUNC *>
  {
    using type = typename pack_from_function<FUNC>::type;
  };
  template<typename RET, typename... ARGS>
  struct pack_from_function<RET(ARGS...)>
  {
    using type = type_pack<RET, ARGS...>;
  };
  template<typename FUNC>
  using pack_from_function_t = typename pack_from_function<FUNC>::type;

  /* --- create_from_pack --- */

  template<template<typename...> typename T, typename PACK>
  struct create_from_pack;
  template<template<typename...> typename T, typename... TYPES>
  struct create_from_pack<T, type_pack<TYPES...>>
  {
    using type = T<TYPES...>;
  };
  template<template<typename...> typename T, typename PACK>
  using create_from_pack_t = typename create_from_pack<T, PACK>::type;
}

#endif
