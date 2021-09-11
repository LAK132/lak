#include "lak/test.hpp"

#include "lak/compare.hpp"

BEGIN_TEST(is_eq)
{
	ASSERT(!lak::is_eq(lak::partial_ordering::less));
	ASSERT(!lak::is_eq(lak::partial_ordering::greater));
	ASSERT(lak::is_eq(lak::partial_ordering::equivalent));
	ASSERT(!lak::is_eq(lak::partial_ordering::unordered));

	ASSERT(!lak::is_eq(lak::weak_ordering::less));
	ASSERT(!lak::is_eq(lak::weak_ordering::greater));
	ASSERT(lak::is_eq(lak::weak_ordering::equivalent));

	ASSERT(!lak::is_eq(lak::strong_ordering::less));
	ASSERT(lak::is_eq(lak::strong_ordering::equal));
	ASSERT(!lak::is_eq(lak::strong_ordering::greater));
	ASSERT(lak::is_eq(lak::strong_ordering::equivalent));

	ASSERT(!lak::is_eq(lak::partial_ordering::less <=> 0));
	ASSERT(!lak::is_eq(lak::partial_ordering::greater <=> 0));
	ASSERT(lak::is_eq(lak::partial_ordering::equivalent <=> 0));
	ASSERT(!lak::is_eq(lak::partial_ordering::unordered <=> 0));

	ASSERT(!lak::is_eq(lak::weak_ordering::less <=> 0));
	ASSERT(!lak::is_eq(lak::weak_ordering::greater <=> 0));
	ASSERT(lak::is_eq(lak::weak_ordering::equivalent <=> 0));

	ASSERT(!lak::is_eq(lak::strong_ordering::less <=> 0));
	ASSERT(lak::is_eq(lak::strong_ordering::equal <=> 0));
	ASSERT(!lak::is_eq(lak::strong_ordering::greater <=> 0));
	ASSERT(lak::is_eq(lak::strong_ordering::equivalent <=> 0));

	return 0;
}
END_TEST()

BEGIN_TEST(is_neq)
{
	ASSERT(lak::is_neq(lak::partial_ordering::less));
	ASSERT(lak::is_neq(lak::partial_ordering::greater));
	ASSERT(!lak::is_neq(lak::partial_ordering::equivalent));
	ASSERT(lak::is_neq(lak::partial_ordering::unordered));

	ASSERT(lak::is_neq(lak::weak_ordering::less));
	ASSERT(lak::is_neq(lak::weak_ordering::greater));
	ASSERT(!lak::is_neq(lak::weak_ordering::equivalent));

	ASSERT(lak::is_neq(lak::strong_ordering::less));
	ASSERT(!lak::is_neq(lak::strong_ordering::equal));
	ASSERT(lak::is_neq(lak::strong_ordering::greater));
	ASSERT(!lak::is_neq(lak::strong_ordering::equivalent));

	ASSERT(lak::is_neq(lak::partial_ordering::less <=> 0));
	ASSERT(lak::is_neq(lak::partial_ordering::greater <=> 0));
	ASSERT(!lak::is_neq(lak::partial_ordering::equivalent <=> 0));
	ASSERT(lak::is_neq(lak::partial_ordering::unordered <=> 0));

	ASSERT(lak::is_neq(lak::weak_ordering::less <=> 0));
	ASSERT(lak::is_neq(lak::weak_ordering::greater <=> 0));
	ASSERT(!lak::is_neq(lak::weak_ordering::equivalent <=> 0));

	ASSERT(lak::is_neq(lak::strong_ordering::less <=> 0));
	ASSERT(!lak::is_neq(lak::strong_ordering::equal <=> 0));
	ASSERT(lak::is_neq(lak::strong_ordering::greater <=> 0));
	ASSERT(!lak::is_neq(lak::strong_ordering::equivalent <=> 0));

	return 0;
}
END_TEST()

BEGIN_TEST(is_lt)
{
	ASSERT(lak::is_lt(lak::partial_ordering::less));
	ASSERT(!lak::is_lt(lak::partial_ordering::greater));
	ASSERT(!lak::is_lt(lak::partial_ordering::equivalent));
	ASSERT(!lak::is_lt(lak::partial_ordering::unordered));

	ASSERT(lak::is_lt(lak::weak_ordering::less));
	ASSERT(!lak::is_lt(lak::weak_ordering::greater));
	ASSERT(!lak::is_lt(lak::weak_ordering::equivalent));

	ASSERT(lak::is_lt(lak::strong_ordering::less));
	ASSERT(!lak::is_lt(lak::strong_ordering::equal));
	ASSERT(!lak::is_lt(lak::strong_ordering::greater));
	ASSERT(!lak::is_lt(lak::strong_ordering::equivalent));

	ASSERT(lak::is_lt(lak::partial_ordering::less <=> 0));
	ASSERT(!lak::is_lt(lak::partial_ordering::greater <=> 0));
	ASSERT(!lak::is_lt(lak::partial_ordering::equivalent <=> 0));
	ASSERT(!lak::is_lt(lak::partial_ordering::unordered <=> 0));

	ASSERT(lak::is_lt(lak::weak_ordering::less <=> 0));
	ASSERT(!lak::is_lt(lak::weak_ordering::greater <=> 0));
	ASSERT(!lak::is_lt(lak::weak_ordering::equivalent <=> 0));

	ASSERT(lak::is_lt(lak::strong_ordering::less <=> 0));
	ASSERT(!lak::is_lt(lak::strong_ordering::equal <=> 0));
	ASSERT(!lak::is_lt(lak::strong_ordering::greater <=> 0));
	ASSERT(!lak::is_lt(lak::strong_ordering::equivalent <=> 0));

	return 0;
}
END_TEST()

