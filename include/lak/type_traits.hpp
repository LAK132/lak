#ifndef LAK_TYPE_TRAITS
#define LAK_TYPE_TRAITS

#include "lak/macro_utils.hpp"
#include "lak/stdint.hpp"

#include <type_traits>

#define TEMPLATE_VALUE_TYPE(NAME, TEMPL_TYPE, VAL_TYPE)                       \
	template<TEMPL_TYPE T>                                                      \
	struct NAME;                                                                \
	template<TEMPL_TYPE T>                                                      \
	static constexpr VAL_TYPE NAME##_v = NAME<T>::value;

#define DEFAULTED_TEMPLATE_VALUE_TYPE(NAME, TEMPL_TYPE, VAL_TYPE, DEFAULT)    \
	template<TEMPL_TYPE T>                                                      \
	struct NAME                                                                 \
	{                                                                           \
		static constexpr VAL_TYPE value = DEFAULT;                                \
	};                                                                          \
	template<TEMPL_TYPE T>                                                      \
	static constexpr VAL_TYPE NAME##_v = NAME<T>::value;

namespace lak
{
	template<typename... T>
	struct type_pack;

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

	using true_type = lak::bool_type<true>;

	using false_type = lak::bool_type<false>;

	static_assert(lak::true_type::value == true);
	static_assert(lak::false_type::value == false);

	/* --- size_type --- */

	template<size_t S>
	using size_type = lak::integral_constant<size_t, S>;

	static_assert(lak::size_type<10>::value == 10);
	static_assert(lak::size_type<100>::value == 100);
	static_assert(lak::size_type<1000>::value == 1000);

	/* --- is_integral_constant --- */

	template<typename T>
	struct is_integral_constant : lak::false_type
	{
	};

	template<typename T, T V>
	struct is_integral_constant<lak::integral_constant<T, V>> : lak::true_type
	{
	};

	template<typename T>
	inline constexpr bool is_integral_constant_v =
	  lak::is_integral_constant<T>::value;

	static_assert(lak::is_integral_constant_v<lak::true_type>);
	static_assert(lak::is_integral_constant_v<lak::false_type>);
	static_assert(lak::is_integral_constant_v<lak::size_type<100>>);
	static_assert(!lak::is_integral_constant_v<decltype(true)>);
	static_assert(!lak::is_integral_constant_v<decltype(100)>);

	/* --- is_lak_type --- */

	template<typename T>
	struct is_lak_type : lak::false_type
	{
	};

	template<typename T>
	constexpr inline bool is_lak_type_v = lak::is_lak_type<T>::value;

	template<typename T, T V>
	struct is_lak_type<lak::integral_constant<T, V>> : lak::true_type
	{
	};

	template<typename T>
	struct is_lak_type<lak::is_integral_constant<T>> : lak::true_type
	{
	};

	/* --- is_same --- */

	template<typename T, typename U>
	struct is_same : lak::false_type
	{
	};

	template<typename T>
	struct is_same<T, T> : lak::true_type
	{
	};

	template<typename T, typename U>
	inline constexpr bool is_same_v = lak::is_same<T, U>::value;

	template<typename T, typename U>
	struct is_lak_type<lak::is_same<T, U>> : lak::true_type
	{
	};

	/* --- is_standard_layout --- */

	template<typename T>
	struct is_standard_layout : public std::is_standard_layout<T>
	{
	};

	template<typename T>
	inline constexpr bool is_standard_layout_v =
	  lak::is_standard_layout<T>::value;

	/* --- type_identity --- */

	template<typename T>
	struct type_identity
	{
		using type = T;
	};

	static_assert(lak::is_same_v<typename lak::type_identity<int>::type, int>);
	static_assert(
	  lak::is_same_v<typename lak::type_identity<int &>::type, int &>);
	static_assert(lak::is_same_v<typename lak::type_identity<const int &>::type,
	                             const int &>);

	/* --- left_flatten --- */

	template<typename CONTAINER>
	struct left_flatten;

	template<template<typename...> typename CONTAINER, typename... T>
	struct left_flatten<CONTAINER<T...>>
	: public lak::type_identity<CONTAINER<T...>>
	{
		static constexpr bool is_flattened = false;
	};

	template<template<typename...> typename CONTAINER,
	         typename... T,
	         typename... U>
	struct left_flatten<CONTAINER<CONTAINER<T...>, U...>>
	: public lak::type_identity<CONTAINER<T..., U...>>
	{
		static constexpr bool is_flattened = true;
	};

	template<typename CONTAINER>
	using left_flatten_t = typename lak::left_flatten<CONTAINER>::type;

	template<typename CONTAINER>
	inline constexpr bool can_flatten_v =
	  lak::left_flatten<CONTAINER>::is_flattened;

	/* --- recursive_left_flatten --- */

	template<typename CONTAINER>
	struct recursive_left_flatten;

	template<template<typename...> typename CONTAINER, typename... T>
	struct recursive_left_flatten<CONTAINER<T...>>
	: public lak::type_identity<CONTAINER<T...>>
	{
		static constexpr bool is_flattened = false;
	};

	template<template<typename...> typename CONTAINER,
	         typename... T,
	         typename... U>
	struct recursive_left_flatten<CONTAINER<CONTAINER<T...>, U...>>
	: public lak::type_identity<
	    typename recursive_left_flatten<CONTAINER<T..., U...>>::type>
	{
		static constexpr bool is_flattened = true;
	};

	template<typename CONTAINER>
	using recursive_left_flatten_t =
	  typename lak::recursive_left_flatten<CONTAINER>::type;

	/* --- conjunction --- */

	template<typename... B>
	struct conjunction : lak::bool_type<((B::value) && ...)>
	{
	};

	template<>
	struct conjunction<> : lak::true_type
	{
	};

	template<typename... B>
	inline constexpr bool conjunction_v = lak::conjunction<B...>::value;

	static_assert(lak::conjunction_v<>);
	static_assert(lak::conjunction_v<lak::true_type>);
	static_assert(!lak::conjunction_v<lak::false_type>);
	static_assert(lak::conjunction_v<lak::true_type, lak::true_type>);
	static_assert(!lak::conjunction_v<lak::true_type, lak::false_type>);
	static_assert(!lak::conjunction_v<lak::false_type, lak::true_type>);
	static_assert(!lak::conjunction_v<lak::false_type, lak::false_type>);

	/* --- disjunction --- */

	template<typename... B>
	struct disjunction : lak::bool_type<((B::value) || ...)>
	{
	};

	template<>
	struct disjunction<> : lak::false_type
	{
	};

	template<typename... B>
	inline constexpr bool disjunction_v = lak::disjunction<B...>::value;

	static_assert(!lak::disjunction_v<>);
	static_assert(lak::disjunction_v<lak::true_type>);
	static_assert(!lak::disjunction_v<lak::false_type>);
	static_assert(lak::disjunction_v<lak::true_type, lak::true_type>);
	static_assert(lak::disjunction_v<lak::true_type, lak::false_type>);
	static_assert(lak::disjunction_v<lak::false_type, lak::true_type>);
	static_assert(!lak::disjunction_v<lak::false_type, lak::false_type>);

	/* --- are_all_same --- */

	template<typename... T>
	struct are_all_same;

	template<typename T, typename... U>
	struct are_all_same<T, U...> : lak::conjunction<lak::is_same<T, U>...>
	{
	};

