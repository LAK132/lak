#ifndef LAK_FUNCTIONAL_HPP
#define LAK_FUNCTIONAL_HPP

#include "lak/concepts.hpp"
#include "lak/utility.hpp"

namespace lak
{
	/* --- equal_to --- */

#ifdef LAK_COMPILER_MSVC
	template<typename T = void, auto... P>
#else
	template<typename T = void, lak::concepts::member_pointer auto... P>
#endif
	struct equal_to
	{
#ifdef LAK_COMPILER_MSVC
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#endif

		template<size_t... Is>
		constexpr force_inline bool operator()(const T &lhs,
		                                       const T &rhs,
		                                       lak::index_sequence<Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const T &lhs, const U &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const U &lhs, const T &rhs) const;
	};

	template<>
	struct equal_to<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>) //
		  constexpr auto
		  operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) == lak::forward<U>(rhs));
	};

	template<typename T>
	struct equal_to<T>
	{
		constexpr inline bool operator()(const T &lhs, const T &rhs) const;
	};

	template<typename T>
	struct equal_to<T *>
	{
		constexpr inline bool operator()(const T *lhs, const T *rhs) const;
	};

	/* --- not_equal_to --- */

#ifdef LAK_COMPILER_MSVC
	template<typename T = void, auto... P>
#else
	template<typename T = void, lak::concepts::member_pointer auto... P>
#endif
	struct not_equal_to
	{
#ifdef LAK_COMPILER_MSVC
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#endif

		template<size_t... Is>
		constexpr force_inline bool operator()(const T &lhs,
		                                       const T &rhs,
		                                       lak::index_sequence<Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const T &lhs, const U &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const U &lhs, const T &rhs) const;
	};

	template<>
	struct not_equal_to<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>) //
		  constexpr auto
		  operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) != lak::forward<U>(rhs));
	};

	template<typename T>
	struct not_equal_to<T>
	{
		constexpr inline bool operator()(const T &lhs, const T &rhs) const;
	};

	template<typename T>
	struct not_equal_to<T *>
	{
		constexpr inline bool operator()(const T *lhs, const T *rhs) const;
	};

	/* --- less --- */

#ifdef LAK_COMPILER_MSVC
	template<typename T = void, auto... P>
#else
	template<typename T = void, lak::concepts::member_pointer auto... P>
#endif
	struct less
	{
#ifdef LAK_COMPILER_MSVC
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#endif

		template<size_t I, size_t... Is>
		constexpr force_inline bool operator()(
		  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const T &lhs, const U &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const U &lhs, const T &rhs) const;
	};

	template<>
	struct less<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>) //
		  constexpr auto
		  operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) < lak::forward<U>(rhs));
	};

	template<typename T>
	struct less<T>
	{
		constexpr inline bool operator()(const T &lhs, const T &rhs) const;
	};

	template<typename T>
	struct less<T *>
	{
		constexpr inline bool operator()(const T *lhs, const T *rhs) const;
	};

	/* --- greater --- */

#ifdef LAK_COMPILER_MSVC
	template<typename T = void, auto... P>
#else
	template<typename T = void, lak::concepts::member_pointer auto... P>
#endif
	struct greater
	{
#ifdef LAK_COMPILER_MSVC
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#endif

		template<size_t I, size_t... Is>
		constexpr force_inline bool operator()(
		  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const T &lhs, const U &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const U &lhs, const T &rhs) const;
	};

	template<>
	struct greater<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>) //
		  constexpr auto
		  operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) > lak::forward<U>(rhs));
	};

	template<typename T>
	struct greater<T>
	{
		constexpr inline bool operator()(const T &lhs, const T &rhs) const;
	};

	template<typename T>
	struct greater<T *>
	{
		constexpr inline bool operator()(const T *lhs, const T *rhs) const;
	};

	/* --- less_equal --- */

#ifdef LAK_COMPILER_MSVC
	template<typename T = void, auto... P>
#else
	template<typename T = void, lak::concepts::member_pointer auto... P>
#endif
	struct less_equal
	{
#ifdef LAK_COMPILER_MSVC
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#endif

		template<size_t I, size_t... Is>
		constexpr force_inline bool operator()(
		  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const T &lhs, const U &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const U &lhs, const T &rhs) const;
	};

	template<>
	struct less_equal<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>) //
		  constexpr auto
		  operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) <= lak::forward<U>(rhs));
	};

	template<typename T>
	struct less_equal<T>
	{
		constexpr inline bool operator()(const T &lhs, const T &rhs) const;
	};

	template<typename T>
	struct less_equal<T *>
	{
		constexpr inline bool operator()(const T *lhs, const T *rhs) const;
	};

	/* --- greater_equal --- */

#ifdef LAK_COMPILER_MSVC
	template<typename T = void, auto... P>
#else
	template<typename T = void, lak::concepts::member_pointer auto... P>
#endif
	struct greater_equal
	{
#ifdef LAK_COMPILER_MSVC
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#endif

		template<size_t I, size_t... Is>
		constexpr force_inline bool operator()(
		  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const T &lhs, const U &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1) //
		  constexpr inline bool
		  operator()(const U &lhs, const T &rhs) const;
	};

	template<>
	struct greater_equal<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>) //
		  constexpr auto
		  operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) >= lak::forward<U>(rhs));
	};

	template<typename T>
	struct greater_equal<T>
	{
		constexpr inline bool operator()(const T &lhs, const T &rhs) const;
	};

	template<typename T>
	struct greater_equal<T *>
	{
		constexpr inline bool operator()(const T *lhs, const T *rhs) const;
	};
}

#include "lak/functional.inl"

#endif
