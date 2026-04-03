#ifndef LAK_NUMERIC_HPP
#define LAK_NUMERIC_HPP

#include "lak/stdint.hpp"

#include <limits>

namespace lak
{
	namespace err
	{
		enum struct string_to_numeric
		{
			invalid_string,
			invalid_base,
			out_of_bounds,
		};
	}

	enum struct numeric_base : uint8_t
	{
		bin = 2,
		oct = 8,
		dec = 10,
		hex = 16,
	};
}

#ifndef LAK_DEBUG_FORWARD_ONLY
#	define LAK_DEBUG_FORWARD_ONLY
#endif
#include "lak/debug.hpp"

#include "lak/format_traits.hpp"
#include "lak/result.hpp"
#include "lak/strconv.hpp"
#include "lak/string_literals/view.hpp"
#include "lak/string_view.hpp"
#include "lak/wide_math.hpp"

namespace lak
{
	template<typename CHAR>
	struct format_traits<lak::err::string_to_numeric, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::err::string_to_numeric &err)
		{
			switch (err)
			{
				case lak::err::string_to_numeric::invalid_string:
					return lak::strconv<CHAR>("invalid string"_view);
				case lak::err::string_to_numeric::invalid_base:
					return lak::strconv<CHAR>("invalid base"_view);
				case lak::err::string_to_numeric::out_of_bounds:
					return lak::strconv<CHAR>("invalid base"_view);
				default: ASSERT_UNREACHABLE(); break;
			}
		}
	};

	lak::u8string_view uintmax_max_bin_str();
	lak::u8string_view uintmax_max_oct_str();
	lak::u8string_view uintmax_max_dec_str();
	lak::u8string_view uintmax_max_hex_str();

	// bin: [0-1]+
	// oct: [0-7]+
	// dec: [0-9]+
	// hex: [0-9a-fA-F]+
	lak::result<uintmax_t, lak::err::string_to_numeric> string_to_uintmax(
	  lak::u8string_view integer,
	  lak::numeric_base base = lak::numeric_base::dec);

	// bin: [+-]?[0-1]+
	// oct: [+-]?[0-7]+
	// dec: [+-]?[0-9]+
	// hex: [+-]?[0-9a-fA-F]+
	lak::result<intmax_t, lak::err::string_to_numeric> string_to_intmax(
	  lak::u8string_view integer,
	  lak::numeric_base base = lak::numeric_base::dec);

	template<typename INTEGER>
	lak::result<INTEGER, lak::err::string_to_numeric> string_to_int(
	  lak::u8string_view integer,
	  lak::numeric_base base = lak::numeric_base::dec)
	{
		static_assert(std::numeric_limits<INTEGER>::is_integer);
		if constexpr (std::numeric_limits<INTEGER>::is_signed)
		{
			return lak::string_to_intmax(integer, base)
			  .and_then(
			    [](intmax_t value)
			      -> lak::result<INTEGER, lak::err::string_to_numeric>
			    {
				    if (value < std::numeric_limits<INTEGER>::lowest() ||
				        value > std::numeric_limits<INTEGER>::max())
					    return lak::err_t{lak::err::string_to_numeric::out_of_bounds};
				    return lak::ok_t{static_cast<INTEGER>(value)};
			    });
		}
		else
		{
			return lak::string_to_uintmax(integer, base)
			  .and_then(
			    [](uintmax_t value)
			      -> lak::result<INTEGER, lak::err::string_to_numeric>
			    {
				    if (value > std::numeric_limits<INTEGER>::max())
					    return lak::err_t{lak::err::string_to_numeric::out_of_bounds};
				    return lak::ok_t{static_cast<INTEGER>(value)};
			    });
		}
	}

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
	lak::result<double, lak::err::string_to_numeric> string_to_double(
	  lak::u8string_view integer_part,
	  lak::u8string_view fraction_part,
	  lak::u8string_view exponent_part,
	  uintmax_t exponent_base_part,
	  lak::numeric_base character_base = lak::numeric_base::dec);
	lak::result<double, lak::err::string_to_numeric> string_to_double(
	  lak::u8string_view integer_part,
	  lak::u8string_view fraction_part,
	  lak::u8string_view exponent_part,
	  lak::u8string_view exponent_base_part,
	  lak::numeric_base character_base = lak::numeric_base::dec);

	// i: [+-]?[0-9]+
	// f: [0-9]*
	// e: [+-]?[0-9]*
	// exponent is *10^e
	lak::result<double, lak::err::string_to_numeric> dec_string_to_double(
	  lak::u8string_view integer_part,
	  lak::u8string_view fraction_part,
	  lak::u8string_view exponent_part);

	// i: [+-]?[0-9a-fA-F]+
	// f: [0-9a-fA-F]*
	// e: [+-]?[0-9a-fA-F]*
	// exponent is *2^e
	lak::result<double, lak::err::string_to_numeric> hex_string_to_double(
	  lak::u8string_view integer_part,
	  lak::u8string_view fraction_part,
	  lak::u8string_view exponent_part);

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
}

#endif
