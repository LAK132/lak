#ifndef LAK_TUPLE_HPP
#define LAK_TUPLE_HPP

#include "lak/type_traits.hpp"
#include "lak/utility.hpp"

#include <tuple>

namespace lak
{
	template<size_t I, typename T>
	struct tuple_element : std::tuple_element<I, T>
	{
	};
	template<typename T>
	struct tuple_size : std::tuple_size<T>
	{
	};

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

		constexpr pair() = default;
		constexpr pair(const pair &p);
		constexpr pair(pair &&p);

		template<typename V, typename W>
		constexpr pair(V &&f, W &&s);

		constexpr pair &operator=(const pair &p);
		constexpr pair &operator=(pair &&p);

		template<typename... V>
		constexpr pair &operator=(const tuple<V...> &p);
		template<typename... V>
		constexpr pair &operator=(tuple<V...> &&p);

		template<size_t I>
		constexpr auto &get();
		template<size_t I>
		constexpr auto &get() const;
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
		template<size_t I>
		using type_of = lak::nth_type_t<I, T, U...>;

		constexpr tuple() = default;
		constexpr tuple(const tuple &p);
		constexpr tuple(tuple &&p);

		template<typename V, typename... W>
		constexpr explicit tuple(V &&v, W &&...n);

		constexpr tuple &operator=(const tuple &p);
		constexpr tuple &operator=(tuple &&p);

		template<typename... V>
		constexpr tuple &operator=(const pair<V...> &p);
		template<typename... V>
		constexpr tuple &operator=(pair<V...> &&p);

		template<typename F>
		auto apply(F &&func);
		template<typename F>
		auto apply(F &&func) const;

		template<typename F>
		void foreach (F &&func);
		template<typename F>
		void foreach (F &&func) const;

		// func(lak::size_type<I>{}, lak::type_identity<T>{})
		template<typename F>
		static constexpr void foreach_type(F &&func);
		template<size_t I, typename F>
		static constexpr force_inline void _foreach_type(F &&func);

		template<size_t I>
		constexpr auto &get();
		template<size_t I>
		constexpr auto &get() const;
	};

	template<>
	struct tuple<>
	{
		template<typename F>
		auto apply(F &&func)
		{
			return func();
		}
		template<typename F>
		auto apply(F &&func) const
		{
			return func();
		}

		template<typename F>
		void foreach (F &&)
		{
		}
		template<typename F>
		void foreach (F &&) const
		{
		}

		template<typename F>
		static constexpr void foreach_type(F &&)
		{
		}
		template<size_t I, typename F>
		static constexpr void _foreach_type(F &&)
		{
		}
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
		auto _tuple_cat = [&]<size_t... I, size_t... J>(
		                    lak::index_sequence<I...>,
		                    lak::index_sequence<J...>) -> lak::tuple<T..., U...>
		{
			static_assert(sizeof...(T) == sizeof...(I));
			static_assert(sizeof...(U) == sizeof...(J));
			if constexpr (sizeof...(T) == 0 && sizeof...(U) == 0)
				return lak::tuple<T..., U...>{};
			else if constexpr (sizeof...(T) == 0)
				return lak::tuple<T..., U...>{lak::forward<U>(b.template get<J>())...};
			else if constexpr (sizeof...(U) == 0)
				return lak::tuple<T..., U...>{lak::forward<T>(a.template get<I>())...};
			else
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
struct std::tuple_size<lak::pair<T, U>>
{
	static constexpr size_t value = 2;
};

template<typename T, typename U>
struct std::tuple_element<0, lak::pair<T, U>>
{
	using type = T;
};

template<typename T, typename U>
struct std::tuple_element<1, lak::pair<T, U>>
{
	using type = U;
};

/* --- tuple --- */

template<typename... T>
struct std::tuple_size<lak::tuple<T...>>
{
	static constexpr size_t value = sizeof...(T);
};

template<typename T, typename... U>
struct std::tuple_element<0, lak::tuple<T, U...>>
{
	using type = T;
};

template<size_t I, typename T, typename... U>
struct std::tuple_element<I, lak::tuple<T, U...>>
{
	using type = typename std::tuple_element<I - 1, lak::tuple<U...>>::type;
};

#include "lak/tuple.inl"

#endif