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

	/* --- advance --- */

	template<typename ITER>
	void advance(ITER &it,
	             typename std::iterator_traits<ITER>::difference_type offset);

	/* --- next --- */

	template<typename ITER>
	ITER next(ITER it,
	          typename std::iterator_traits<ITER>::difference_type offset = 1);

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

	template<typename ITER>
	ITER stable_partition(ITER begin, ITER end, auto predicate);

	/* --- binary_partition --- */

	template<typename ITER, typename CMP = lak::less<>>
	ITER binary_partition(ITER begin, ITER mid, ITER end, CMP cmp = {});


	/* --- mark_and_sweep_parition --- */

	template<typename T, typename ITER, typename CMP = lak::less<T *>>
	ITER mark_and_sweep_parition(
	  T *root, auto sweep, ITER begin, ITER end, auto transform);

	/* --- merge --- */

	// Merge the sorted ranges [begin, mid) and [mid, end) into a single sorted
	// range

	template<typename ITER, typename CMP = lak::less<>>
	ITER merge(ITER begin, ITER mid, ITER end, CMP cmp = {});

	/* --- heapsort --- */

	template<typename ITER, typename CMP = lak::less<>>
	void heapsort(ITER begin, ITER end, CMP compare = {});

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

	template<typename ITER, typename U, typename CMP = lak::less<>>
	ITER lower_bound(ITER begin, ITER end, const U &value, CMP compare = {});

	/* --- upper_bound --- */

	template<typename ITER, typename U, typename CMP = lak::less<>>
	ITER upper_bound(ITER begin, ITER end, const U &value, CMP compare = {});

	/* --- equal_range --- */

	template<typename T, typename U = T, typename CMP = lak::less<>>
	lak::span<T> equal_range(T *begin, T *end, const U &value, CMP compare = {});
}

#include "lak/algorithm.inl"

#endif
