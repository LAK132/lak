#include "lak/array.hpp"

#include "lak/concepts.hpp"
#include "lak/test.hpp"
#include "lak/utility.hpp"

static_assert(lak::concepts::contiguous_range_of<lak::array<int>, int>);
static_assert(lak::concepts::contiguous_range_of<lak::uninit_array<int>, int>);
static_assert(lak::concepts::contiguous_range_of<lak::array<lak::incomplete>,
                                                 lak::incomplete>);
static_assert(
  lak::concepts::contiguous_range_of<lak::uninit_array<lak::incomplete>,
                                     lak::incomplete>);

template<typename T>
lak::array<T> one_page_array(const T &default_value = {})
{
	lak::array<T> result;

	result.resize(lak::page_size() / sizeof(T), default_value);

	return result;
}

BEGIN_TEST(uninit_array)
{
	lak::uninit_array<int> array;
	{
		auto result{array.push_back()};
		ASSERT(!!result);
		ASSERT(result->empty());
		ASSERT_GREATER_OR_EQUAL(array.capacity(), 0x1);
		ASSERT_EQUAL(array.size(), 0x1);
	}
	{
		auto result{array.reserve(0x100)};
		ASSERT(!!result);
		ASSERT_EQUAL(result->size(), 1U);
		ASSERT_GREATER_OR_EQUAL(array.capacity(), 0x100);
	}
	{
		auto result{array.resize(0x10)};
		ASSERT(!result);
		ASSERT_GREATER_OR_EQUAL(array.capacity(), 0x100);
		ASSERT_EQUAL(array.size(), 0x10);
	}
	{
		lak::uninit_array<int> moved{lak::move(array)};
		ASSERT_EQUAL(array.capacity(), 0x0U);
		ASSERT_EQUAL(array.committed(), 0x0U);
		ASSERT_EQUAL(array.size(), 0x0U);
		ASSERT_GREATER_OR_EQUAL(moved.capacity(), 0x100);
		ASSERT_EQUAL(moved.size(), 0x10);
		array = lak::move(moved);
		ASSERT_EQUAL(moved.capacity(), 0x0U);
		ASSERT_EQUAL(moved.committed(), 0x0U);
		ASSERT_EQUAL(moved.size(), 0x0U);
		ASSERT_GREATER_OR_EQUAL(array.capacity(), 0x100);
		ASSERT_EQUAL(array.size(), 0x10);
	}
	return 0;
}
END_TEST()

BEGIN_TEST(array)
{
	lak::array<int> array;
	array.reserve(0x100);
	array.resize(0x10);
	array[0] = 0x10;
	array.resize(0x1000);
	array.resize(0x100);
	DEBUG(array[0]);
	DEBUG(uintptr_t(&array[0]));
	DEBUG(array.size());
	DEBUG(array.capacity());
	DEBUG(array.committed());

	[[maybe_unused]] auto span = lak::span(array);

	lak::array<int> array2 = lak::move(array);
	array2.resize(0x10);

	for (const auto &i : array2)
	{
		DEBUG(i);
	}

	{
		lak::array<int> array3;
		array3.resize(0x30);
		array3[10] = 40;

		ASSERT_EQUAL(array3.size(), 0x30U);
		ASSERT_EQUAL(array3[10], 40);

		array2 = array3; // test copying.

		ASSERT_NOT_EQUAL(array2.data(), array3.data());

		ASSERT_EQUAL(array3.size(), 0x30U);
		ASSERT_EQUAL(array3[10], 40);

		ASSERT_EQUAL(array2.size(), 0x30U);
		ASSERT_EQUAL(array2[10], 40);
	}

	ASSERT_EQUAL(array2.size(), 0x30U);
	ASSERT_EQUAL(array2[10], 40);

	array.force_clear();
	ASSERT_EQUAL(array.size(), 0x0U);
	ASSERT_EQUAL(array.capacity(), 0x0U);

	auto equals_index = [](lak::span<int> sp, size_t expected_size)
	{
		ASSERT_EQUAL(sp.size(), expected_size);
		for (size_t i = 0; i < sp.size(); ++i) ASSERT_EQUAL(size_t(sp[i]), i);
	};

	array.push_back(0);
	equals_index(array, 1);
	array.push_back({1});
	equals_index(array, 2);
	array.push_back(2);
	equals_index(array, 3);
	int i = 3;
	array.emplace_back(lak::move(i));
	equals_index(array, 4);
	array.push_back(4);
	equals_index(array, 5);
	i = 5;
	array.push_back(lak::move(i));
	equals_index(array, 6);
	array.push_back(6);
	equals_index(array, 7);
	array.push_back(7);
	equals_index(array, 8);
	array.push_back(8);

	return 0;
}
END_TEST()

BEGIN_TEST(array_insert)
{
	lak::array<uint8_t> arr = one_page_array<uint8_t>(10U);

	const size_t size = arr.size();

	ASSERT_GREATER_OR_EQUAL(size, 2U);

	const uint8_t *inserted = arr.insert(arr.begin() + (size / 2U), 20U);

	ASSERT_EQUAL(uintptr_t(inserted), uintptr_t(arr.begin() + (size / 2U)));

	ASSERT_EQUAL(inserted[0], 20U);
	ASSERT_EQUAL(inserted[-1], 10U);
	ASSERT_EQUAL(inserted[1], 10U);

	ASSERT_EQUAL(arr.size(), size + 1);

	ASSERT_GREATER(arr.capacity(), lak::page_size());

	inserted = arr.insert(arr.end(), 0U);
	ASSERT_EQUAL(uintptr_t(inserted), uintptr_t(arr.end() - 1U));
	ASSERT_EQUAL(uintptr_t(inserted), uintptr_t(arr.begin() + size + 1U));

	return 0;
}
END_TEST()
