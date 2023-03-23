#include "lak/test.hpp"

#include "lak/ptr_intrin.hpp"

BEGIN_TEST(ptr_intrin)
{
	static constinit const char str[14]{"hello, world!"};

	static_assert(lak::ptr_compare(str, str + 2) == lak::strong_ordering::less);
	static_assert(lak::ptr_compare(str + 2, str) ==
	              lak::strong_ordering::greater);
	static_assert(lak::ptr_compare(str, str) == lak::strong_ordering::equal);

	ASSERT_EQUAL(__lakc_ptr_diff(str, str).diff, 0U);
	ASSERT_EQUAL(__lakc_ptr_diff(str, str).overflow, 0U);

	ASSERT_EQUAL(__lakc_ptr_diff(str + 2, str).diff, 2U);
	ASSERT_EQUAL(__lakc_ptr_diff(str + 2, str).overflow, 0U);

	ASSERT_EQUAL(__lakc_ptr_diff(str, str + 2).diff, LAKC_UPTRDIFF_MAX - 1U);
	ASSERT_EQUAL(__lakc_ptr_diff(str, str + 2).overflow, 1U);

	ASSERT(__lakc_ptr_lt(str, str + 2));
	ASSERT(!__lakc_ptr_lt(str + 2, str));
	ASSERT(!__lakc_ptr_lt(str, str));
	ASSERT_LESS(str, str + 2);

	ASSERT(__lakc_ptr_le(str, str + 2));
	ASSERT(!__lakc_ptr_le(str + 2, str));
	ASSERT(__lakc_ptr_le(str, str));
	ASSERT_LESS_OR_EQUAL(str, str + 2);
	ASSERT_LESS_OR_EQUAL(str, str);

	ASSERT(!__lakc_ptr_gt(str, str + 2));
	ASSERT(__lakc_ptr_gt(str + 2, str));
	ASSERT(!__lakc_ptr_gt(str, str));
	ASSERT_GREATER(str + 2, str);

	ASSERT(!__lakc_ptr_ge(str, str + 2));
	ASSERT(__lakc_ptr_ge(str + 2, str));
	ASSERT(__lakc_ptr_ge(str, str));
	ASSERT_GREATER_OR_EQUAL(str + 2, str);
	ASSERT_GREATER_OR_EQUAL(str, str);

	ASSERT(!__lakc_ptr_eq(str, str + 2));
	ASSERT(!__lakc_ptr_eq(str + 2, str));
	ASSERT(__lakc_ptr_eq(str, str));
	ASSERT_EQUAL(str, str);

	ASSERT(__lakc_ptr_neq(str, str + 2));
	ASSERT(__lakc_ptr_neq(str + 2, str));
	ASSERT(!__lakc_ptr_neq(str, str));
	ASSERT_NOT_EQUAL(str, str + 2);
	ASSERT_NOT_EQUAL(str + 2, str);

	ASSERT(!__lakc_ptr_in_range(str, str + 1, 4));
	ASSERT(!__lakc_ptr_in_range(str + 5, str, 4));
	ASSERT(__lakc_ptr_in_range(str + 2, str, 4));

	return 0;
}
END_TEST()
