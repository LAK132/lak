#include "lak/numeric.hpp"

#include "lak/architecture.hpp"
#include "lak/memmanip.hpp"
#include "lak/span_manip.hpp"

lak::u8string_view uintmax_max_dec_str()
{
	static lak::u8string str = []() -> lak::u8string
	{
		lak::u8string str;
		str.reserve((UINTMAX_MAX / 9) + 1);
		uintmax_t max = UINTMAX_MAX;
		for (size_t i = 0; max != 0; ++i)
		{
			str.push_back(static_cast<char8_t>((max % 10) + u8'0'));
			max /= 10;
		}
		lak::reverse(lak::span<char8_t>(str.data(), str.size()));
		return str;
	}();
	return lak::u8string_view(str);
}

lak::result<uintmax_t, lak::string_to_numeric_error> lak::string_to_uintmax(
  lak::u8string_view integer, lak::numeric_base base)
{
	if (integer.empty())
		return lak::err_t{lak::string_to_numeric_error::invalid_string};

	switch (base)
	{
		case lak::numeric_base::bin:
		{
			// [0-1]+
			if (integer.size() > (sizeof(uintmax_t) * CHAR_BIT))
				return lak::err_t{lak::string_to_numeric_error::out_of_bounds};

			for (const char8_t &c : integer)
				if (c < u8'0' || c > u8'1')
					return lak::err_t{lak::string_to_numeric_error::invalid_string};

			uintmax_t result = 0;
			for (const char8_t &c : integer)
			{
				result <<= 1U;
				result |= static_cast<uint8_t>(c - u8'0');
			}

			return lak::ok_t<uintmax_t>{result};
		}
		break;

		case lak::numeric_base::oct:
		{
			// [0-7]+
			if ((integer.size() * 3) < (sizeof(uintmax_t) * CHAR_BIT))
			{
				// fast path
			}
			else if (((integer.size() - 1) * 3) < (sizeof(uintmax_t) * CHAR_BIT))
			{
				// have to check first value
				const size_t max_high_bits =
				  (sizeof(uintmax_t) * CHAR_BIT) - ((integer.size() - 1) * 3);

				if (integer[0] < u8'0' || integer[0] > u8'7')
					return lak::err_t{lak::string_to_numeric_error::invalid_string};

				if (const uintmax_t first_value =
				      static_cast<uintmax_t>(integer[0] - u8'0');
				    (static_cast<uintmax_t>(~(UINTMAX_MAX << max_high_bits)) &
				     first_value) != first_value)
					return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
			}
			else
				return lak::err_t{lak::string_to_numeric_error::out_of_bounds};

			for (const char8_t &c : integer)
				if (c < u8'0' || c > u8'7')
					return lak::err_t{lak::string_to_numeric_error::invalid_string};

			uintmax_t result = 0;
			for (const char8_t &c : integer)
			{
				result <<= 3U;
				result |= static_cast<uint8_t>(c - u8'0');
			}

			return lak::ok_t<uintmax_t>{result};
		}
		break;

		case lak::numeric_base::dec:
		{
			// [0-9]+
			uintmax_t result = 0;

			if ((integer.size() * 9) < (sizeof(uintmax_t) * CHAR_BIT))
			{
				// fast path
				for (const char8_t &c : integer)
					if (c < u8'0' || c > u8'9')
						return lak::err_t{lak::string_to_numeric_error::invalid_string};
			}
			else if (((integer.size() - 1) * 9) < (sizeof(uintmax_t) * CHAR_BIT))
			{
				// have to bounds check
				lak::u8string_view max = uintmax_max_dec_str();
				while (!integer.empty())
				{
					if (integer[0] > max[0])
						return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
					if (integer[0] < max[0]) break; // continue with the fast path
					result *= 10U;
					result += static_cast<uint8_t>(integer[0] - u8'0');
					max     = max.substr(1);
					integer = integer.substr(1);
				}
			}
			else
				return lak::err_t{lak::string_to_numeric_error::out_of_bounds};

			for (const char8_t &c : integer)
			{
				result *= 10U;
				result += static_cast<uint8_t>(c - u8'0');
			}

			return lak::ok_t<uintmax_t>{result};
		}
		break;

		case lak::numeric_base::hex:
		{
			// [0-9a-fA-F]+
			if (integer.size() > (sizeof(uintmax_t) * CHAR_BIT))
				return lak::err_t{lak::string_to_numeric_error::out_of_bounds};

			uintmax_t result = 0;
			for (const char8_t &c : integer)
			{
				result <<= 4U;
				if (c >= u8'0' && c <= u8'9')
					result |= static_cast<uint8_t>(c - u8'0');
				else if (c >= u8'a' && c <= u8'f')
					result |= static_cast<uint8_t>(c - u8'a');
				else if (c >= u8'A' && c <= u8'F')
					result |= static_cast<uint8_t>(c - u8'A');
				else
					return lak::err_t{lak::string_to_numeric_error::invalid_string};
			}

			return lak::ok_t<uintmax_t>{result};
		}
		break;

		default:
			return lak::err_t{lak::string_to_numeric_error::invalid_base};
	}
}

