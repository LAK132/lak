#ifndef LAK_ALGORITHM_HPP
#define LAK_ALGORITHM_HPP

#include "lak/tuple.hpp"

#define LAK_FUNCTIONAL_FORWARD_ONLY
#include "lak/functional.hpp"

#define LAK_SPAN_FORWARD_ONLY
#include "lak/span.hpp"

namespace lak
{
	/* --- move --- */

	template<typename IN_ITER, typename OUT_ITER>
	OUT_ITER move(IN_ITER begin, IN_ITER end, OUT_ITER output);

	/* --- copy --- */

	template<typename IN_ITER, typename OUT_ITER>
	OUT_ITER copy(IN_ITER begin, IN_ITER end, OUT_ITER output);

	/* --- rotate_left --- */

	template<typename T>
	void rotate_left(T *begin, T *end, size_t distance = 1U);

	/* --- rotate_right --- */

	template<typename T>
	void rotate_right(T *begin, T *end, size_t distance = 1U);

	/* --- partition --- */

	template<typename ITER>
	ITER partition(ITER begin, ITER end, auto predicate);

	/* --- stable_partition --- */

	template<typename T>
	T *stable_partition(T *begin, T *end, auto predicate);

	/* --- mark_and_sweep_parition --- */

	template<typename T, typename ITER, typename CMP = lak::less<T *>>
	ITER mark_and_sweep_parition(
	  T *root, auto sweep, ITER begin, ITER end, auto transform);

	/* --- lower_bound --- */

	template<typename T, typename U = T>
	T *lower_bound(T *begin,
	               T *end,
	               const U &value,
	               auto compare = lak::less<>{});

	/* --- upper_bound --- */

	template<typename T, typename U = T>
	T *upper_bound(T *begin,
	               T *end,
	               const U &value,
	               auto compare = lak::less<>{});

	/* --- equal_range --- */

	template<typename T, typename U = T>
	lak::span<T> equal_range(T *begin,
	                         T *end,
	                         const U &value,
	                         auto compare = lak::less<>{});
}

#include "lak/algorithm.inl"

#endif
