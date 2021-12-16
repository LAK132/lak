#include "lak/test.hpp"

#include "lak/integer_range.hpp"

BEGIN_TEST(integer_range)
{
	{
		size_t i = 10U;
		for (size_t x : lak::size_range::from_to(10U, 100U))
		{
			ASSERT_EQUAL(i, x);
			++i;
		}
		ASSERT_EQUAL(i, 100U);
	}

	{
		size_t i = 10U;
		for (size_t x : lak::size_range::from_count(10U, 100U))
		{
			ASSERT_EQUAL(i, x);
			++i;
		}
		ASSERT_EQUAL(i, 110U);
	}

	return 0;
}
END_TEST()
