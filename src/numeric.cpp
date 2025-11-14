#include "lak/numeric.hpp"

#include "lak/memmanip.hpp"
#include "lak/span_manip.hpp"
#include "lak/system/architecture.hpp"

template<lak::numeric_base base>
static force_inline uint8_t char_to_value(char8_t c)
{
	if constexpr (base == lak::numeric_base::bin ||
	              base == lak::numeric_base::oct ||
	              base == lak::numeric_base::dec)
		return static_cast<uint8_t>(c - u8'0');
	else
	{
		static_assert(base == lak::numeric_base::hex);
		if (c >= u8'0' && c <= u8'9')
			return static_cast<uint8_t>(c - u8'0');
		else if (c >= u8'a' && c <= u8'f')
			return static_cast<uint8_t>(c - u8'a') + 0xAU;
		else
			return static_cast<uint8_t>(c - u8'A') + 0xAU;
	}
}

lak::u8string_view lak::uintmax_max_bin_str()
{
	static lak::u8string str = []() -> lak::u8string
	{
		lak::u8string str;
		str.resize(sizeof(uintmax_t) * CHAR_BIT, u8'1');
		return str;
	}();
	return lak::u8string_view(str);
}

lak::u8string_view lak::uintmax_max_oct_str()
{
	static lak::u8string str = []() -> lak::u8string
	{
		lak::u8string str;
		uintmax_t max = UINTMAX_MAX;
		for (; max != 0U;)
		{
			str.push_back(static_cast<char8_t>((max % 010U) + u8'0'));
			max /= 010U;
		}
		lak::reverse(lak::span<char8_t>(str.data(), str.size()));
		return str;
	}();
	return lak::u8string_view(str);
}

lak::u8string_view lak::uintmax_max_dec_str()
{
	static lak::u8string str = []() -> lak::u8string
	{
		lak::u8string str;
		uintmax_t max = UINTMAX_MAX;
		for (; max != 0U;)
		{
			str.push_back(static_cast<char8_t>((max % 10U) + u8'0'));
			max /= 10U;
		}
		lak::reverse(lak::span<char8_t>(str.data(), str.size()));
		return str;
	}();
	return lak::u8string_view(str);
}

lak::u8string_view lak::uintmax_max_hex_str()
{
	static lak::u8string str = []() -> lak::u8string
	{
		lak::u8string str;
		uintmax_t max = UINTMAX_MAX;
		for (; max != 0U;)
		{
			if (uintmax_t m = max % 0x10U; m < 0xA)
				str.push_back(static_cast<char8_t>(m + u8'0'));
			else
				str.push_back(static_cast<char8_t>(m + u8'A'));
			max /= 0x10U;
		}
		lak::reverse(lak::span<char8_t>(str.data(), str.size()));
		return str;
	}();
	return lak::u8string_view(str);
}

static lak::error_code<lak::string_to_numeric_error> validate_bin_str(
  lak::u8string_view str)
{
	lak::u8string_view max = lak::uintmax_max_bin_str();
	if (str.size() > max.size())
		return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
	for (const char8_t &c : str)
		if (c < u8'0' || c > u8'1')
			return lak::err_t{lak::string_to_numeric_error::invalid_string};
	if (str.size() == max.size())
		for (size_t i = 0; i < str.size(); ++i)
			if (char_to_value<lak::numeric_base::bin>(str[i]) >
			    char_to_value<lak::numeric_base::bin>(max[i]))
				return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
	return lak::ok_t{};
}

static lak::error_code<lak::string_to_numeric_error> validate_oct_str(
  lak::u8string_view str)
{
	lak::u8string_view max = lak::uintmax_max_oct_str();
	if (str.size() > max.size())
		return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
	for (const char8_t &c : str)
		if (c < u8'0' || c > u8'7')
			return lak::err_t{lak::string_to_numeric_error::invalid_string};
	if (str.size() == max.size())
		for (size_t i = 0; i < str.size(); ++i)
			if (char_to_value<lak::numeric_base::oct>(str[i]) >
			    char_to_value<lak::numeric_base::oct>(max[i]))
				return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
	return lak::ok_t{};
}

static lak::error_code<lak::string_to_numeric_error> validate_dec_str(
  lak::u8string_view str)
{
	lak::u8string_view max = lak::uintmax_max_dec_str();
	if (str.size() > max.size())
		return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
	for (const char8_t &c : str)
		if (c < u8'0' || c > u8'9')
			return lak::err_t{lak::string_to_numeric_error::invalid_string};
	if (str.size() == max.size())
		for (size_t i = 0; i < str.size(); ++i)
			if (char_to_value<lak::numeric_base::dec>(str[i]) >
			    char_to_value<lak::numeric_base::dec>(max[i]))
				return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
	return lak::ok_t{};
}

static lak::error_code<lak::string_to_numeric_error> validate_hex_str(
  lak::u8string_view str)
{
	lak::u8string_view max = lak::uintmax_max_hex_str();
	if (str.size() > max.size())
		return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
	for (const char8_t &c : str)
		if (!(c >= u8'0' && c <= u8'9') && !(c >= u8'a' && c <= u8'f') &&
		    !(c >= u8'A' && c <= u8'F'))
			return lak::err_t{lak::string_to_numeric_error::invalid_string};
	if (str.size() == max.size())
		for (size_t i = 0; i < str.size(); ++i)
			if (char_to_value<lak::numeric_base::hex>(str[i]) >
			    char_to_value<lak::numeric_base::hex>(max[i]))
				return lak::err_t{lak::string_to_numeric_error::out_of_bounds};
	return lak::ok_t{};
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
			RES_TRY(validate_bin_str(integer));

			uintmax_t result = 0;
			for (const char8_t &c : integer)
			{
				result <<= 1U;
				result |= char_to_value<lak::numeric_base::bin>(c);
			}

			return lak::ok_t<uintmax_t>{result};
		}
		break;

		case lak::numeric_base::oct:
		{
			// [0-7]+
			RES_TRY(validate_oct_str(integer));

			uintmax_t result = 0;
			for (const char8_t &c : integer)
			{
				result <<= 3U;
				result |= char_to_value<lak::numeric_base::oct>(c);
			}

			return lak::ok_t<uintmax_t>{result};
		}
		break;

		case lak::numeric_base::dec:
		{
			// [0-9]+
			RES_TRY(validate_dec_str(integer));

			uintmax_t result = 0;
			for (const char8_t &c : integer)
			{
				result *= 10U;
				result += char_to_value<lak::numeric_base::dec>(c);
				DEBUG_EXPR(result);
			}

			return lak::ok_t<uintmax_t>{result};
		}
		break;

		case lak::numeric_base::hex:
		{
			// [0-9a-fA-F]+
			RES_TRY(validate_hex_str(integer));

			uintmax_t result = 0;
			for (const char8_t &c : integer)
			{
				result <<= 4U;
				result |= char_to_value<lak::numeric_base::hex>(c);
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
