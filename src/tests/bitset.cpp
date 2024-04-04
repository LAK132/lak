#include "lak/bitset.hpp"

#include "lak/test.hpp"

BEGIN_TEST(bitset)
{
	{
		lak::bitset<4> test;
		ASSERT(!test.get(0));
		ASSERT(!test.get(1));
		ASSERT(!test.get(2));
		ASSERT(!test.get(3));

		test.set(2, true);
		ASSERT(!test.get(0));
		ASSERT(!test.get(1));
		ASSERT(test.get(2));
		ASSERT(!test.get(3));

		test.set(1, true);
		ASSERT(!test.get(0));
		ASSERT(test.get(1));
		ASSERT(test.get(2));
		ASSERT(!test.get(3));

		test.set(3, true);
		ASSERT(!test.get(0));
		ASSERT(test.get(1));
		ASSERT(test.get(2));
		ASSERT(test.get(3));

		test.set(0, true);
		ASSERT(test.get(0));
		ASSERT(test.get(1));
		ASSERT(test.get(2));
		ASSERT(test.get(3));
	}

	return 0;
}
END_TEST()
