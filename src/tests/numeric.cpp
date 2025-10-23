#include "lak/test.hpp"

#include "lak/numeric.hpp"

#include "lak/string_literals.hpp"

BEGIN_TEST(string_to_number)
{
	{
		auto dub = lak::dec_string_to_double(u8"-1"_view, u8"0"_view, {}).UNWRAP();
		ASSERT_EQUAL(dub, -1.0);
	}

	{
		auto dub = lak::dec_string_to_double(u8"0"_view, u8"5"_view, {}).UNWRAP();
		ASSERT_EQUAL(dub, 0.5);
	}

	{
		auto dub = lak::dec_string_to_double(u8"-0"_view, u8"5"_view, {}).UNWRAP();
		ASSERT_EQUAL(dub, -0.5);
	}

	{
		auto num =
		  lak::string_to_uintmax(u8"10101"_view, lak::numeric_base::bin).UNWRAP();
		ASSERT_EQUAL(num, 0b10101U);
	}

	{
		auto num =
		  lak::string_to_uintmax(u8"10101"_view, lak::numeric_base::oct).UNWRAP();
		ASSERT_EQUAL(num, 010101U);
	}

	{
		auto num =
		  lak::string_to_uintmax(u8"10101"_view, lak::numeric_base::dec).UNWRAP();
		ASSERT_EQUAL(num, 10101U);
	}

	{
		auto num =
		  lak::string_to_uintmax(u8"10101"_view, lak::numeric_base::hex).UNWRAP();
		ASSERT_EQUAL(num, 0x10101U);
	}

	{
		auto num =
		  lak::string_to_uintmax(u8"255"_view, lak::numeric_base::dec).UNWRAP();
		ASSERT_EQUAL(num, 255U);
	}

	return 0;
}
END_TEST()

BEGIN_TEST(mul_uintmax2)
{
	DEBUG_EXPR(lak::mul_uintmax2(uintmax_t(2), UINTMAX_MAX).low);
	DEBUG_EXPR(lak::mul_uintmax2(uintmax_t(2), UINTMAX_MAX).high);
	DEBUG_EXPR(lak::mul_uintmax2(uintmax_t(-2), UINTMAX_MAX).low);
	DEBUG_EXPR(lak::mul_uintmax2(uintmax_t(-2), UINTMAX_MAX).high);

	auto v = lak::mul_uintmax2(uintmax_t(-2), UINTMAX_MAX);
	v.low  = ~v.low;
	v.high = ~v.high;
	v      = lak::add_uintmax2(v, 1U);
	DEBUG_EXPR(v.low);
	DEBUG_EXPR(v.high);

	return 0;
}
END_TEST()
