#include "lak/test.hpp"

#include "lak/array.hpp"
#include "lak/span.hpp"
#include "lak/span_manip.hpp"

template<typename T, size_t S>
[[maybe_unused]] void print_arr(lak::span<T, S> arr)
{
	lak::u8string str;
	for (const auto &v : arr) str += lak::streamify(v, ", ");
	DEBUG(str);
};

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

BEGIN_TEST(shift_left)
{
	lak::array<size_t, 13> src{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}};
	lak::array<size_t, 13> arr{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}};

	size_t cumulative_offset = 0;

	auto shift_test = [&](size_t offset)
	{
		cumulative_offset += offset;
		lak::shift_left(lak::span<size_t>{arr}, offset);
		for (size_t i = cumulative_offset; i < arr.size(); ++i)
			ASSERT_EQUAL(src[i], arr[i - cumulative_offset]);
	};

	{
		SCOPED_CHECKPOINT("left 0");
		shift_test(0);
	}

	{
		SCOPED_CHECKPOINT("left 1");
		shift_test(1);
	}

	{
		SCOPED_CHECKPOINT("left 2");
		shift_test(2);
	}

	{
		SCOPED_CHECKPOINT("left 5");
		shift_test(5);
	}

	return 0;
}
END_TEST()

BEGIN_TEST(shift_right)
{
	{
		lak::array<uint32_t, 4> src{
		  {0x00000000, 0xffffffff, 0xffffffff, 0xcdcdcdcd}};
		lak::array<uint32_t, 4> arr{
		  {0x00000000, 0xffffffff, 0xffffffff, 0xcdcdcdcd}};

		size_t cumulative_offset = 0;

		auto shift_test = [&](size_t offset)
		{
			cumulative_offset += offset;
			lak::shift_right(lak::span<uint32_t>{arr}, offset);
			for (size_t i = cumulative_offset; i < arr.size(); ++i)
				ASSERT_EQUAL(src[i - cumulative_offset], arr[i]);
		};

		{
			SCOPED_CHECKPOINT("right 0");
			shift_test(0);
		}

		{
			SCOPED_CHECKPOINT("right 1");
			shift_test(1);
		}
	}

	{
		lak::array<size_t, 13> src{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}};
		lak::array<size_t, 13> arr{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}};

		size_t cumulative_offset = 0;

		auto shift_test = [&](size_t offset)
		{
			cumulative_offset += offset;
			lak::shift_right(lak::span<size_t>{arr}, offset);
			for (size_t i = cumulative_offset; i < arr.size(); ++i)
				ASSERT_EQUAL(src[i - cumulative_offset], arr[i]);
		};

		{
			SCOPED_CHECKPOINT("right 0");
			shift_test(0);
		}

		{
			SCOPED_CHECKPOINT("right 1");
			shift_test(1);
		}

		{
			SCOPED_CHECKPOINT("right 2");
			shift_test(2);
		}

		{
			SCOPED_CHECKPOINT("right 5");
			shift_test(5);
		}
	}

	return 0;
}
END_TEST()
