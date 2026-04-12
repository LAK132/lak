#include "lak/format.hpp"

template<typename CHAR>
struct lak::format_traits<lak::bit_count, CHAR>
{
	struct format_args
	{
		bool show_base = false; // '#'

		// [bodxX]
		lak::numeric_base base = lak::numeric_base::dec;
		bool uppercase         = false;
	};

	static consteval format_args parse_args(lak::string_view<CHAR> args)
	{
		format_args result;
		if (args.empty()) return result;

		if (args[0] == CHAR('#'))
		{
			result.show_base = true;
			args             = args.substr(1U);
			if (args.empty()) return result;
		}

		switch (args[0])
		{
			case CHAR('b'): result.base = lak::numeric_base::bin; break;
			case CHAR('o'): result.base = lak::numeric_base::oct; break;
			case CHAR('d'): result.base = lak::numeric_base::dec; break;
			case CHAR('X'): result.uppercase = true; [[fallthrough]];
			case CHAR('x'): result.base = lak::numeric_base::hex; break;
			default:        throw "invalid base";
		}
		args = args.substr(1U);

		if (!args.empty()) throw "invalid format arguments";
		return result;
	}

	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::bit_count &value)
	{
		return lak::fmt<CHAR, "{}.{}">(
		  lak::format_traits<size_t, CHAR>::to_string(
		    {.left_justified = false,
		     .show_base      = args.show_base,
		     .fill           = ' ',
		     .precision      = 1U,
		     .base           = args.base,
		     .uppercase      = args.uppercase},
		    value.bytes),
		  lak::format_traits<size_t, CHAR>::to_string(
		    {.left_justified = false,
		     .show_base      = false,
		     .fill           = '0',
		     .precision      = (args.base == lak::numeric_base::bin   ? 8U
		                        : args.base == lak::numeric_base::oct ? 4U
		                        : args.base == lak::numeric_base::hex ? 2U
		                                                              : 1U),
		     .base           = args.base,
		     .uppercase      = args.uppercase},
		    size_t(value.bits)));
	}
};

inline lak::bit_count lak::bit_count::from_bits(size_t bits)
{
	return {.bytes = bits / CHAR_BIT, .bits = uint8_t(bits % CHAR_BIT)};
}

inline lak::bit_count lak::bit_count::from_bytes(size_t bytes)
{
	return {.bytes = bytes, .bits = 0U};
}

inline size_t lak::bit_count::to_bits() const
{
	return (bytes * CHAR_BIT) + size_t(bits);
}

inline lak::bit_count &lak::bit_count::normalise()
{
	bytes += bits / CHAR_BIT;
	bits %= CHAR_BIT;
	return *this;
}

inline lak::bit_count lak::bit_count::normalised() const
{
	lak::bit_count result = *this;
	result.normalise();
	return result;
}

inline lak::bit_count &lak::bit_count::operator+=(const lak::bit_count &other)
{
	size_t _bits = bits + other.bits;
	bits         = uint8_t(_bits % CHAR_BIT);
	bytes += other.bytes + (_bits / CHAR_BIT);
	return *this;
}

inline lak::bit_count lak::bit_count::operator+(
  const lak::bit_count &other) const
{
	lak::bit_count result = *this;
	result += other;
	return result;
}

inline lak::bit_count &lak::bit_count::operator-=(const lak::bit_count &other)
{
	if (other.bits > bits)
	{
		bits = CHAR_BIT - ((other.bits % CHAR_BIT) - bits);
		bytes -= other.bytes + 1U + (other.bits / CHAR_BIT);
	}
	else
	{
		size_t _bits = bits - other.bits;
		bits         = uint8_t(_bits % CHAR_BIT);
		bytes -= other.bytes + (_bits / CHAR_BIT);
	}
	return *this;
}

inline lak::bit_count lak::bit_count::operator-(
  const lak::bit_count &other) const
{
	lak::bit_count result = *this;
	result -= other;
	return result;
}

inline lak::strong_ordering lak::bit_count::operator<=>(
  const lak::bit_count &other) const
{
	lak::bit_count lhs = normalised();
	lak::bit_count rhs = other.normalised();
	if (lhs.bytes > rhs.bytes) return lak::strong_ordering::greater;
	if (lhs.bytes < rhs.bytes) return lak::strong_ordering::less;
	if (lhs.bits > rhs.bits) return lak::strong_ordering::greater;
	if (lhs.bits < rhs.bits) return lak::strong_ordering::less;
	return lak::strong_ordering::equal;
}

inline bool lak::bit_count::operator==(const lak::bit_count &other) const
{
	return (*this <=> other) == 0;
}
