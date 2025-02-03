#ifndef LAK_NUMERIC_HPP
#define LAK_NUMERIC_HPP

#include "lak/result.hpp"
#include "lak/stdint.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	enum struct numeric_base : uint8_t
	{
		bin = 2,
		oct = 8,
		dec = 10,
		hex = 16,
	};

	enum struct string_to_numeric_error
	{
		invalid_string,
		invalid_base,
		out_of_bounds,
	};

	// bin: [0-1]+
	// oct: [0-7]+
	// dec: [0-9]+
	// hex: [0-9a-fA-F]+
	lak::result<uintmax_t, lak::string_to_numeric_error> string_to_uintmax(
	  lak::u8string_view integer,
	  lak::numeric_base base = lak::numeric_base::dec);

	// bin: [+-]?[0-1]+
	// oct: [+-]?[0-7]+
	// dec: [+-]?[0-9]+
	// hex: [+-]?[0-9a-fA-F]+
	lak::result<intmax_t, lak::string_to_numeric_error> string_to_intmax(
	  lak::u8string_view integer,
	  lak::numeric_base base = lak::numeric_base::dec);

	// bin:
	// i: [+-]?[0-1]+
	// f: [0-1]*
	// e: [+-]?[0-1]*
	// b: [0-1]+
	//
	// oct:
	// i: [+-]?[0-7]+
	// f: [0-7]*
	// e: [+-]?[0-7]*
	// b: [0-1]+
	//
	// dec:
	// i: [+-]?[0-9]+
	// f: [0-9]*
	// e: [+-]?[0-9]*
	// b: [0-1]+
	//
	// hex:
	// i: [+-]?[0-9a-fA-F]+
	// f: [0-9a-fA-F]*
	// e: [+-]?[0-9a-fA-F]*
	// b: [0-1]+
	//
	// (i.f)*b^e
	lak::result<double, lak::string_to_numeric_error> string_to_double(
	  lak::u8string_view integer_part,
	  lak::u8string_view fraction_part,
	  lak::u8string_view exponent_part,
	  uintmax_t exponent_base_part,
	  lak::numeric_base character_base = lak::numeric_base::dec);
	lak::result<double, lak::string_to_numeric_error> string_to_double(
	  lak::u8string_view integer_part,
	  lak::u8string_view fraction_part,
	  lak::u8string_view exponent_part,
	  lak::u8string_view exponent_base_part,
	  lak::numeric_base character_base = lak::numeric_base::dec);

	// i: [+-]?[0-9]+
	// f: [0-9]*
	// e: [+-]?[0-9]*
	// exponent is *10^e
	lak::result<double, lak::string_to_numeric_error> dec_string_to_double(
	  lak::u8string_view integer_part,
	  lak::u8string_view fraction_part,
	  lak::u8string_view exponent_part);

	// i: [+-]?[0-9a-fA-F]+
	// f: [0-9a-fA-F]*
	// e: [+-]?[0-9a-fA-F]*
	// exponent is *2^e
	lak::result<double, lak::string_to_numeric_error> hex_string_to_double(
	  lak::u8string_view integer_part,
	  lak::u8string_view fraction_part,
	  lak::u8string_view exponent_part);

	struct uint128_t
	{
		uint64_t high;
		uint64_t low;
	};

	lak::uint128_t add_u128(uint64_t A, uint64_t B);
	lak::uint128_t mul_u128(uint64_t A, uint64_t B);

	struct uintmax2_t
	{
		uintmax_t high;
		uintmax_t low;
	};

	lak::uintmax2_t add_uintmax2(uintmax_t A, uintmax_t B, uintmax_t C = 0U);
	lak::uintmax2_t add_uintmax2(lak::uintmax2_t A,
	                             uintmax_t B,
	                             uintmax_t C = 0U);
	lak::uintmax2_t sub_uintmax2(uintmax_t A, uintmax_t B);
	lak::uintmax2_t sub_uintmax2(uintmax_t A,
	                             uintmax_t B,
	                             uintmax_t C); // A - (B + (~C + 1))
	lak::uintmax2_t mul_uintmax2(uintmax_t A, uintmax_t B);

	std::ostream &operator<<(std::ostream &strm,
	                         lak::string_to_numeric_error err);
}

#endif
