#ifndef LAK_WIDE_MATH_HPP
#define LAK_WIDE_MATH_HPP

#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

namespace lak
{
	struct uint128_t
	{
		uint64_t high;
		uint64_t low;

		force_inline uint128_t operator~() const
		{
			return {.high = ~high, .low = ~low};
		}
	};

	/* --- add u32 --- */

	force_inline unsigned char add_carry_u32(uint32_t A,
	                                         uint32_t B,
	                                         uint32_t *O);

	force_inline unsigned char add_carry_u32(uint32_t A,
	                                         uint32_t B,
	                                         uint32_t *O,
	                                         unsigned char C);

	force_inline uint32_t add_carry_u32(lak::span<uint32_t> A, uint32_t B);

	// precondition: !A.empty()
	force_inline uint32_t add_carry_u32(lak::span<uint32_t> A,
	                                    uint32_t B,
	                                    unsigned char C);

	// precondition: A.size() >= B.size()
	force_inline uint32_t add_carry_u32(lak::span<uint32_t> A,
	                                    lak::span<const uint32_t> B);

	/* --- add u64 --- */

	force_inline unsigned char add_carry_u64(uint64_t A,
	                                         uint64_t B,
	                                         uint64_t *O);

	force_inline unsigned char add_carry_u64(uint64_t A,
	                                         uint64_t B,
	                                         uint64_t *O,
	                                         unsigned char C);

	force_inline uint64_t add_carry_u64(lak::span<uint64_t> A, uint64_t B);

	// precondition: !A.empty()
	force_inline uint64_t add_carry_u64(lak::span<uint64_t> A,
	                                    uint64_t B,
	                                    unsigned char C);

	// precondition: A.size() >= B.size()
	force_inline uint64_t add_carry_u64(lak::span<uint64_t> A,
	                                    lak::span<const uint64_t> B);

	/* --- add u128 --- */

	force_inline unsigned char add_carry_u128(lak::uint128_t A,
	                                          lak::uint128_t B,
	                                          lak::uint128_t *O,
	                                          unsigned char C = 0U);

	force_inline lak::uint128_t add_u128(uint64_t A,
	                                     uint64_t B,
	                                     unsigned char C = 0U);

	force_inline lak::uint128_t add_u128(lak::uint128_t A,
	                                     uint64_t B,
	                                     unsigned char C = 0U);

	/* --- sub u32 --- */

	// A - (B + C)
	force_inline unsigned char sub_carry_u32(uint32_t A,
	                                         uint32_t B,
	                                         uint32_t *O,
	                                         unsigned char C = 0U);

	force_inline uint32_t sub_carry_u32(lak::span<uint32_t> A, uint32_t B);

	/* --- sub u64 --- */

	// A - (B + C)
	force_inline unsigned char sub_carry_u64(uint64_t A,
	                                         uint64_t B,
	                                         uint64_t *O,
	                                         unsigned char C = 0U);

	force_inline uint64_t sub_carry_u64(lak::span<uint64_t> A, uint64_t B);

	/* --- sub u128 --- */

	// A - (B + C)
	force_inline unsigned char sub_carry_u128(lak::uint128_t A,
	                                          lak::uint128_t B,
	                                          lak::uint128_t *O,
	                                          unsigned char C = 0U);

	force_inline lak::uint128_t sub_carry_u128(lak::span<lak::uint128_t> A,
	                                           lak::uint128_t B);

	/* --- mul u32 --- */

	force_inline uint32_t mul_carry_u32(uint32_t A, uint32_t B, uint32_t *O);

	force_inline uint32_t mul_carry_u32(uint32_t A,
	                                    uint32_t B,
	                                    uint32_t *O,
	                                    uint32_t C);

	/* --- mul u64 --- */

	force_inline uint64_t mul_u64(uint32_t A, uint32_t B);

	/* --- mul u128 --- */

	force_inline lak::uint128_t mul_u128(uint64_t A, uint64_t B);

	/* --- mul add u64 --- */

	// (A * B) + C
	force_inline uint64_t mul_add_u64(uint32_t A, uint32_t B, uint32_t C);

	/* --- mul add u128 --- */

	// (A * B) + C
	force_inline lak::uint128_t mul_add_u128(uint64_t A, uint64_t B, uint64_t C);
}

#include "lak/wide_math.inl"

#endif
