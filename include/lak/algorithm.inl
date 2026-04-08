#include "lak/algorithm.hpp"

#include "lak/atomic_optional.hpp"
#include "lak/concepts.hpp"
#include "lak/functional.hpp"
#include "lak/integer_range.hpp"
#include "lak/math.hpp"
#include "lak/ptr_intrin.hpp"
#include "lak/span.hpp"
#include "lak/tuple.hpp"
#include "lak/utility.hpp"

#ifndef LAK_ARRAY_FORWARD_ONLY
#	define LAK_ARRAY_FORWARD_ONLY
#endif
#include "lak/array.hpp"

#include <ranges>
#include <set>
#include <thread>
#include <vector>

/* --- move --- */

template<std::input_iterator IN_ITER,
         std::output_iterator<
           typename std::iterator_traits<IN_ITER>::value_type> OUT_ITER>
OUT_ITER lak::move(IN_ITER begin, IN_ITER end, OUT_ITER output)
{
	for (; begin != end; ++begin, ++output) *output = lak::move(*begin);
	return output;
}

/* --- copy --- */

template<std::input_iterator IN_ITER,
         std::output_iterator<
           typename std::iterator_traits<IN_ITER>::value_type> OUT_ITER>
OUT_ITER lak::copy(IN_ITER begin, IN_ITER end, OUT_ITER output)
{
	for (; begin != end; ++begin, ++output) *output = *begin;
	return output;
}

template<std::input_iterator IN_ITER,
         std::output_iterator<
           typename std::iterator_traits<IN_ITER>::value_type> OUT_ITER>
requires std::equality_comparable<OUT_ITER>
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

template<std::forward_iterator ITER_A, std::forward_iterator ITER_B>
lak::pair<ITER_A, ITER_B> lak::swap(ITER_A begin_a,
                                    ITER_B begin_b,
                                    size_t count)
{
	for (size_t i = 0; i < count; ++i, ++begin_a, ++begin_b)
		lak::swap(*begin_a, *begin_b);
	return {begin_a, begin_b};
}

template<std::forward_iterator ITER_A, std::forward_iterator ITER_B>
lak::pair<ITER_A, ITER_B> lak::swap(ITER_A begin_a,
                                    ITER_A end_a,
                                    ITER_B begin_b,
                                    ITER_B end_b)
{
	for (; begin_a != end_a && begin_b != end_b; ++begin_a, ++begin_b)
		lak::swap(*begin_a, *begin_b);
	return {begin_a, begin_b};
}

/* --- pivot_swap --- */

template<std::forward_iterator ITER>
ITER lak::pivot_swap(ITER begin, ITER pivot, ITER end)
{
	if (begin == pivot)
		return end;
	else if (pivot == end)
		return begin;

	auto after_pivot{lak::next(pivot)};

	if constexpr (std::random_access_iterator<ITER>)
	{
		// TTTTTTTTTTTFFFFFFATTTTTTTFFFFFFFFFFFFF
		//            ^~~~~~~~~~~~~~^ <- reverse

		lak::reverse(begin, end);
		return begin + (end - after_pivot);
	}
	else
	{
		// TTTTTTTTTTTFFFFFFATTTTTTTFFFFFFFFFFFFF
		//            ^~~~~~^^~~~~~~^ <- swap

		auto [sbegin, send] = lak::swap(begin, pivot, after_pivot, end);

		if (sbegin != pivot)
		{
			// TTTTTTTTTTTFFFFFFATTTFFFFFFFFFFFFFFFFF
			//            ^~~~~~^^~~^ <- swap
			//
			//                  v <- pivot
			// TTTTTTTTTTTTTTFFFAFFFFFFFFFFFFFFFFFFFF
			//               ^ <- sbegin
			//
			// TTTTTTTTTTTTTTFFFAFFFFFFFFFFFFFFFFFFFF
			//               ^  ^ <- swap

			lak::swap(*sbegin, *pivot);
			return sbegin;
		}
		else if (send != end)
		{
			// TTTTTTTTTTTFFFFFFATTTTTTTTTFFFFFFFFFFF
			//            ^~~~~~^^~~~~~~~~^ <- swap
			//
			//                         v <- send
			// TTTTTTTTTTTTTTTTTAFFFFFFTTTFFFFFFFFFFF
			//                  ^ <-pivot ^ <- end
			//
			// TTTTTTTTTTTTTTTTTAFFFFFFTTTFFFFFFFFFFF
			//                  ^~~~~~~^~~^ <- swap
			//
			//                         v <- send
			// TTTTTTTTTTTTTTTTTTTTFFFFAFFFFFFFFFFFFF
			//                     ^ <- .first
			//
			// TTTTTTTTTTTTTTTTTTTTFFFFAFFFFFFFFFFFFF
			//                     ^   ^ <- swap
			//
			// TTTTTTTTTTTTTTTTTTTTAFFFFFFFFFFFFFFFFF
			//                     ^ <- result

			auto result{lak::swap(pivot, send, send, end).first};
			lak::swap(*send, *result);
			return result;
		}
		else
			return pivot;
	}
}

/* --- stable_pivot_swap --- */

