#include "lak/bigint.hpp"

#include "lak/test.hpp"

BEGIN_TEST(bigint)
{
	lak::bigint value;

	value = 0;
	ASSERT_EQUAL(value.to_uintmax().UNWRAP(), 0U);

	value += UINTMAX_MAX;
	ASSERT_EQUAL(value.to_uintmax().UNWRAP(), UINTMAX_MAX);

	value += UINTMAX_MAX;
	value.to_uintmax().UNWRAP_ERR();

	value -= UINTMAX_MAX;
	ASSERT_EQUAL(value.to_uintmax().UNWRAP(), UINTMAX_MAX);

	value -= UINTMAX_MAX;
	ASSERT_EQUAL(value.to_uintmax().UNWRAP(), 0U);

	value += UINTMAX_MAX;
	ASSERT_EQUAL(value.to_uintmax().UNWRAP(), UINTMAX_MAX);

	value *= UINTMAX_MAX;
	value *= UINTMAX_MAX;
	value *= UINTMAX_MAX;
	value *= value;
	ASSERT_EQUAL(value, value);

	ASSERT_GREATER(value, 0);

	ASSERT_EQUAL(lak::bigint(10) * lak::bigint(20), lak::bigint(10 * 20));

	ASSERT_EQUAL((lak::bigint(1U) << 10U), (uintmax_t(1U) << 10U));

	ASSERT_EQUAL((lak::bigint(2U) << 10U), (uintmax_t(2U) << 10U));

	ASSERT_EQUAL((lak::bigint(24U) >> 3U), (uintmax_t(24U) >> 3U));

	ASSERT_EQUAL(lak::bigint(UINTMAX_MAX), UINTMAX_MAX);

	ASSERT_GREATER_OR_EQUAL(lak::bigint(UINTMAX_MAX), UINTMAX_MAX);

	ASSERT_GREATER(lak::bigint(UINTMAX_MAX) * 2U, UINTMAX_MAX);

	ASSERT_EQUAL(lak::bigint(10U) / 2U, 5U);

	ASSERT_EQUAL(((lak::bigint(UINTMAX_MAX) * 2U) / 2U), UINTMAX_MAX);

	ASSERT_EQUAL(((lak::bigint(UINTMAX_MAX) * UINTMAX_MAX) / UINTMAX_MAX),
	             UINTMAX_MAX);

	ASSERT_EQUAL(((lak::bigint(UINTMAX_MAX) * 10U) / UINTMAX_MAX), 10U);

	ASSERT_EQUAL(((lak::bigint(UINTMAX_MAX) * 2U) % UINTMAX_MAX), 0U);

	ASSERT_EQUAL(lak::bigint(UINTMAX_MAX) / -1, -lak::bigint(UINTMAX_MAX));

	ASSERT_EQUAL(lak::bigint(UINTMAX_MAX) % -1, 0);

	ASSERT_EQUAL(lak::bigint(5).to_double(), 5.0);

	ASSERT_EQUAL(lak::bigint(-5).to_double(), -5.0);

	ASSERT_EQUAL((lak::bigint(2) * UINTMAX_MAX).to_double(), 2.0 * UINTMAX_MAX);

	ASSERT_EQUAL(-(lak::bigint(-2) * 2), lak::bigint(2) * 2);

	ASSERT_EQUAL(-(lak::bigint(-12) * UINTMAX_MAX),
	             lak::bigint(12) * UINTMAX_MAX);

	ASSERT_EQUAL((lak::bigint(-12) * UINTMAX_MAX).to_double(),
	             -12.0 * UINTMAX_MAX);

	ASSERT_EQUAL(lak::bigint(132).to_double(), 132.0);

	return 0;
}
END_TEST()