	template<>
	struct are_all_same<> : lak::conjunction<>
	{
	};

	template<typename... T>
	inline constexpr bool are_all_same_v = lak::are_all_same<T...>::value;

	/* --- remove_const --- */

	template<typename T>
	struct remove_const : lak::type_identity<T>
	{
	};

	template<typename T>
	struct remove_const<const T> : lak::type_identity<T>
	{
	};

	template<typename T>
	using remove_const_t = typename lak::remove_const<T>::type;

	static_assert(lak::is_same_v<lak::remove_const_t<int>, int>);
	static_assert(lak::is_same_v<lak::remove_const_t<const int>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_const_t<volatile int>, volatile int>);
	static_assert(
	  lak::is_same_v<lak::remove_const_t<const volatile int>, volatile int>);

	static_assert(lak::is_same_v<lak::remove_const_t<int *>, int *>);
	static_assert(lak::is_same_v<lak::remove_const_t<const int *>, const int *>);
	static_assert(
	  lak::is_same_v<lak::remove_const_t<volatile int *>, volatile int *>);
	static_assert(lak::is_same_v<lak::remove_const_t<const volatile int *>,
	                             const volatile int *>);

	static_assert(lak::is_same_v<lak::remove_const_t<int &>, int &>);
	static_assert(lak::is_same_v<lak::remove_const_t<const int &>, const int &>);
	static_assert(
	  lak::is_same_v<lak::remove_const_t<volatile int &>, volatile int &>);
	static_assert(lak::is_same_v<lak::remove_const_t<const volatile int &>,
	                             const volatile int &>);

	static_assert(lak::is_same_v<lak::remove_const_t<int &&>, int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_const_t<const int &&>, const int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_const_t<volatile int &&>, volatile int &&>);
	static_assert(lak::is_same_v<lak::remove_const_t<const volatile int &&>,
	                             const volatile int &&>);

	/* --- remove_volatile --- */

	template<typename T>
	struct remove_volatile : lak::type_identity<T>
	{
	};

	template<typename T>
	struct remove_volatile<volatile T> : lak::type_identity<T>
	{
	};

	template<typename T>
	using remove_volatile_t = typename lak::remove_volatile<T>::type;

	static_assert(lak::is_same_v<lak::remove_volatile_t<int>, int>);
	static_assert(lak::is_same_v<lak::remove_volatile_t<const int>, const int>);
	static_assert(lak::is_same_v<lak::remove_volatile_t<volatile int>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_volatile_t<const volatile int>, const int>);

	static_assert(lak::is_same_v<lak::remove_volatile_t<int *>, int *>);
	static_assert(
	  lak::is_same_v<lak::remove_volatile_t<const int *>, const int *>);
	static_assert(
	  lak::is_same_v<lak::remove_volatile_t<volatile int *>, volatile int *>);
	static_assert(lak::is_same_v<lak::remove_volatile_t<const volatile int *>,
	                             const volatile int *>);

	static_assert(lak::is_same_v<lak::remove_volatile_t<int &>, int &>);
	static_assert(
	  lak::is_same_v<lak::remove_volatile_t<const int &>, const int &>);
	static_assert(
	  lak::is_same_v<lak::remove_volatile_t<volatile int &>, volatile int &>);
	static_assert(lak::is_same_v<lak::remove_volatile_t<const volatile int &>,
	                             const volatile int &>);

	static_assert(lak::is_same_v<lak::remove_volatile_t<int &&>, int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_volatile_t<const int &&>, const int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_volatile_t<volatile int &&>, volatile int &&>);
	static_assert(lak::is_same_v<lak::remove_volatile_t<const volatile int &&>,
	                             const volatile int &&>);

	/* --- remove_cv --- */

	template<typename T>
	struct remove_cv : lak::remove_const<lak::remove_volatile_t<T>>
	{
	};

	template<typename T>
	using remove_cv_t = typename lak::remove_cv<T>::type;

	static_assert(lak::is_same_v<lak::remove_cv_t<int>, int>);
	static_assert(lak::is_same_v<lak::remove_cv_t<const int>, int>);
	static_assert(lak::is_same_v<lak::remove_cv_t<volatile int>, int>);
	static_assert(lak::is_same_v<lak::remove_cv_t<const volatile int>, int>);

	static_assert(lak::is_same_v<lak::remove_cv_t<int *>, int *>);
	static_assert(lak::is_same_v<lak::remove_cv_t<const int *>, const int *>);
	static_assert(
	  lak::is_same_v<lak::remove_cv_t<volatile int *>, volatile int *>);
	static_assert(lak::is_same_v<lak::remove_cv_t<const volatile int *>,
	                             const volatile int *>);

	static_assert(lak::is_same_v<lak::remove_cv_t<int &>, int &>);
	static_assert(lak::is_same_v<lak::remove_cv_t<const int &>, const int &>);
	static_assert(
	  lak::is_same_v<lak::remove_cv_t<volatile int &>, volatile int &>);
	static_assert(lak::is_same_v<lak::remove_cv_t<const volatile int &>,
	                             const volatile int &>);

	static_assert(lak::is_same_v<lak::remove_cv_t<int &&>, int &&>);
	static_assert(lak::is_same_v<lak::remove_cv_t<const int &&>, const int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_cv_t<volatile int &&>, volatile int &&>);
	static_assert(lak::is_same_v<lak::remove_cv_t<const volatile int &&>,
	                             const volatile int &&>);

	/* --- remove_reference --- */

	template<typename T>
	struct remove_reference : lak::type_identity<T>
	{
	};

	template<typename T>
	struct remove_reference<T &> : lak::type_identity<T>
	{
	};

	template<typename T>
	struct remove_reference<T &&> : lak::type_identity<T>
	{
	};

	template<typename T>
	using remove_reference_t = typename lak::remove_reference<T>::type;

	static_assert(lak::is_same_v<lak::remove_reference_t<int>, int>);
	static_assert(lak::is_same_v<lak::remove_reference_t<const int>, const int>);
	static_assert(
	  lak::is_same_v<lak::remove_reference_t<volatile int>, volatile int>);
	static_assert(lak::is_same_v<lak::remove_reference_t<const volatile int>,
	                             const volatile int>);

	static_assert(lak::is_same_v<lak::remove_reference_t<int *>, int *>);
	static_assert(
	  lak::is_same_v<lak::remove_reference_t<const int *>, const int *>);
	static_assert(
	  lak::is_same_v<lak::remove_reference_t<volatile int *>, volatile int *>);
	static_assert(lak::is_same_v<lak::remove_reference_t<const volatile int *>,
	                             const volatile int *>);

	static_assert(lak::is_same_v<lak::remove_reference_t<int &>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_reference_t<const int &>, const int>);
	static_assert(
	  lak::is_same_v<lak::remove_reference_t<volatile int &>, volatile int>);
	static_assert(lak::is_same_v<lak::remove_reference_t<const volatile int>,
	                             const volatile int>);

	static_assert(lak::is_same_v<lak::remove_reference_t<int &&>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_reference_t<const int &&>, const int>);
	static_assert(
	  lak::is_same_v<lak::remove_reference_t<volatile int &&>, volatile int>);
	static_assert(lak::is_same_v<lak::remove_reference_t<const volatile int &&>,
	                             const volatile int>);

	/* --- remove_lvalue_reference --- */