template<std::forward_iterator ITER>
ITER lak::stable_pivot_swap(ITER begin, ITER pivot, ITER end)
{
	if (begin == pivot)
		return end;
	else if (pivot == end)
		return begin;

	auto after_pivot{lak::next(pivot)};

	auto [sbegin, send] = lak::swap(begin, pivot, after_pivot, end);

	if constexpr (std::bidirectional_iterator<ITER>)
	{
		lak::reverse(sbegin, send);
		pivot = sbegin + ((send - 1) - pivot);
		lak::reverse(sbegin, pivot);
		lak::reverse(pivot + 1, send);
		return pivot;
	}
	else
	{
		if (sbegin != pivot)
		{
			//   begin -> v         v <- end
			// TTTTTTTTTTTFFFFFFATTTFFFFFFFFFFFFFFFFF
			//            ^~~~~~^^~~^ <- swap
			//
			//                   v <- after pivot
			// TTTTTTTTTTTTTTFFFAFFFFFFFFFFFFFFFFFFFF
			//     sbegin -> ^      ^ <- end
			//
			// TTTTTTTTTTTTTTFFFAFFFFFFFFFFFFFFFFFFFF
			//               ^~~~~~~^ <- rotate left

			lak::rotate_left(sbegin, pivot, end);
			return sbegin;
		}
		else if (send != end)
		{
			//   begin -> v               v <- end
			// TTTTTTTTTTTFFFFFFATTTTTTTTTFFFFFFFFFFF
			//            ^~~~~~^^~~~~~~~~^ <- swap
			//
			//                         v <- send
			// TTTTTTTTTTTTTTTTTAFFFFFFTTTFFFFFFFFFFF
			//                  ^ <-pivot ^ <- end
			//
			// TTTTTTTTTTTTTTTTTAFFFFFFTTTFFFFFFFFFFF
			//                  ^~~~~~~~~~^ <- rotate left
			//
			// TTTTTTTTTTTTTTTTTTTTAFFFFFFFFFFFFFFFFF
			//                     ^ <- result

			const auto pivot_to_send{lak::distance(pivot, send)};
			const auto send_to_end{lak::distance(send, end)};
			lak::rotate_right(pivot, pivot_to_send + send_to_end, send_to_end);
			return lak::next(pivot, send_to_end);
		}
		else
			return pivot;
	}
}

/* --- accumulate --- */

template<std::input_iterator ITER, typename T, typename OP>
T lak::accumulate(ITER begin, ITER end, T init, OP op)
{
	for (; begin != end; ++begin) init = op(lak::move(init), *begin);
	return init;
}

/* --- threaded --- */

template<typename INPUT, typename OUTPUT>
void lak::threaded(auto thread_func, auto control_func, size_t thread_count)
{
	// try and make jthread dependent on the template arguments
	using thread_type =
	  lak::nth_type_t<lak::is_same_v<INPUT, std::jthread> ? 1U : 0U,
	                  std::jthread,
	                  INPUT>;

	if constexpr (lak::is_void_v<INPUT> && lak::is_void_v<OUTPUT>)
	{
		lak::array<thread_type, lak::dynamic_extent> threads;
		threads.reserve(thread_count);

		for (size_t tid = 0U; tid < thread_count; ++tid)
			threads.emplace_back([&thread_func, id = tid] { thread_func(id); });

		control_func();
	}
	else if constexpr (lak::is_void_v<INPUT>)
	{
		lak::array<lak::atomic_optional<OUTPUT>, lak::dynamic_extent>
		  thread_outputs;
		thread_outputs.resize(thread_count);

		lak::array<thread_type, lak::dynamic_extent> threads;
		threads.reserve(thread_count);

		for (size_t tid = 0U; tid < thread_count; ++tid)
			threads.emplace_back([&thread_func, &thread_outputs, id = tid]
			                     { thread_func(id, lak::span(thread_outputs)); });

		control_func(lak::span(thread_outputs));
	}
	else if constexpr (lak::is_void_v<OUTPUT>)
	{
		lak::array<lak::atomic_optional<INPUT>, lak::dynamic_extent> thread_inputs;
		thread_inputs.resize(thread_count);

		lak::array<thread_type, lak::dynamic_extent> threads;
		threads.reserve(thread_count);

		for (size_t tid = 0U; tid < thread_count; ++tid)
			threads.emplace_back([&thread_func, &thread_inputs, id = tid]
			                     { thread_func(id, lak::span(thread_inputs)); });

		control_func(lak::span(thread_inputs));

		for (auto &in : thread_inputs) in.stop();
	}
	else
	{
		lak::array<lak::atomic_optional<INPUT>, lak::dynamic_extent> thread_inputs;
		thread_inputs.resize(thread_count);
		lak::array<lak::atomic_optional<OUTPUT>, lak::dynamic_extent>
		  thread_outputs;
		thread_outputs.resize(thread_count);

		lak::array<thread_type, lak::dynamic_extent> threads;
		threads.reserve(thread_count);

		for (size_t tid = 0U; tid < thread_count; ++tid)
			threads.emplace_back(
			  [&thread_func, &thread_inputs, &thread_outputs, id = tid]
			  {
				  thread_func(id, lak::span(thread_inputs), lak::span(thread_outputs));
			  });

		control_func(lak::span(thread_inputs), lak::span(thread_outputs));

		for (auto &in : thread_inputs) in.stop();
	}
}

/* --- for_each --- */

template<std::input_iterator ITER, typename END, std::input_iterator... ITERS>
void lak::for_each(ITER begin, END end, auto func, ITERS... begins)
{
	lak::for_each(lak::execution::seq, begin, end, func, begins...);
}

template<lak::execution::concepts::policy POLICY,
         std::input_iterator ITER,
         typename END,
         std::input_iterator... ITERS>
