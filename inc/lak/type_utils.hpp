#ifndef LAK_TYPE_UTILS_HPP
#define LAK_TYPE_UTILS_HPP

#include <optional>

#define TEMPLATE_VALUE_TYPE(NAME, TEMPL_TYPE, VAL_TYPE)                       \
  template<TEMPL_TYPE T>                                                      \
  struct NAME;                                                                \
  template<TEMPL_TYPE T>                                                      \
  inline constexpr VAL_TYPE NAME##_v = NAME<T>::value;

#define DEFAULTED_TEMPLATE_VALUE_TYPE(NAME, TEMPL_TYPE, VAL_TYPE, DEFAULT)    \
  template<TEMPL_TYPE T>                                                      \
  struct NAME                                                                 \
  {                                                                           \
    static constexpr VAL_TYPE value = DEFAULT;                                \
  };                                                                          \
  template<TEMPL_TYPE T>                                                      \
  inline constexpr VAL_TYPE NAME##_v = NAME<T>::value;

namespace lak
{
  /* --- integral_constant --- */

  template<typename T, T V>
  struct integral_constant
  {
    using type               = T;
    static constexpr T value = V;
  };

  /* --- bool_type --- */

  template<bool B>
  using bool_type = lak::integral_constant<bool, B>;

  /* --- true/false_type --- */

  using true_type  = lak::bool_type<true>;
  using false_type = lak::bool_type<false>;

  /* --- size_type --- */

  template<size_t S>
  using size_type = lak::integral_constant<size_t, S>;

  /* --- is_integral_constant --- */

  template<typename T>
  struct is_integral_constant : public lak::false_type
  {
  };
  template<typename T, T V>
  struct is_integral_constant<lak::integral_constant<T, V>>
  : public lak::true_type
  {
  };
  template<typename T>
  static constexpr bool is_integral_constant_v =
    lak::is_integral_constant<T>::value;

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
  struct remove_reference : public lak::type_identity<T>
  {
  };
  template<typename T>
  struct remove_reference<T &> : public lak::type_identity<T>
  {
  };
  template<typename T>
  struct remove_reference<T &&> : public lak::type_identity<T>
  {
  };
  template<typename T>
  using remove_reference_t = typename lak::remove_reference<T>::type;

  /* --- remove_lvalue_reference --- */

  template<typename T>
  struct remove_lvalue_reference : public lak::type_identity<T>
  {
  };
  template<typename T>
  struct remove_lvalue_reference<T &> : public lak::type_identity<T>
  {
  };
  template<typename T>
  using remove_lvalue_reference_t =
    typename lak::remove_lvalue_reference<T>::type;

  /* --- remove_rvalue_reference --- */

  template<typename T>
  struct remove_rvalue_reference : public lak::type_identity<T>
  {
  };
  template<typename T>
  struct remove_rvalue_reference<T &&> : public lak::type_identity<T>
  {
  };
  template<typename T>
  using remove_rvalue_reference_t =
    typename lak::remove_rvalue_reference<T>::type;

  /* --- remove_cvref --- */

  template<typename T>
  using remove_cvref_t = lak::remove_cv_t<lak::remove_reference_t<T>>;

  /* --- remove_refs_ptrs --- */

  template<typename T>
  struct remove_refs_ptrs
  {
    using type = T;
  };
  template<typename T>
  struct remove_refs_ptrs<T *>
  {
    using type = typename lak::remove_refs_ptrs<T>::type;
  };
  template<typename T>
  struct remove_refs_ptrs<T &>
  {
    using type = typename lak::remove_refs_ptrs<T>::type;
  };
  template<typename T>
  struct remove_refs_ptrs<T &&>
  {
    using type = typename lak::remove_refs_ptrs<T>::type;
  };
  template<typename T>
  using remove_refs_ptrs_t = typename lak::remove_refs_ptrs<T>::type;

  /* --- add_lvalue_reference --- */

