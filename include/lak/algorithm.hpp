#ifndef LAK_ALGORITHM_HPP
#define LAK_ALGORITHM_HPP

#include "lak/tuple.hpp"

#define LAK_FUNCTIONAL_FORWARD_ONLY
#include "lak/functional.hpp"

#define LAK_SPAN_FORWARD_ONLY
#include "lak/span.hpp"

#include <concepts>
#include <iterator>

namespace lak
{
	/* --- move --- */

	template<std::input_iterator IN_ITER,
	         std::output_iterator<
	           typename std::iterator_traits<IN_ITER>::value_type> OUT_ITER>
	OUT_ITER move(IN_ITER begin, IN_ITER end, OUT_ITER output);

	/* --- copy --- */

	template<std::input_iterator IN_ITER,
	         std::output_iterator<
	           typename std::iterator_traits<IN_ITER>::value_type> OUT_ITER>
	OUT_ITER copy(IN_ITER begin, IN_ITER end, OUT_ITER output);

	template<std::input_iterator IN_ITER,
	         std::output_iterator<
	           typename std::iterator_traits<IN_ITER>::value_type> OUT_ITER>
	requires std::equality_comparable<OUT_ITER>
	OUT_ITER copy(IN_ITER begin,
	              IN_ITER end,
	              OUT_ITER out_begin,
	              OUT_ITER out_end);

	/* --- swap --- */

	template<std::forward_iterator ITER_A, std::forward_iterator ITER_B>
	lak::pair<ITER_A, ITER_B> swap(ITER_A begin_a,
	                               ITER_A end_a,
	                               ITER_B begin_b,
	                               ITER_B end_b);

	/* --- count --- */

	template<std::forward_iterator ITER, typename T>
	size_t count(ITER begin, ITER end, const T &value);

	/* --- distance --- */

	template<std::input_iterator ITER>
	typename std::iterator_traits<ITER>::difference_type distance(ITER begin,
	                                                              ITER end);

	/* --- advance --- */

	template<std::input_iterator ITER>
	void advance(ITER &it,
	             typename std::iterator_traits<ITER>::difference_type offset);

	/* --- next --- */

	template<std::input_iterator ITER>
	ITER next(ITER it,
	          typename std::iterator_traits<ITER>::difference_type offset = 1);

	/* --- find --- */

	template<std::forward_iterator ITER, typename T>
	ITER find(ITER begin, ITER end, const T &value);

	/* --- find_if --- */

	template<std::forward_iterator ITER>
	ITER find_if(ITER begin, ITER end, auto predicate);

	/* --- any_of --- */

	template<std::forward_iterator ITER>
	bool any_of(ITER begin, ITER end, auto predicate);

	/* --- all_of --- */

	template<std::forward_iterator ITER>
	bool all_of(ITER begin, ITER end, auto predicate);

	/* --- none_of --- */

	template<std::forward_iterator ITER>
	bool none_of(ITER begin, ITER end, auto predicate);

	/* --- mismatch --- */

	template<std::forward_iterator ITER_A, std::forward_iterator ITER_B>
	lak::pair<ITER_A, ITER_B> mismatch(ITER_A begin_a,
	                                   ITER_A end_a,
	                                   ITER_B begin_b,
	                                   ITER_B end_b);

	/* --- is_permutation --- */

	template<std::forward_iterator ITER_A, std::forward_iterator ITER_B>
	bool is_permutation(ITER_A begin_a,
	                    ITER_A end_a,
	                    ITER_B begin_b,
	                    ITER_B end_b);

	/* --- rotate_left --- */

	template<std::forward_iterator ITER>
	void rotate_left(ITER begin, ITER end, size_t distance = 1U);

	/* --- rotate_right --- */

	template<std::forward_iterator ITER>
	void rotate_right(ITER begin, ITER end, size_t distance = 1U);

	/* --- reverse --- */

	template<std::bidirectional_iterator ITER>
	void reverse(ITER begin, ITER end);

	/* --- partition --- */

	template<std::forward_iterator ITER>
	ITER partition(ITER begin, ITER end, auto predicate);

	/* --- stable_partition --- */

	template<std::random_access_iterator ITER>
	ITER stable_partition(ITER begin, ITER end, auto predicate);

	/* --- binary_partition --- */

	template<std::forward_iterator ITER, typename CMP = lak::less<>>
	ITER binary_partition(ITER begin, ITER mid, ITER end, CMP compare = {});

	/* --- mark_and_sweep_parition --- */

	template<typename T,
	         std::forward_iterator ITER,
	         typename CMP = lak::less<T *>>
	ITER mark_and_sweep_parition(
	  T *root, auto sweep, ITER begin, ITER end, auto transform);

	/* --- dedup --- */

	template<std::forward_iterator ITER, typename CMP = lak::not_equal_to<>>
	ITER dedup(ITER begin, ITER end, CMP compare = {});

	/* --- stable_dedup --- */

	template<std::random_access_iterator ITER,
	         typename CMP = lak::not_equal_to<>>
	ITER stable_dedup(ITER begin, ITER end, CMP compare = {});

	/* --- merge --- */