	template<typename T>
	struct remove_lvalue_reference : lak::type_identity<T>
	{
	};

	template<typename T>
	struct remove_lvalue_reference<T &> : lak::type_identity<T>
	{
	};

	template<typename T>
	using remove_lvalue_reference_t =
	  typename lak::remove_lvalue_reference<T>::type;

	static_assert(lak::is_same_v<lak::remove_lvalue_reference_t<int>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_lvalue_reference_t<const int>, const int>);
	static_assert(lak::is_same_v<lak::remove_lvalue_reference_t<volatile int>,
	                             volatile int>);
	static_assert(
	  lak::is_same_v<lak::remove_lvalue_reference_t<const volatile int>,
	                 const volatile int>);

	static_assert(lak::is_same_v<lak::remove_lvalue_reference_t<int *>, int *>);
	static_assert(
	  lak::is_same_v<lak::remove_lvalue_reference_t<const int *>, const int *>);
	static_assert(lak::is_same_v<lak::remove_lvalue_reference_t<volatile int *>,
	                             volatile int *>);
	static_assert(
	  lak::is_same_v<lak::remove_lvalue_reference_t<const volatile int *>,
	                 const volatile int *>);

	static_assert(lak::is_same_v<lak::remove_lvalue_reference_t<int &>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_lvalue_reference_t<const int &>, const int>);
	static_assert(lak::is_same_v<lak::remove_lvalue_reference_t<volatile int &>,
	                             volatile int>);
	static_assert(
	  lak::is_same_v<lak::remove_lvalue_reference_t<const volatile int &>,
	                 const volatile int>);

	static_assert(
	  lak::is_same_v<lak::remove_lvalue_reference_t<int &&>, int &&>);
	static_assert(lak::is_same_v<lak::remove_lvalue_reference_t<const int &&>,
	                             const int &&>);
	static_assert(lak::is_same_v<lak::remove_lvalue_reference_t<volatile int &&>,
	                             volatile int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_lvalue_reference_t<const volatile int &&>,
	                 const volatile int &&>);

	/* --- remove_rvalue_reference --- */

	template<typename T>
	struct remove_rvalue_reference : lak::type_identity<T>
	{
	};

	template<typename T>
	struct remove_rvalue_reference<T &&> : lak::type_identity<T>
	{
	};

	template<typename T>
	using remove_rvalue_reference_t =
	  typename lak::remove_rvalue_reference<T>::type;

	static_assert(lak::is_same_v<lak::remove_rvalue_reference_t<int>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_rvalue_reference_t<const int>, const int>);
	static_assert(lak::is_same_v<lak::remove_rvalue_reference_t<volatile int>,
	                             volatile int>);
	static_assert(
	  lak::is_same_v<lak::remove_rvalue_reference_t<const volatile int>,
	                 const volatile int>);

	static_assert(lak::is_same_v<lak::remove_rvalue_reference_t<int *>, int *>);
	static_assert(
	  lak::is_same_v<lak::remove_rvalue_reference_t<const int *>, const int *>);
	static_assert(lak::is_same_v<lak::remove_rvalue_reference_t<volatile int *>,
	                             volatile int *>);
	static_assert(
	  lak::is_same_v<lak::remove_rvalue_reference_t<const volatile int *>,
	                 const volatile int *>);

	static_assert(lak::is_same_v<lak::remove_rvalue_reference_t<int &>, int &>);
	static_assert(
	  lak::is_same_v<lak::remove_rvalue_reference_t<const int &>, const int &>);
	static_assert(lak::is_same_v<lak::remove_rvalue_reference_t<volatile int &>,
	                             volatile int &>);
	static_assert(
	  lak::is_same_v<lak::remove_rvalue_reference_t<const volatile int &>,
	                 const volatile int &>);

	static_assert(lak::is_same_v<lak::remove_rvalue_reference_t<int &&>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_rvalue_reference_t<const int &&>, const int>);
	static_assert(lak::is_same_v<lak::remove_rvalue_reference_t<volatile int &&>,
	                             volatile int>);
	static_assert(
	  lak::is_same_v<lak::remove_rvalue_reference_t<const volatile int &&>,
	                 const volatile int>);

	/* --- remove_cvref --- */

	template<typename T>
	struct remove_cvref : lak::remove_cv<lak::remove_reference_t<T>>
	{
	};

	template<typename T>
	using remove_cvref_t = typename lak::remove_cvref<T>::type;

	static_assert(lak::is_same_v<lak::remove_cvref_t<int>, int>);
	static_assert(lak::is_same_v<lak::remove_cvref_t<const int>, int>);
	static_assert(lak::is_same_v<lak::remove_cvref_t<volatile int>, int>);
	static_assert(lak::is_same_v<lak::remove_cvref_t<const volatile int>, int>);

	static_assert(lak::is_same_v<lak::remove_cvref_t<int *>, int *>);
	static_assert(lak::is_same_v<lak::remove_cvref_t<const int *>, const int *>);
	static_assert(
	  lak::is_same_v<lak::remove_cvref_t<volatile int *>, volatile int *>);
	static_assert(lak::is_same_v<lak::remove_cvref_t<const volatile int *>,
	                             const volatile int *>);

	static_assert(lak::is_same_v<lak::remove_cvref_t<int &>, int>);
	static_assert(lak::is_same_v<lak::remove_cvref_t<const int &>, int>);
	static_assert(lak::is_same_v<lak::remove_cvref_t<volatile int &>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_cvref_t<const volatile int &>, int>);

	static_assert(lak::is_same_v<lak::remove_cvref_t<int &&>, int>);
	static_assert(lak::is_same_v<lak::remove_cvref_t<const int &&>, int>);
	static_assert(lak::is_same_v<lak::remove_cvref_t<volatile int &&>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_cvref_t<const volatile int &&>, int>);

	/* --- remove_pointer --- */

	template<typename T>
	struct remove_pointer : lak::type_identity<T>
	{
	};

	template<typename T>
	struct remove_pointer<T *> : lak::type_identity<T>
	{
	};

	template<typename T>
	using remove_pointer_t = typename lak::remove_pointer<T>::type;

	static_assert(lak::is_same_v<lak::remove_pointer_t<int>, int>);
	static_assert(lak::is_same_v<lak::remove_pointer_t<const int>, const int>);
	static_assert(
	  lak::is_same_v<lak::remove_pointer_t<volatile int>, volatile int>);
	static_assert(lak::is_same_v<lak::remove_pointer_t<const volatile int>,
	                             const volatile int>);

	static_assert(lak::is_same_v<lak::remove_pointer_t<int *>, int>);
	static_assert(lak::is_same_v<lak::remove_pointer_t<const int *>, const int>);
	static_assert(
	  lak::is_same_v<lak::remove_pointer_t<volatile int *>, volatile int>);
	static_assert(lak::is_same_v<lak::remove_pointer_t<const volatile int *>,
	                             const volatile int>);

	static_assert(lak::is_same_v<lak::remove_pointer_t<int &>, int &>);
	static_assert(
	  lak::is_same_v<lak::remove_pointer_t<const int &>, const int &>);
	static_assert(
	  lak::is_same_v<lak::remove_pointer_t<volatile int &>, volatile int &>);
	static_assert(lak::is_same_v<lak::remove_pointer_t<const volatile int &>,
	                             const volatile int &>);