  template<typename T>
  struct add_lvalue_reference : public lak::type_identity<T &>
  {
  };
  template<>
  struct add_lvalue_reference<void> : public lak::type_identity<void>
  {
  };
  template<typename T>
  struct add_lvalue_reference<T &> : public lak::type_identity<T &>
  {
  };
  template<typename T>
  struct add_lvalue_reference<T &&> : public lak::type_identity<T &>
  {
  };
  template<typename T>
  using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

  /* --- add_lvalue_reference --- */

  template<typename T>
  struct add_rvalue_reference : public lak::type_identity<T &&>
  {
  };
  template<>
  struct add_rvalue_reference<void> : public lak::type_identity<void>
  {
  };
  template<typename T>
  struct add_rvalue_reference<T &> : public lak::type_identity<T &>
  {
  };
  template<typename T>
  struct add_rvalue_reference<T &&> : public lak::type_identity<T &&>
  {
  };
  template<typename T>
  using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

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
  struct is_same : public lak::false_type
  {
  };
  template<typename T>
  struct is_same<T, T> : public lak::true_type
  {
  };
  template<typename T, typename U>
  static constexpr bool is_same_v = is_same<T, U>::value;

  /* --- is_const --- */

  template<typename T>
  struct is_const : public lak::false_type
  {
  };
  template<typename T>
  struct is_const<const T> : public lak::true_type
  {
  };
  template<typename T>
  static constexpr bool is_const_v = is_const<T>::value;

  static_assert(!lak::is_const_v<char>);
  static_assert(lak::is_const_v<const char>);
  static_assert(lak::is_const_v<const volatile char>);
  static_assert(!lak::is_const_v<char &>);
  static_assert(!lak::is_const_v<const volatile char &>);
  static_assert(!lak::is_const_v<char &&>);
  static_assert(!lak::is_const_v<const volatile char &&>);

  /* --- is_void --- */

  template<typename T>
  struct is_void : public lak::is_same<lak::remove_cv_t<T>, void>
  {
  };
  template<typename T>
  static constexpr bool is_void_v = lak::is_void<T>::value;

  static_assert(!lak::is_void_v<char>);
  static_assert(!lak::is_void_v<const char>);
  static_assert(!lak::is_void_v<const volatile char>);
  static_assert(!lak::is_void_v<char &>);
  static_assert(!lak::is_void_v<const volatile char &>);
  static_assert(!lak::is_void_v<char &&>);
  static_assert(!lak::is_void_v<const volatile char &&>);
  static_assert(lak::is_void_v<void>);
  static_assert(lak::is_void_v<const void>);
  static_assert(lak::is_void_v<const volatile void>);

  /* --- is_lvalue_reference --- */

  template<typename T>
  struct is_lvalue_reference : public lak::false_type
  {
  };
  template<typename T>
  struct is_lvalue_reference<T &> : public lak::true_type
  {
  };
  template<typename T>
  static constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

  static_assert(!lak::is_lvalue_reference_v<char>);
  static_assert(!lak::is_lvalue_reference_v<const volatile char>);
  static_assert(lak::is_lvalue_reference_v<char &>);
  static_assert(lak::is_lvalue_reference_v<const volatile char &>);
  static_assert(!lak::is_lvalue_reference_v<char &&>);
  static_assert(!lak::is_lvalue_reference_v<const volatile char &&>);

  /* --- is_rvalue_reference --- */

  template<typename T>
  struct is_rvalue_reference : public lak::false_type
  {
  };
  template<typename T>
  struct is_rvalue_reference<T &&> : public lak::true_type
  {
  };
  template<typename T>
  static constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

  static_assert(!lak::is_rvalue_reference_v<char>);
  static_assert(!lak::is_rvalue_reference_v<const volatile char>);
  static_assert(!lak::is_rvalue_reference_v<char &>);
  static_assert(!lak::is_rvalue_reference_v<const volatile char &>);
  static_assert(lak::is_rvalue_reference_v<char &&>);
  static_assert(lak::is_rvalue_reference_v<const volatile char &&>);