lak::result<intmax_t, lak::string_to_numeric_error> lak::string_to_intmax(
  lak::u8string_view integer, lak::numeric_base base)
{
	if (integer.empty())
		return lak::err_t{lak::string_to_numeric_error::invalid_string};

	bool is_negative = integer[0] == u8'-';
	if (is_negative || integer[0] == u8'+') integer = integer.substr(1);

	return lak::string_to_uintmax(integer, base)
	  .and_then(
	    [is_negative](uintmax_t uresult)
	      -> lak::result<intmax_t, lak::string_to_numeric_error>
	    {
		    // C++20 guarantees signed integers are two's complement, bit cast and
		    // negation is always safe.
		    intmax_t result = lak::bit_cast<intmax_t>(uresult);
		    if (result < 0)
			    return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
		    return lak::ok_t<intmax_t>{is_negative ? -result : result};
	    });
}

#if 0
lak::result<double, lak::string_to_numeric_error> dec_string_to_double(
  lak::u8string_view integer_part,
  lak::u8string_view fraction_part,
  lak::u8string_view exponent_part)
{
	LAK_UNUSED(integer_part);
	LAK_UNUSED(fraction_part);
	LAK_UNUSED(exponent_part);

	// Eisel-Lemire
	// https://nigeltao.github.io/blog/2020/eisel-lemire.html

	// ParseNumberF64 by Simple Decimal Conversion
	// https://nigeltao.github.io/blog/2020/parse-number-f64-simple.html

	ASSERT_NYI();
}
#endif

lak::result<double, lak::string_to_numeric_error> lak::string_to_double(
  lak::u8string_view integer_part,
  lak::u8string_view fraction_part,
  lak::u8string_view exponent_part,
  uintmax_t exponent_base_part,
  lak::numeric_base character_base)
{
	if (integer_part.empty())
		return lak::err_t{lak::string_to_numeric_error::invalid_string};

	const bool is_negative = integer_part[0] == u8'-';
	if (is_negative || integer_part[0] == u8'+')
		integer_part = integer_part.substr(1);

	// uintmax parser guarantees all characters are digits, no + or -
	RES_TRY_ASSIGN(const uintmax_t integer_integer =,
	               lak::string_to_uintmax(integer_part, character_base));

	uintmax_t integer_fraction = 0;
	if (!fraction_part.empty())
	{
		RES_TRY_ASSIGN(integer_fraction =,
		               lak::string_to_uintmax(fraction_part, character_base));
	}

	intmax_t integer_exponent = 0;
	if (!exponent_part.empty())
	{
		RES_TRY_ASSIGN(integer_exponent =,
		               lak::string_to_intmax(exponent_part, character_base));
	}

	const double float_exponent =
	  std::pow(static_cast<double>(exponent_base_part),
	           static_cast<double>(integer_exponent));

	const double float_fraction_exponent =
	  std::pow(static_cast<double>(static_cast<uint8_t>(character_base)),
	           -static_cast<double>(fraction_part.size()));

	const double res =
	  (static_cast<double>(integer_integer) +
	   (static_cast<double>(integer_fraction) * float_fraction_exponent)) *
	  float_exponent;

	return lak::ok_t{is_negative ? -res : res};
}

lak::result<double, lak::string_to_numeric_error> lak::string_to_double(
  lak::u8string_view integer_part,
  lak::u8string_view fraction_part,
  lak::u8string_view exponent_part,
  lak::u8string_view exponent_base_part,
  lak::numeric_base character_base)
{
	RES_TRY_ASSIGN(const uintmax_t integer_base =,
	               lak::string_to_uintmax(exponent_base_part, character_base));

	return lak::string_to_double(
	  integer_part, fraction_part, exponent_part, integer_base, character_base);
}

lak::result<double, lak::string_to_numeric_error> lak::dec_string_to_double(
  lak::u8string_view integer_part,
  lak::u8string_view fraction_part,
  lak::u8string_view exponent_part)
{
	return lak::string_to_double(
	  integer_part, fraction_part, exponent_part, 10, lak::numeric_base::dec);
}

lak::result<double, lak::string_to_numeric_error> lak::hex_string_to_double(
  lak::u8string_view integer_part,
  lak::u8string_view fraction_part,
  lak::u8string_view exponent_part)
{
	return lak::string_to_double(
	  integer_part, fraction_part, exponent_part, 2, lak::numeric_base::hex);
}

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

std::ostream &lak::operator<<(std::ostream &strm,
                              lak::string_to_numeric_error err)
{
	switch (err)
	{
		case lak::string_to_numeric_error::invalid_string:
			strm << "invalid string";
			break;
		case lak::string_to_numeric_error::invalid_base:
			strm << "invalid base";
			break;
		case lak::string_to_numeric_error::out_of_bounds:
			strm << "out of bounds";
			break;
	}
	return strm;
}

static_assert(lak::concepts::streamable<lak::string_to_numeric_error>);
