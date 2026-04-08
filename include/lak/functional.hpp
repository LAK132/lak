#ifndef LAK_FUNCTIONAL_HPP
#	define LAK_FUNCTIONAL_HPP

#	include "lak/concepts.hpp"
#	include "lak/type_pack.hpp"

#	include <functional>

#	ifndef LAK_COMPILER_MSVC
#		define LAK_CAN_USE_CONCEPT_AUTO
#	endif

namespace lak
{
	/* --- bind_front --- */

	template<typename F, typename... ARGS>
	auto bind_front(F &&f, ARGS &&...args)
	{
		return std::bind_front(lak::forward<F>(f), lak::forward<ARGS>(args)...);
	}

	/* --- equal_to --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct equal_to;

	/* --- not_equal_to --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct not_equal_to;

	/* --- less --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct less;

	/* --- greater --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct greater;

	/* --- less_equal --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct less_equal;

	/* --- greater_equal --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct greater_equal;

	/* --- plus --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct plus;

	/* --- minus --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct minus;

	/* --- multiplies --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct multiplies;

	/* --- divides --- */

#	ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T = void, lak::concepts::member_pointer auto... P>
#	else
	template<typename T = void, auto... P>
#	endif
	struct divides;

}

#endif

#ifdef LAK_FUNCTIONAL_FORWARD_ONLY
#	undef LAK_FUNCTIONAL_FORWARD_ONLY
#else
#	ifndef LAK_FUNCTIONAL_HPP_IMPL
#		define LAK_FUNCTIONAL_HPP_IMPL

#		include "lak/memmanip.hpp"
#		include "lak/utility.hpp"

namespace lak
{
	/* --- equal_to --- */

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto... P>
#		else
	template<typename T, auto... P>
#		endif
	struct equal_to
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#		endif

		template<size_t... Is>
		constexpr force_inline bool operator()(const T &lhs,
		                                       const T &rhs,
		                                       lak::index_sequence<Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const T &lhs, const U &rhs) const
		{
			return lak::equal_to<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
		}

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const U &lhs, const T &rhs) const
		{
			return lak::equal_to<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
		}
	};

	template<>
	struct equal_to<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
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

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto... P>
#		else
	template<typename T, auto... P>
#		endif
	struct not_equal_to
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#		endif

		template<size_t... Is>
		constexpr force_inline bool operator()(const T &lhs,
		                                       const T &rhs,
		                                       lak::index_sequence<Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const T &lhs, const U &rhs) const
		{
			return lak::not_equal_to<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
		}

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const U &lhs, const T &rhs) const
		{
			return lak::not_equal_to<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
		}
	};

	template<>
	struct not_equal_to<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
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

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto... P>
#		else
	template<typename T, auto... P>
#		endif
	struct less
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#		endif

		template<size_t I, size_t... Is>
		constexpr force_inline bool operator()(
		  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const T &lhs, const U &rhs) const
		{
			return lak::less<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
		}

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const U &lhs, const T &rhs) const
		{
			return lak::less<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
		}
	};

	template<>
	struct less<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
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

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto... P>
#		else
	template<typename T, auto... P>
#		endif
	struct greater
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#		endif

		template<size_t I, size_t... Is>
		constexpr force_inline bool operator()(
		  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const T &lhs, const U &rhs) const
		{
			return lak::greater<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
		}

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const U &lhs, const T &rhs) const
		{
			return lak::greater<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
		}
	};

	template<>
	struct greater<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
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

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto... P>
#		else
	template<typename T, auto... P>
#		endif
	struct less_equal
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#		endif

		template<size_t I, size_t... Is>
		constexpr force_inline bool operator()(
		  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const T &lhs, const U &rhs) const
		{
			return lak::less_equal<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
		}

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const U &lhs, const T &rhs) const
		{
			return lak::less_equal<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
		}
	};

	template<>
	struct less_equal<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
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

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto... P>
#		else
	template<typename T, auto... P>