	static_assert(lak::is_same_v<lak::remove_pointer_t<int *&>, int *&>);
	static_assert(
	  lak::is_same_v<lak::remove_pointer_t<const int *&>, const int *&>);
	static_assert(
	  lak::is_same_v<lak::remove_pointer_t<volatile int *&>, volatile int *&>);
	static_assert(lak::is_same_v<lak::remove_pointer_t<const volatile int *&>,
	                             const volatile int *&>);

	static_assert(lak::is_same_v<lak::remove_pointer_t<int &&>, int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_pointer_t<const int &&>, const int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_pointer_t<volatile int &&>, volatile int &&>);
	static_assert(lak::is_same_v<lak::remove_pointer_t<const volatile int &&>,
	                             const volatile int &&>);

	/* --- remove_cvptr --- */

	template<typename T>
	struct remove_cvptr : lak::remove_cv<lak::remove_pointer_t<T>>
	{
	};

	template<typename T>
	using remove_cvptr_t = typename lak::remove_cvptr<T>::type;

	static_assert(lak::is_same_v<lak::remove_cvptr_t<int>, int>);
	static_assert(lak::is_same_v<lak::remove_cvptr_t<const int>, int>);
	static_assert(lak::is_same_v<lak::remove_cvptr_t<volatile int>, int>);
	static_assert(lak::is_same_v<lak::remove_cvptr_t<const volatile int>, int>);

	static_assert(lak::is_same_v<lak::remove_cvptr_t<int *>, int>);
	static_assert(lak::is_same_v<lak::remove_cvptr_t<const int *>, int>);
	static_assert(lak::is_same_v<lak::remove_cvptr_t<volatile int *>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_cvptr_t<const volatile int *>, int>);

	static_assert(lak::is_same_v<lak::remove_cvptr_t<int &>, int &>);
	static_assert(lak::is_same_v<lak::remove_cvptr_t<const int &>, const int &>);
	static_assert(
	  lak::is_same_v<lak::remove_cvptr_t<volatile int &>, volatile int &>);
	static_assert(lak::is_same_v<lak::remove_cvptr_t<const volatile int &>,
	                             const volatile int &>);

	static_assert(lak::is_same_v<lak::remove_cvptr_t<int *&>, int *&>);
	static_assert(
	  lak::is_same_v<lak::remove_cvptr_t<const int *&>, const int *&>);
	static_assert(
	  lak::is_same_v<lak::remove_cvptr_t<volatile int *&>, volatile int *&>);
	static_assert(lak::is_same_v<lak::remove_cvptr_t<const volatile int *&>,
	                             const volatile int *&>);

	static_assert(lak::is_same_v<lak::remove_cvptr_t<int &&>, int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_cvptr_t<const int &&>, const int &&>);
	static_assert(
	  lak::is_same_v<lak::remove_cvptr_t<volatile int &&>, volatile int &&>);
	static_assert(lak::is_same_v<lak::remove_cvptr_t<const volatile int &&>,
	                             const volatile int &&>);

	static_assert(
	  lak::is_same_v<lak::remove_cvptr_t<lak::remove_cvref_t<int *&>>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_cvptr_t<lak::remove_cvref_t<const int *&>>,
	                 int>);
	static_assert(
	  lak::is_same_v<lak::remove_cvptr_t<lak::remove_cvref_t<volatile int *&>>,
	                 int>);
	static_assert(
	  lak::is_same_v<
	    lak::remove_cvptr_t<lak::remove_cvref_t<const volatile int *&>>,
	    int>);

	/* --- remove_refs_ptrs --- */

	template<typename T>
	struct remove_refs_ptrs : lak::type_identity<T>
	{
	};

	template<typename T>
	struct remove_refs_ptrs<T *> : lak::remove_refs_ptrs<T>
	{
	};

	template<typename T>
	struct remove_refs_ptrs<T &> : lak::remove_refs_ptrs<T>
	{
	};

	template<typename T>
	struct remove_refs_ptrs<T &&> : lak::remove_refs_ptrs<T>
	{
	};

	template<typename T>
	using remove_refs_ptrs_t = typename lak::remove_refs_ptrs<T>::type;

	static_assert(lak::is_same_v<lak::remove_refs_ptrs_t<int>, int>);
	static_assert(lak::is_same_v<lak::remove_refs_ptrs_t<const int>, const int>);
	static_assert(
	  lak::is_same_v<lak::remove_refs_ptrs_t<volatile int>, volatile int>);
	static_assert(lak::is_same_v<lak::remove_refs_ptrs_t<const volatile int>,
	                             const volatile int>);

	static_assert(lak::is_same_v<lak::remove_refs_ptrs_t<int *>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_refs_ptrs_t<const int *>, const int>);
	static_assert(
	  lak::is_same_v<lak::remove_refs_ptrs_t<volatile int *>, volatile int>);
	static_assert(lak::is_same_v<lak::remove_refs_ptrs_t<const volatile int *>,
	                             const volatile int>);

	static_assert(lak::is_same_v<lak::remove_refs_ptrs_t<int &>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_refs_ptrs_t<const int &>, const int>);
	static_assert(
	  lak::is_same_v<lak::remove_refs_ptrs_t<volatile int &>, volatile int>);
	static_assert(lak::is_same_v<lak::remove_refs_ptrs_t<const volatile int &>,
	                             const volatile int>);

	static_assert(lak::is_same_v<lak::remove_refs_ptrs_t<int &&>, int>);
	static_assert(
	  lak::is_same_v<lak::remove_refs_ptrs_t<const int &&>, const int>);
	static_assert(
	  lak::is_same_v<lak::remove_refs_ptrs_t<volatile int &&>, volatile int>);
	static_assert(lak::is_same_v<lak::remove_refs_ptrs_t<const volatile int &&>,
	                             const volatile int>);

	/* --- add_wconst --- */

	// west-const

	template<typename T>
	struct add_wconst : lak::type_identity<const T>
	{
	};

	template<typename T>
	struct add_wconst<T *> : lak::type_identity<const T *>
	{
	};

	template<typename T>
	struct add_wconst<T &> : lak::type_identity<const T &>
	{
	};

	template<typename T>
	struct add_wconst<T &&> : lak::type_identity<const T &&>
	{
	};

	template<typename T>
	using add_wconst_t = typename lak::add_wconst<T>::type;

	static_assert(lak::is_same_v<lak::add_wconst_t<int>, const int>);
	static_assert(lak::is_same_v<lak::add_wconst_t<const int>, const int>);
	static_assert(
	  lak::is_same_v<lak::add_wconst_t<volatile int>, const volatile int>);
	static_assert(
	  lak::is_same_v<lak::add_wconst_t<const volatile int>, const volatile int>);

	static_assert(lak::is_same_v<lak::add_wconst_t<int *>, const int *>);
	static_assert(lak::is_same_v<lak::add_wconst_t<const int *>, const int *>);
	static_assert(
	  lak::is_same_v<lak::add_wconst_t<volatile int *>, const volatile int *>);
	static_assert(lak::is_same_v<lak::add_wconst_t<const volatile int *>,
	                             const volatile int *>);

	static_assert(lak::is_same_v<lak::add_wconst_t<int &>, const int &>);
	static_assert(lak::is_same_v<lak::add_wconst_t<const int &>, const int &>);
	static_assert(
	  lak::is_same_v<lak::add_wconst_t<volatile int &>, const volatile int &>);
	static_assert(lak::is_same_v<lak::add_wconst_t<const volatile int &>,
	                             const volatile int &>);

