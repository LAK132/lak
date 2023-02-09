#ifndef LAK_TUPLE_HPP
#define LAK_TUPLE_HPP

#include "lak/type_traits.hpp"
#include "lak/utility.hpp"

#ifndef LAK_NO_STD
#	include <tuple>
#endif

namespace lak
{
#ifdef LAK_NO_STD
#	define LAK_TUPLE_ELEMENT lak::tuple_element
#	define LAK_TUPLE_SIZE    lak::tuple_size
	template<size_t I, typename T>
	struct tuple_element;
	template<typename T>
	struct tuple_size;
#else
#	define LAK_TUPLE_ELEMENT std::tuple_element
#	define LAK_TUPLE_SIZE    std::tuple_size
	template<size_t I, typename T>
	struct tuple_element : std::tuple_element<I, T>
	{
	};
	template<typename T>
	struct tuple_size : std::tuple_size<T>
	{
	};
#endif

	template<size_t I, typename T>
	using tuple_element_t = typename lak::tuple_element<I, T>::type;

	template<typename T>
	constexpr inline size_t tuple_size_v = lak::tuple_size<T>::value;

	template<typename... T>
	struct tuple;
}

#include "lak/type_pack.hpp"

namespace lak
{
	/* --- pair --- */

	template<typename T, typename U>
	struct pair
	{
		T first;
		U second;

		pair() = default;
		pair(const pair &p);
		pair(pair &&p);

		template<typename V, typename W>
		pair(V &&f, W &&s);

		pair &operator=(const pair &p);
		pair &operator=(pair &&p);

		template<typename... V>
		pair &operator=(const tuple<V...> &p);
		template<typename... V>
		pair &operator=(tuple<V...> &&p);

		template<size_t I>
		auto &get();
		template<size_t I>
		auto &get() const;
	};

	template<typename T, typename U>
	pair(const T &, const U &) -> pair<T, U>;

	template<typename T, typename U>
	struct is_lak_type<lak::pair<T, U>> : lak::true_type
	{
	};

	/* --- tuple --- */

	template<typename T, typename... U>
	struct tuple<T, U...>
	{
		T value;
		lak::tuple<U...> next;

		template<typename V>
		static constexpr size_t index_of = lak::index_of_element_v<V, T, U...>;

		tuple() = default;
		tuple(const tuple &p);
		tuple(tuple &&p);

		template<typename V, typename... W>
		explicit tuple(V &&v, W &&...n);

		tuple &operator=(const tuple &p);
		tuple &operator=(tuple &&p);

		template<typename... V>
		tuple &operator=(const pair<V...> &p);
		template<typename... V>
		tuple &operator=(pair<V...> &&p);

		template<typename F>
		auto apply(F &&func);
		template<typename F>
		auto apply(F &&func) const;

		template<typename F>
		void foreach(F &&func);
		template<typename F>
		void foreach(F &&func) const;

		template<size_t I>
		auto &get();
		template<size_t I>
		auto &get() const;
	};

	template<>
	struct tuple<>
	{
	};

	template<typename... T>
	tuple(const T &...) -> tuple<T...>;

	template<typename T>
	struct is_tuple : lak::false_type
	{
	};
	template<typename... T>
	struct is_tuple<lak::tuple<T...>> : lak::true_type
	{
	};
	template<typename T>
	inline constexpr bool is_tuple_v = lak::is_tuple<T>::value;

	template<typename... T>
	struct is_lak_type<lak::tuple<T...>> : lak::true_type
	{
	};

	/* --- free functions --- */

	template<typename... T>
	lak::tuple<T &...> tie(T &...args)
	{
		return lak::tuple<T &...>{args...};
	}

	template<typename... T, typename... U>
	lak::tuple<T..., U...> tuple_cat(lak::tuple<T...> a, lak::tuple<U...> b)
	{
		auto _tuple_cat = [&]<size_t... I, size_t... J>(lak::index_sequence<I...>,
		                                                lak::index_sequence<J...>)
		                    ->lak::tuple<T..., U...>
		{
			static_assert(sizeof...(T) == sizeof...(I));
			static_assert(sizeof...(U) == sizeof...(J));
			return lak::tuple<T..., U...>{lak::forward<T>(a.template get<I>())...,
			                              lak::forward<U>(b.template get<J>())...};
		};

		return _tuple_cat(lak::index_sequence_for<T...>{},
		                  lak::index_sequence_for<U...>{});
	}

	template<typename... T>
	lak::tuple<T &&...> forward_as_tuple(T &&...args)
	{
		return lak::tuple<T &&...>(lak::forward<T>(args)...);
	}
}

/* --- pair --- */

template<typename T, typename U>
struct LAK_TUPLE_SIZE<lak::pair<T, U>>
{
	static constexpr size_t value = 2;
};

template<typename T, typename U>
struct LAK_TUPLE_ELEMENT<0, lak::pair<T, U>>
{
	using type = T;
};

template<typename T, typename U>
struct LAK_TUPLE_ELEMENT<1, lak::pair<T, U>>
{
	using type = U;
};

/* --- tuple --- */

template<typename... T>
struct LAK_TUPLE_SIZE<lak::tuple<T...>>
{
	static constexpr size_t value = sizeof...(T);
};

template<typename T, typename... U>
struct LAK_TUPLE_ELEMENT<0, lak::tuple<T, U...>>
{
	using type = T;
};

template<size_t I, typename T, typename... U>
struct LAK_TUPLE_ELEMENT<I, lak::tuple<T, U...>>
{
	using type = typename LAK_TUPLE_ELEMENT<I - 1, lak::tuple<U...>>::type;
};

#include "lak/tuple.inl"

#endif