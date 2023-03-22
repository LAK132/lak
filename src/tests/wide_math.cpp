#include "lak/wide_math.hpp"

#include "lak/test.hpp"

BEGIN_TEST(wide_math)
{
	{
		uint32_t result;
		ASSERT_EQUAL(lak::add_carry_u32(UINT32_MAX, UINT32_MAX, &result, 0), 1U);
		ASSERT_EQUAL(result, (UINT32_MAX - 1U));
	}

	{
		uint32_t result;
		ASSERT_EQUAL(lak::add_carry_u32(UINT32_MAX, UINT32_MAX, &result, 3), 2U);
		ASSERT_EQUAL(result, 1U);
	}
	return 0;
}
END_TEST()