	static_assert(lak::is_same_v<lak::add_wconst_t<int &&>, const int &&>);
	static_assert(lak::is_same_v<lak::add_wconst_t<const int &&>, const int &&>);
	static_assert(
	  lak::is_same_v<lak::add_wconst_t<volatile int &&>, const volatile int &&>);
	static_assert(lak::is_same_v<lak::add_wconst_t<const volatile int &&>,
	                             const volatile int &&>);

	/* --- add_econst --- */

	// east-const

	template<typename T>
	struct add_econst : lak::type_identity<T const>
	{
	};

	template<typename T>
	using add_econst_t = typename lak::add_econst<T>::type;

	static_assert(lak::is_same_v<lak::add_econst_t<int>, const int>);
	static_assert(lak::is_same_v<lak::add_econst_t<const int>, const int>);
	static_assert(
	  lak::is_same_v<lak::add_econst_t<volatile int>, const volatile int>);
	static_assert(
	  lak::is_same_v<lak::add_econst_t<const volatile int>, const volatile int>);

	static_assert(lak::is_same_v<lak::add_econst_t<int *>, int *const>);
	static_assert(
	  lak::is_same_v<lak::add_econst_t<const int *>, const int *const>);
	static_assert(
	  lak::is_same_v<lak::add_econst_t<volatile int *>, volatile int *const>);
	static_assert(lak::is_same_v<lak::add_econst_t<const volatile int *>,
	                             const volatile int *const>);

	static_assert(lak::is_same_v<lak::add_econst_t<int &>, int &>);
	static_assert(lak::is_same_v<lak::add_econst_t<const int &>, const int &>);
	static_assert(
	  lak::is_same_v<lak::add_econst_t<volatile int &>, volatile int &>);
	static_assert(lak::is_same_v<lak::add_econst_t<const volatile int &>,
	                             const volatile int &>);

	static_assert(lak::is_same_v<lak::add_econst_t<int &&>, int &&>);
	static_assert(lak::is_same_v<lak::add_econst_t<const int &&>, const int &&>);
	static_assert(
	  lak::is_same_v<lak::add_econst_t<volatile int &&>, volatile int &&>);
	static_assert(lak::is_same_v<lak::add_econst_t<const volatile int &&>,
	                             const volatile int &&>);

	/* --- add_lvalue_reference --- */

	template<typename T>
	struct add_lvalue_reference : lak::type_identity<T &>
	{
	};

	template<>
	struct add_lvalue_reference<void> : lak::type_identity<void>
	{
	};

	template<typename T>
	struct add_lvalue_reference<T &> : lak::type_identity<T &>
	{
	};

	template<typename T>
	struct add_lvalue_reference<T &&> : lak::type_identity<T &>
	{
	};

	template<typename T>
	using add_lvalue_reference_t = typename lak::add_lvalue_reference<T>::type;

	static_assert(lak::is_same_v<lak::add_lvalue_reference_t<int>, int &>);
	static_assert(
	  lak::is_same_v<lak::add_lvalue_reference_t<const int>, const int &>);
	static_assert(
	  lak::is_same_v<lak::add_lvalue_reference_t<volatile int>, volatile int &>);
	static_assert(lak::is_same_v<lak::add_lvalue_reference_t<const volatile int>,
	                             const volatile int &>);

	static_assert(lak::is_same_v<lak::add_lvalue_reference_t<int *>, int *&>);
	static_assert(
	  lak::is_same_v<lak::add_lvalue_reference_t<const int *>, const int *&>);
	static_assert(lak::is_same_v<lak::add_lvalue_reference_t<volatile int *>,
	                             volatile int *&>);
	static_assert(
	  lak::is_same_v<lak::add_lvalue_reference_t<const volatile int *>,
	                 const volatile int *&>);

	static_assert(lak::is_same_v<lak::add_lvalue_reference_t<int &>, int &>);
	static_assert(
	  lak::is_same_v<lak::add_lvalue_reference_t<const int &>, const int &>);
	static_assert(lak::is_same_v<lak::add_lvalue_reference_t<volatile int &>,
	                             volatile int &>);
	static_assert(
	  lak::is_same_v<lak::add_lvalue_reference_t<const volatile int &>,
	                 const volatile int &>);

	static_assert(lak::is_same_v<lak::add_lvalue_reference_t<int &&>, int &>);
	static_assert(
	  lak::is_same_v<lak::add_lvalue_reference_t<const int &&>, const int &>);
	static_assert(lak::is_same_v<lak::add_lvalue_reference_t<volatile int &&>,
	                             volatile int &>);
	static_assert(
	  lak::is_same_v<lak::add_lvalue_reference_t<const volatile int &&>,
	                 const volatile int &>);

	/* --- add_rvalue_reference --- */

	template<typename T>
	struct add_rvalue_reference : lak::type_identity<T &&>
	{
	};

	template<>
	struct add_rvalue_reference<void> : lak::type_identity<void>
	{
	};

	template<typename T>
	struct add_rvalue_reference<T &> : lak::type_identity<T &>
	{
	};

	template<typename T>
	struct add_rvalue_reference<T &&> : lak::type_identity<T &&>
	{
	};

	template<typename T>
	using add_rvalue_reference_t = typename lak::add_rvalue_reference<T>::type;

	static_assert(lak::is_same_v<lak::add_rvalue_reference_t<int>, int &&>);
	static_assert(
	  lak::is_same_v<lak::add_rvalue_reference_t<const int>, const int &&>);
	static_assert(lak::is_same_v<lak::add_rvalue_reference_t<volatile int>,
	                             volatile int &&>);
	static_assert(lak::is_same_v<lak::add_rvalue_reference_t<const volatile int>,
	                             const volatile int &&>);

	static_assert(lak::is_same_v<lak::add_rvalue_reference_t<int *>, int *&&>);
	static_assert(
	  lak::is_same_v<lak::add_rvalue_reference_t<const int *>, const int *&&>);
	static_assert(lak::is_same_v<lak::add_rvalue_reference_t<volatile int *>,
	                             volatile int *&&>);
	static_assert(
	  lak::is_same_v<lak::add_rvalue_reference_t<const volatile int *>,
	                 const volatile int *&&>);

	static_assert(lak::is_same_v<lak::add_rvalue_reference_t<int &>, int &>);
	static_assert(
	  lak::is_same_v<lak::add_rvalue_reference_t<const int &>, const int &>);
	static_assert(lak::is_same_v<lak::add_rvalue_reference_t<volatile int &>,
	                             volatile int &>);
	static_assert(
	  lak::is_same_v<lak::add_rvalue_reference_t<const volatile int &>,
	                 const volatile int &>);

	static_assert(lak::is_same_v<lak::add_rvalue_reference_t<int &&>, int &&>);
	static_assert(
	  lak::is_same_v<lak::add_rvalue_reference_t<const int &&>, const int &&>);
	static_assert(lak::is_same_v<lak::add_rvalue_reference_t<volatile int &&>,
	                             volatile int &&>);
	static_assert(
	  lak::is_same_v<lak::add_rvalue_reference_t<const volatile int &&>,
	                 const volatile int &&>);

	/* --- declval --- */
	// :TODO: this should be in utility.hpp but circular dependencies suck

