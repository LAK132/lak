#include "lak/bit_count.hpp"

#include "lak/test.hpp"

BEGIN_TEST(bit_count)
{
	DEBUG_EXPR(lak::bit_count::from_bits(1U));
	DEBUG_EXPR(lak::bit_count::from_bits(7U));
	DEBUG_EXPR(lak::bit_count::from_bits(8U));
	DEBUG_EXPR(lak::bit_count::from_bits(10U));
	DEBUG_EXPR(lak::bit_count::from_bits(20U));
	ASSERT_EQUAL(lak::bit_count::from_bits(10U).to_bits(), 10U);
	ASSERT_EQUAL(lak::bit_count::from_bytes(10U).to_bits(), 10U * CHAR_BIT);
	ASSERT_GREATER(lak::bit_count::from_bits(2U), lak::bit_count::from_bits(1U));
	ASSERT_EQUAL(
	  (lak::bit_count::from_bits(10U) + lak::bit_count::from_bits(20U)),
	  lak::bit_count::from_bits(30U));
	ASSERT_EQUAL(
	  (lak::bit_count::from_bits(30U) - lak::bit_count::from_bits(10U)),
	  lak::bit_count::from_bits(20U));
	return EXIT_SUCCESS;
}
END_TEST()
