#ifndef LAK_CONCEPTS_HPP
#define LAK_CONCEPTS_HPP

#include "type_traits.hpp"

namespace lak
{
  namespace concepts
  {
    template<typename T>
    concept integral_constant = lak::is_integral_constant_v<T>;

    template<typename T, typename U>
    concept same_as = lak::is_same_v<T, U>;

    template<typename T>
    concept lvalue_reference = lak::is_lvalue_reference_v<T>;

    template<typename T>
    concept rvalue_reference = lak::is_rvalue_reference_v<T>;

    template<typename T>
    concept reference = lak::is_reference_v<T>;

    template<typename T>
    concept pointer = lak::is_pointer_v<T>;

    template<typename T>
    concept array = lak::is_array_v<T>;

    template<typename T>
    concept function = lak::is_function_v<T>;

    template<typename T>
    concept function_pointer = lak::is_function_pointer_v<T>;

    template<typename T>
    concept function_or_function_pointer =
      lak::is_function_of_function_pointer_v<T>;

    template<template<typename...> typename OP, typename... ARGS>
    concept detected = lak::is_detected_v<OP, ARGS...>;

    template<typename EXPECTED,
             template<typename...>
             typename OP,
             typename... ARGS>
    concept detected_exact = lak::is_detected_exact_v<EXPECTED, OP, ARGS...>;

    template<typename T>
    concept default_constructible = lak::is_default_constructible_v<T>;

    template<typename T, typename U>
    concept castable = lak::is_castable_v<T, U>;

    template<typename T>
    concept resizable = lak::is_resizable_v<T>;

    template<template<typename...> typename T,
             template<typename...>
             typename U>
    concept same_template = lak::is_same_template_v<T, U>;

    template<typename T, template<typename...> typename U>
    concept of_template = lak::is_of_template_v<T, U>;

    template<typename F, typename... ARGS>
    concept invocable = lak::is_invocable_v<F, ARGS...>;

    template<typename F, template<typename...> typename R, typename... ARGS>
    concept invocable_result_of_template = lak::concepts::invocable<F, ARGS...>
      &&lak::concepts::of_template<lak::invoke_result_t<F, ARGS...>, R>;
  }
}

#endif