	template<typename T>
	lak::add_rvalue_reference_t<T> declval() noexcept;

	/* --- conditional --- */

	template<bool B, typename T, typename F>
	struct conditional;

	template<typename T, typename F>
	struct conditional<true, T, F> : lak::type_identity<T>
	{
	};

	template<typename T, typename F>
	struct conditional<false, T, F> : lak::type_identity<F>
	{
	};

	template<bool B, typename T, typename F>
	using conditional_t = typename lak::conditional<B, T, F>::type;

	/* --- deferred_conditional --- */

	template<bool B,
	         template<typename...>
	         typename T,
	         template<typename...>
	         typename F,
	         typename... ARGS>
	struct deferred_conditional
	{
		struct _true
		{
			template<typename... A>
			using type = T<A...>;
		};

		struct _false
		{
			template<typename... A>
			using type = F<A...>;
		};

		using type =
		  typename lak::conditional_t<B, _true, _false>::template type<ARGS...>;
	};

	template<bool B,
	         template<typename...>
	         typename T,
	         template<typename...>
	         typename F,
	         typename... ARGS>
	using deferred_conditional_t =
	  typename deferred_conditional<B, T, F, ARGS...>::type;

	/* --- enable_if --- */

	template<bool B, typename T = void>
	struct enable_if
	{
	};

	template<typename T>
	struct enable_if<true, T> : lak::type_identity<T>
	{
	};

	template<bool B, typename T = void>
	using enable_if_t = typename lak::enable_if<B, T>::type;

	template<bool B>
	using enable_if_i = typename lak::enable_if<B, int>::type;

	/* --- is_const --- */

	template<typename T>
	struct is_const : lak::false_type
	{
	};

	template<typename T>
	struct is_const<const T> : lak::true_type
	{
	};

	template<typename T>
	inline constexpr bool is_const_v = lak::is_const<T>::value;

	static_assert(!lak::is_const_v<char>);
	static_assert(lak::is_const_v<const char>);
	static_assert(lak::is_const_v<const volatile char>);
	static_assert(!lak::is_const_v<char *>);
	static_assert(!lak::is_const_v<const volatile char *>);
	static_assert(lak::is_const_v<const volatile char *const>);
	static_assert(!lak::is_const_v<char &>);
	static_assert(!lak::is_const_v<const volatile char &>);
	static_assert(!lak::is_const_v<char &&>);
	static_assert(!lak::is_const_v<const volatile char &&>);
	static_assert(!lak::is_const_v<char[]>);
	static_assert(lak::is_const_v<const volatile char[]>);

	/* --- is_void --- */

	template<typename T>
	struct is_void : lak::is_same<lak::remove_cv_t<T>, void>
	{
	};

	template<typename T>
	inline constexpr bool is_void_v = lak::is_void<T>::value;

	static_assert(!lak::is_void_v<char>);
	static_assert(!lak::is_void_v<const char>);
	static_assert(!lak::is_void_v<const volatile char>);
	static_assert(!lak::is_void_v<char *>);
	static_assert(!lak::is_void_v<const volatile char *>);
	static_assert(!lak::is_void_v<char &>);
	static_assert(!lak::is_void_v<const volatile char &>);
	static_assert(!lak::is_void_v<char &&>);
	static_assert(!lak::is_void_v<const volatile char &&>);
	static_assert(!lak::is_void_v<char[]>);
	static_assert(!lak::is_void_v<const volatile char[]>);
	static_assert(lak::is_void_v<void>);
	static_assert(lak::is_void_v<const void>);
	static_assert(lak::is_void_v<const volatile void>);

	/* --- is_lvalue_reference --- */

	template<typename T>
	struct is_lvalue_reference : lak::false_type
	{
	};

	template<typename T>
	struct is_lvalue_reference<T &> : lak::true_type
	{
	};

	template<typename T>
	inline constexpr bool is_lvalue_reference_v =
	  lak::is_lvalue_reference<T>::value;

	static_assert(!lak::is_lvalue_reference_v<char>);
	static_assert(!lak::is_lvalue_reference_v<const volatile char>);
	static_assert(!lak::is_lvalue_reference_v<char *>);
	static_assert(!lak::is_lvalue_reference_v<const volatile char *>);
	static_assert(lak::is_lvalue_reference_v<char &>);
	static_assert(lak::is_lvalue_reference_v<const volatile char &>);
	static_assert(!lak::is_lvalue_reference_v<char &&>);
	static_assert(!lak::is_lvalue_reference_v<const volatile char &&>);
	static_assert(!lak::is_lvalue_reference_v<char[]>);
	static_assert(!lak::is_lvalue_reference_v<const volatile char[]>);

	/* --- is_rvalue_reference --- */

	template<typename T>
	struct is_rvalue_reference : lak::false_type
	{
	};

	template<typename T>
	struct is_rvalue_reference<T &&> : lak::true_type
	{
	};

	template<typename T>
	inline constexpr bool is_rvalue_reference_v =
	  lak::is_rvalue_reference<T>::value;

	static_assert(!lak::is_rvalue_reference_v<char>);
	static_assert(!lak::is_rvalue_reference_v<const volatile char>);
	static_assert(!lak::is_rvalue_reference_v<char *>);
	static_assert(!lak::is_rvalue_reference_v<const volatile char *>);
	static_assert(!lak::is_rvalue_reference_v<char &>);
	static_assert(!lak::is_rvalue_reference_v<const volatile char &>);
	static_assert(lak::is_rvalue_reference_v<char &&>);
	static_assert(lak::is_rvalue_reference_v<const volatile char &&>);
	static_assert(!lak::is_rvalue_reference_v<char[]>);
	static_assert(!lak::is_rvalue_reference_v<const volatile char[]>);

	/* --- is_reference --- */

	template<typename T>
	struct is_reference
	: lak::disjunction<lak::is_lvalue_reference<T>, lak::is_rvalue_reference<T>>
	{
	};

	template<typename T>
	inline constexpr bool is_reference_v = lak::is_reference<T>::value;

	static_assert(!lak::is_reference_v<char>);
	static_assert(!lak::is_reference_v<const volatile char>);
	static_assert(!lak::is_reference_v<char *>);
	static_assert(!lak::is_reference_v<const volatile char *>);
	static_assert(lak::is_reference_v<char &>);
	static_assert(lak::is_reference_v<const volatile char &>);
	static_assert(lak::is_reference_v<char &&>);
	static_assert(lak::is_reference_v<const volatile char &&>);
	static_assert(!lak::is_reference_v<char[]>);
	static_assert(!lak::is_reference_v<const volatile char[]>);

	/* --- is_pointer --- */

	template<typename T>
	struct is_pointer : lak::false_type
	{
	};

	template<typename T>
	struct is_pointer<T *> : lak::true_type
	{
	};

	template<typename T>
	inline constexpr bool is_pointer_v = lak::is_pointer<T>::value;

	static_assert(!lak::is_pointer_v<char>);
	static_assert(!lak::is_pointer_v<const volatile char>);
	static_assert(lak::is_pointer_v<char *>);
	static_assert(lak::is_pointer_v<const volatile char *>);
	static_assert(!lak::is_pointer_v<char &>);
	static_assert(!lak::is_pointer_v<const volatile char &>);
	static_assert(!lak::is_pointer_v<char &&>);
	static_assert(!lak::is_pointer_v<const volatile char &&>);
	static_assert(!lak::is_pointer_v<char[]>);
	static_assert(!lak::is_pointer_v<const volatile char[]>);

