#ifndef LAK_TYPE_PACK_HPP
#define LAK_TYPE_PACK_HPP

#include "lak/tuple.hpp"
#include "lak/type_traits.hpp"

namespace lak
{
	/* --- type_pack --- */

	template<typename... TYPES>
	struct type_pack
	{
	};

	/* --- tuple_element --- */

	template<size_t I, typename T, typename... U>
	struct tuple_element<I, lak::type_pack<T, U...>>
	: public lak::tuple_element<I - 1, lak::type_pack<U...>>
	{
	};
	template<typename T, typename... U>
	struct tuple_element<0, lak::type_pack<T, U...>>
	: public lak::type_identity<T>
	{
	};

	/* --- index_of_element --- */

	template<size_t I, typename T, typename... U>
	struct _index_of_element;
	template<size_t I, typename T, typename... U>
	struct _index_of_element<I, T, T, U...> : public lak::size_type<I>
	{
	};
	template<size_t I, typename T, typename U, typename... V>
	struct _index_of_element<I, T, U, V...>
	: public lak::_index_of_element<I + 1, T, V...>
	{
	};
	template<typename T, typename... U>
	struct index_of_element : public lak::_index_of_element<0, T, U...>
	{
	};
	template<typename T, typename... U>
	static constexpr size_t index_of_element_v =
	  lak::index_of_element<T, U...>::value;

	/* --- prepend_to_pack --- */

	template<typename T, typename PACK>
	struct prepend_to_pack;
	template<typename T, typename... TYPES>
	struct prepend_to_pack<T, lak::type_pack<TYPES...>>
	{
		using type = lak::type_pack<T, TYPES...>;
	};
	template<typename T, typename PACK>
	using prepend_to_pack_t = typename lak::prepend_to_pack<T, PACK>::type;

	/* --- append_to_pack --- */

	template<typename PACK, typename T>
	struct append_to_pack;
	template<typename... TYPES, typename T>
	struct append_to_pack<lak::type_pack<TYPES...>, T>
	{
		using type = lak::type_pack<TYPES..., T>;
	};
	template<typename PACK, typename T>
	using append_to_pack_t = typename lak::append_to_pack<PACK, T>::type;

	/* --- concat_packs --- */

	template<typename... PACKS>
	struct concat_packs;
	template<>
	struct concat_packs<>
	{
		using type = lak::type_pack<>;
	};
	template<typename... PACK>
	struct concat_packs<lak::type_pack<PACK...>>
	{
		using type = lak::type_pack<PACK...>;
	};
	template<typename... PACK1, typename... PACK2>
	struct concat_packs<lak::type_pack<PACK1...>, lak::type_pack<PACK2...>>
	{
		using type = lak::type_pack<PACK1..., PACK2...>;
	};
	template<typename... PACK1, typename... PACK2, typename... PACKS>
	struct concat_packs<lak::type_pack<PACK1...>,
	                    lak::type_pack<PACK2...>,
	                    PACKS...>
	{
		using type = typename lak::concat_packs<lak::type_pack<PACK1..., PACK2...>,
		                                        PACKS...>::type;
	};
	template<typename... PACKS>
	using concat_packs_t = typename lak::concat_packs<PACKS...>::type;

	/* --- flatten_pack --- */

	// pack<pack<T...>, pack<U...>> -> pack<T..., U...>

	template<typename PACK>
	struct flatten_pack;
	template<typename... PACKS>
	struct flatten_pack<lak::type_pack<PACKS...>>
	{
		using type = lak::concat_packs_t<PACKS...>;
	};
	template<typename PACK>
	using flatten_pack_t = typename lak::flatten_pack<PACK>::type;

	/* --- remove_from_pack --- */

	template<typename T, typename PACK>
	struct remove_from_pack;
	template<typename T>
	struct remove_from_pack<T, lak::type_pack<>>
	{
		using type = lak::type_pack<>;
	};
	template<typename T, typename T1, typename... TYPES>
	struct remove_from_pack<T, lak::type_pack<T1, TYPES...>>
	{
		using next =
		  typename lak::remove_from_pack<T, lak::type_pack<TYPES...>>::type;
		using type = lak::conditional_t<lak::is_same_v<T, T1>,
		                                next,
		                                lak::prepend_to_pack_t<T1, next>>;
	};
	template<typename T, typename PACK>
	using remove_from_pack_t = typename lak::remove_from_pack<T, PACK>::type;

