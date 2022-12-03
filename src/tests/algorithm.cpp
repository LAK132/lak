#include "lak/test.hpp"

#include "lak/algorithm.hpp"

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