  /* --- is_reference --- */

  template<typename T>
  struct is_reference : public lak::false_type
  {
  };
  template<typename T>
  struct is_reference<T &> : public lak::true_type
  {
  };
  template<typename T>
  struct is_reference<T &&> : public lak::true_type
  {
  };
  template<typename T>
  static constexpr bool is_reference_v = is_reference<T>::value;

  static_assert(!lak::is_reference_v<char>);
  static_assert(!lak::is_reference_v<const volatile char>);
  static_assert(lak::is_reference_v<char &>);
  static_assert(lak::is_reference_v<const volatile char &>);
  static_assert(lak::is_reference_v<char &&>);
  static_assert(lak::is_reference_v<const volatile char &&>);

  /* --- is_pointer --- */

  template<typename T>
  struct is_pointer : public lak::false_type
  {
  };
  template<typename T>
  struct is_pointer<T *> : public lak::true_type
  {
  };
  template<typename T>
  static constexpr bool is_pointer_v = lak::is_pointer<T>::value;

  static_assert(!lak::is_pointer_v<char>);
  static_assert(lak::is_pointer_v<char *>);
  static_assert(lak::is_pointer_v<const volatile char *>);
  static_assert(!lak::is_pointer_v<char &>);
  static_assert(!lak::is_pointer_v<const volatile char &>);
  static_assert(!lak::is_pointer_v<char &&>);
  static_assert(!lak::is_pointer_v<const volatile char &&>);

  /* --- lvalue_to_ptr --- */

  template<typename T>
  struct lvalue_to_ptr : public lak::type_identity<T>
  {
  };
  template<typename T>
  struct lvalue_to_ptr<T &> : public lak::type_identity<T *>
  {
  };
  template<typename T>
  using lvalue_to_ptr_t = typename lvalue_to_ptr<T>::type;

  static_assert(lak::is_same_v<char, lak::lvalue_to_ptr_t<char>>);
  static_assert(lak::is_same_v<const volatile char,
                               lak::lvalue_to_ptr_t<const volatile char>>);
  static_assert(lak::is_same_v<char *, lak::lvalue_to_ptr_t<char *>>);
  static_assert(lak::is_same_v<const volatile char *,
                               lak::lvalue_to_ptr_t<const volatile char *>>);
  static_assert(lak::is_same_v<char *, lak::lvalue_to_ptr_t<char &>>);
  static_assert(lak::is_same_v<const volatile char *,
                               lak::lvalue_to_ptr_t<const volatile char &>>);
  static_assert(lak::is_same_v<char &&, lak::lvalue_to_ptr_t<char &&>>);
  static_assert(lak::is_same_v<const volatile char &&,
                               lak::lvalue_to_ptr_t<const volatile char &&>>);

  /* --- copy_const --- */

  template<typename FROM, typename TO>
  using copy_const_t = lak::conditional_t<lak::is_const_v<FROM>,
                                          const lak::remove_const_t<TO>,
                                          lak::remove_const_t<TO>>;

  /* --- nth_type --- */

  template<size_t I, typename... T>
  struct nth_type;

  template<typename T, typename... U>
  struct nth_type<0, T, U...> : public lak::type_identity<T>
  {
  };

  template<size_t I, typename T, typename... U>
  struct nth_type<I, T, U...>
  {
    static_assert(I <= sizeof...(U));
    using type = typename lak::nth_type<I - 1, U...>::type;
  };

  template<size_t I, typename... T>
  using nth_type_t = typename lak::nth_type<I, T...>::type;

  static_assert(lak::is_same_v<char, lak::nth_type_t<0, char, float, void>>);
  static_assert(lak::is_same_v<float, lak::nth_type_t<1, char, float, void>>);
  static_assert(lak::is_same_v<void, lak::nth_type_t<2, char, float, void>>);

  /* --- integer_sequence --- */

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
}

#endif
