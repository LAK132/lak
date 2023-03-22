#include "lak/wide_math.hpp"

#include "lak/compiler.hpp"

#if defined(LAK_COMPILER_MSVC) && defined(LAK_ARCH_X86_COMPAT)
#	include <intrin.h>
#endif

/* --- add u32 --- */

unsigned char lak::add_carry_u32(uint32_t A, uint32_t B, uint32_t *O)
{
#if defined(LAK_COMPILER_MSVC)
#	if defined(LAK_ARCH_X86_COMPAT)
	return _addcarry_u32(0, A, B, O);
#	else
#		error Arch not supported
#	endif
#elif defined(LAK_COMPILER_CLANG) || defined(LAK_COMPILER_GNUC)
	return __builtin_add_overflow(A, B, O) ? 1U : 0U;
#else
#	error Compiler and/or arch not supported
#endif
}

unsigned char lak::add_carry_u32(uint32_t A,
                                 uint32_t B,
                                 uint32_t *O,
                                 unsigned char C)
{
	uint32_t temp;
	const unsigned char carry = lak::add_carry_u32(A, B, &temp);
	return carry + lak::add_carry_u32(temp, C, O);
}

uint32_t lak::add_carry_u32(lak::span<uint32_t> A, uint32_t B)
{
	for (uint32_t &a : A)
	{
		if (B == 0) break;
		B = lak::add_carry_u32(a, B, &a);
	}
	return B;
}

uint32_t lak::add_carry_u32(lak::span<uint32_t> A, uint32_t B, unsigned char C)
{
	return lak::add_carry_u32(A.subspan(1U),
	                          lak::add_carry_u32(A[0], B, &A[0], C));
}

uint32_t lak::add_carry_u32(lak::span<uint32_t> A, lak::span<const uint32_t> B)
{
	unsigned char carry = 0U;
	size_t i            = 0U;
	for (; i < A.size() && i < B.size(); ++i)
		carry = lak::add_carry_u32(A[i], B[i], &A[i], carry);
	return lak::add_carry_u32(A.subspan(i), carry);
}

/* --- add u64 --- */

unsigned char lak::add_carry_u64(uint64_t A, uint64_t B, uint64_t *O)
{
#if defined(LAK_COMPILER_MSVC)
#	if defined(LAK_ARCH_X86_64)
	return _addcarry_u64(0, A, B, O);
#	elif defined(LAK_ARCH_X86_COMPAT)
	uint32_t result_low, result_high;

	unsigned char carry = lak::add_carry_u32(
	  static_cast<uint32_t>(A >> 32U),
	  static_cast<uint32_t>(B >> 32U),
	  &result_high,
	  lak::add_carry_u32(
	    static_cast<uint32_t>(A), static_cast<uint32_t>(B), &result_low, C));

	*O = (static_cast<uint64_t>(result_high) << 32U) |
	     static_cast<uint64_t>(result_low);

	return carry;
#	else
#		error Arch not supported
#	endif
#elif defined(LAK_COMPILER_CLANG) || defined(LAK_COMPILER_GNUC)
	return __builtin_add_overflow(A, B, O) ? 1U : 0U;
#else
#	error Compiler and/or arch not supported
#endif
}

unsigned char lak::add_carry_u64(uint64_t A,
                                 uint64_t B,
                                 uint64_t *O,
                                 unsigned char C)
{
	uint64_t temp;
	const unsigned char carry = lak::add_carry_u64(A, B, &temp);
	return carry + lak::add_carry_u64(temp, C, O);
}

uint64_t lak::add_carry_u64(lak::span<uint64_t> A, uint64_t B)
{
	for (uint64_t &a : A)
	{
		if (B == 0) break;
		B = lak::add_carry_u64(a, B, &a);
	}
	return B;
}

uint64_t lak::add_carry_u64(lak::span<uint64_t> A, uint64_t B, unsigned char C)
{
	return lak::add_carry_u64(A.subspan(1U),
	                          lak::add_carry_u64(A[0], B, &A[0], C));
}

uint64_t lak::add_carry_u64(lak::span<uint64_t> A, lak::span<const uint64_t> B)
{
	unsigned char carry = 0U;
	size_t i            = 0U;
	for (; i < A.size() && i < B.size(); ++i)
		carry = lak::add_carry_u64(A[i], B[i], &A[i], carry);
	return lak::add_carry_u64(A.subspan(i), carry);
}

/* --- add u128 --- */

unsigned char lak::add_carry_u128(lak::uint128_t A,
                                  lak::uint128_t B,
                                  lak::uint128_t *O,
                                  unsigned char C)
{
	return lak::add_carry_u64(
	  A.high, B.high, &O->high, lak::add_carry_u64(A.low, B.low, &O->low, C));
}

