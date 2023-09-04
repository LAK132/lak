#include "lak/algorithm.hpp"

#include "lak/concepts.hpp"
#include "lak/functional.hpp"
#include "lak/integer_range.hpp"
#include "lak/math.hpp"
#include "lak/ptr_intrin.hpp"
#include "lak/span.hpp"
#include "lak/tuple.hpp"
#include "lak/utility.hpp"

#include <set>
#include <vector>

/* --- move --- */

template<typename IN_ITER, typename OUT_ITER>
OUT_ITER lak::move(IN_ITER begin, IN_ITER end, OUT_ITER output)
{
	for (; begin != end; ++begin, ++output) *output = lak::move(*begin);
	return output;
}

/* --- copy --- */

template<typename IN_ITER, typename OUT_ITER>
OUT_ITER lak::copy(IN_ITER begin, IN_ITER end, OUT_ITER output)
{
	for (; begin != end; ++begin, ++output) *output = *begin;
	return output;
}

template<typename IN_ITER, typename OUT_ITER>
OUT_ITER lak::copy(IN_ITER begin,
                   IN_ITER end,
                   OUT_ITER out_begin,
                   OUT_ITER out_end)
{
	for (; begin != end && out_begin != out_end; ++begin, ++out_begin)
		*out_begin = *begin;
	return out_begin;
}

/* --- swap --- */

template<typename ITER_A, typename ITER_B>
lak::pair<ITER_A, ITER_B> lak::swap(ITER_A begin_a,
                                    ITER_A end_a,
                                    ITER_B begin_b,
                                    ITER_B end_b)
{
	for (; begin_a != end_a && begin_b != end_b; ++begin_a, ++begin_b)
		lak::swap(*begin_a, *begin_b);
	return {begin_a, begin_b};
}

/* --- count --- */

template<typename ITER, typename T>
size_t lak::count(ITER begin, ITER end, const T &value)
{
	size_t result = 0;

	for (; begin != end; ++begin)
		if (*begin == value) ++result;

	return result;
}

/* --- distance --- */

template<typename ITER>
typename std::iterator_traits<ITER>::difference_type lak::distance(ITER begin,
                                                                   ITER end)
{
	static_assert(std::input_iterator<ITER>);

	if constexpr (std::random_access_iterator<ITER>)
	{
		return end - begin;
	}
	else
	{
		typename std::iterator_traits<ITER>::difference_type result = 0;
		for (; begin != end; ++begin, ++result)
			;
		return result;
	}
}

/* --- advance --- */

template<typename ITER>
void lak::advance(ITER &it,
                  typename std::iterator_traits<ITER>::difference_type offset)
{
	static_assert(std::input_iterator<ITER>);

	if constexpr (std::random_access_iterator<ITER>)
	{
		it += offset;
	}
	else if constexpr (std::bidirectional_iterator<ITER>)
	{
		if (offset >= 0)
			while (offset-- > 0) ++it;
		else
			while (offset++ < 0) --it;
	}
	else
	{
		ASSERT_GREATER_OR_EQUAL(offset, 0);
		while (offset-- > 0) ++it;
	}
}

/* --- next --- */

template<typename ITER>
ITER lak::next(ITER it,
               typename std::iterator_traits<ITER>::difference_type offset)
{
	lak::advance(it, offset);
	return it;
}

/* --- find --- */

template<typename ITER, typename T>
ITER lak::find(ITER begin, ITER end, const T &value)
{
	for (; begin != end && *begin != value; ++begin)
		;
	return begin;
}

/* --- mismatch --- */

template<typename ITER_A, typename ITER_B>
lak::pair<ITER_A, ITER_B> lak::mismatch(ITER_A begin_a,
                                        ITER_A end_a,
                                        ITER_B begin_b,
                                        ITER_B end_b)
{
	for (; begin_a != end_a && begin_b != end_b && *begin_a == *begin_b;
	     ++begin_a, ++begin_b)
		;
	return {begin_a, begin_b};
}

/* --- is_permutation --- */