	// Merge the non-descending (*(it+1) >= *it) sorted ranges [begin, mid) and
	// [mid, end) into a single sorted range

	template<std::forward_iterator ITER, typename CMP = lak::less<>>
	ITER merge(ITER begin, ITER mid, ITER end, CMP compare = {});

	// Merge the non-descending (*(it+1) >= *it) sorted streams [begin_a, end_a)
	// and [begin_b, end_b) into a single sorted output range

	template<std::forward_iterator ITER_A,
	         std::forward_iterator ITER_B,
	         typename ITER_OUT,
	         typename CMP = lak::less<>>
	requires(
	  std::output_iterator<typename std::iterator_traits<ITER_A>::value_type> &&
	  std::output_iterator<typename std::iterator_traits<ITER_B>::value_type>)
	ITER_OUT merge(ITER_A begin_a,
	               ITER_A end_a,
	               ITER_B begin_b,
	               ITER_B end_b,
	               ITER_OUT output,
	               CMP compare = {});

	/* --- binary_tree_is_left_child --- */

	constexpr inline bool binary_tree_is_left_child(size_t child);

	template<std::random_access_iterator ITER>
	bool binary_tree_is_left_child(ITER root, ITER child);

	/* --- binary_tree_left_child --- */

	constexpr inline size_t binary_tree_left_child(size_t parent);

	template<std::random_access_iterator ITER>
	ITER binary_tree_left_child(ITER root, ITER parent);

	/* --- binary_tree_right_child --- */

	constexpr inline size_t binary_tree_right_child(size_t parent);

	template<std::random_access_iterator ITER>
	ITER binary_tree_right_child(ITER root, ITER parent);

	/* --- binary_tree_parent --- */

	constexpr inline size_t binary_tree_parent(size_t child);

	template<std::random_access_iterator ITER>
	ITER binary_tree_parent(ITER root, ITER child);

	/* --- make_heap --- */

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	void make_heap(ITER begin, ITER end, CMP compare = {});

	/* --- is_heap --- */

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	bool is_heap(ITER begin, ITER end, CMP compare = {});

	/* --- is_heap_until --- */

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	ITER is_heap_until(ITER begin, ITER end, CMP compare = {});

	/* --- sift_down_heap --- */

	// Precondition: to_sift's children form valid heaps

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	void sift_down_heap(ITER begin, ITER to_sift, ITER end, CMP compare = {});

	/* --- sift_up_heap --- */

	// Precondition: to_sift's parents form a valid heap

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	void sift_up_heap(ITER begin, ITER to_sift, CMP compare = {});

	/* --- push_heap --- */

	// Push end-1 into the heap [begin, end-1)

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	void push_heap(ITER begin, ITER end, CMP compare = {});

	/* --- pop_heap --- */

	// Move begin to end-1 and repair heap [begin, end-1)

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	void pop_heap(ITER begin, ITER end, CMP compare = {});

	/* --- sort_heap --- */

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	void sort_heap(ITER begin, ITER end, CMP compare = {});

	/* --- reverse_sort_heap --- */

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	void reverse_sort_heap(ITER begin, ITER end, CMP compare = {});

	/* --- breadth_first_search_heap --- */

	template<std::input_iterator ITER, typename F>
	ITER breadth_first_search_heap(ITER begin, ITER end, F &&predicate);

	/* --- depth_first_search_heap --- */

	template<std::random_access_iterator ITER, typename F>
	ITER depth_first_search_heap(ITER begin, ITER end, F &&predicate);

	/* --- heapsort --- */

	template<std::random_access_iterator ITER, typename CMP = lak::less<>>
	void heapsort(ITER begin, ITER end, CMP compare = {});

	/* --- partial_order_sort --- */

	// Works in situations where a<b is ordered but !(a<b) is unordered

	template<std::forward_iterator ITER, typename CMP = lak::less<>>
	void partial_order_sort(ITER begin, ITER end, CMP compare = {});

	/* --- minmax_element --- */

	template<std::forward_iterator ITER, typename CMP = lak::less<>>
	lak::pair<ITER, ITER> minmax_element(ITER begin, ITER end, CMP compare = {});

	/* --- max_element --- */

	template<std::forward_iterator ITER, typename CMP = lak::less<>>
	ITER max_element(ITER begin, ITER end, CMP compare = {});

	/* --- min_element --- */

	template<std::forward_iterator ITER, typename CMP = lak::less<>>
	ITER min_element(ITER begin, ITER end, CMP compare = {});

	/* --- lower_bound --- */

	template<std::forward_iterator ITER, typename U, typename CMP = lak::less<>>
	ITER lower_bound(ITER begin, ITER end, const U &value, CMP compare = {});

	/* --- upper_bound --- */

	template<std::forward_iterator ITER, typename U, typename CMP = lak::less<>>
	ITER upper_bound(ITER begin, ITER end, const U &value, CMP compare = {});

	/* --- equal_range --- */

	template<typename T, typename U = T, typename CMP = lak::less<>>
	lak::span<T> equal_range(T *begin, T *end, const U &value, CMP compare = {});
}

#include "lak/algorithm.inl"

#endif
