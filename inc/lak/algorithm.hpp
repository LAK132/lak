#ifndef LAK_ALGORITHM_HPP
#define LAK_ALGORITHM_HPP

#include "lak/concepts.hpp"
#include "lak/utility.hpp"

namespace lak
{
	template<typename IN_ITER, typename OUT_ITER>
	OUT_ITER move(IN_ITER begin, IN_ITER end, OUT_ITER output)
	{
		for (; begin != end; ++begin, ++output) *output = lak::move(*begin);
		return output;
	}

	template<typename IN_ITER, typename OUT_ITER>
	OUT_ITER copy(IN_ITER begin, IN_ITER end, OUT_ITER output)
	{
		for (; begin != end; ++begin, ++output) *output = *begin;
		return output;
	}

	template<typename T, lak::concepts::member_pointer auto... P>
	struct less
	{
		static_assert(sizeof...(P) > 0);

		constexpr inline bool operator()(const T &a,
		                                 const T &b,
		                                 lak::index_sequence<0>) const
		{
			return a.*(lak::nth_value_v<0, P...>) < b.*(lak::nth_value_v<0, P...>);
		}

		template<size_t... I>
		constexpr inline bool operator()(const T &a,
		                                 const T &b,
		                                 lak::index_sequence<0, I...>) const
		{
			return a.*(lak::nth_value_v<0, P...>) < b.*(lak::nth_value_v<0, P...>) ||
			       ((!(b.*(lak::nth_value_v<I - 1, P...>) <
			           a.*(lak::nth_value_v<I - 1, P...>)) &&
			         a.*(lak::nth_value_v<I, P...>) <
			           b.*(lak::nth_value_v<I, P...>)) ||
			        ...);
		}

		constexpr inline bool operator()(const T &a, const T &b) const
		{
			return (*this)(a, b, lak::make_index_sequence<sizeof...(P)>{});
		}
	};
}

#endif