void lak::for_each(
  const POLICY &, ITER begin, END end, auto func, ITERS... begins)
{
	if (begin == end) return;

	if constexpr (lak::execution::concepts::parallel<POLICY>)
	{
		const size_t thread_count = [&]() -> size_t
		{
			if constexpr (std::random_access_iterator<ITER>)
				return std::max<size_t>(
				  0U,
				  std::min<size_t>(size_t(lak::distance(begin, end)),
				                   std::thread::hardware_concurrency()));
			else
				return std::thread::hardware_concurrency();
		}();

		if constexpr (std::random_access_iterator<ITER> &&
		              ((std::random_access_iterator<ITERS>) && ...))
		{
			const size_t dist = size_t(lak::distance(begin, end));

			std::atomic_size_t work_index = 0U;
			std::atomic_bool done         = false;

			lak::threaded(
			  [&, dist](size_t)
			  {
				  for (size_t ind = work_index.fetch_add(1U); !done && ind < dist;
				       ind        = work_index.fetch_add(1U))
				  {
					  func(begin[ind], begins[ind]...);
				  }
				  done = true;
			  },
			  [] {},
			  thread_count);
		}
		else
		{
			using input_type =
			  lak::tuple<std::iter_value_t<ITER>, std::iter_value_t<ITERS>...>;

			lak::threaded<input_type>(
			  [&](size_t id,
			      lak::span<lak::atomic_optional<input_type>> thread_inputs)
			  {
				  auto &input = thread_inputs[id];
				  lak::while_some([&] { return input.try_release(); },
				                  [&](input_type &&in) { lak::move(in).apply(func); });
			  },
			  [&](lak::span<lak::atomic_optional<input_type>> thread_inputs)
			  {
				  while (begin != end)
				  {
					  for (auto &in : thread_inputs)
					  {
						  if (in.has_value()) continue;
						  in.emplace(*begin, *begins...);
						  ++begin;
						  ((++begins), ...);
						  break;
					  }
				  }
			  });
		}
	}
	else
	{
		for (size_t i = 0U; begin != end; ++begin, ++i) func(*begin);
	}
}

/* --- trasnsform --- */

template<std::input_iterator ITER_IN,
         typename ITER_OUT,
         std::input_iterator... ITER_INS>
ITER_OUT lak::transform(ITER_IN begin,
                        ITER_IN end,
                        ITER_OUT out,
                        auto trans_func,
                        ITER_INS... begins)
{
	return lak::transform(lak::execution::seq, begin, end, trans_func);
}

template<lak::execution::concepts::policy POLICY,
         std::input_iterator ITER_IN,
         typename ITER_OUT,
         std::input_iterator... ITER_INS>
ITER_OUT lak::transform(const POLICY &,
                        ITER_IN begin,
                        ITER_IN end,
                        ITER_OUT out,
                        auto trans_func,
                        ITER_INS... begins)
{
	if (begin == end) return out;

	static_assert(std::output_iterator<ITER_OUT, decltype(trans_func(*begin))>);

	if constexpr (lak::execution::concepts::parallel<POLICY> &&
	              std::forward_iterator<ITER_OUT>)
	{
		if constexpr (std::random_access_iterator<ITER_OUT>)
		{
			std::atomic<std::iter_difference_t<ITER_OUT>> out_count = 0U;

			lak::for_each(
			  POLICY{},
			  begin,
			  end,
			  [&]<typename T, typename... U>(T &&t, size_t i, U &&...u)
			  {
				  out[i] = trans_func(lak::forward<T>(t), lak::forward<U>(u)...);
				  ++out_count;
			  },
			  std::views::iota(size_t(0U)).begin(),
			  begins...);

			lak::advance(out, out_count);
		}
		else if constexpr (std::random_access_iterator<ITER_IN> &&
		                   ((std::random_access_iterator<ITER_INS>) && ...))
		{
			const size_t dist = size_t(lak::distance(begin, end));

			const size_t thread_count = std::max<size_t>(
			  1U, std::min<size_t>(dist, std::thread::hardware_concurrency()));

			using output_type = decltype(trans_func(*begin, *begins...));

			lak::threaded<void, output_type>(
			  [&](size_t id,
			      lak::span<lak::atomic_optional<output_type>> thread_outputs)
			  {
				  const size_t stride = thread_outputs.size();
				  auto &output        = thread_outputs[id];
				  for (size_t i = id; i < dist; i += stride)
					  output.emplace(trans_func(begin[i], begins[i]...));
			  },
			  [&](lak::span<lak::atomic_optional<output_type>> thread_outputs)
			  {
				  for (size_t i = 0U; i < dist; ++i)
				  {
					  *out = thread_outputs[i % thread_outputs.size()].release();
					  ++out;
				  }
			  },
			  thread_count);
		}
		else
		{
			const size_t thread_count = std::thread::hardware_concurrency();

			using input_type =
			  lak::tuple<std::iter_value_t<ITER_IN>, std::iter_value_t<ITER_INS>...>;
			using output_type = decltype(trans_func(*begin, *begins...));

			lak::threaded<input_type, output_type>(
			  [&](size_t id,
			      lak::span<lak::atomic_optional<input_type>> thread_inputs,
			      lak::span<lak::atomic_optional<output_type>> thread_outputs)
			  {
				  auto &input  = thread_inputs[id];
				  auto &output = thread_outputs[id];
				  lak::while_some(
				    [&] { return input.try_release(); },
				    [&](input_type &&in)
				    { output.emplace(lak::move(in).apply(trans_func)); });
			  },
			  [&](lak::span<lak::atomic_optional<input_type>> thread_inputs,
			      lak::span<lak::atomic_optional<output_type>> thread_outputs)
			  {
				  // preload inputs
				  for (size_t i = 0U; i < thread_count;)
				  {
					  thread_inputs[i].emplace(*begin, *begins...);
					  ++begin;
					  ++i;
					  if (begin == end)
					  {
						  // early termination
						  for (size_t o = 0U; o < i; ++o)
						  {
							  *out = thread_outputs[o].release();
							  ++out;
						  }
						  return;
					  }
				  }

				  // run threads
				  size_t index = 0U;
				  while (begin != end)
				  {
					  *out = thread_outputs[index].release();
					  ++out;
					  thread_inputs[index].emplace(*begin, *begins...);
					  ++begin;
					  ++index;
					  index %= thread_count;
				  }

				  // finish outputs
				  for (size_t o = 0U; o < thread_count; ++o)
				  {
					  *out = thread_outputs[(o + index) % thread_count].release();
					  ++out;
				  }
			  },
			  thread_count);
		}
	}
	else
	{
		for (; begin != end; ++begin, ((++begins), ...), ++out)
			*out = trans_func(*begin, *begins...);
	}

	return out;
}

/* --- transform_reduce --- */

