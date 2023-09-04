#include "lak/priority_queue.hpp"

#include "lak/test.hpp"

static_assert(
  lak::concepts::contiguous_range_of<lak::priority_queue<int>, const int>);

BEGIN_TEST(priority_queue)
{
	auto tostr = []<typename T>(lak::span<T> sp)
	{
		lak::u8string result;
		for (const auto &t : sp) result += lak::streamify(t, " ");
		return result;
	};

	lak::priority_queue<int> queue;
	ASSERT(queue.empty());
	ASSERT_EQUAL(queue.size(), 0U);
	DEBUG(tostr(lak::span(queue)));

	queue.push(0x10);
	ASSERT(!queue.empty());
	ASSERT_EQUAL(queue.size(), 1U);
	DEBUG(tostr(lak::span(queue)));

	ASSERT_EQUAL(queue.top(), 0x10);

	queue.push({0x20, 0x4, 0x2, 0x54, 0x69, 0x420});
	ASSERT(!queue.empty());
	ASSERT_EQUAL(queue.size(), 7U);
	DEBUG(tostr(lak::span(queue)));

	ASSERT_EQUAL(queue.top(), 0x420);
	ASSERT_EQUAL(queue.top(), 0x420);

	ASSERT_EQUAL(queue.pop(), 0x420);
	DEBUG(tostr(lak::span(queue)));
	ASSERT_EQUAL(queue.pop(), 0x69);
	DEBUG(tostr(lak::span(queue)));
	ASSERT_EQUAL(queue.pop(), 0x54);
	DEBUG(tostr(lak::span(queue)));
	ASSERT_EQUAL(queue.pop(), 0x20);
	DEBUG(tostr(lak::span(queue)));
	ASSERT_EQUAL(queue.pop(), 0x10);
	DEBUG(tostr(lak::span(queue)));
	ASSERT_EQUAL(queue.pop(), 0x4);
	DEBUG(tostr(lak::span(queue)));
	ASSERT_EQUAL(queue.pop(), 0x2);
	DEBUG(tostr(lak::span(queue)));

	ASSERT(queue.empty());

	return 0;
}
END_TEST();
