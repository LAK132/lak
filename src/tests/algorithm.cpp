#include "lak/test.hpp"

#include "lak/algorithm.hpp"
#include "lak/array.hpp"
#include "lak/span_manip.hpp"

#include "iterator_wrappers.hpp"

BEGIN_TEST(iterator_wrapper)
{
	lak::array values{1, 2, 3};

	lak::swap(*forward_iterator_wrapper{values.begin()},
	          *forward_iterator_wrapper{values.begin() + 1});

	ASSERT_EQUAL(values[0], 2);
	ASSERT_EQUAL(values[1], 1);

	lak::swap(*bidirectional_iterator_wrapper{values.begin()},
	          *bidirectional_iterator_wrapper{values.begin() + 1});

	ASSERT_EQUAL(values[0], 1);
	ASSERT_EQUAL(values[1], 2);

	lak::swap(*random_access_iterator_wrapper{values.begin()},
	          *random_access_iterator_wrapper{values.begin() + 1});

	ASSERT_EQUAL(values[0], 2);
	ASSERT_EQUAL(values[1], 1);

	return EXIT_SUCCESS;
}
END_TEST()

// BEGIN_TEST(range_swap)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(count)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(distance)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(advance)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(next)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(find)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(mismatch)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

BEGIN_TEST(is_permutation)
{
	auto a = {1, 2, 3, 4, 5};
	auto b = {4, 3, 5, 2, 1};
	auto c = {4, 3, 5, 2, 2};

	ASSERT(lak::is_permutation(a.begin(), a.end(), b.begin(), b.end()));
	ASSERT(!lak::is_permutation(a.begin() + 1, a.end(), b.begin(), b.end()));
	ASSERT(!lak::is_permutation(a.begin(), a.end(), c.begin(), c.end()));

	return EXIT_SUCCESS;
}
END_TEST()

// BEGIN_TEST(rotate_left)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(rotate_right)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

BEGIN_TEST(reverse)
{
	auto wrapped_test = []<typename WRAPPER>(WRAPPER &&)
	{
		using wrapper_t =
		  typename WRAPPER::template type<lak::array<intmax_t>::iterator>;

		{
			auto source   = {1, 2, 3, 4, 5, 6};
			auto expected = {6, 5, 4, 3, 2, 1};

			lak::array<intmax_t> values{source.begin(), source.end()};

			lak::reverse(wrapper_t{values.begin()}, wrapper_t{values.end()});

			for (size_t i = 0; i < expected.size(); ++i)
			{
				ASSERT_EQUAL(expected.begin()[i], values[i]);
			}
		}

		{
			auto source   = {1, 2, 3, 4, 5, 6, 7};
			auto expected = {7, 6, 5, 4, 3, 2, 1};

			lak::array<intmax_t> values{source.begin(), source.end()};

			lak::reverse(wrapper_t{values.begin()}, wrapper_t{values.end()});

			for (size_t i = 0; i < expected.size(); ++i)
			{
				ASSERT_EQUAL(expected.begin()[i], values[i]);
			}
		}
	};

	wrapped_test(bidirectional_iterator_wrapper<void>{});
	wrapped_test(random_access_iterator_wrapper<void>{});

	return EXIT_SUCCESS;
}
END_TEST()

BEGIN_TEST(partition)
{
	auto wrapped_test = []<typename WRAPPER>(WRAPPER &&)
	{
		auto source = {-4, 3, -2, -1, 0, 1, 2, -3, 4, 5};

		lak::array<intmax_t> values{source.begin(), source.end()};

		using wrapper_t =
		  typename WRAPPER::template type<lak::array<intmax_t>::iterator>;

		auto mid = lak::partition(wrapper_t{values.begin()},
		                          wrapper_t{values.end()},
		                          [](intmax_t v) { return v >= 0; })
		             ._iter;

		ASSERT(lak::is_permutation(
		  source.begin(), source.end(), values.begin(), values.end()));

		auto [pos, neg] = lak::split(lak::span(values), mid);

		ASSERT_EQUAL(pos.size(), 6U);
		ASSERT_EQUAL(neg.size(), 4U);

		for (const auto &p : pos) ASSERT_GREATER_OR_EQUAL(p, intmax_t(0));
		for (const auto &n : neg) ASSERT_LESS(n, intmax_t(0));
	};

	wrapped_test(forward_iterator_wrapper<void>{});
	wrapped_test(bidirectional_iterator_wrapper<void>{});
	wrapped_test(random_access_iterator_wrapper<void>{});

	return EXIT_SUCCESS;
}
END_TEST()

