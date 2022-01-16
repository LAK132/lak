#include "lak/test.hpp"

#include "lak/array.hpp"
#include "lak/span.hpp"
#include "lak/span_manip.hpp"

BEGIN_TEST(rotate_left)
{
	lak::array<size_t, 13> arr{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}};

	size_t cumulative_offset = 0;

	auto rotate_test = [&](size_t offset)
	{
		cumulative_offset += offset;
		lak::rotate_left(lak::span<size_t>{arr}, offset);
		for (size_t i = 0; i < arr.size(); ++i)
		{
			ASSERT_EQUAL(arr[i], (i + cumulative_offset) % arr.size());
		}
	};

	[[maybe_unused]] auto print_arr = [&]()
	{
		lak::u8string str;
		for (const auto &v : arr) str += lak::streamify(v, ", ");
		DEBUG(str);
	};

	{
		SCOPED_CHECKPOINT("left 0");
		rotate_test(0);
	}

	{
		SCOPED_CHECKPOINT("left 1");
		rotate_test(1);
	}

	{
		SCOPED_CHECKPOINT("left 2");
		rotate_test(2);
	}

	{
		SCOPED_CHECKPOINT("left 5");
		rotate_test(5);
	}

	{
		SCOPED_CHECKPOINT("left 6");
		rotate_test(6);
	}

	{
		SCOPED_CHECKPOINT("left 0");
		rotate_test(0);
	}

	{
		SCOPED_CHECKPOINT("left 10");
		rotate_test(10);
	}

	return 0;
}
END_TEST()

BEGIN_TEST(rotate_right)
{
	lak::array<size_t, 13> arr{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}};

	size_t cumulative_offset = 0;

	auto rotate_test = [&](size_t offset)
	{
		cumulative_offset += offset;
		lak::rotate_right(lak::span<size_t>{arr}, offset);
		for (size_t i = 0; i < arr.size(); ++i)
		{
			ASSERT_EQUAL(arr[i],
			             (i + (arr.size() - (cumulative_offset % arr.size()))) %
			               arr.size());
		}
	};

	[[maybe_unused]] auto print_arr = [&]()
	{
		lak::u8string str;
		for (const auto &v : arr) str += lak::streamify(v, ", ");
		DEBUG(str);
	};

	{
		SCOPED_CHECKPOINT("right 0");
		rotate_test(0);
	}

	{
		SCOPED_CHECKPOINT("right 1");
		rotate_test(1);
	}

	{
		SCOPED_CHECKPOINT("right 2");
		rotate_test(2);
	}

	{
		SCOPED_CHECKPOINT("right 5");
		rotate_test(5);
	}

	{
		SCOPED_CHECKPOINT("right 6");
		rotate_test(6);
	}

	{
		SCOPED_CHECKPOINT("right 0");
		rotate_test(0);
	}

	{
		SCOPED_CHECKPOINT("right 10");
		rotate_test(10);
	}

	return 0;
}
END_TEST()
