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

	/* --- swap --- */

	template<typename ITER_A, typename ITER_B>
	lak::pair<ITER_A, ITER_B> swap(ITER_A begin_a,
	                               ITER_A end_a,
	                               ITER_B begin_b,
	                               ITER_B end_b);

	/* --- count --- */

	template<typename ITER, typename T>
	size_t count(ITER begin, ITER end, const T &value);

	/* --- distance --- */

	template<typename ITER>
	typename std::iterator_traits<ITER>::difference_type distance(ITER begin,
	                                                              ITER end);

	/* --- find --- */

	template<typename ITER, typename T>
	ITER find(ITER begin, ITER end, const T &value);

	/* --- mismatch --- */

	template<typename ITER_A, typename ITER_B>
	lak::pair<ITER_A, ITER_B> mismatch(ITER_A begin_a,
	                                   ITER_A end_a,
	                                   ITER_B begin_b,
	                                   ITER_B end_b);

	/* --- is_permutation --- */

	template<typename ITER_A, typename ITER_B>
	bool is_permutation(ITER_A begin_a,
	                    ITER_A end_a,
	                    ITER_B begin_b,
	                    ITER_B end_b);

	/* --- rotate_left --- */

	template<typename ITER>
	void rotate_left(ITER begin, ITER end, size_t distance = 1U);

	/* --- rotate_right --- */

	template<typename ITER>
	void rotate_right(ITER begin, ITER end, size_t distance = 1U);

	/* --- reverse --- */

	template<typename ITER>
	void reverse(ITER begin, ITER end);

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

	/* --- minmax_element --- */

	template<typename ITER, typename CMP = lak::less<>>
	lak::pair<ITER, ITER> minmax_element(ITER begin, ITER end, CMP compare = {});

	/* --- max_element --- */

	template<typename ITER, typename CMP = lak::less<>>
	ITER max_element(ITER begin, ITER end, CMP compare = {});

	/* --- min_element --- */

	template<typename ITER, typename CMP = lak::less<>>
	ITER min_element(ITER begin, ITER end, CMP compare = {});

	/* --- lower_bound --- */

	template<typename T, typename U = T, typename CMP = lak::less<>>
	T *lower_bound(T *begin, T *end, const U &value, CMP compare = {});

	/* --- upper_bound --- */

	template<typename T, typename U = T, typename CMP = lak::less<>>
	T *upper_bound(T *begin, T *end, const U &value, CMP compare = {});

	/* --- equal_range --- */

	template<typename T, typename U = T, typename CMP = lak::less<>>
	lak::span<T> equal_range(T *begin, T *end, const U &value, CMP compare = {});
}

#include "lak/algorithm.inl"

#endif
