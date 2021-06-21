#ifndef LAK_CONCEPTS_HPP
#define LAK_CONCEPTS_HPP

#include "type_traits.hpp"

namespace lak
{
  namespace concepts
  {
    /* --- integral_contant --- */

    template<typename T>
    concept integral_constant = lak::is_integral_constant_v<T>;

    /* --- same_as --- */

    template<typename T, typename U>
    concept same_as = lak::is_same_v<T, U>;

    /* --- void_type --- */

    template<typename T>
    concept void_type = lak::is_void_v<T>;

    /* --- lvalue_reference --- */

    template<typename T>
    concept lvalue_reference = lak::is_lvalue_reference_v<T>;

    /* --- rvalue_reference --- */

    template<typename T>
    concept rvalue_reference = lak::is_rvalue_reference_v<T>;

    /* --- reference --- */

    template<typename T>
    concept reference = lak::is_reference_v<T>;

    /* --- pointer --- */

    template<typename T>
    concept pointer = lak::is_pointer_v<T>;

    /* --- array --- */

    template<typename T>
    concept array = lak::is_array_v<T>;

    /* --- function --- */

    template<typename T>
    concept function = lak::is_function_v<T>;

    /* --- function_pointer --- */

    template<typename T>
    concept function_pointer = lak::is_function_pointer_v<T>;

    /* --- function_or_function_pointer --- */

    template<typename T>
    concept function_or_function_pointer =
      lak::is_function_of_function_pointer_v<T>;

    /* --- detected --- */

    template<template<typename...> typename OP, typename... ARGS>
    concept detected = lak::is_detected_v<OP, ARGS...>;

    template<typename EXPECTED,
             template<typename...>
             typename OP,
             typename... ARGS>
    concept detected_exact = lak::is_detected_exact_v<EXPECTED, OP, ARGS...>;

    template<typename T>
    concept default_constructible = lak::is_default_constructible_v<T>;

    /* --- static_castable --- */

    template<typename FROM, typename TO>
    concept static_castable = lak::is_static_castable_v<FROM, TO>;

    static_assert(lak::concepts::static_castable<int *, int *>);
    static_assert(lak::concepts::static_castable<int *, const int *>);

    /* --- reinterpret_castable --- */

    template<typename FROM, typename TO>
    concept reinterpret_castable = lak::is_reinterpret_castable_v<FROM, TO>;

    static_assert(lak::concepts::reinterpret_castable<int *, char *>);
    static_assert(lak::concepts::reinterpret_castable<int *, const char *>);
    static_assert(!lak::concepts::reinterpret_castable<const int *, int *>);

    /* --- static_castable_from --- */

    template<typename TO, typename FROM>
    concept static_castable_from = lak::is_static_castable_v<FROM, TO>;

    static_assert(lak::concepts::static_castable_from<int *, int *>);
    static_assert(lak::concepts::static_castable_from<const int *, int *>);

    /* --- reinterpret_castable --- */

    template<typename TO, typename FROM>
    concept reinterpret_castable_from =
      lak::is_reinterpret_castable_v<FROM, TO>;

    static_assert(lak::concepts::reinterpret_castable_from<int *, char *>);
    static_assert(
      lak::concepts::reinterpret_castable_from<const int *, char *>);
    static_assert(
      !lak::concepts::reinterpret_castable_from<int *, const int *>);

    /* --- ptr_static_castable_from --- */

    template<typename TO, typename FROM>
    concept ptr_static_castable_from = lak::is_static_castable_v<FROM *, TO *>;

    static_assert(lak::concepts::ptr_static_castable_from<int, int>);
    static_assert(lak::concepts::ptr_static_castable_from<const int, int>);

    /* --- ptr_reinterpret_castable_from --- */

    template<typename TO, typename FROM>
    concept ptr_reinterpret_castable_from =
      lak::is_reinterpret_castable_v<FROM *, TO *>;

    static_assert(lak::concepts::ptr_reinterpret_castable_from<int, char>);
    static_assert(
      lak::concepts::ptr_reinterpret_castable_from<const int, char>);
    static_assert(
      !lak::concepts::ptr_reinterpret_castable_from<int, const int>);

    /* --- resizable --- */

    template<typename T>
    concept resizable = lak::is_resizable_v<T>;

    /* --- same_template --- */

    template<template<typename...> typename T,
             template<typename...>
             typename U>
    concept same_template = lak::is_same_template_v<T, U>;

    /* --- of_template --- */

    template<typename T, template<typename...> typename U>
    concept of_template = lak::is_of_template_v<T, U>;

    /* --- invocable --- */

    template<typename F, typename... ARGS>
    concept invocable = lak::is_invocable_v<F, ARGS...>;

    /* --- invocable_result_of --- */

    template<typename F, typename R, typename... ARGS>
    concept invocable_result_of = lak::concepts::invocable<F, ARGS...>
      &&lak::concepts::same_as<lak::invoke_result_t<F, ARGS...>, R>;

    /* --- invocable_result_static_castable_to --- */

    template<typename F, typename R, typename... ARGS>
    concept invocable_result_static_castable_to =
      lak::concepts::invocable<F, ARGS...>
        &&lak::concepts::static_castable<lak::invoke_result_t<F, ARGS...>, R>;

    /* --- invocable_result_of_template --- */

    template<typename F, template<typename...> typename R, typename... ARGS>
    concept invocable_result_of_template = lak::concepts::invocable<F, ARGS...>
      &&lak::concepts::of_template<lak::invoke_result_t<F, ARGS...>, R>;

    /* --- contiguous_range --- */

    template<typename R>
    concept contiguous_range = requires(R range)
    {
      typename R::value_type;
      typename R::size_type;

      {
        range.data()
      }
      ->lak::concepts::same_as<typename R::value_type *>;
      {
        range.size()
      }
      ->lak::concepts::same_as<typename R::size_type>;
    };

    /* --- fixed_contiguous_range --- */

    template<typename R>
    concept fixed_contiguous_range = requires(R range)
    {
      requires lak::concepts::contiguous_range<R>;
      requires lak::has_type_size_signature_v<R>;
    };

    /* --- contiguous_range_of --- */

    template<typename R, typename T>
    concept contiguous_range_of = requires(R range)
    {
      requires lak::concepts::contiguous_range<R>;
      requires lak::concepts::static_castable<typename R::value_type *, T *>;
    };

    /* --- fixed_contiguous_range_of --- */

    template<typename R, typename T, size_t S>
    concept fixed_contiguous_range_of = requires(R range)
    {
      requires lak::concepts::fixed_contiguous_range<R>;
      requires lak::concepts::static_castable<typename R::value_type *, T *>;
      requires lak::type_size_signature_size_v<R> == S;
    };
  }
}

#endif