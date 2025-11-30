#include "lak/test.hpp"

#include "lak/memory.hpp"

template<template<typename> typename SHARED_PTR>
void shared_ptr_test_type_erasure()
{
	{
		auto ptr{SHARED_PTR<int>::make(0)};

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

			auto ptr3{SHARED_PTR<void>(SHARED_PTR<int>(lak::move(ptr2)))};

			ASSERT_EQUAL(ptr2.use_count(), 0U);
			ASSERT_EQUAL(ptr.use_count(), 2U);
			ASSERT_EQUAL(ptr3.use_count(), 2U);
			ASSERT(lak::equal_to<>{}(ptr.get(), ptr3.get()));

			auto ptr4{SHARED_PTR<const void>(SHARED_PTR<void>(lak::move(ptr3)))};

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
		auto ptr{SHARED_PTR<int[]>::make(5, 0, 1, 2, 3, 4)};

		ASSERT_EQUAL(ptr.use_count(), 1U);

		lak::span<int> s = *ptr;
		ASSERT_EQUAL(s.size(), 5U);

		const auto expected = {0, 1, 2, 3, 4};
		ASSERT_ARRAY_EQUAL(s, lak::span<const int>{expected});

		{
			auto ptr2{ptr};
			ASSERT_EQUAL(ptr.use_count(), 2U);
			ASSERT_EQUAL(ptr2.use_count(), 2U);
			ASSERT(lak::equal_to<>{}(ptr.get(), ptr2.get()));

			auto ptr3{SHARED_PTR<void>(SHARED_PTR<int[]>(lak::move(ptr2)))};
			ASSERT_EQUAL(ptr2.use_count(), 0U);
			ASSERT_EQUAL(ptr.use_count(), 2U);
			ASSERT_EQUAL(ptr3.use_count(), 2U);
			ASSERT(lak::equal_to<>{}(s.data(), ptr3.get()));

			auto ptr4{SHARED_PTR<const void>(SHARED_PTR<void>(lak::move(ptr3)))};
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
}

template<template<typename> typename SHARED_PTR,
         template<typename> typename SHARED_REF>
void shared_ptr_test_ref()
{
	{
		auto ptr{SHARED_PTR<int>::make(0)};

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

			auto ref{SHARED_REF<int>::make(ptr).UNWRAP()};

			ASSERT_EQUAL(ptr.use_count(), 3U);
			ASSERT_EQUAL(ref.use_count(), 3U);
			ASSERT(lak::equal_to<>{}(ptr.get(), ref.get()));
		}

		ASSERT_EQUAL(ptr.use_count(), 1U);

		ptr.reset();
		ASSERT_EQUAL(ptr.use_count(), 0U);
		ASSERT_EQUAL(ptr.get(), nullptr);
	}

	{
		auto ptr{SHARED_PTR<int[]>::make(5, 0, 1, 2, 3, 4)};

		ASSERT_EQUAL(ptr.use_count(), 1U);

		lak::span<int> s = *ptr;
		ASSERT_EQUAL(s.size(), 5U);

		const auto expected = {0, 1, 2, 3, 4};
		ASSERT_ARRAY_EQUAL(s, lak::span<const int>{expected});

		{
			auto ptr2{ptr};
			ASSERT_EQUAL(ptr.use_count(), 2U);
			ASSERT_EQUAL(ptr2.use_count(), 2U);
			ASSERT(lak::equal_to<>{}(ptr.get(), ptr2.get()));

			auto ref{SHARED_REF<int[]>::make(ptr).UNWRAP()};

			ASSERT_EQUAL(ptr.use_count(), 3U);
			ASSERT_EQUAL(ref.use_count(), 3U);
			ASSERT(lak::equal_to<>{}(s.data(), ref->data()));
			ASSERT_EQUAL(s.size(), ref->size());
		}

		ptr.reset();
		ASSERT_EQUAL(ptr.use_count(), 0U);
		ASSERT_EQUAL(ptr.get(), nullptr);
	}
}

BEGIN_TEST(memory)
{
	shared_ptr_test_ref<lak::tiny_shared_ptr, lak::tiny_shared_ref>();

	shared_ptr_test_ref<lak::shared_ptr, lak::shared_ref>();
	shared_ptr_test_type_erasure<lak::shared_ptr>();

	shared_ptr_test_ref<lak::strong_ptr, lak::strong_ref>();
	shared_ptr_test_type_erasure<lak::strong_ptr>();

	{
		auto strong{lak::strong_ptr<int>::make(132)};

		ASSERT_EQUAL(strong.use_count(), 1U);
		ASSERT_EQUAL(*strong, 132);

		auto weak{lak::weak_ptr(strong)};

		ASSERT_EQUAL(strong.use_count(), 1U);

		{
			auto ptr{weak.get()};

			ASSERT_EQUAL(strong.use_count(), 2U);
			ASSERT_EQUAL(ptr.use_count(), 2U);
			ASSERT(lak::equal_to<>{}(strong.get(), ptr.get()));
		}

		strong.reset();

		ASSERT_EQUAL(strong.use_count(), 0U);

		{
			auto ptr{weak.get()};
			ASSERT(!ptr);
		}
	}

	{
		auto ptr{lak::unique_ptr<int>::make(132)};

		ASSERT(!!ptr);

		ASSERT_EQUAL(*ptr, 132);
	}

	return 0;
}
END_TEST()