	/* --- is_member_pointer --- */

	template<typename T>
	struct is_member_pointer : lak::false_type
	{
	};

	template<typename T, typename U>
	struct is_member_pointer<U T::*> : lak::true_type
	{
	};

	template<typename T>
	inline constexpr bool is_member_pointer_v = lak::is_member_pointer<T>::value;

	static_assert(!lak::is_member_pointer_v<char>);
	static_assert(!lak::is_member_pointer_v<const volatile char>);
	static_assert(!lak::is_member_pointer_v<char *>);
	static_assert(!lak::is_member_pointer_v<const volatile char *>);
	static_assert(!lak::is_member_pointer_v<char &>);
	static_assert(!lak::is_member_pointer_v<const volatile char &>);
	static_assert(!lak::is_member_pointer_v<char &&>);
	static_assert(!lak::is_member_pointer_v<const volatile char &&>);
	static_assert(!lak::is_member_pointer_v<char[]>);
	static_assert(!lak::is_member_pointer_v<const volatile char[]>);

	/* --- is_member_function_pointer --- */

	template<typename T>
	struct is_member_function_pointer : lak::false_type
	{
	};

	template<typename T, typename U, typename... ARGS>
	struct is_member_function_pointer<U (T::*)(ARGS...)> : lak::true_type
	{
	};

	template<typename T>
	inline constexpr bool is_member_function_pointer_v =
	  lak::is_member_function_pointer<T>::value;

	static_assert(!lak::is_member_function_pointer_v<char>);
	static_assert(!lak::is_member_function_pointer_v<const volatile char>);
	static_assert(!lak::is_member_function_pointer_v<char *>);
	static_assert(!lak::is_member_function_pointer_v<const volatile char *>);
	static_assert(!lak::is_member_function_pointer_v<char &>);
	static_assert(!lak::is_member_function_pointer_v<const volatile char &>);
	static_assert(!lak::is_member_function_pointer_v<char &&>);
	static_assert(!lak::is_member_function_pointer_v<const volatile char &&>);
	static_assert(!lak::is_member_function_pointer_v<char[]>);
	static_assert(!lak::is_member_function_pointer_v<const volatile char[]>);

	/* --- member_function_traits --- */

	template<typename T>
	struct member_function_traits;

	template<typename T, typename U, typename... ARGS>
	struct member_function_traits<U (T::*)(ARGS...)>
	{
		using class_type     = T;
		using result_type    = U;
		using argument_types = lak::type_pack<ARGS...>;
	};

	/* --- is_array --- */

	template<typename T>
	struct is_array : lak::false_type
	{
	};

	template<typename T>
	struct is_array<T[]> : lak::true_type
	{
	};

	template<typename T, size_t N>
	struct is_array<T[N]> : lak::true_type
	{
	};

	template<typename T>
	inline constexpr bool is_array_v = lak::is_array<T>::value;

	static_assert(!lak::is_array_v<char>);
	static_assert(!lak::is_array_v<const volatile char>);
	static_assert(!lak::is_array_v<char *>);
	static_assert(!lak::is_array_v<const volatile char *>);
	static_assert(!lak::is_array_v<char &>);
	static_assert(!lak::is_array_v<const volatile char &>);
	static_assert(!lak::is_array_v<char &&>);
	static_assert(!lak::is_array_v<const volatile char &&>);
	static_assert(lak::is_array_v<char[]>);
	static_assert(lak::is_array_v<const volatile char[]>);

	/* --- is_function --- */

	template<typename T>
	struct is_function : lak::false_type
	{
	};

#define LAK_IS_FUNCTION(CVT, A, B)                                            \
	template<typename RET, typename... ARGS>                                    \
	struct is_function<A CVT B> : lak::true_type                                \
	{                                                                           \
	};
	LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS...), )
	// LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS......), )
	LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS...), &)
	// LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS......), &)
	LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS...), &&)
	// LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS......), &&)
	LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS...), noexcept)
	// LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS......), noexcept)
	LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS...), & noexcept)
	// LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS......), &noexcept)
	LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS...), && noexcept)
	// LAK_ALL_CVTS(LAK_IS_FUNCTION, RET(ARGS......), &&noexcept)