template<typename ITER_A, typename ITER_B>
bool lak::is_permutation(ITER_A begin_a,
                         ITER_A end_a,
                         ITER_B begin_b,
                         ITER_B end_b)
{
	lak::tie(begin_a, begin_b) = lak::mismatch(begin_a, end_a, begin_b, end_b);

	if (lak::distance(begin_a, end_a) != lak::distance(begin_b, end_b))
		return false;

	for (auto it = begin_a; it != end_a; ++it)
	{
		if (lak::find(begin_a, it, *it) != it) continue;
		if (size_t other_count = lak::count(begin_b, end_b, *it);
		    other_count == 0 || other_count != lak::count(begin_a, end_a, *it))
			return false;
	}

	return true;
}

/* --- rotate_left --- */

template<typename ITER>
void lak::rotate_left(ITER begin, ITER end, size_t distance)
{
	static_assert(std::forward_iterator<ITER>);

	const size_t data_size = lak::distance(begin, end);
	if (data_size == 0 || distance % data_size == 0) return;

	for (ITER iter_begin = begin; distance > 0;)
	{
		const size_t working_size = lak::distance(iter_begin, end);

		ITER b = lak::next(iter_begin, distance);
		for (size_t i = 0; i < working_size - distance; ++i, ++iter_begin, ++b)
			lak::swap(*iter_begin, *b);

		distance = lak::slack<size_t>(working_size, distance);
	}
}

/* --- rotate_right --- */

template<typename ITER>
void lak::rotate_right(ITER begin, ITER end, size_t distance)
{
	static_assert(std::forward_iterator<ITER>);

	const size_t data_size = lak::distance(begin, end);
	if (data_size == 0 || distance % data_size == 0) return;

	// change direction so we can use the forward_iterator friendly rotate_left
	// algorithm
	distance = lak::slack<size_t>(distance, data_size);

	for (ITER iter_begin = begin; distance > 0;)
	{
		const size_t working_size = lak::distance(iter_begin, end);

		ITER b = lak::next(iter_begin, distance);
		for (size_t i = 0; i < working_size - distance; ++i, ++iter_begin, ++b)
			lak::swap(*iter_begin, *b);

		distance = lak::slack<size_t>(working_size, distance);
	}
}

/* --- reverse --- */

template<typename ITER>
void lak::reverse(ITER begin, ITER end)
{
	static_assert(std::bidirectional_iterator<ITER>);

	if (begin == end) return;
	--end;

	if constexpr (std::random_access_iterator<ITER>)
	{
		while (begin < end)
		{
			lak::swap(*begin, *end);
			++begin;
			--end;
		}
	}
	else
	{
		while (begin != end)
		{
			lak::swap(*begin, *end);
			++begin;
			if (begin == end) return;
			--end;
		}
	}
}

/* --- partition --- */

