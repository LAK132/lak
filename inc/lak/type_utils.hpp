#ifndef LAK_TYPE_UTILS_HPP
#define LAK_TYPE_UTILS_HPP

#include <optional>

namespace lak
{
  /* --- true/false_type --- */

  struct true_type
  {
    static constexpr bool value = true;
  };
  struct false_type
  {
    static constexpr bool value = false;
  };

  /* --- bool_type --- */

  template<bool B>
  struct bool_type;
  template<>
  struct bool_type<true> : public true_type
  {
  };
  template<>
  struct bool_type<false> : public false_type
  {
  };

  /* --- type_identity --- */

  template<typename T>
  struct type_identity
  {
    using type = T;
  };

  /* --- remove_const --- */

  template<typename T>
  struct remove_const : public type_identity<T>
  {
  };
  template<typename T>
  struct remove_const<const T> : public type_identity<T>
  {
  };
  template<typename T>
  using remove_const_t = typename remove_const<T>::type;

  /* --- remove_volatile --- */

  template<typename T>
  struct remove_volatile : public type_identity<T>
  {
  };
  template<typename T>
  struct remove_volatile<volatile T> : public type_identity<T>
  {
  };
  template<typename T>
  using remove_volatile_t = typename remove_volatile<T>::type;

  /* --- remove_cv --- */

  template<typename T>
  using remove_cv_t = remove_const_t<remove_volatile_t<T>>;

  /* --- remove_reference --- */

  template<typename T>
  struct remove_reference : public type_identity<T>
  {
  };
  template<typename T>
  struct remove_reference<T &> : public type_identity<T>
  {
  };
  template<typename T>
  struct remove_reference<T &&> : public type_identity<T>
  {
  };
  template<typename T>
  using remove_reference_t = typename remove_reference<T>::type;

  /* --- remove_cvref --- */

  template<typename T>
  using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;

  /* --- remove_refs_ptrs --- */

  template<typename T>
  struct remove_refs_ptrs
  {
    using type = T;
  };
  template<typename T>
  struct remove_refs_ptrs<T *>
  {
    using type = typename remove_refs_ptrs<T>::type;
  };
  template<typename T>
  struct remove_refs_ptrs<T &>
  {
    using type = typename remove_refs_ptrs<T>::type;
  };
  template<typename T>
  struct remove_refs_ptrs<T &&>
  {
    using type = typename remove_refs_ptrs<T>::type;
  };
  template<typename T>
  using remove_refs_ptrs_t = typename remove_refs_ptrs<T>::type;

  /* --- is_function --- */

  template<typename T>
  struct is_function : public std::is_function<lak::remove_refs_ptrs_t<T>>
  {
  };
  template<typename T>
  static constexpr bool is_function_v = is_function<T>::value;

  /* --- conditional --- */

  template<bool B, typename T, typename U>
  struct conditional;
  template<typename T, typename U>
  struct conditional<true, T, U> : lak::type_identity<T>
  {
  };
  template<typename T, typename U>
  struct conditional<false, T, U> : lak::type_identity<U>
  {
  };
  template<bool B, typename T, typename U>
  using conditional_t = typename conditional<B, T, U>::type;

  /* --- enable_if --- */

  template<bool B, typename T = void>
  struct enable_if
  {
  };
  template<typename T>
  struct enable_if<true, T>
  {
    using type = T;
  };
  template<bool B, typename T = void>
  using enable_if_t = typename enable_if<B, T>::type;
  template<bool B>
  using enable_if_i = typename enable_if<B, int>::type;

  /* --- is_same --- */

  template<typename T, typename U>
  struct is_same : public false_type
  {
  };
  template<typename T>
  struct is_same<T, T> : public true_type
  {
  };
  template<typename T, typename U>
  static constexpr bool is_same_v = is_same<T, U>::value;
}

#endif
