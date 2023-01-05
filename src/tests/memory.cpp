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
		ASSERT(lak::equal_to<>{}(ptr.get(), ptr2.get()));

		*ptr2 = 10;

		ASSERT_EQUAL(*ptr, 10);
		ASSERT_EQUAL(*ptr2, 10);

		auto ptr3{lak::shared_ptr<void>(lak::shared_ptr<int>(lak::move(ptr2)))};

		ASSERT_EQUAL(ptr2.use_count(), 0U);
		ASSERT_EQUAL(ptr.use_count(), 2U);
		ASSERT_EQUAL(ptr3.use_count(), 2U);
		ASSERT(lak::equal_to<>{}(ptr.get(), ptr3.get()));

		auto ptr4{
		  lak::shared_ptr<const void>(lak::shared_ptr<void>(lak::move(ptr3)))};

		ASSERT_EQUAL(ptr3.use_count(), 0U);
		ASSERT_EQUAL(ptr2.use_count(), 0U);
		ASSERT_EQUAL(ptr.use_count(), 2U);
		ASSERT_EQUAL(ptr4.use_count(), 2U);
		ASSERT(lak::equal_to<>{}(ptr.get(), ptr4.get()));
	}

	ASSERT_EQUAL(ptr.use_count(), 1U);

	return 0;
}
END_TEST()