template<typename ITER>
ITER lak::partition(ITER begin, ITER end, auto predicate)
{
	static_assert(std::forward_iterator<ITER>);

	if (begin == end) return end;

	if constexpr (std::random_access_iterator<ITER>)
	{
		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		// ^ <- begin                            ^ <- end
		//
		// TTTTFFFFFFTTTTTTTTFFFFTTTTFFFFFTTTFFFF
		//     ^ <- first false             ^ <- last true
		//
		// TTTTFFFFFFTTTTTTTTFFFFTTTTFFFFFTTTFFFF
		//     ^                            ^ <- swap
		// TTTTTFFFFFTTTTTTTTFFFFTTTTFFFFFTTFFFFF
		//      ^                          ^ <- swap
		// TTTTTTFFFFTTTTTTTTFFFFTTTTFFFFFTFFFFFF
		//       ^                        ^ <- swap
		// TTTTTTTFFFTTTTTTTTFFFFTTTTFFFFFFFFFFFF
		//        ^                 ^ <- swap
		// TTTTTTTTFFTTTTTTTTFFFFTTTFFFFFFFFFFFFF
		//         ^               ^ <- swap
		// TTTTTTTTTFTTTTTTTTFFFFTTFFFFFFFFFFFFFF
		//          ^             ^ <- swap
		// TTTTTTTTTTTTTTTTTTFFFFTFFFFFFFFFFFFFFF
		//                   ^   ^ <- swap
		//
		// TTTTTTTTTTTTTTTTTTTFFFFFFFFFFFFFFFFFFF
		//                    ^ <- result

		ITER first_false = begin;
		ITER last_true   = end - 1;

		while (first_false != last_true && predicate(*first_false)) ++first_false;
		while (last_true != first_false && !predicate(*last_true)) --last_true;

		while (first_false < last_true)
		{
			lak::swap(*first_false, *last_true);
			while (first_false != last_true && predicate(*first_false))
				++first_false;
			while (last_true != first_false && !predicate(*last_true)) --last_true;
		}

		return first_false;
	}
	else
	{
		// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		// ^ <- begin                            ^ <- end
		//
		// TTTTFFFFFFTTTTTTTTFFFFTTTTFFFFFTTTFFFF
		//     ^ <- first false
		//
		// TTTTFFFFFFTTTTTTTTFFFFTTTTFFFFFTTTFFFF
		//     ^     ^ <- swap
		// TTTTTFFFFFFTTTTTTTFFFFTTTTFFFFFTTTFFFF
		//      ^     ^ <- swap
		// TTTTTTFFFFFFTTTTTTFFFFTTTTFFFFFTTTFFFF
		//       ^     ^ <- swap
		// ...
		// TTTTTTTTTTTTFFFFFFFFFFTTTTFFFFFTTTFFFF
		//             ^         ^ <- swap
		// ...
		//
		// TTTTTTTTTTTTTTTTTTTFFFFFFFFFFFFFFFFFFF
		//                    ^ <- result

		ITER first_false = begin;
		while (first_false != end && predicate(*first_false)) ++first_false;

		ITER iter = first_false;
		if (iter != end) ++iter;

		for (; iter != end; ++iter)
		{
			if (predicate(*iter))
			{
				lak::swap(*iter, *first_false);
				++first_false;
			}
		}

		return first_false;
	}
}

/* --- stable_partition --- */

template<typename ITER>
ITER lak::stable_partition(ITER begin, ITER end, auto predicate)
{
	static_assert(std::random_access_iterator<ITER>);

	// TTTTTFFFFFFTTTTTTTFFFFTTTTFFFFFTTTFFFF
	//      ^~~~~~^~~~~~~^ <- rotate
	// TTTTTTTTTTTTFFFFFFFFFFTTTTFFFFFTTTFFFF
	//             ^~~~~~~~~~^~~~^ <- rotate
	// TTTTTTTTTTTTTTTTFFFFFFFFFFFFFFFTTTFFFF
	//                 ^~~~~~~~~~~~~~~^~~^ <- rotate
	// TTTTTTTTTTTTTTTTTTTFFFFFFFFFFFFFFFFFFF
	//                    ^ <- return

	ITER first_false = begin;

	while (first_false != end && predicate(*first_false)) ++first_false;

	for (ITER last_end = first_false; first_false != end;)
	{
		ITER first_true = last_end;
		while (first_true != end && !predicate(*first_true)) ++first_true;

		if (first_true == end) break;

		ITER second_false = first_true;
		while (second_false != end && predicate(*second_false)) ++second_false;

		const size_t false_count                   = first_true - first_false;
		const size_t true_count                    = second_false - first_true;
		[[maybe_unused]] const size_t total_extent = false_count + true_count;

		lak::rotate_left(first_false, second_false, false_count);

		first_false += true_count;
		last_end = second_false;
	}

	return first_false;
}

/* --- binary_partition --- */

