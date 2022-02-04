#include "lak/test.hpp"

#include "lak/memory.hpp"

BEGIN_TEST(memory)
{
	auto ptr{lak::shared_ptr<int>::make(0)};

	ASSERT_EQUAL(ptr.use_count(), 1U);
	ASSERT_EQUAL(*ptr, 0);

	{
		auto ptr2{ptr};

		ASSERT_EQUAL(ptr.use_count(), 2U);
		ASSERT_EQUAL(ptr2.use_count(), 2U);

		*ptr2 = 10;

		ASSERT_EQUAL(*ptr, 10);
		ASSERT_EQUAL(*ptr2, 10);
	}

	ASSERT_EQUAL(ptr.use_count(), 1U);

	return 0;
}
END_TEST()
