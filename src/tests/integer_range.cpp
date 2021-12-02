#include "lak/test.hpp"

#include "lak/integer_range.hpp"

BEGIN_TEST(integer_range)
{
	{
		size_t i = 10;
		for (size_t x : lak::size_range::from_to(10, 100))
		{
			ASSERT_EQUAL(i, x);
			++i;
		}
		ASSERT_EQUAL(i, 100);
	}

	{
		size_t i = 10;
		for (size_t x : lak::size_range::from_count(10, 100))
		{
			ASSERT_EQUAL(i, x);
			++i;
		}
		ASSERT_EQUAL(i, 110);
	}

	return 0;
}
END_TEST()
