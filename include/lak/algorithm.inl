#include "lak/algorithm.hpp"

#include "lak/concepts.hpp"
#include "lak/functional.hpp"
#include "lak/integer_range.hpp"
#include "lak/math.hpp"
#include "lak/ptr_intrin.hpp"
#include "lak/span.hpp"
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

	if (std::random_access_iterator<ITER>)
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

/* --- rotate_left --- */

template<typename T>
void lak::rotate_left(T *begin, T *end, size_t distance)
{
	const size_t data_size = end - begin;
	if (data_size == 0 || distance % data_size == 0) return;

	for (T *iter_begin = begin; distance > 0;)
	{
		const size_t working_size = end - iter_begin;

		for (size_t i = 0; i < working_size - distance; ++i)
			lak::swap(iter_begin[i], iter_begin[i + distance]);

		iter_begin += working_size - distance;
		distance = lak::slack<size_t>(working_size, distance);
	}
}

/* --- rotate_right --- */

template<typename T>
void lak::rotate_right(T *begin, T *end, size_t distance)
{
	const size_t data_size = end - begin;
	if (data_size == 0 || distance % data_size == 0) return;

	for (T *iter_end = end; distance > 0;)
	{
		const size_t working_size = iter_end - begin;

		for (size_t i = working_size - distance; i-- > 0;)
			lak::swap(begin[i], begin[i + distance]);

		iter_end -= working_size - distance;
		distance = lak::slack<size_t>(working_size, distance);
	}
}

/* --- partition --- */

template<typename ITER>
ITER lak::partition(ITER begin, ITER end, auto predicate)
{
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

/* --- stable_partition --- */

template<typename T>
T *lak::stable_partition(T *begin, T *end, auto predicate)
{
	// TTTTTFFFFFFTTTTTTTFFFFTTTTFFFFFTTTFFFF
	//      ^~~~~~^~~~~~~^ <- rotate
	// TTTTTTTTTTTTFFFFFFFFFFTTTTFFFFFTTTFFFF
	//             ^~~~~~~~~~^~~~^ <- rotate
	// TTTTTTTTTTTTTTTTFFFFFFFFFFFFFFFTTTFFFF
	//                 ^~~~~~~~~~~~~~~^~~^ <- rotate
	// TTTTTTTTTTTTTTTTTTTFFFFFFFFFFFFFFFFFFF
	//                    ^ <- return

	T *first_false = begin;

	while (first_false != end && predicate(*first_false)) ++first_false;

	for (T *last_end = first_false; first_false != end;)
	{
		T *first_true = last_end;
		while (first_true != end && !predicate(*first_true)) ++first_true;

		if (first_true == end) break;

		T *second_false = first_true;
		while (second_false != end && predicate(*second_false)) ++second_false;

		const size_t false_count  = first_true - first_false;
		const size_t true_count   = second_false - first_true;
		const size_t total_extent = false_count + true_count;

		lak::rotate_left(first_false, second_false, false_count);

		first_false += true_count;
		last_end = second_false;
	}

	return first_false;
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

template<typename T, typename U, typename CMP>
T *lak::lower_bound(T *begin, T *end, const U &value, CMP compare)
{
	ASSERT_LESS_OR_EQUAL(begin, end);
	auto diff = end - begin;

	for (T *iter = begin; diff > 0;)
	{
		const auto step = diff / 2;
		iter += step;
		if (compare(*iter, value))
		{
			// move the lower bound up
			begin = iter + 1;
			diff -= step + 1;
		}
		else
			// move the upper bound down
			diff = step;
	}

	return begin;
}

/* --- upper_bound --- */

template<typename T, typename U, typename CMP>
T *lak::upper_bound(T *begin, T *end, const U &value, CMP compare)
{
	ASSERT_LESS_OR_EQUAL(begin, end);
	auto diff = end - begin;

	for (T *iter = begin; diff > 0;)
	{
		const auto step = diff / 2;
		iter += step;
		if (!compare(value, *iter))
		{
			// move the lower bound up
			begin = iter + 1;
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
