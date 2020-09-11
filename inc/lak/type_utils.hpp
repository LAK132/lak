#ifndef LAK_TYPE_UTILS_HPP
#define LAK_TYPE_UTILS_HPP

#include <optional>

namespace lak
{
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
  struct is_function : std::is_function<lak::remove_refs_ptrs_t<T>>
  {
  };
  template<typename T>
  static constexpr bool is_function_v = is_function<T>::value;
}

#endif