template<typename T,
         std::input_iterator ITER_IN,
         std::input_iterator... ITER_INS>
T lak::transform_reduce(ITER_IN begin,
                        ITER_IN end,
                        T init,
                        auto binary_reduce,
                        auto trans_func,
                        ITER_INS... begins)
{
	return lak::transform_reduce(lak::execution::seq,
	                             begin,
	                             end,
	                             init,
	                             binary_reduce,
	                             trans_func,
	                             begins...);
}

template<typename T,
         lak::execution::concepts::policy POLICY,
         std::input_iterator ITER_IN,
         std::input_iterator... ITER_INS>
T lak::transform_reduce(const POLICY &,
                        ITER_IN begin,
                        ITER_IN end,
                        T init,
                        auto binary_reduce,
                        auto trans_func,
                        ITER_INS... begins)
{
	if (begin == end) return init;

	if constexpr (lak::execution::concepts::parallel<POLICY>)
	{
		std::mutex write_mutex;

		if constexpr (lak::execution::concepts::unsequenced<POLICY> &&
		              std::random_access_iterator<ITER_IN> &&
		              (std::random_access_iterator<ITER_INS> && ...))
		{
			const size_t dist = size_t(lak::distance(begin, end));
			const size_t stride =
			  std::max<size_t>(1U, dist / std::thread::hardware_concurrency());
			const size_t thread_count =
			  (dist / stride) + ((dist % stride) ? 1U : 0U);

			ASSERT_LESS((thread_count - 1U) * stride, dist);
			ASSERT_GREATER_OR_EQUAL(thread_count * stride, dist);

			lak::threaded<void, T>(
			  [&](size_t i, lak::span<lak::atomic_optional<T>> thread_outputs)
			  {
				  size_t start       = i * stride;
				  size_t range_start = start + 1U;
				  size_t stop =
				    start + std::min<size_t>(size_t(lak::distance(begin + start, end)),
				                             stride);

				  BOUNDS_ASSERT_GREATER_OR_EQUAL(stop, range_start);

				  thread_outputs[i].emplace(lak::transform_reduce<T>(
				    lak::execution::unseq,
				    lak::next(begin, range_start),
				    lak::next(begin, stop),
				    trans_func(*lak::next(begin, start), *lak::next(begins, start)...),
				    binary_reduce,
				    trans_func,
				    lak::next(begins, range_start)...));
			  },
			  [&](lak::span<lak::atomic_optional<T>> thread_outputs)
			  {
				  init =
				    lak::transform_reduce<T>(lak::execution::unseq,
				                             thread_outputs.begin(),
				                             thread_outputs.end(),
				                             lak::move(init),
				                             binary_reduce,
				                             [](lak::atomic_optional<T> &result) -> T
				                             { return result.release(); });
			  },
			  thread_count);
		}
		else
		{
			const size_t thread_count = std::thread::hardware_concurrency();

			using input_type =
			  lak::tuple<std::iter_value_t<ITER_IN>, std::iter_value_t<ITER_INS>...>;
			using output_type = decltype(trans_func(*begin, *begins...));

			lak::threaded<input_type, output_type>(
			  [&](size_t id,
			      lak::span<lak::atomic_optional<input_type>> thread_inputs,
			      lak::span<lak::atomic_optional<output_type>> thread_outputs)
			  {
				  auto &input  = thread_inputs[id];
				  auto &output = thread_outputs[id];
				  lak::while_some(
				    [&] { return input.try_release(); },
				    [&](input_type &&in)
				    { output.emplace(lak::move(in).apply(trans_func)); });
			  },
			  [&](lak::span<lak::atomic_optional<input_type>> thread_inputs,
			      lak::span<lak::atomic_optional<output_type>> thread_outputs)
			  {
				  // preload inputs
				  for (size_t i = 0U; i < thread_count;)
				  {
					  thread_inputs[i].emplace(*begin, *begins...);
					  ++begin;
					  ++i;
					  if (begin == end)
					  {
						  // early termination
						  for (size_t o = 0U; o < i; ++o)
						  {
							  init =
							    binary_reduce(lak::move(init), thread_outputs[o].release());
						  }
						  return;
					  }
				  }

				  // run threads
				  size_t index = 0U;
				  while (begin != end)
				  {
					  init =
					    binary_reduce(lak::move(init), thread_outputs[index].release());
					  thread_inputs[index].emplace(*begin, *begins...);
					  ++begin;
					  ++index;
					  index %= thread_count;
				  }

				  // finish outputs
				  for (size_t o = 0U; o < thread_count; ++o)
				  {
					  init = binary_reduce(
					    lak::move(init),
					    thread_outputs[(o + index) % thread_count].release());
				  }
			  },
			  thread_count);
		}
	}
	else
	{
		for (; begin != end; ++begin, ((++begins), ...))
			init = binary_reduce(lak::move(init), trans_func(*begin, *begins...));
	}

	return init;
}

/* --- count --- */

template<std::forward_iterator ITER, typename T>
size_t lak::count(ITER begin, ITER end, const T &value)
{
	size_t result = 0;

	for (; begin != end; ++begin)
		if (*begin == value) ++result;

	return result;
}

/* --- distance --- */

template<std::input_iterator ITER>
typename std::iterator_traits<ITER>::difference_type lak::distance(ITER begin,
                                                                   ITER end)
{
	if constexpr (std::random_access_iterator<ITER>)
	{
		return end - begin;
	}
	else
	{
		typename std::iterator_traits<ITER>::difference_type result = 0;
		for (; begin != end; ++begin, ++result);
		return result;
	}
}

/* --- advance --- */