BEGIN_TEST(stable_partition)
{
	auto source   = {-4, 3, -2, -1, 0, 1, 2, -3, 4, 5};
	auto expected = {3, 0, 1, 2, 4, 5, -4, -2, -1, -3};

	lak::array<intmax_t> values{source.begin(), source.end()};

	auto mid = lak::stable_partition(
	  values.begin(), values.end(), [](intmax_t v) { return v >= 0; });

	ASSERT_EQUAL(mid - values.begin(), 6);

	for (size_t i = 0; i < expected.size(); ++i)
	{
		ASSERT_EQUAL(expected.begin()[i], values[i]);
	}

	return EXIT_SUCCESS;
}
END_TEST()

BEGIN_TEST(binary_partition)
{
	auto wrapped_test = []<typename WRAPPER>(WRAPPER &&)
	{
		auto source = {-4, 3, -2, -1, 0, 1, 2, -3, 4, 5};

		lak::array<intmax_t> values{source.begin(), source.end()};

		using wrapper_t =
		  typename WRAPPER::template type<lak::array<intmax_t>::iterator>;

		auto zero = lak::find(values.begin(), values.end(), 0);
		ASSERT(zero != values.end());
		ASSERT_EQUAL(*zero, 0);

		auto mid = lak::binary_partition(wrapper_t{values.begin()},
		                                 wrapper_t{zero},
		                                 wrapper_t{values.end()})
		             ._iter;

		ASSERT(mid != values.end());
		ASSERT_EQUAL(*mid, 0);

		ASSERT(lak::is_permutation(
		  source.begin(), source.end(), values.begin(), values.end()));

		auto [neg, pos] = lak::split(lak::span(values), mid);

		ASSERT_EQUAL(pos.size(), 6U);
		ASSERT_EQUAL(neg.size(), 4U);

		for (const auto &p : pos) ASSERT_GREATER_OR_EQUAL(p, intmax_t(0));
		for (const auto &n : neg) ASSERT_LESS(n, intmax_t(0));
	};

	wrapped_test(forward_iterator_wrapper<void>{});
	wrapped_test(bidirectional_iterator_wrapper<void>{});
	wrapped_test(random_access_iterator_wrapper<void>{});

	return EXIT_SUCCESS;
}
END_TEST()

// BEGIN_TEST(merge)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

BEGIN_TEST(make_heap)
{
	{
		auto values = lak::fixed_array(0);
		auto copy   = values;
		lak::make_heap(values.begin(), values.end());
		ASSERT(lak::is_heap(values.begin(), values.end()));
		ASSERT(lak::is_permutation(
		  values.begin(), values.end(), copy.begin(), copy.end()));
	}

	{
		auto values = lak::fixed_array(1, 2, 3, 4, 5, 6, 7);
		auto copy   = values;
		ASSERT(!lak::is_heap(values.begin(), values.end()));
		lak::make_heap(values.begin(), values.end());
		ASSERT(lak::is_heap(values.begin(), values.end()));
		ASSERT(lak::is_permutation(
		  values.begin(), values.end(), copy.begin(), copy.end()));
	}

	{
		auto values = lak::fixed_array(1, 2, 3, 5, 6, 4, 0, -1);
		auto copy   = values;
		ASSERT(!lak::is_heap(values.begin(), values.end()));
		lak::make_heap(values.begin(), values.end());
		ASSERT(lak::is_heap(values.begin(), values.end()));
		ASSERT(lak::is_permutation(
		  values.begin(), values.end(), copy.begin(), copy.end()));
	}

	return EXIT_SUCCESS;
}
END_TEST()