template<typename ITER, typename CMP>
ITER lak::binary_partition(ITER begin, ITER mid, ITER end, CMP compare)
{
	static_assert(std::forward_iterator<ITER>);

	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// ^ <- begin       ^ <- mid             ^ <- end
	//
	//                   v~~~~~~~~~~~~~~~~~~~v <- partition
	// TTTTFFFFFFTTTTTTTAFFFFTTTTFFFFFTTTFFFF
	// ^~~~~~~~~~~~~~~~~^ <- partition
	//
	// TTTTTTTTTTTFFFFFFATTTTTTTFFFFFFFFFFFFF
	//            ^~~~~~~~~~~~~~^ <- reverse
	//
	// TTTTTTTTTTTTTTTTTTAFFFFFFFFFFFFFFFFFFF
	//                   ^ <- result

	if (begin == end) return end;

	auto after_mid{mid};
	++after_mid;
	auto left{lak::partition(
	  begin, mid, [&](const auto &v) { return compare(v, *mid); })};
	auto right{lak::partition(
	  after_mid, end, [&](const auto &v) { return compare(v, *mid); })};

	if constexpr (std::random_access_iterator<ITER>)
	{
		// TTTTTTTTTTTFFFFFFATTTTTTTFFFFFFFFFFFFF
		//            ^~~~~~~~~~~~~~^ <- reverse

		lak::reverse(left, right);
		return left + (right - after_mid);
	}
	else
	{
		// TTTTTTTTTTTFFFFFFATTTTTTTFFFFFFFFFFFFF
		//            ^~~~~~^^~~~~~~^ <- swap

		auto [sleft, sright] = lak::swap(left, mid, after_mid, right);

		if (sleft != mid)
		{
			// TTTTTTTTTTTFFFFFFATTTFFFFFFFFFFFFFFFFF
			//            ^~~~~~^^~~^ <- swap
			//
			//                  v <- mid
			// TTTTTTTTTTTTTTFFFAFFFFFFFFFFFFFFFFFFFF
			//               ^ <- sleft
			//
			// TTTTTTTTTTTTTTFFFAFFFFFFFFFFFFFFFFFFFF
			//               ^  ^ <- swap

			lak::swap(*sleft, *mid);
			return sleft;
		}
		else if (sright != right)
		{
			// TTTTTTTTTTTFFFFFFATTTTTTTTTFFFFFFFFFFF
			//            ^~~~~~^^~~~~~~~~^ <- swap
			//
			//                         v <- sright
			// TTTTTTTTTTTTTTTTTAFFFFFFTTTFFFFFFFFFFF
			//                  ^ <- mid  ^ <- right
			//
			// TTTTTTTTTTTTTTTTTAFFFFFFTTTFFFFFFFFFFF
			//                  ^~~~~~~^~~^ <- swap
			//
			//                         v <- sright
			// TTTTTTTTTTTTTTTTTTTTFFFFAFFFFFFFFFFFFF
			//                     ^ .first

			auto result{lak::swap(mid, sright, sright, right).first};
			lak::swap(*sright, *result);
			return result;
		}
		else
			return mid;
	}
}

/* --- mark_and_sweep_parition --- */

template<typename T, typename ITER, typename CMP>
ITER lak::mark_and_sweep_parition(
  T *root, auto sweep, ITER begin, ITER end, auto transform)
{
	std::set<T *, CMP> marked_set;
	marked_set.insert(root);

	{
		std::vector<T *> stack;
		stack.push_back(root);

		while (!stack.empty())
		{
			T *to_sweep = stack.back();
			stack.pop_back();
			sweep(*to_sweep,
			      [&](T *marked)
			      {
				      ASSERT_NOT_EQUAL(marked, nullptr);
				      if (marked_set.insert(marked).second) stack.push_back(marked);
			      });
		}
	}

	return lak::partition<ITER>(
	  begin,
	  end,
	  [&](auto &elem) -> bool { return marked_set.contains(transform(elem)); });
}

/* --- merge --- */

template<typename ITER, typename CMP>
ITER lak::merge(ITER begin, ITER mid, ITER end, CMP compare)
{
	static_assert(std::forward_iterator<ITER>);

	while (begin != mid && mid != end)
	{
		begin = lak::lower_bound(begin, mid, *mid, compare);

		// [old begin, begin): < *mid
		// [begin, mid):      >= *mid

		size_t offset = 0;
		if constexpr (std::random_access_iterator<ITER>)
		{
			auto new_mid{lak::lower_bound(mid, end, *begin, compare)};
			offset = size_t(new_mid - mid);
			mid    = new_mid;
		}
		else
		{
			while (mid != end && !compare(*begin, *mid))
			{
				++mid;
				++offset;
			}
		}

		// [old mid, mid): < *begin
		// [mid, end):    >= *begin

		lak::rotate_right(begin, mid, offset);
	}
}

/* --- make_heap --- */

template<typename ITER, typename CMP>
void lak::make_heap(ITER begin, ITER end, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	auto index_of = [&](const ITER &iter) -> size_t
	{ return size_t(iter - begin); };

	auto parent      = [](size_t index) -> size_t { return (index - 1U) >> 1U; };
	auto parent_iter = [&](const ITER &iter) -> ITER
	{ return begin + parent(index_of(iter)); };

	for (ITER it = parent_iter(end - 1) + 1; it != begin;)
		lak::sift_down_heap(begin, --it, end, compare);
}