	/* --- unique_pack --- */

	template<typename PACK>
	struct unique_pack;
	template<>
	struct unique_pack<lak::type_pack<>>
	{
		using type = lak::type_pack<>;
	};
	template<typename T>
	struct unique_pack<lak::type_pack<T>>
	{
		using type = lak::type_pack<T>;
	};
	template<typename T1, typename... TYPES>
	struct unique_pack<lak::type_pack<T1, TYPES...>>
	{
		using type = lak::prepend_to_pack_t<
		  T1,
		  lak::remove_from_pack_t<
		    T1,
		    typename lak::unique_pack<lak::type_pack<TYPES...>>::type>>;
	};
	template<typename PACK>
	using unique_pack_t = typename lak::unique_pack<PACK>::type;
	template<typename... T>
	using make_unique_pack_t = lak::unique_pack_t<lak::type_pack<T...>>;

	/* --- pack_apply --- */

	template<template<typename> typename T, typename PACK>
	struct pack_apply;
	template<template<typename> typename T, typename... TYPES>
	struct pack_apply<T, lak::type_pack<TYPES...>>
	{
		using type = lak::type_pack<typename T<TYPES>::type...>;
	};
	template<template<typename...> typename T, typename PACK>
	using pack_apply_t = typename lak::pack_apply<T, PACK>::type;

	/* --- pack_from_function --- */

	template<typename FUNC>
	struct pack_from_function;
	template<typename FUNC>
	struct pack_from_function<FUNC *>
	{
		using type = typename lak::pack_from_function<FUNC>::type;
	};
	template<typename RET, typename... ARGS>
	struct pack_from_function<RET(ARGS...)>
	{
		using type = lak::type_pack<RET, ARGS...>;
	};
	template<typename FUNC>
	using pack_from_function_t = typename lak::pack_from_function<FUNC>::type;

	/* --- create_from_pack --- */

	template<template<typename...> typename T, typename PACK>
	struct create_from_pack;
	template<template<typename...> typename T, typename... TYPES>
	struct create_from_pack<T, lak::type_pack<TYPES...>>
	{
		using type = T<TYPES...>;
	};
	template<template<typename...> typename T, typename PACK>
	using create_from_pack_t = typename lak::create_from_pack<T, PACK>::type;

	/* --- does_pack_contain --- */

	template<typename PACK, typename T>
	struct does_pack_contain;
	template<typename T>
	struct does_pack_contain<lak::type_pack<>, T> : public lak::false_type
	{
	};
	template<typename TYPE, typename T>
	struct does_pack_contain<lak::type_pack<TYPE>, T>
	: public lak::bool_type<lak::is_same_v<TYPE, T>>
	{
	};
	template<typename TYPE, typename... TYPES, typename T>
	struct does_pack_contain<lak::type_pack<TYPE, TYPES...>, T>
	: public lak::bool_type<
	    lak::is_same_v<TYPE, T> ||
	    lak::does_pack_contain<lak::type_pack<TYPES...>, T>::value>
	{
	};
	template<typename PACK, typename T>
	static constexpr bool does_pack_contain_v =
	  lak::does_pack_contain<PACK, T>::value;

	/* --- sizeof_pack_args --- */

	template<typename PACK>
	struct sizeof_pack_args;
	template<typename T, typename... U>
	struct sizeof_pack_args<lak::type_pack<T, U...>>
	{
		static constexpr size_t value =
		  sizeof(T) + sizeof_pack_args<lak::type_pack<U...>>::value;
	};
	template<>
	struct sizeof_pack_args<lak::type_pack<>>
	{
		static constexpr size_t value = 0;
	};
	template<typename PACK>
	inline constexpr size_t sizeof_pack_args_v = sizeof_pack_args<PACK>::value;
}

#endif
