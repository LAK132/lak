#include "lak/blob_search.hpp"

#include "lak/test.hpp"

BEGIN_TEST(blob_search)
{
	lak::blob_search<4U> accel;

	lak::array<byte_t> arr;
	arr.resize(8U);
	for (size_t i = 0U; i < arr.size(); ++i) arr[i] = byte_t(i);

	accel.init(arr);

	{
		lak::array<byte_t> find;
		find.resize(1U);
		find[0U] = byte_t(3U);
		auto res = accel.find(find);
		ASSERT(res.has_value());
		ASSERT_EQUAL(*res, 3U);
	}

	{
		lak::array<byte_t> find;
		find.resize(2U);
		find[0U] = byte_t(3U);
		find[1U] = byte_t(4U);
		auto res = accel.find(find);
		ASSERT(res.has_value());
		ASSERT_EQUAL(*res, 3U);
	}

	{
		lak::array<byte_t> find;
		find.resize(4U);
		find[0U] = byte_t(3U);
		find[1U] = byte_t(4U);
		find[2U] = byte_t(5U);
		find[3U] = byte_t(6U);
		auto res = accel.find(find);
		ASSERT(res.has_value());
		ASSERT_EQUAL(*res, 3U);
	}

	return 0;
}
END_TEST()