/* --- is_heap --- */

template<typename ITER, typename CMP>
bool lak::is_heap(ITER begin, ITER end, CMP compare)
{
	return lak::is_heap_until(begin, end, compare) == end;
}

/* --- is_heap_until --- */

template<typename ITER, typename CMP>
ITER lak::is_heap_until(ITER begin, ITER end, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	if ((end - begin) <= 1U) return end;

	auto parent_iter = [&](size_t index) -> ITER
	{ return begin + ((index - 1U) >> 1U); };

	ITER it = begin + 1U;
	for (size_t i = 1U; it != end && !compare(*parent_iter(i), *it); ++it, ++i)
		;
	return it;
}

/* --- sift_down_heap --- */

template<typename ITER, typename CMP>
void lak::sift_down_heap(ITER begin, ITER to_sift, ITER end, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	auto index_of = [&](const ITER &iter) -> size_t
	{ return size_t(iter - begin); };

	if (index_of(end) <= 1) return;

	// end index rounded to the nearest left <= end
	auto left_end = [&]() -> size_t
	{
		const size_t last = index_of(end);
		return last - (1U - (last & 1U));
	};

	auto left_child = [](size_t index) -> size_t { return (index << 1U) + 1U; };

	// if there's an odd number of elements in the heap, then there's always a
	// left and right child. if there's an even number of elements, then the last
	// element is a left child (which has no right sibling).

	for (size_t end_index   = left_end(),
	            child_index = left_child(index_of(to_sift));
	     child_index < end_index;
	     child_index = left_child(index_of(to_sift)))
	{
		ITER left         = begin + child_index;
		ITER right        = left + 1;
		ITER bigger_child = compare(*left, *right) ? right : left;

		if (compare(*to_sift, *bigger_child))
		{
			lak::swap(*to_sift, *bigger_child);
			to_sift = bigger_child;
		}
		else
			return;
	}

	if (size_t left_index = left_child(index_of(to_sift));
	    left_index < index_of(end))
	{
		ITER left = begin + left_index;
		if (compare(*to_sift, *left)) lak::swap(*to_sift, *left);
	}
}

/* --- sift_up_heap --- */

template<typename ITER, typename CMP>
void lak::sift_up_heap(ITER begin, ITER to_sift, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	if (to_sift == begin) return;

	auto index_of = [&](const ITER &iter) -> size_t
	{ return size_t(iter - begin); };

	auto parent      = [](size_t index) -> size_t { return (index - 1U) >> 1U; };
	auto parent_iter = [&](const ITER &iter) -> ITER
	{ return begin + parent(index_of(iter)); };

	for (ITER sift_parent = parent_iter(to_sift);
	     compare(*sift_parent, *to_sift);)
	{
		lak::swap(*sift_parent, *to_sift);
		if (sift_parent == begin) break;
		to_sift     = sift_parent;
		sift_parent = parent_iter(to_sift);
	}
}

/* --- push_heap --- */

template<typename ITER, typename CMP>
void lak::push_heap(ITER begin, ITER end, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	if ((end - begin) <= 1U) return;

	lak::sift_up_heap(begin, end - 1, compare);
}

/* --- pop_heap --- */

template<typename ITER, typename CMP>
void lak::pop_heap(ITER begin, ITER end, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	if ((end - begin) <= 2U) return;

	--end;
	lak::swap(*begin, *end);
	lak::sift_down_heap(begin, begin, end, compare);
}

/* --- sort_heap --- */

template<typename ITER, typename CMP>
void lak::sort_heap(ITER begin, ITER end, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	if (begin != end) --end;
	for (; begin != end; --end)
	{
		lak::swap(*begin, *end);
		lak::sift_down_heap(begin, begin, end, compare);
	}
}

/* --- reverse_sort_heap --- */

template<typename ITER, typename CMP>
void lak::reverse_sort_heap(ITER begin, ITER end, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	lak::sort_heap(begin, end, compare);
	lak::reverse(begin, end);
}

