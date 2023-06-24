#include "lak/test.hpp"

#include "lak/memory.hpp"

BEGIN_TEST(memory)
{
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

		ptr.reset();
		ASSERT_EQUAL(ptr.use_count(), 0U);
		ASSERT_EQUAL(ptr.get(), nullptr);
	}

	{
		auto ptr{lak::shared_ptr<int[]>::make(5, 0, 1, 2, 3, 4)};

		ASSERT_EQUAL(ptr.use_count(), 1U);

		lak::span<int> s = *ptr;
		ASSERT_EQUAL(s.size(), 5);

		const auto expected = {0, 1, 2, 3, 4};
		ASSERT_ARRAY_EQUAL(s, lak::span<const int>{expected});

		{
			auto ptr2{ptr};
			ASSERT_EQUAL(ptr.use_count(), 2U);
			ASSERT_EQUAL(ptr2.use_count(), 2U);
			ASSERT(lak::equal_to<>{}(ptr.get(), ptr2.get()));

			auto ptr3{
			  lak::shared_ptr<void>(lak::shared_ptr<int[]>(lak::move(ptr2)))};
			ASSERT_EQUAL(ptr2.use_count(), 0U);
			ASSERT_EQUAL(ptr.use_count(), 2U);
			ASSERT_EQUAL(ptr3.use_count(), 2U);
			ASSERT(lak::equal_to<>{}(s.data(), ptr3.get()));

			auto ptr4{
			  lak::shared_ptr<const void>(lak::shared_ptr<void>(lak::move(ptr3)))};
			ASSERT_EQUAL(ptr3.use_count(), 0U);
			ASSERT_EQUAL(ptr2.use_count(), 0U);
			ASSERT_EQUAL(ptr.use_count(), 2U);
			ASSERT_EQUAL(ptr4.use_count(), 2U);
			ASSERT(lak::equal_to<>{}(s.data(), ptr4.get()));
		}

		ptr.reset();
		ASSERT_EQUAL(ptr.use_count(), 0U);
		ASSERT_EQUAL(ptr.get(), nullptr);
	}

	return 0;
}
END_TEST()