lak::uint128_t lak::add_u128(uint64_t A, uint64_t B, unsigned char C)
{
	lak::uint128_t result;
	result.high = lak::add_carry_u64(A, B, &result.low, C);
	return result;
}

lak::uint128_t lak::add_u128(lak::uint128_t A, uint64_t B, unsigned char C)
{
	lak::uint128_t result = lak::add_u128(A.low, B, C);
	result.high += A.high;
	return result;
}

/* --- sub u32 --- */

unsigned char lak::sub_carry_u32(uint32_t A,
                                 uint32_t B,
                                 uint32_t *O,
                                 unsigned char C)
{
	const unsigned char borrow = lak::add_carry_u32(B, C, &B);
	return borrow + (lak::add_carry_u32(A, ~B, O, 1U) ? 0U : 1U);
}

uint32_t lak::sub_carry_u32(lak::span<uint32_t> A, uint32_t B)
{
	for (uint32_t &a : A)
	{
		if (B == 0) break;
		B = lak::sub_carry_u32(a, B, &a);
	}
	return B;
}

/* --- sub u64 --- */

unsigned char lak::sub_carry_u64(uint64_t A,
                                 uint64_t B,
                                 uint64_t *O,
                                 unsigned char C)
{
	const unsigned char borrow = lak::add_carry_u64(B, C, &B);
	return borrow + (lak::add_carry_u64(A, ~B, O, 1U) ? 0U : 1U);
}

uint64_t lak::sub_carry_u64(lak::span<uint64_t> A, uint64_t B)
{
	for (uint64_t &a : A)
	{
		if (B == 0) break;
		B = lak::sub_carry_u64(a, B, &a);
	}
	return B;
}

/* --- sub u128 --- */

unsigned char lak::sub_carry_u128(lak::uint128_t A,
                                  lak::uint128_t B,
                                  lak::uint128_t *O,
                                  unsigned char C)
{
	const unsigned char borrow =
	  lak::add_carry_u128(B, lak::uint128_t{.high = 0U, .low = C}, &B);
	return borrow + (lak::add_carry_u128(A, ~B, O, 1U) ? 0U : 1U);
}

lak::uint128_t lak::sub_carry_u128(lak::span<lak::uint128_t> A,
                                   lak::uint128_t B)
{
	for (lak::uint128_t &a : A)
	{
		if ((B.high == 0) & (B.low == 0)) break;
		B = lak::uint128_t{.high = 0U, .low = lak::sub_carry_u128(a, B, &a)};
	}
	return B;
}

/* --- mul u32 --- */

uint32_t lak::mul_carry_u32(uint32_t A, uint32_t B, uint32_t *O)
{
	const uint64_t v = lak::mul_u64(A, B);
	*O               = static_cast<uint32_t>(v);
	return v >> 32U;
}

uint32_t lak::mul_carry_u32(uint32_t A, uint32_t B, uint32_t *O, uint32_t C)
{
	uint32_t temp;
	const auto carry{lak::mul_carry_u32(A, B, &temp)};
	return lak::add_carry_u32(temp, C, O) + carry;
}

/* --- mul u64 --- */

uint64_t lak::mul_u64(uint32_t A, uint32_t B)
{
#if defined(LAK_COMPILER_MSVC) && defined(LAK_ARCH_X86_COMPAT)
	return __emulu(A, B);
#else
	return static_cast<uint64_t>(A) * static_cast<uint64_t>(B);
#endif
}

/* --- mul u128 --- */

lak::uint128_t lak::mul_u128(uint64_t A, uint64_t B)
{
#if 0 && defined(LAK_COMPILER_MSVC) && defined(LAK_ARCH_X86_64)
	lak::uint128_t result;
	result.low = _umul128(A, B, &result.high);
	return result;
#else
	const auto a_low{static_cast<uint32_t>(A)};
	const auto a_high{static_cast<uint32_t>(A >> 32U)};
	const auto b_low{static_cast<uint32_t>(B)};
	const auto b_high{static_cast<uint32_t>(B >> 32U)};

	const auto low{lak::mul_u64(a_low, b_low)};

	auto mid{
	  lak::add_u128(lak::mul_u64(a_low, b_high), lak::mul_u64(a_high, b_low))};
	mid.high = (mid.high << 32U) | (mid.low >> 32U);
	mid.low  = mid.low << 32U;

	auto result{lak::add_u128(mid, low)};

	result.high += lak::mul_u64(a_high, b_high);

	return result;
#endif
}

/* --- mul add u64 --- */

uint64_t lak::mul_add_u64(uint32_t A, uint32_t B, uint32_t C)
{
	return lak::mul_u64(A, B) + C;
}

/* --- mul add u128 --- */

lak::uint128_t lak::mul_add_u128(uint64_t A, uint64_t B, uint64_t C)
{
	return lak::add_u128(lak::mul_u128(A, B), C);
}