#		endif
	struct greater_equal
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert((lak::is_member_pointer_v<decltype(P)> && ...));
#		endif

		template<size_t I, size_t... Is>
		constexpr force_inline bool operator()(
		  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const;

		constexpr inline bool operator()(const T &lhs, const T &rhs) const;

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const T &lhs, const U &rhs) const
		{
			return lak::greater_equal<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
		}

		template<typename U>
		requires(sizeof...(P) == 1)
		constexpr inline bool operator()(const U &lhs, const T &rhs) const
		{
			return lak::greater_equal<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
		}
	};

	template<>
	struct greater_equal<void>
	{
		template<typename T>
		constexpr bool operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
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

	/* --- plus --- */

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto P>
#		else
	template<typename T, auto P>
#		endif
	struct plus<T, P>
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert(lak::is_member_pointer_v<decltype(P)>);
#		endif

		constexpr inline auto operator()(const T &lhs, const T &rhs) const
		{
			return lak::plus<>{}(lhs.*(P), rhs.*(P));
		}

		template<typename U>
		constexpr inline auto operator()(const T &lhs, const U &rhs) const
		{
			return lak::plus<U>{}(lhs.*(P), rhs);
		}

		template<typename U>
		constexpr inline auto operator()(const U &lhs, const T &rhs) const
		{
			return lak::plus<U>{}(lhs, rhs.*(P));
		}
	};

	template<>
	struct plus<void>
	{
		template<typename T>
		constexpr auto operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) + lak::forward<U>(rhs));
	};

	template<typename T>
	struct plus<T>
	{
		constexpr inline auto operator()(const T &lhs, const T &rhs) const;
	};

	/* --- minus --- */

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto P>
#		else
	template<typename T, auto P>
#		endif
	struct minus<T, P>
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert(lak::is_member_pointer_v<decltype(P)>);
#		endif

		constexpr inline auto operator()(const T &lhs, const T &rhs) const
		{
			return lak::minus<>{}(lhs.*(P), rhs.*(P));
		}

		template<typename U>
		constexpr inline auto operator()(const T &lhs, const U &rhs) const
		{
			return lak::minus<U>{}(lhs.*(P), rhs);
		}

		template<typename U>
		constexpr inline auto operator()(const U &lhs, const T &rhs) const
		{
			return lak::minus<U>{}(lhs, rhs.*(P));
		}
	};

	template<>
	struct minus<void>
	{
		template<typename T>
		constexpr auto operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) - lak::forward<U>(rhs));
	};

	template<typename T>
	struct minus<T>
	{
		constexpr inline auto operator()(const T &lhs, const T &rhs) const;
	};

	/* --- multiplies --- */

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto P>
#		else
	template<typename T, auto P>
#		endif
	struct multiplies<T, P>
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert(lak::is_member_pointer_v<decltype(P)>);
#		endif

		constexpr inline auto operator()(const T &lhs, const T &rhs) const
		{
			return lak::multiplies<>{}(lhs.*(P), rhs.*(P));
		}

		template<typename U>
		constexpr inline auto operator()(const T &lhs, const U &rhs) const
		{
			return lak::multiplies<U>{}(lhs.*(P), rhs);
		}

		template<typename U>
		constexpr inline auto operator()(const U &lhs, const T &rhs) const
		{
			return lak::multiplies<U>{}(lhs, rhs.*(P));
		}
	};

	template<>
	struct multiplies<void>
	{
		template<typename T>
		constexpr auto operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) * lak::forward<U>(rhs));
	};

	template<typename T>
	struct multiplies<T>
	{
		constexpr inline auto operator()(const T &lhs, const T &rhs) const;
	};

	/* --- divides --- */

#		ifdef LAK_CAN_USE_CONCEPT_AUTO
	template<typename T, lak::concepts::member_pointer auto P>
#		else
	template<typename T, auto P>
#		endif
	struct divides<T, P>
	{
#		ifndef LAK_CAN_USE_CONCEPT_AUTO
		static_assert(lak::is_member_pointer_v<decltype(P)>);
#		endif

		constexpr inline auto operator()(const T &lhs, const T &rhs) const
		{
			return lak::divides<>{}(lhs.*(P), rhs.*(P));
		}

		template<typename U>
		constexpr inline auto operator()(const T &lhs, const U &rhs) const
		{
			return lak::divides<U>{}(lhs.*(P), rhs);
		}

		template<typename U>
		constexpr inline auto operator()(const U &lhs, const T &rhs) const
		{
			return lak::divides<U>{}(lhs, rhs.*(P));
		}
	};

	template<>
	struct divides<void>
	{
		template<typename T>
		constexpr auto operator()(const T &lhs, const T &rhs) const;

		template<typename T, typename U>
		requires(!lak::is_same_v<T, U>)
		constexpr auto operator()(T &&lhs, U &&rhs) const
		  -> decltype(lak::forward<T>(lhs) / lak::forward<U>(rhs));
	};

	template<typename T>
	struct divides<T>
	{
		constexpr inline auto operator()(const T &lhs, const T &rhs) const;
	};
}

#		include "lak/functional.inl"

#	endif
#endif
