#include "lak/char_utils.hpp"
#include "lak/numeric.hpp"
#include "lak/stdint.hpp"
#include "lak/strconv.hpp"
#include "lak/utility.hpp"

#include "lak/string_literals/view.hpp"

#include "lak/string_view.hpp"

#include <cmath>
#include <system_error>

template<typename T, typename CHAR>
requires requires(const T obj) {
	{
		lak::strconv<CHAR>(obj.to_string())
	} -> lak::concepts::same_as<lak::string<CHAR>>;
}
struct lak::format_traits<T, CHAR>
{
	static lak::string<CHAR> to_string(const T &value)
	{
		return lak::strconv<CHAR>(value.to_string());
	}
};

template<typename CHAR>
struct lak::format_traits<lak::monostate, CHAR>
{
	static lak::string<CHAR> to_string(const lak::monostate &) { return {}; }
};

template<typename T, typename CHAR>
requires(std::is_enum_v<T>)
struct lak::format_traits<T, CHAR>
{
	static lak::string<CHAR> to_string(const T &value)
	{
		return lak::fmt<CHAR, "{:#0X}">(static_cast<uintmax_t>(value));
	}
};
static_assert(lak::concepts::static_formattable<lak::numeric_base, char8_t>);
static_assert(!lak::concepts::dynamic_formattable<lak::numeric_base, char8_t>);
static_assert(lak::concepts::formattable<lak::numeric_base, char8_t>);

template<typename T, typename CHAR>
requires((lak::is_array_v<T> || lak::is_pointer_v<T>) &&
         lak::is_character_type_v<
           lak::remove_const_t<lak::remove_pointer_t<lak::decay_t<T>>>>)
struct lak::format_traits<T, CHAR>
{
	static lak::string<CHAR> to_string(const T &value)
	{
		return lak::strconv<CHAR>(
		  lak::string_view<lak::remove_const_t<
		    lak::remove_pointer_t<lak::decay_t<T>>>>::from_c_str(value));
	}
};
static_assert(lak::concepts::static_formattable<char *, char8_t>);
static_assert(!lak::concepts::dynamic_formattable<char *, char8_t>);
static_assert(lak::concepts::formattable<char *, char8_t>);
static_assert(lak::concepts::static_formattable<const char8_t *, char8_t>);
static_assert(!lak::concepts::dynamic_formattable<const char8_t *, char8_t>);
static_assert(lak::concepts::formattable<const char8_t *, char8_t>);
static_assert(lak::concepts::static_formattable<char[], char8_t>);
static_assert(!lak::concepts::dynamic_formattable<char[], char8_t>);
static_assert(lak::concepts::formattable<char[], char8_t>);
static_assert(lak::concepts::static_formattable<char[2], char8_t>);
static_assert(!lak::concepts::dynamic_formattable<char[2], char8_t>);
static_assert(lak::concepts::formattable<char[2], char8_t>);

template<typename CHAR>
struct lak::format_traits<bool, CHAR>
{
	static lak::string<CHAR> to_string(bool value)
	{
		return lak::strconv<CHAR>(value ? "true"_view : "false"_view);
	}
};
static_assert(lak::concepts::static_formattable<bool, char8_t>);
static_assert(lak::concepts::formattable<bool, char8_t>);

template<typename CHAR>
struct lak::format_traits<nullptr_t, CHAR>
{
	static lak::string<CHAR> to_string(nullptr_t)
	{
		return lak::strconv<CHAR>("nullptr"_view);
	}
};
static_assert(lak::concepts::static_formattable<nullptr_t, char8_t>);
static_assert(lak::concepts::formattable<nullptr_t, char8_t>);

template<typename C, typename CHAR>
struct lak::format_traits<lak::string<C>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::string<C> &value)
	{
		return lak::strconv<CHAR>(value);
	}
};
static_assert(lak::concepts::static_formattable<lak::u8string, char8_t>);
static_assert(!lak::concepts::dynamic_formattable<lak::u8string, char8_t>);
static_assert(lak::concepts::formattable<lak::u8string, char8_t>);

template<typename T, typename CHAR>
struct lak::format_traits<lak::string_view<T>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::string_view<T> &value)
	{
		return lak::strconv<CHAR>(value);
	}
};
static_assert(lak::concepts::static_formattable<lak::u8string_view, char>);
static_assert(
  lak::concepts::static_formattable<lak::u16string_view, char32_t>);
static_assert(lak::concepts::static_formattable<lak::u8string_view, char8_t>);
static_assert(
  !lak::concepts::dynamic_formattable<lak::u8string_view, char8_t>);
static_assert(lak::concepts::formattable<lak::u8string_view, char8_t>);

