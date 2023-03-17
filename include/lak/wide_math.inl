#include "lak/wide_math.hpp"

lak::uint128_t lak::add_u128(uint64_t A, uint64_t B)
{
	const uint64_t low = static_cast<uint64_t>(static_cast<uint32_t>(A)) +
	                     static_cast<uint64_t>(static_cast<uint32_t>(B));

	const uint64_t mid = static_cast<uint64_t>(A >> 32U) +
	                     static_cast<uint64_t>(B >> 32U) +
	                     static_cast<uint64_t>(low >> 32U);

	return {
	  .high = static_cast<uint64_t>(mid >> 32U),
	  .low  = static_cast<uint64_t>(static_cast<uint32_t>(low)) |
	         static_cast<uint64_t>(mid << 32U),
	};
}

lak::uint128_t lak::mul_u128(uint64_t A, uint64_t B)
{
	const uint64_t a_low  = static_cast<uint32_t>(A);
	const uint64_t a_high = static_cast<uint32_t>(A >> 32U);
	const uint64_t b_low  = static_cast<uint32_t>(B);
	const uint64_t b_high = static_cast<uint32_t>(B >> 32U);

	const uint64_t low       = a_low * b_low;
	const lak::uint128_t mid = lak::add_u128(a_low * b_high, a_high * b_low);
	const uint64_t high      = a_high * b_high;

	const lak::uint128_t low2 = lak::add_u128(low, mid.low << 32U);

	return {
	  .high = high + (mid.high << 32U) + (mid.low >> 32U) + low2.high,
	  .low  = low2.low,
	};
}

lak::uintmax2_t lak::add_uintmax2(uintmax_t A, uintmax_t B, uintmax_t C)
{
	constexpr uintmax_t half_shift = (CHAR_BIT * sizeof(uintmax_t)) / 2U;
	constexpr uintmax_t half_mask  = UINTMAX_MAX >> half_shift;

	const uintmax_t low = static_cast<uintmax_t>(A & half_mask) +
	                      static_cast<uintmax_t>(B & half_mask) +
	                      static_cast<uintmax_t>(C & half_mask);

	const uintmax_t mid = static_cast<uintmax_t>(A >> half_shift) +
	                      static_cast<uintmax_t>(B >> half_shift) +
	                      static_cast<uintmax_t>(C >> half_shift) +
	                      static_cast<uintmax_t>(low >> half_shift);

	return {
	  .high = static_cast<uintmax_t>(mid >> half_shift),
	  .low  = static_cast<uintmax_t>(low & half_mask) |
	         static_cast<uintmax_t>(mid << half_shift),
	};
}

lak::uintmax2_t lak::add_uintmax2(lak::uintmax2_t A, uintmax_t B, uintmax_t C)
{
	lak::uintmax2_t result = lak::add_uintmax2(A.low, B, C);
	return {
	  .high = result.high + A.high,
	  .low  = result.low,
	};
}

lak::uintmax2_t lak::sub_uintmax2(uintmax_t A, uintmax_t B)
{
	return lak::add_uintmax2(lak::add_uintmax2(
	                           lak::uintmax2_t{
	                             .high = UINTMAX_MAX,
	                             .low  = ~B,
	                           },
	                           1U),
	                         A);
}

lak::uintmax2_t lak::sub_uintmax2(uintmax_t A, uintmax_t B, uintmax_t C)
{
	lak::uintmax2_t BC = lak::add_uintmax2(B, (~C) + 1U);
	return lak::add_uintmax2(lak::add_uintmax2(
	                           lak::uintmax2_t{
	                             .high = ~BC.high,
	                             .low  = ~BC.low,
	                           },
	                           1U),
	                         A);
}

lak::uintmax2_t lak::mul_uintmax2(uintmax_t A, uintmax_t B)
{
	constexpr uintmax_t half_shift = (CHAR_BIT * sizeof(uintmax_t)) / 2U;
	constexpr uintmax_t half_mask  = UINTMAX_MAX >> half_shift;

	const uintmax_t a_low  = static_cast<uintmax_t>(A & half_mask);
	const uintmax_t a_high = static_cast<uintmax_t>(A >> half_shift);
	const uintmax_t b_low  = static_cast<uintmax_t>(B & half_mask);
	const uintmax_t b_high = static_cast<uintmax_t>(B >> half_shift);

	const uintmax_t low = a_low * b_low;
	const lak::uintmax2_t mid =
	  lak::add_uintmax2(a_low * b_high, a_high * b_low, low >> half_shift);
	const uintmax_t high = a_high * b_high;

	return {
	  .high = high + (mid.high << half_shift) + (mid.low >> half_shift),
	  .low  = static_cast<uintmax_t>(low & half_mask) |
	         static_cast<uintmax_t>(mid.low << half_shift),
	};
}