template<std::input_iterator ITER>
void lak::advance(ITER &it,
                  typename std::iterator_traits<ITER>::difference_type offset)
{
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

template<std::input_iterator ITER>
ITER lak::next(ITER it,
               typename std::iterator_traits<ITER>::difference_type offset)
{
	lak::advance(it, offset);
	return it;
}

/* --- find --- */

template<std::forward_iterator ITER, typename T>
ITER lak::find(ITER begin, ITER end, const T &value)
{
	for (; begin != end && *begin != value; ++begin);
	return begin;
}

/* --- find_if --- */

template<std::forward_iterator ITER>
ITER lak::find_if(ITER begin, ITER end, auto predicate)
{
	for (; begin != end && !predicate(*begin); ++begin);
	return begin;
}

/* --- any_of --- */

template<std::forward_iterator ITER>
bool lak::any_of(ITER begin, ITER end, auto predicate)
{
	for (; begin != end; ++begin)
		if (predicate(*begin)) return true;
	return false;
}

/* --- all_of --- */

template<std::forward_iterator ITER>
bool lak::all_of(ITER begin, ITER end, auto predicate)
{
	for (; begin != end; ++begin)
		if (!predicate(*begin)) return false;
	return true;
}

/* --- none_of --- */

template<std::forward_iterator ITER>
bool lak::none_of(ITER begin, ITER end, auto predicate)
{
	for (; begin != end; ++begin)
		if (predicate(*begin)) return false;
	return true;
}

/* --- mismatch --- */

template<std::forward_iterator ITER_A, std::forward_iterator ITER_B>
lak::pair<ITER_A, ITER_B> lak::mismatch(ITER_A begin_a,
                                        ITER_A end_a,
                                        ITER_B begin_b,
                                        ITER_B end_b)
{
	for (; begin_a != end_a && begin_b != end_b && *begin_a == *begin_b;
	     ++begin_a, ++begin_b);
	return {begin_a, begin_b};
}

/* --- is_permutation --- */

template<std::forward_iterator ITER_A, std::forward_iterator ITER_B>
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

template<std::forward_iterator ITER>
void lak::rotate_left(
  ITER begin,
  typename std::iterator_traits<ITER>::difference_type end_offset,
  size_t distance)
{
	//  rotate left 11
	//
	// v <- working_size = 26 -> v
	// ABCDEFGHIJKLMNOPQRSTUVWXYZ
	// ^ <- begin ^ <- target    ^ <- end
	//
	// distance = 11
	// working_size = 26
	//
	// v <-   15   -> v            <- working_size - distance
	// ABCDEFGHIJKLMNOPQRSTUVWXYZ    swap
	// ^ <- 11 -> ^ <-   15   -> ^
	//
	//                v <- 11 -> v <- working_size
	// LMNOPQRSTUVWXYZEFGHIJKABCD
	//
	// distance = slack(26, 11) = 7
	// working_size = prev distance = 11
	//
	//                v 4 v        <- working_size - distance
	// LMNOPQRSTUVWXYZEFGHIJKABCD    swap
	//                ^  7   ^ 4 ^
	//
	//                    v  7   v <- working_size
	// LMNOPQRSTUVWXYZABCDIJKEFGH
	//
	// distance = slack(11, 7) = 3
	// working_size = prev distance = 7
	//
	//                    v 4 v    <- working_size - distance
	// LMNOPQRSTUVWXYZABCDIJKEFGH    swap
	//                    ^3 ^ 4 ^
	//
	//                        v  v <- working_size
	// LMNOPQRSTUVWXYZABCDEFGHJKI
	//
	// distance = slack(7, 3) = 2
	// working_size = prev distance = 3
	//
	//                        vv   <- working_size - distance
	// LMNOPQRSTUVWXYZABCDEFGHJKI
	//                          ^^
	//
	//                         v v <- working_size
	// LMNOPQRSTUVWXYZABCDEFGHIKJ
	//
	// distance = slack(3, 2) = 1
	// working_size = prev distance = 2
	//
	//                         vv  <- working_size - distance
	// LMNOPQRSTUVWXYZABCDEFGHIKJ
	//                          ^^
	//
	//                          vv <- working_size
	// LMNOPQRSTUVWXYZABCDEFGHIJK
	//
	// distance = slack(2, 1) = 1
	// working_size = prev distance = 1
	//
	//                          v  <- working_size - distance
	// LMNOPQRSTUVWXYZABCDEFGHIJK
	//
	// distance = slack(1, 1) = 0

	if (end_offset < 0) return;
	size_t working_size = static_cast<size_t>(end_offset);
	if (working_size == 0 || distance % working_size == 0) return;
	distance %= working_size;

	while (distance > 0 && working_size - distance > 0)
	{
		begin =
		  lak::swap(begin, lak::next(begin, distance), working_size - distance)
		    .first;
		distance =
		  lak::slack<size_t>(lak::exchange(working_size, distance), distance);
	}
}

template<std::forward_iterator ITER>
void lak::rotate_left(ITER begin, ITER end, size_t distance)
{
	lak::rotate_left(begin, lak::distance(begin, end), distance);
}

template<std::forward_iterator ITER>
void lak::rotate_left(ITER begin, ITER mid, ITER end)
{
	const size_t distance   = lak::distance(begin, mid);
	const auto working_size = distance + lak::distance(mid, end);
	lak::rotate_left(begin, working_size, distance);
}

/* --- rotate_right --- */

template<std::forward_iterator ITER>
void lak::rotate_right(
  ITER begin,
  typename std::iterator_traits<ITER>::difference_type end_offset,
  size_t distance)
{
	lak::rotate_left(
	  begin, end_offset, lak::slack<size_t>(distance, end_offset));
}

template<std::forward_iterator ITER>
void lak::rotate_right(ITER begin, ITER end, size_t distance)
{
	const size_t working_size = lak::distance(begin, end);
	lak::rotate_left(
	  begin, working_size, lak::slack<size_t>(distance, working_size));
}

template<std::forward_iterator ITER>
void lak::rotate_right(ITER begin, ITER mid, ITER end)
{
	const size_t distance   = lak::distance(mid, end);
	const auto working_size = distance + lak::distance(begin, mid);
	lak::rotate_left(
	  begin, working_size, lak::slack<size_t>(distance, working_size));
}

/* --- reverse --- */

template<std::bidirectional_iterator ITER>
void lak::reverse(ITER begin, ITER end)
{
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

/* --- erase_if --- */

template<std::forward_iterator ITER>
ITER lak::erase_if(ITER begin, ITER end, auto predicate)
{
	if (begin == end) return end;

	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// ^ <- begin                            ^ <- end
	//
	// KKKKEEEEEEKKKKKKKKEEEEKKKKEEEEEKKKEEEE
	//     ^ <- first erase
	//
	// KKKKEEEEEEKKKKKKKKEEEEKKKKEEEEEKKKEEEE
	//     ^     ^ <- swap
	// KKKKKEEEEEEKKKKKKKEEEEKKKKEEEEEKKKEEEE
	//      ^     ^ <- swap
	// KKKKKKEEEEEEKKKKKKEEEEKKKKEEEEEKKKEEEE
	//       ^     ^ <- swap
	// ...
	// KKKKKKKKKKKKEEEEEEEEEEKKKKEEEEEKKKEEEE
	//             ^         ^ <- swap
	// ...
	//
	// KKKKKKKKKKKKKKKKKKKEEEEEEEEEEEEEEEEEEE
	//                    ^ <- result

	ITER first_erase = begin;
	while (first_erase != end && !predicate(*first_erase)) ++first_erase;

	ITER iter = first_erase;
	if (iter != end) ++iter;

	for (; iter != end; ++iter)
	{
		if (!predicate(*iter))
		{
			lak::swap(*iter, *first_erase);
			++first_erase;
		}
	}

	return first_erase;
}

/* --- erase_if_contains --- */

template<std::forward_iterator ITER1,
         std::forward_iterator ITER2,
         typename CMP>
ITER1 lak::erase_if_contains(ITER1 begin,
                             ITER1 end,
                             ITER2 erase_begin,
                             ITER2 erase_end,
                             CMP compare_i1_i2)
{
	return lak::erase_if(begin,
	                     end,
	                     [&](const auto &v)
	                     {
		                     return lak::any_of(erase_begin,
		                                        erase_end,
		                                        [&](const auto &e)
		                                        { return compare_i1_i2(v, e); });
	                     });
}

/* --- partition --- */

template<std::forward_iterator ITER>
ITER lak::partition(ITER begin, ITER end, auto predicate)
{
	if (begin == end) return end;

	if constexpr (std::bidirectional_iterator<ITER>)
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
		ITER last_true   = lak::next(end, -1);

		while (first_false != last_true && predicate(*first_false)) ++first_false;
		while (last_true != first_false && !predicate(*last_true)) --last_true;

		while (first_false != last_true)
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

template<std::random_access_iterator ITER>
ITER lak::stable_partition(ITER begin, ITER end, auto predicate)
{
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

template<std::forward_iterator ITER, typename CMP>
ITER lak::binary_partition(ITER begin, ITER mid, ITER end, CMP compare)
{
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// ^ <- begin       ^ <- mid             ^ <- end
	//
	//                   v~~~~~~~~~~~~~~~~~~~v <- partition
	// TTTTFFFFFFTTTTTTTAFFFFTTTTFFFFFTTTFFFF
	// ^~~~~~~~~~~~~~~~~^ <- partition
	//
	// TTTTTTTTTTTFFFFFFATTTTTTTFFFFFFFFFFFFF
	//            ^~~~~~^~~~~~~~^ <- pivot swap
	//
	// TTTTTTTTTTTTTTTTTTAFFFFFFFFFFFFFFFFFFF
	//                   ^ <- result

	if (begin == end) return end;

	auto predicate = [&](const auto &v) { return compare(v, *mid); };

	return lak::pivot_swap(lak::partition(begin, mid, predicate),
	                       mid,
	                       lak::partition(lak::next(mid), end, predicate));
}

/* --- mark_and_sweep_parition --- */

template<typename T, std::forward_iterator ITER, typename CMP>
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

/* --- dedup --- */

template<std::forward_iterator ITER, typename CMP>
ITER lak::dedup(ITER begin, ITER end, CMP compare)
{
	for (; begin != end; ++begin)
		end =
		  lak::partition(lak::next(begin),
		                 end,
		                 [&](const auto &rhs) { return !compare(*begin, rhs); });
	return begin;
}

/* --- stable_dedup --- */

template<std::random_access_iterator ITER, typename CMP>
ITER lak::stable_dedup(ITER begin, ITER end, CMP compare)
{
	for (; begin != end; ++begin)
		end = lak::stable_partition(lak::next(begin),
		                            end,
		                            [&](const auto &rhs)
		                            { return !compare(*begin, rhs); });
	return begin;
}

/* --- merge --- */

template<std::forward_iterator ITER, typename CMP>
ITER lak::merge(ITER begin, ITER mid, ITER end, CMP compare)
{
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

template<std::forward_iterator ITER_A,
         std::forward_iterator ITER_B,
         typename ITER_OUT,
         typename CMP>
requires(
  std::output_iterator<typename std::iterator_traits<ITER_A>::value_type,
                       ITER_OUT> &&
  std::output_iterator<typename std::iterator_traits<ITER_B>::value_type,
                       ITER_OUT>)
ITER_OUT lak::merge(ITER_A begin_a,
                    ITER_A end_a,
                    ITER_B begin_b,
                    ITER_B end_b,
                    ITER_OUT output,
                    CMP compare)
{
	for (; begin_a != end_a && begin_b != end_b; ++output)
		if (compare(*begin_a, *begin_b))
		{
			*output = *begin_a;
			++begin_a;
		}
		else
		{
			*output = *begin_b;
			++begin_b;
		}

	if (begin_a != end_a)
		return lak::copy(begin_a, end_a, output);
	else if (begin_b != end_b)
		return lak::copy(begin_b, end_b, output);
	else
		return output;
}

/* --- binary_tree_is_left_child --- */

constexpr inline bool lak::binary_tree_is_left_child(size_t child)
{
	return (child & 1U) != 0U;
}

template<std::random_access_iterator ITER>
bool lak::binary_tree_is_left_child(ITER root, ITER child)
{
	return lak::binary_tree_is_left_child(child - root);
}

/* --- binary_tree_left_child --- */

constexpr inline size_t lak::binary_tree_left_child(size_t parent)
{
	return (parent << 1U) + 1U;
}

template<std::random_access_iterator ITER>
ITER lak::binary_tree_left_child(ITER root, ITER parent)
{
	return root + lak::binary_tree_left_child(parent - root);
}

/* --- binary_tree_right_child --- */

constexpr inline size_t lak::binary_tree_right_child(size_t parent)
{
	return (parent << 1U) + 2U;
}

template<std::random_access_iterator ITER>
ITER lak::binary_tree_right_child(ITER root, ITER parent)
{
	return root + lak::binary_tree_right_child(parent - root);
}

/* --- binary_tree_parent --- */

constexpr inline size_t lak::binary_tree_parent(size_t child)
{
	return (child - 1U) >> 1U;
}

template<std::random_access_iterator ITER>
ITER lak::binary_tree_parent(ITER root, ITER child)
{
	return root + lak::binary_tree_parent(child - root);
}

/* --- make_heap --- */

template<std::random_access_iterator ITER, typename CMP>
void lak::make_heap(ITER begin, ITER end, CMP compare)
{
	for (ITER it = lak::binary_tree_parent(begin, end - 1) + 1; it != begin;)
		lak::sift_down_heap(begin, --it, end, compare);
}

/* --- is_heap --- */

template<std::random_access_iterator ITER, typename CMP>
bool lak::is_heap(ITER begin, ITER end, CMP compare)
{
	return lak::is_heap_until(begin, end, compare) == end;
}

/* --- is_heap_until --- */

template<std::random_access_iterator ITER, typename CMP>
ITER lak::is_heap_until(ITER begin, ITER end, CMP compare)
{
	if ((end - begin) <= 1U) return end;

	auto parent_iter = [&](size_t index) -> ITER
	{ return begin + ((index - 1U) >> 1U); };

	ITER it = begin + 1U;
	for (size_t i = 1U; it != end && !compare(*parent_iter(i), *it); ++it, ++i);
	return it;
}

/* --- sift_down_heap --- */

template<std::random_access_iterator ITER, typename CMP>
void lak::sift_down_heap(ITER begin, ITER to_sift, ITER end, CMP compare)
{
	auto index_of = [&](const ITER &iter) -> size_t
	{ return size_t(iter - begin); };

	if (index_of(end) <= 1) return;

	// end index rounded to the nearest left <= end
	auto left_end = [&]() -> size_t
	{
		const size_t last = index_of(end);
		return last - (1U - (last & 1U));
	};

	// if there's an odd number of elements in the heap, then there's always a
	// left and right child. if there's an even number of elements, then the last
	// element is a left child (which has no right sibling).

	for (size_t end_index   = left_end(),
	            child_index = lak::binary_tree_left_child(index_of(to_sift));
	     child_index < end_index;
	     child_index = lak::binary_tree_left_child(index_of(to_sift)))
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

	if (size_t left_index = lak::binary_tree_left_child(index_of(to_sift));
	    left_index < index_of(end))
	{
		ITER left = begin + left_index;
		if (compare(*to_sift, *left)) lak::swap(*to_sift, *left);
	}
}

/* --- sift_up_heap --- */

template<std::random_access_iterator ITER, typename CMP>
void lak::sift_up_heap(ITER begin, ITER to_sift, CMP compare)
{
	if (to_sift == begin) return;

	for (ITER sift_parent = lak::binary_tree_parent(begin, to_sift);
	     compare(*sift_parent, *to_sift);)
	{
		lak::swap(*sift_parent, *to_sift);
		if (sift_parent == begin) break;
		to_sift     = sift_parent;
		sift_parent = lak::binary_tree_parent(begin, to_sift);
	}
}

/* --- push_heap --- */

template<std::random_access_iterator ITER, typename CMP>
void lak::push_heap(ITER begin, ITER end, CMP compare)
{
	if ((end - begin) <= 1U) return;

	lak::sift_up_heap(begin, end - 1, compare);
}

/* --- pop_heap --- */

template<std::random_access_iterator ITER, typename CMP>
void lak::pop_heap(ITER begin, ITER end, CMP compare)
{
	if ((end - begin) <= 1U) return;

	--end;
	lak::swap(*begin, *end);
	lak::sift_down_heap(begin, begin, end, compare);
}

/* --- sort_heap --- */

template<std::random_access_iterator ITER, typename CMP>
void lak::sort_heap(ITER begin, ITER end, CMP compare)
{
	if (begin != end) --end;
	for (; begin != end; --end)
	{
		lak::swap(*begin, *end);
		lak::sift_down_heap(begin, begin, end, compare);
	}
}

/* --- reverse_sort_heap --- */

template<std::random_access_iterator ITER, typename CMP>
void lak::reverse_sort_heap(ITER begin, ITER end, CMP compare)
{
	lak::sort_heap(begin, end, compare);
	lak::reverse(begin, end);
}

/* --- breadth_first_search_heap --- */

template<std::input_iterator ITER, typename F>
ITER lak::breadth_first_search_heap(ITER begin, ITER end, F &&predicate)
{
	for (; begin != end && !predicate(*begin); ++begin);
	return begin;
}

/* --- depth_first_search_heap --- */

template<std::random_access_iterator ITER, typename F>
ITER lak::depth_first_search_heap(ITER begin, ITER end, F &&predicate)
{
	if (begin == end) return begin;
	size_t size = end - begin;

	auto next_index = [size](size_t index) -> size_t
	{
		auto right_sibling = [](size_t index) -> size_t { return index + 1U; };

		if (index >= size - 1U) return size;

		if (auto l = lak::binary_tree_left_child(index); l < size) return l;

		while (index != 0U)
		{
			while (!lak::binary_tree_is_left_child(index))
				index = lak::binary_tree_parent(index);
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

template<std::random_access_iterator ITER, typename CMP>
void lak::heapsort(ITER begin, ITER end, CMP compare)
{
	if ((end - begin) <= 1U) return;

	lak::make_heap(begin, end, compare);
	lak::sort_heap(begin, end, compare);
}

/* --- partition_sort --- */

template<std::forward_iterator ITER, typename CMP>
ITER lak::partition_sort(ITER begin, ITER end, CMP compare)
{
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// ^ <- cursor                           ^ <- end
	//
	// ATTTTFFFFFFTTTTTTTFFFFTTTTFFFFFTTTFFFF
	//  ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^ <- partition
	// ATTTTTTTTTTTTTTTTTTFFFFFFFFFFFFFFFFFFF
	//  ^ <- cursor       ^ <- end
	//
	// ABFFFTTTFFFFFFFTTFF0000000000000000000
	//   ^~~~~~~~~~~~~~~~^ <- partition
	// ABTTTTTFFFFFFFFFFFF0000000000000000000
	//   ^cur ^end
	//
	// ABCFFFF1111111111110000000000000000000
	//    ^~~^ <- partition
	// ABCFFFF1111111111110000000000000000000
	//    ^ <- cursor = end
	//
	// ABC22221111111111110000000000000000000
	//    ^ <- result

	if (begin == end) return end;

	auto cursor{begin};
	auto iter{lak::next(begin)};

	for (; cursor != end && iter != end; ++iter)
	{
		end = lak::partition(
		  iter, end, [&](const auto &value) { return compare(value, *cursor); });

		cursor = iter;
	}

	return end;
}

/* --- stable_partition_sort --- */

template<std::forward_iterator ITER, typename CMP>
ITER lak::stable_partition_sort(ITER begin, ITER end, CMP compare)
{
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// ^ <- cursor                           ^ <- end
	//
	// ATTTTFFFFFFTTTTTTTFFFFTTTTFFFFFTTTFFFF
	//  ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^ <- stable partition
	// ATTTTTTTTTTTTTTTTTTFFFFFFFFFFFFFFFFFFF
	//  ^ <- cursor       ^ <- end
	//
	// ABFFFTTTFFFFFFFTTFF0000000000000000000
	//   ^~~~~~~~~~~~~~~~^ <- stable partition
	// ABTTTTTFFFFFFFFFFFF0000000000000000000
	//   ^cur ^end
	//
	// ABCFFFF1111111111110000000000000000000
	//    ^~~^ <- stable partition
	// ABCFFFF1111111111110000000000000000000
	//    ^ <- cursor = end
	//
	// ABC22221111111111110000000000000000000
	//    ^ <- result

	if (begin == end) return end;

	auto cursor{begin};
	auto iter{lak::next(begin)};

	for (; cursor != end && iter != end; ++iter)
	{
		end = lak::stable_partition(
		  iter, end, [&](const auto &value) { return compare(value, *cursor); });

		cursor = iter;
	}

	return end;
}

/* --- partial_order_sort --- */

template<std::forward_iterator ITER, typename CMP>
void lak::partial_order_sort(ITER begin, ITER end, CMP compare)
{
	while (lak::distance(begin, end) > 1)
	{
		if (lak::none_of(lak::next(begin),
		                 end,
		                 [&](const auto &v) { return compare(v, *begin); }))
		{
			++begin;
			continue;
		}
		for (ITER it = lak::next(begin); it != end; ++it)
			if (auto cmp = [&](const auto &v) { return compare(v, *it); };
			    lak::none_of(begin, it, cmp) &&
			    lak::none_of(lak::next(it), end, cmp))
				lak::swap(*(begin++), *it);
	}
}

/* --- minmax_element --- */

template<std::forward_iterator ITER, typename CMP>
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

template<std::forward_iterator ITER, typename CMP>
ITER lak::max_element(ITER begin, ITER end, CMP compare)
{
	if (begin == end) return end;

	ITER result = begin;

	for (++begin; begin != end; ++begin)
		if (compare(*result, *begin)) result = begin;

	return result;
}

/* --- min_element --- */

template<std::forward_iterator ITER, typename CMP>
ITER lak::min_element(ITER begin, ITER end, CMP compare)
{
	if (begin == end) return end;

	ITER result = begin;

	for (++begin; begin != end; ++begin)
		if (compare(*begin, *result)) result = begin;

	return result;
}

/* --- lower_bound --- */

template<std::forward_iterator ITER, typename U, typename CMP>
ITER lak::lower_bound(ITER begin, ITER end, const U &value, CMP compare)
{
	auto diff{lak::distance(begin, end)};

	while (diff > 0U)
	{
		ITER iter       = begin;
		const auto step = diff / 2U;
		lak::advance(iter, step);
		if (compare(*iter, value)) // *iter < value
		{
			// move the lower bound up
			begin = lak::next(iter);
			diff -= step + 1U;
		}
		else // *iter >= value
			// move the upper bound down
			diff /= 2U;
	}

	return begin;
}

/* --- upper_bound --- */

template<std::forward_iterator ITER, typename U, typename CMP>
ITER lak::upper_bound(ITER begin, ITER end, const U &value, CMP compare)
{
	auto diff{lak::distance(begin, end)};

	while (diff > 0U)
	{
		ITER iter       = begin;
		const auto step = diff / 2U;
		lak::advance(iter, step);
		if (!compare(value, *iter)) // value >= *iter
		{
			// move the lower bound up
			begin = lak::next(iter);
			diff -= step + 1U;
		}
		else // value < *iter
			// move the upper bound down
			diff /= 2U;
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
