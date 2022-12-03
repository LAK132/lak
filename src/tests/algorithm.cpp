#include "lak/test.hpp"

#include "lak/algorithm.hpp"
#include "lak/array.hpp"

#include "iterator_wrappers.hpp"

BEGIN_TEST(iterator_wrapper)
{
	lak::array values{1, 2, 3};

	lak::swap(*forward_iterator_wrapper(values.begin()),
	          *forward_iterator_wrapper(values.begin() + 1));

	ASSERT_EQUAL(values[0], 2);
	ASSERT_EQUAL(values[1], 1);

	lak::swap(*bidirectional_iterator_wrapper(values.begin()),
	          *bidirectional_iterator_wrapper(values.begin() + 1));

	ASSERT_EQUAL(values[0], 1);
	ASSERT_EQUAL(values[1], 2);

	lak::swap(*random_access_iterator_wrapper(values.begin()),
	          *random_access_iterator_wrapper(values.begin() + 1));

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