BEGIN_TEST(is_heap)
{
	{
		auto values = lak::fixed_array(0);
		ASSERT_EQUAL(
		  lak::is_heap_until(values.begin(), values.end()) - values.end(), 0U);
		ASSERT(lak::is_heap(values.begin(), values.end()));
	}

	{
		auto values = lak::fixed_array(0, 0, 0, 0, 0);
		ASSERT_EQUAL(
		  lak::is_heap_until(values.begin(), values.end()) - values.end(), 0U);
		ASSERT(lak::is_heap(values.begin(), values.end()));
	}

	{
		auto values = lak::fixed_array(7, 6, 5, 4, 3, 3, 2, 1);
		ASSERT_EQUAL(
		  lak::is_heap_until(values.begin(), values.end()) - values.end(), 0U);
		ASSERT(lak::is_heap(values.begin(), values.end()));
	}

	{
		auto values = lak::fixed_array(1, 2, 3, 4, 5, 6);
		ASSERT_EQUAL(
		  lak::is_heap_until(values.begin(), values.end()) - values.begin(), 1U);
		ASSERT(!lak::is_heap(values.begin(), values.end()));
	}

	{
		auto values = lak::fixed_array(7, 6, 5, 4, 1, 2, 3, 4, 5, 6);
		ASSERT_EQUAL(
		  lak::is_heap_until(values.begin(), values.end()) - values.begin(), 8U);
		ASSERT(!lak::is_heap(values.begin(), values.end()));
	}

	return EXIT_SUCCESS;
}
END_TEST()

BEGIN_TEST(sift_down_heap)
{
	{
		auto values = lak::fixed_array(1, 2, 3, 5, 6, 4, 0, -1);

		auto index_of = [&](const auto &iter) -> size_t
		{ return size_t(iter - values.begin()); };

		auto parent = [](size_t index) -> size_t { return (index - 1U) >> 1U; };
		auto parent_iter = [&](const auto &iter)
		{ return values.begin() + parent(index_of(iter)); };

		for (auto it = parent_iter(values.end() - 1) + 1; it != values.begin();)
			lak::sift_down_heap(values.begin(), --it, values.end());

		ASSERT(lak::is_heap(values.begin(), values.end()));
	}

	return EXIT_SUCCESS;
}
END_TEST()

// BEGIN_TEST(sift_up_heap)
// {
// 	//
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(push_heap)
// {
// 	//
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(pop_heap)
// {
// 	//
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(sort_heap)
// {
// 	//
// 	return EXIT_SUCCESS;
// }
// END_TEST()

BEGIN_TEST(heapsort)
{
	auto source   = {5, 2, 4, 3, 6, 1};
	auto expected = {1, 2, 3, 4, 5, 6};

	lak::array<intmax_t> values{source.begin(), source.end()};

	lak::heapsort(values.begin(), values.end());

	ASSERT_ARRAY_EQUAL(lak::span(values), lak::span(expected));

	return EXIT_SUCCESS;
}
END_TEST()

BEGIN_TEST(minmax_element)
{
	auto values = {0, 1, 2, 4, -10, 100, -1000, 4};

	auto [min, max] = lak::minmax_element(values.begin(), values.end());

	ASSERT(min == values.begin() + 6);
	ASSERT(max == values.begin() + 5);

	return EXIT_SUCCESS;
}
END_TEST()

BEGIN_TEST(min_element)
{
	auto values = {0, 1, 2, 4, -10, 100, -1000, 4};

	auto min = lak::min_element(values.begin(), values.end());

	ASSERT(min == values.begin() + 6);

	return EXIT_SUCCESS;
}
END_TEST()

BEGIN_TEST(max_element)
{
	auto values = {0, 1, 2, 4, -10, 100, -1000, 4};

	auto max = lak::max_element(values.begin(), values.end());

	ASSERT(max == values.begin() + 5);

	return EXIT_SUCCESS;
}
END_TEST()

// BEGIN_TEST(lower_bound)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(upper_bound)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()

// BEGIN_TEST(equal_range)
// {
// 	return EXIT_SUCCESS;
// }
// END_TEST()