BEGIN_TEST(is_lteq)
{
	ASSERT(lak::is_lteq(lak::partial_ordering::less));
	ASSERT(!lak::is_lteq(lak::partial_ordering::greater));
	ASSERT(lak::is_lteq(lak::partial_ordering::equivalent));
	ASSERT(!lak::is_lteq(lak::partial_ordering::unordered));

	ASSERT(lak::is_lteq(lak::weak_ordering::less));
	ASSERT(!lak::is_lteq(lak::weak_ordering::greater));
	ASSERT(lak::is_lteq(lak::weak_ordering::equivalent));

	ASSERT(lak::is_lteq(lak::strong_ordering::less));
	ASSERT(lak::is_lteq(lak::strong_ordering::equal));
	ASSERT(!lak::is_lteq(lak::strong_ordering::greater));
	ASSERT(lak::is_lteq(lak::strong_ordering::equivalent));

	ASSERT(lak::is_lteq(lak::partial_ordering::less <=> 0));
	ASSERT(!lak::is_lteq(lak::partial_ordering::greater <=> 0));
	ASSERT(lak::is_lteq(lak::partial_ordering::equivalent <=> 0));
	ASSERT(!lak::is_lteq(lak::partial_ordering::unordered <=> 0));

	ASSERT(lak::is_lteq(lak::weak_ordering::less <=> 0));
	ASSERT(!lak::is_lteq(lak::weak_ordering::greater <=> 0));
	ASSERT(lak::is_lteq(lak::weak_ordering::equivalent <=> 0));

	ASSERT(lak::is_lteq(lak::strong_ordering::less <=> 0));
	ASSERT(lak::is_lteq(lak::strong_ordering::equal <=> 0));
	ASSERT(!lak::is_lteq(lak::strong_ordering::greater <=> 0));
	ASSERT(lak::is_lteq(lak::strong_ordering::equivalent <=> 0));

	return 0;
}
END_TEST()

BEGIN_TEST(is_gt)
{
	ASSERT(!lak::is_gt(lak::partial_ordering::less));
	ASSERT(lak::is_gt(lak::partial_ordering::greater));
	ASSERT(!lak::is_gt(lak::partial_ordering::equivalent));
	ASSERT(!lak::is_gt(lak::partial_ordering::unordered));

	ASSERT(!lak::is_gt(lak::weak_ordering::less));
	ASSERT(lak::is_gt(lak::weak_ordering::greater));
	ASSERT(!lak::is_gt(lak::weak_ordering::equivalent));

	ASSERT(!lak::is_gt(lak::strong_ordering::less));
	ASSERT(!lak::is_gt(lak::strong_ordering::equal));
	ASSERT(lak::is_gt(lak::strong_ordering::greater));
	ASSERT(!lak::is_gt(lak::strong_ordering::equivalent));

	ASSERT(!lak::is_gt(lak::partial_ordering::less <=> 0));
	ASSERT(lak::is_gt(lak::partial_ordering::greater <=> 0));
	ASSERT(!lak::is_gt(lak::partial_ordering::equivalent <=> 0));
	ASSERT(!lak::is_gt(lak::partial_ordering::unordered <=> 0));

	ASSERT(!lak::is_gt(lak::weak_ordering::less <=> 0));
	ASSERT(lak::is_gt(lak::weak_ordering::greater <=> 0));
	ASSERT(!lak::is_gt(lak::weak_ordering::equivalent <=> 0));

	ASSERT(!lak::is_gt(lak::strong_ordering::less <=> 0));
	ASSERT(!lak::is_gt(lak::strong_ordering::equal <=> 0));
	ASSERT(lak::is_gt(lak::strong_ordering::greater <=> 0));
	ASSERT(!lak::is_gt(lak::strong_ordering::equivalent <=> 0));

	return 0;
}
END_TEST()

BEGIN_TEST(is_gteq)
{
	ASSERT(!lak::is_gteq(lak::partial_ordering::less));
	ASSERT(lak::is_gteq(lak::partial_ordering::greater));
	ASSERT(lak::is_gteq(lak::partial_ordering::equivalent));
	ASSERT(!lak::is_gteq(lak::partial_ordering::unordered));

	ASSERT(!lak::is_gteq(lak::weak_ordering::less));
	ASSERT(lak::is_gteq(lak::weak_ordering::greater));
	ASSERT(lak::is_gteq(lak::weak_ordering::equivalent));

	ASSERT(!lak::is_gteq(lak::strong_ordering::less));
	ASSERT(lak::is_gteq(lak::strong_ordering::equal));
	ASSERT(lak::is_gteq(lak::strong_ordering::greater));
	ASSERT(lak::is_gteq(lak::strong_ordering::equivalent));

	ASSERT(!lak::is_gteq(lak::partial_ordering::less <=> 0));
	ASSERT(lak::is_gteq(lak::partial_ordering::greater <=> 0));
	ASSERT(lak::is_gteq(lak::partial_ordering::equivalent <=> 0));
	ASSERT(!lak::is_gteq(lak::partial_ordering::unordered <=> 0));

	ASSERT(!lak::is_gteq(lak::weak_ordering::less <=> 0));
	ASSERT(lak::is_gteq(lak::weak_ordering::greater <=> 0));
	ASSERT(lak::is_gteq(lak::weak_ordering::equivalent <=> 0));

	ASSERT(!lak::is_gteq(lak::strong_ordering::less <=> 0));
	ASSERT(lak::is_gteq(lak::strong_ordering::equal <=> 0));
	ASSERT(lak::is_gteq(lak::strong_ordering::greater <=> 0));
	ASSERT(lak::is_gteq(lak::strong_ordering::equivalent <=> 0));

	return 0;
}
END_TEST()