/* --- breadth first search heap --- */

template<typename ITER, typename F>
ITER lak::breadth_first_search_heap(ITER begin, ITER end, F &&predicate)
{
	while (begin != end && !predicate(*begin))
		;
	return begin;
}

/* --- depth first search heap --- */

template<typename ITER, typename F>
ITER lak::depth_first_search_heap(ITER begin, ITER end, F &&predicate)
{
	if (begin == end) return begin;
	size_t size = end - begin;

	auto next_index = [size](size_t index) -> size_t
	{
		auto is_left    = [](size_t index) -> bool { return (index & 1U) == 1U; };
		auto left_child = [](size_t index) -> size_t
		{ return (index << 1U) + 1U; };
		auto right_sibling = [](size_t index) -> size_t { return index + 1U; };
		auto parent        = [](size_t index) -> size_t
		{ return ((index + 1U) >> 1U) - 1U; };

		if (index >= size - 1U) return size;

		if (auto l = left_child(index); l < size) return l;

		while (index != 0U)
		{
			while (!is_left(index)) index = parent(index);
			if (auto r = right_sibling(index); r < size) return r;
		}

		return size;
	};

	for (size_t index = 0U; index != size; index = next_index(index))
	{
		if (predicate(*(begin + index))) return begin + index;
	}

	return end;
}

/* --- heapsort --- */

template<typename ITER, typename CMP>
void lak::heapsort(ITER begin, ITER end, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	if ((end - begin) <= 1U) return;

	lak::make_heap(begin, end, compare);
	lak::sort_heap(begin, end, compare);
}

/* --- minmax_element --- */

template<typename ITER, typename CMP>
lak::pair<ITER, ITER> lak::minmax_element(ITER begin, ITER end, CMP compare)
{
	if (begin == end) return {begin, begin};

	lak::pair<ITER, ITER> result{begin, begin};

	for (++begin; begin != end; ++begin)
		if (compare(*begin, *result.first))
			result.first = begin;
		else if (!compare(*begin, *result.second))
			result.second = begin;

	return result;
}

/* --- max_element --- */

template<typename ITER, typename CMP>
ITER lak::max_element(ITER begin, ITER end, CMP compare)
{
	if (begin == end) return end;

	ITER result = begin;

	for (++begin; begin != end; ++begin)
		if (compare(*result, *begin)) result = begin;

	return result;
}

/* --- min_element --- */

template<typename ITER, typename CMP>
ITER lak::min_element(ITER begin, ITER end, CMP compare)
{
	if (begin == end) return end;

	ITER result = begin;

	for (++begin; begin != end; ++begin)
		if (compare(*begin, *result)) result = begin;

	return result;
}

/* --- lower_bound --- */

template<typename ITER, typename U, typename CMP>
ITER lak::lower_bound(ITER begin, ITER end, const U &value, CMP compare)
{
	static_assert(std::forward_iterator<ITER>);

	auto diff{lak::distance(begin, end)};

	for (ITER iter = begin; diff > 0;)
	{
		const auto step = diff / 2;
		lak::advance(iter, step);
		if (compare(*iter, value))
		{
			// move the lower bound up
			begin = lak::next(iter);
			diff -= step + 1;
		}
		else
			// move the upper bound down
			diff = step;
	}

	return begin;
}

/* --- upper_bound --- */

template<typename ITER, typename U, typename CMP>
ITER lak::upper_bound(ITER begin, ITER end, const U &value, CMP compare)
{
	static_assert(std::forward_iterator<ITER>);

	auto diff{lak::distance(begin, end)};

	for (ITER iter = begin; diff > 0;)
	{
		const auto step = diff / 2;
		lak::advance(iter, step);
		if (!compare(value, *iter))
		{
			// move the lower bound up
			begin = lak::next(iter);
			diff -= step + 1;
		}
		else
			// move the upper bound down
			diff = step;
	}

	return begin;
}

/* --- equal_range --- */

template<typename T, typename U, typename CMP>
lak::span<T> lak::equal_range(T *begin, T *end, const U &value, CMP compare)
{
	begin = lak::lower_bound(begin, end, value, compare);
	end   = lak::upper_bound(begin, end, value, compare);
	return lak::span<T>{begin, end};
}