template<lak::concepts::one_of<LAK_ALL_STD_UNSIGNED_INTEGERS> T, typename CHAR>
struct lak::format_traits<T, CHAR>
{
	struct format_args
	{
		bool left_justified = false; // '-'
		bool show_base      = false; // '#'
		char fill           = ' ';   // '0'
		size_t precision    = 1U;    // .ddd

		// [bodxX]
		lak::numeric_base base = lak::numeric_base::dec;
		bool uppercase         = false;
	};

	static consteval format_args parse_args(lak::string_view<CHAR> args)
	{
		format_args result;
		if (args.empty()) return result;

		if (args[0] == CHAR('-'))
		{
			ASSERT_NYI();
			result.left_justified = true;
			args                  = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('#'))
		{
			result.show_base = true;
			args             = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('0'))
		{
			if (result.left_justified) throw "invalid argument combination";
			result.fill = '0';
			args        = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('.'))
		{
			result.precision = 0U;
			args             = args.substr(1U);
			if (args.empty()) return result;
			while (args[0] >= CHAR('0') && args[0] <= CHAR('9'))
			{
				result.precision *= 10U;
				result.precision += args[0] - CHAR('0');
				args = args.substr(1U);
				if (args.empty()) return result;
			}
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

	static lak::string<CHAR> to_string(const format_args &args, const T &value)
	{
		if (args.base == lak::numeric_base::bin)
		{
			lak::string<CHAR> result;
			result.reserve(sizeof(T) * CHAR_BIT + (args.show_base ? 2U : 0U));
			for (size_t i = 0U; (i < sizeof(T) * CHAR_BIT) && ((value >> i) != 0U);
			     ++i)
			{
				result.push_back(((value >> i) & 1U) ? CHAR('1') : CHAR('0'));
			}
			while (result.size() < args.precision) result.push_back(args.fill);
			if (args.show_base)
			{
				result.push_back(CHAR('b'));
				result.push_back(CHAR('0'));
			}
			lak::reverse(lak::span<CHAR>(result.data(), result.size()));
			return result;
		}
		else
		{
			std::stringstream strm;
			switch (args.base)
			{
				case lak::numeric_base::dec: strm << std::dec; break;
				case lak::numeric_base::hex: strm << std::hex; break;
				case lak::numeric_base::oct: strm << std::oct; break;
				case lak::numeric_base::bin: /* strm << std::bin; */ break;
				default:                     break;
			}
			strm << std::noshowbase;
			if (args.uppercase) strm << std::uppercase;

			if (args.show_base)
			{
				switch (args.base)
				{
					case lak::numeric_base::dec: break;
					case lak::numeric_base::hex: strm << "0x"; break;
					case lak::numeric_base::oct: strm << "0"; break;
					case lak::numeric_base::bin: strm << "0b"; break;
				}
			}

			strm << std::setfill(args.fill) << std::setw(args.precision)
			     << static_cast<uintmax_t>(value);

			return lak::strconv<CHAR>(lak::string_view<char>(strm.view()));
		}
	}
};
static_assert(lak::concepts::dynamic_formattable<unsigned int, char>);
static_assert(lak::concepts::dynamic_formattable<unsigned int, char8_t>);
static_assert(!lak::concepts::static_formattable<unsigned int, char8_t>);
static_assert(lak::concepts::formattable<unsigned int, char8_t>);

template<lak::concepts::one_of<LAK_ALL_STD_SIGNED_INTEGERS> T, typename CHAR>
struct lak::format_traits<T, CHAR>
{
	struct format_args
	{
		bool left_justified = false; // '-'
		bool force_sign     = false; // '+'
		bool sign_pad       = false; // ' '
		bool show_base      = false; // '#'
		char fill           = ' ';   // '0'
		size_t precision    = 1U;    // .ddd

		// [bodxX]
		lak::numeric_base base = lak::numeric_base::dec;
		bool uppercase         = false;
	};

	static consteval format_args parse_args(lak::string_view<CHAR> args)
	{
		format_args result;
		if (args.empty()) return result;

		if (args[0] == CHAR('-'))
		{
			throw "NYI";
			result.left_justified = true;
			args                  = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('+'))
		{
			result.force_sign = true;
			args              = args.substr(1U);
			if (args.empty()) return result;
		}
		else if (args[0] == CHAR(' '))
		{
			result.sign_pad = true;
			args            = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('#'))
		{
			result.show_base = true;
			args             = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('0'))
		{
			if (result.left_justified) throw "invalid argument combination";
			result.fill = '0';
			args        = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('.'))
		{
			result.precision = 0U;
			args             = args.substr(1U);
			if (args.empty()) return result;
			while (args[0] >= CHAR('0') && args[0] <= CHAR('9'))
			{
				result.precision *= 10U;
				result.precision += args[0] - CHAR('0');
				args = args.substr(1U);
				if (args.empty()) return result;
			}
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

	static lak::string<CHAR> to_string(const format_args &args, const T &value)
	{
		if (args.base == lak::numeric_base::bin)
		{
			auto str = lak::format_traits<std::make_unsigned_t<T>, CHAR>::to_string(
			  {
			    .left_justified = args.left_justified,
			    .show_base      = args.show_base,
			    .fill           = args.fill,
			    .precision      = args.precision,
			    .base           = args.base,
			    .uppercase      = args.uppercase,
			  },
			  static_cast<std::make_unsigned_t<T>>(value));
			return lak::fmt<CHAR, "{}{}">(value < T(0)      ? "-"_view
			                              : args.force_sign ? "+"_view
			                              : args.sign_pad   ? " "_view
			                                                : ""_view,
			                              str);
		}
		else
		{
			std::stringstream strm;
			switch (args.base)
			{
				case lak::numeric_base::dec: strm << std::dec; break;
				case lak::numeric_base::hex: strm << std::hex; break;
				case lak::numeric_base::oct: strm << std::oct; break;
				case lak::numeric_base::bin: /* strm << std::bin; */ break;
				default:                     break;
			}
			strm << std::noshowbase;
			if (args.uppercase) strm << std::uppercase;

			if (static_cast<intmax_t>(value) < intmax_t(0))
				strm << "-";
			else if (args.force_sign)
				strm << "+";
			else if (args.sign_pad)
				strm << " ";

			if (args.show_base)
			{
				switch (args.base)
				{
					case lak::numeric_base::dec: break;
					case lak::numeric_base::hex: strm << "0x"; break;
					case lak::numeric_base::oct: strm << "0"; break;
					case lak::numeric_base::bin: strm << "0b"; break;
				}
			}

			strm << std::setfill(args.fill) << std::setw(args.precision);

			if (static_cast<intmax_t>(value) < intmax_t(0))
				strm << static_cast<uintmax_t>(-static_cast<intmax_t>(value));
			else
				strm << static_cast<uintmax_t>(value);

			return lak::strconv<CHAR>(lak::string_view<char>(strm.view()));
		}
	}
};
static_assert(lak::concepts::dynamic_formattable<signed int, char>);
static_assert(lak::concepts::dynamic_formattable<signed int, char8_t>);
static_assert(lak::concepts::dynamic_formattable<signed int, char16_t>);
static_assert(lak::concepts::dynamic_formattable<signed int, char32_t>);
static_assert(lak::concepts::dynamic_formattable<signed int, wchar_t>);
static_assert(lak::concepts::dynamic_formattable<int, char8_t>);
static_assert(lak::concepts::dynamic_formattable<int, char>);
static_assert(!lak::concepts::static_formattable<signed int, char8_t>);
static_assert(lak::concepts::formattable<signed int, char8_t>);

template<lak::concepts::one_of<LAK_ALL_STD_FLOATS> T, typename CHAR>
struct lak::format_traits<T, CHAR>
{
	struct format_args
	{
		bool left_justified = false; // '-'
		bool force_sign     = false; // '+'
		bool sign_pad       = false; // ' '
		bool show_base      = false; // '#'
		char fill           = ' ';   // '0'
		size_t min_width    = 1U;    // ddd
		size_t precision    = 6U;    // .ddd

		// [bodxX]
		lak::numeric_base base = lak::numeric_base::dec;
		bool uppercase         = false;
	};

	static consteval format_args parse_args(lak::string_view<CHAR> args)
	{
		format_args result;
		if (args.empty()) return result;

		if (args[0] == CHAR('-'))
		{
			throw "NYI";
			result.left_justified = true;
			args                  = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('+'))
		{
			result.force_sign = true;
			args              = args.substr(1U);
			if (args.empty()) return result;
		}
		else if (args[0] == CHAR(' '))
		{
			result.sign_pad = true;
			args            = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('#'))
		{
			result.show_base = true;
			args             = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] == CHAR('0'))
		{
			if (result.left_justified) throw "invalid argument combination";
			result.fill = '0';
			args        = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] >= CHAR('1') && args[0] <= CHAR('9'))
		{
			result.min_width = 0U;
			args             = args.substr(1U);
			if (args.empty()) return result;
			while (args[0] >= CHAR('0') && args[0] <= CHAR('9'))
			{
				result.min_width *= 10U;
				result.min_width += args[0] - CHAR('0');
				args = args.substr(1U);
				if (args.empty()) return result;
			}
		}

		if (args[0] == CHAR('.'))
		{
			result.precision = 0U;
			args             = args.substr(1U);
			if (args.empty()) return result;
			while (args[0] >= CHAR('0') && args[0] <= CHAR('9'))
			{
				result.precision *= 10U;
				result.precision += args[0] - CHAR('0');
				args = args.substr(1U);
				if (args.empty()) return result;
			}
		}

		switch (args[0])
		{
			// case CHAR('b'): result.base = lak::numeric_base::bin; break;
			// case CHAR('o'): result.base = lak::numeric_base::oct; break;
			case CHAR('d'): result.base = lak::numeric_base::dec; break;
			// case CHAR('X'): result.uppercase = true; [[fallthrough]];
			// case CHAR('x'): result.base = lak::numeric_base::hex; break;
			default:        throw "invalid base";
		}
		args = args.substr(1U);

		if (!args.empty()) throw "invalid format arguments";
		return result;
	}

	static lak::string<CHAR> to_string(const format_args &args, const T &value)
	{
		std::stringstream strm;
		switch (args.base)
		{
			case lak::numeric_base::dec: strm << std::dec; break;
			case lak::numeric_base::hex: strm << std::hex; break;
			case lak::numeric_base::oct: strm << std::oct; break;
			case lak::numeric_base::bin: /* strm << std::bin; */ break;
			default:                     break;
		}
		strm << std::noshowbase;
		if (args.uppercase) strm << std::uppercase;

		if (std::signbit(value))
			strm << "-";
		else if (args.force_sign)
			strm << "+";
		else if (args.sign_pad)
			strm << " ";

		if (args.show_base)
		{
			switch (args.base)
			{
				case lak::numeric_base::dec: break;
				case lak::numeric_base::hex: strm << "0x"; break;
				case lak::numeric_base::oct: strm << "0"; break;
				case lak::numeric_base::bin: strm << "0b"; break;
			}
		}

		strm << std::setfill(args.fill) << std::setw(args.min_width)
		     << std::setprecision(args.precision)
		     << (std::signbit(value) ? -value : value);

		return lak::strconv<CHAR>(lak::string_view<char>(strm.view()));
	}
};
static_assert(lak::concepts::dynamic_formattable<float, char>);
static_assert(lak::concepts::dynamic_formattable<float, char8_t>);
static_assert(lak::concepts::dynamic_formattable<float, char16_t>);
static_assert(lak::concepts::dynamic_formattable<float, char32_t>);
static_assert(lak::concepts::dynamic_formattable<float, wchar_t>);
static_assert(lak::concepts::dynamic_formattable<double, char8_t>);
static_assert(lak::concepts::dynamic_formattable<double, char>);
static_assert(!lak::concepts::static_formattable<long double, char8_t>);
static_assert(lak::concepts::formattable<long double, char8_t>);

template<lak::concepts::one_of<LAK_ALL_CHARS> T, typename CHAR>
struct lak::format_traits<T, CHAR>
{
	enum struct format_args
	{
		always_character,     // C
		always_hex,           // H
		ascii_printable_only, // A
	};

	static consteval format_args parse_args(lak::string_view<CHAR> args)
	{
		if (args.empty()) return format_args::always_character;
		if (args.size() != 1) throw "invalid format arguments";
		if (args[0] == CHAR('C')) return format_args::always_character;
		if (args[0] == CHAR('H')) return format_args::always_hex;
		if (args[0] == CHAR('A')) return format_args::ascii_printable_only;
		throw "invalid format arguments";
	}

	static lak::string<CHAR> to_string(const format_args &args, const T &value)
	{
		switch (args)
		{
			case format_args::ascii_printable_only:
				if (lak::is_ascii_printable(char32_t(value)))
				{
					default:
					case format_args::always_character:
						return lak::strconv<CHAR>(lak::string_view(&value, 1U));
				}
				else
				{
					case format_args::always_hex:
						if constexpr (sizeof(T) == sizeof(char8_t))
							return lak::fmt<CHAR, "\\x{:0.2X}">(uintmax_t(value));
						else if constexpr (sizeof(T) == sizeof(char16_t))
							return lak::fmt<CHAR, "\\x{:0.4X}">(uintmax_t(value));
						else if constexpr (sizeof(T) == sizeof(char32_t))
							return lak::fmt<CHAR, "\\x{:0.8X}">(uintmax_t(value));
						else
							return lak::fmt<CHAR, "\\x{:0X}">(uintmax_t(value));
				}
		}
	}
};
static_assert(!lak::concepts::static_formattable<wchar_t, char8_t>);
static_assert(lak::concepts::dynamic_formattable<wchar_t, char8_t>);
static_assert(lak::concepts::formattable<wchar_t, char8_t>);

template<typename CHAR>
struct lak::format_traits<std::error_code, CHAR>
{
	static lak::string<CHAR> to_string(const std::error_code &err)
	{
		return lak::fmt<CHAR, "{}:{} ({})">(
		  err.category().name(), err.value(), err.message());
	}
};
static_assert(lak::concepts::static_formattable<std::error_code, char8_t>);
static_assert(!lak::concepts::dynamic_formattable<std::error_code, char8_t>);
static_assert(lak::concepts::formattable<std::error_code, char8_t>);