#undef LAK_IS_FUNCTION

	template<typename T>
	inline constexpr bool is_function_v = lak::is_function<T>::value;

	/* --- is_function_pointer --- */

	template<typename T>
	struct is_function_pointer
	: lak::conjunction<lak::is_pointer<T>,
	                   lak::is_function<lak::remove_pointer_t<T>>>
	{
	};

	template<typename T>
	inline constexpr bool is_function_pointer_v =
	  lak::is_function_pointer<T>::value;

	/* --- is_function_or_function_pointer --- */

	template<typename T>
	struct is_function_or_function_pointer
	: lak::disjunction<lak::is_function<T>, lak::is_function_pointer<T>>
	{
	};

	template<typename T>
	inline constexpr bool is_function_of_function_pointer_v =
	  lak::is_function_or_function_pointer<T>::value;

	/* --- lvalue_to_ptr --- */

	template<typename T>
	struct lvalue_to_ptr : lak::type_identity<T>
	{
	};

	template<typename T>
	struct lvalue_to_ptr<T &> : lak::type_identity<T *>
	{
	};

	template<typename T>
	using lvalue_to_ptr_t = typename lak::lvalue_to_ptr<T>::type;

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
	struct nth_type<0, T, U...> : lak::type_identity<T>
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

	/* --- nth_value --- */

	template<size_t I, auto... T>
	struct nth_value;

	template<auto T, auto... U>
	struct nth_value<0, T, U...>
	{
		static constexpr auto value = T;
	};

	template<size_t I, auto T, auto... U>
	struct nth_value<I, T, U...>
	{
		static_assert(I <= sizeof...(U));
		static constexpr auto value = lak::nth_value<I - 1, U...>::value;
	};

	template<size_t I, auto... T>
	inline constexpr auto nth_value_v = lak::nth_value<I, T...>::value;

	static_assert(
	  lak::is_same_v<const char,
	                 decltype(lak::nth_value_v<0, char{}, unsigned{}, int{}>)>);
	static_assert(
	  lak::is_same_v<const unsigned,
	                 decltype(lak::nth_value_v<1, char{}, unsigned{}, int{}>)>);
	static_assert(
	  lak::is_same_v<const int,
	                 decltype(lak::nth_value_v<2, char{}, unsigned{}, int{}>)>);

	/* --- integer_sequence --- */

	template<typename T, T... I>
	struct integer_sequence
	{
	};

	template<size_t... I>
	using index_sequence = lak::integer_sequence<size_t, I...>;

	template<typename T, T STOP, T I, T... Is>
	struct _make_integer_sequence
	{
		struct cont
		{
			template<typename U, U J, U... Js>
			using type = typename lak::
			  _make_integer_sequence<U, STOP, J - 1, J - 1, Js...>::type;
		};

		struct done
		{
			template<typename U, U J, U... Js>
			using type = lak::integer_sequence<U, Js...>;
		};

		template<typename U, U... Js>
		using next = typename lak::conditional_t<I == STOP, done, cont>::
		  template type<U, I, Js...>;

		using type = next<T, Is...>;
	};

	template<typename T, T I>
	using make_integer_sequence =
	  typename lak::_make_integer_sequence<T, T{0}, I>::type;

	template<size_t I>
	using make_index_sequence = lak::make_integer_sequence<size_t, I>;

	template<typename T, T BEGIN, T END>
	using make_integer_sequence_range =
	  typename lak::_make_integer_sequence<T, BEGIN, END>::type;

	template<size_t BEGIN, size_t END>
	using make_index_sequence_range =
	  lak::make_integer_sequence_range<size_t, BEGIN, END>;

	static_assert(
	  lak::is_same_v<lak::make_index_sequence<0>, lak::index_sequence<>>);
	static_assert(
	  lak::is_same_v<lak::make_index_sequence<1>, lak::index_sequence<0>>);
	static_assert(
	  lak::is_same_v<lak::make_index_sequence<10>,
	                 lak::index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>>);
	static_assert(lak::is_same_v<lak::make_index_sequence_range<5, 10>,
	                             lak::index_sequence<5, 6, 7, 8, 9>>);

	template<typename... T>
	using index_sequence_for = lak::make_index_sequence<sizeof...(T)>;

	/* --- void_t --- */

	template<typename...>
	using void_t = void;

	/* --- nonsuch --- */

	struct nonesuch
	{
		~nonesuch()                      = delete;
		nonesuch(const nonesuch &)       = delete;
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
	  lak::is_detected_exact<EXPECTED, OP, ARGS...>::value;

	/* --- is_constructible --- */

	template<typename T, typename... ARGS>
	struct is_constructible : lak::false_type
	{
	};

	template<typename T, typename... ARGS>
	requires requires {
		{
			T{lak::declval<ARGS>()...}
		};
	}
	struct is_constructible<T, ARGS...> : lak::true_type
	{
	};

	template<typename T, typename... ARGS>
	inline constexpr bool is_constructible_v =
	  lak::is_constructible<T, ARGS...>::value;

	static_assert(lak::is_constructible_v<int, int>);
	static_assert(!lak::is_constructible_v<void, int>);

	/* --- is_default_constructible --- */

	template<typename T>
	struct is_default_constructible : lak::is_constructible<T>
	{
	};

	template<typename T>
	inline constexpr bool is_default_constructible_v =
	  lak::is_default_constructible<T>::value;

	static_assert(lak::is_default_constructible_v<int>);

	/* --- is_copy_constructible --- */

	template<typename T>
	struct is_copy_constructible : lak::is_constructible<T, const T &>
	{
	};

	template<typename T>
	inline constexpr bool is_copy_constructible_v =
	  lak::is_copy_constructible<T>::value;

	static_assert(lak::is_copy_constructible_v<int>);

	/* --- is_move_constructible --- */

	template<typename T>
	struct is_move_constructible : lak::is_constructible<T, T &&>
	{
	};

	template<typename T>
	inline constexpr bool is_move_constructible_v =
	  lak::is_move_constructible<T>::value;

	static_assert(lak::is_move_constructible_v<int>);

	/* --- is_static_castable --- */

	template<typename T, typename U>
	struct is_static_castable : lak::false_type
	{
	};

	template<typename T, typename U>
	requires requires(T t) {
		{
			static_cast<U>(t)
		};
	}
	struct is_static_castable<T, U> : lak::true_type
	{
	};

	template<typename T, typename U>
	inline constexpr bool is_static_castable_v =
	  lak::is_static_castable<T, U>::value;

	static_assert(lak::is_static_castable_v<int *, const int *>);

	/* --- is_reinterpret_castable --- */

	template<typename T, typename U>
	struct is_reinterpret_castable : lak::false_type
	{
	};

	template<typename T, typename U>
	requires requires(T t) {
		{
			reinterpret_cast<U>(t)
		};
	}
	struct is_reinterpret_castable<T, U> : lak::true_type
	{
	};

	template<typename T, typename U>
	inline constexpr bool is_reinterpret_castable_v =
	  lak::is_reinterpret_castable<T, U>::value;

	static_assert(lak::is_reinterpret_castable_v<int *, const int *>);
	static_assert(lak::is_reinterpret_castable_v<int *, const char *>);
	static_assert(!lak::is_reinterpret_castable_v<const int *, int *>);

	/* --- is_resizable --- */

	template<typename T>
	struct is_resizable : lak::false_type
	{
	};

	template<typename T>
	requires requires(T t) {
		{
			t.resize(0)
		};
	}
	struct is_resizable<T> : lak::true_type
	{
	};

	template<typename T>
	inline constexpr bool is_resizable_v = lak::is_resizable<T>::value;

	/* --- is_same_template --- */

	template<template<typename...> typename T, template<typename...> typename U>
	struct is_same_template : lak::false_type
	{
	};

	template<template<typename...> typename T>
	struct is_same_template<T, T> : lak::true_type
	{
	};

	template<template<typename...> typename T, template<typename...> typename U>
	static constexpr bool is_same_template_v =
	  lak::is_same_template<T, U>::value;

	/* --- is_of_template --- */

	template<typename T, template<typename...> typename U>
	struct is_of_template : lak::false_type
	{
	};

	template<template<typename...> typename T, typename... U>
	struct is_of_template<T<U...>, T> : lak::true_type
	{
	};

	template<typename T, template<typename...> typename U>
	static constexpr bool is_of_template_v = lak::is_of_template<T, U>::value;

	/* --- invoke_result --- */

	template<typename F, typename... ARGS>
	struct invoke_result
	: lak::type_identity<decltype(lak::declval<F>()(lak::declval<ARGS>()...))>
	{
	};

	template<typename F, typename... ARGS>
	using invoke_result_t = typename lak::invoke_result<F, ARGS...>::type;

	/* --- is_invocable --- */

	template<typename F, typename... ARGS>
	struct is_invocable : lak::false_type
	{
	};

	template<typename F, typename... ARGS>
	requires requires(F f) {
		{
			f(lak::declval<ARGS>()...)
		};
	}
	struct is_invocable<F, ARGS...> : lak::true_type
	{
	};

	template<typename F, typename... ARGS>
	static constexpr bool is_invocable_v = lak::is_invocable<F, ARGS...>::value;

	static_assert(!lak::is_invocable_v<int>);
	static_assert(!lak::is_invocable_v<int, int>);
	static_assert(!lak::is_invocable_v<void(), int>);
	static_assert(!lak::is_invocable_v<void(int &), const int &>);
	static_assert(!lak::is_invocable_v<void(int &&), const int &>);
	static_assert(!lak::is_invocable_v<decltype([](int &&) {}), const int &>);
	static_assert(lak::is_invocable_v<void(int), int>);
	static_assert(lak::is_invocable_v<void()>);

	/* --- has_type_size_signature --- */

	template<typename T>
	struct has_type_size_signature : lak::false_type
	{
	};

	template<template<typename, size_t> typename T, typename U, size_t S>
	struct has_type_size_signature<T<U, S>> : lak::true_type
	{
		static constexpr size_t value = S;
	};

	template<typename T>
	static constexpr bool has_type_size_signature_v =
	  lak::has_type_size_signature<T>::value;

	template<typename T>
	requires lak::has_type_size_signature_v<T>
	static constexpr size_t type_size_signature_size_v =
	  lak::has_type_size_signature<T>::value;
}

#endif
