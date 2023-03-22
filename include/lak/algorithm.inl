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
ITER lak::binary_partition(ITER begin, ITER mid, ITER end, CMP cmp)
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
	auto left{
	  lak::partition(begin, mid, [&](const auto &v) { return cmp(v, *mid); })};
	auto right{lak::partition(
	  after_mid, end, [&](const auto &v) { return cmp(v, *mid); })};

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

/* --- heapsort --- */

template<typename ITER, typename CMP>
void lak::heapsort(ITER begin, ITER end, CMP compare)
{
	static_assert(std::random_access_iterator<ITER>);

	if ((end - begin) < 2) return;

	auto index_of = [&](const ITER &iter) -> size_t
	{ return size_t(iter - begin); };

	auto parent      = [](size_t index) -> size_t { return (index - 1U) / 2U; };
	auto parent_iter = [&](const ITER &iter) -> ITER
	{ return begin + parent(index_of(iter)); };

	auto left_child = [](size_t index) -> size_t { return (index * 2U) + 1U; };
	[[maybe_unused]] auto left_child_iter = [&](const ITER &iter) -> ITER
	{ return begin + left_child(index_of(iter)); };

	// Repair the heap whose root element is at index 'start', assuming the heaps
	// rooted at its children are valid
	auto sift_down = [&](ITER sift_begin, ITER sift_end)
	{
		for (size_t end_index   = index_of(sift_end),
		            child_index = left_child(index_of(sift_begin));
		     child_index <= end_index;
		     child_index = left_child(index_of(sift_begin)))
		{
			ITER largest = sift_begin;
			ITER child   = begin + child_index;

			if (compare(*largest, *child)) largest = child;

			if (child++ != sift_end && compare(*largest, *child)) largest = child;

			if (largest == sift_begin) return;

			lak::swap(*sift_begin, *largest);
			sift_begin = largest;
		}
	};

	--end; // make begin-end an inclusive range

	// initialise the heap
	for (ITER it = parent_iter(end) + 1; it != begin;) sift_down(--it, end);

	while (begin != end)
	{
		lak::swap(*begin, *end);
		sift_down(begin, --end);
	}
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
