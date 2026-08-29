#include "lak/char_utils.hpp"
#include "lak/numeric.hpp"
#include "lak/stdint.hpp"
#include "lak/strconv.hpp"
#include "lak/utility.hpp"

#include "lak/string_literals/view.hpp"

#include "lak/string_view.hpp"

#include <cmath>
#include <stddef.h>
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

namespace lak
{
	template<typename CHAR>
	struct char_format_args
	{
		bool escape_ascii_printable     = false;     // H, !C
		bool escape_ascii_non_printable = false;     // H, A
		CHAR hex_char                   = CHAR('x'); // xXuU

		static consteval void _parse_args(char_format_args &result,
		                                  lak::string_view<CHAR> &args)
		{
			if (args.empty()) return;

			if (args[0] == CHAR('C'))
			{
				result.escape_ascii_printable     = false;
				result.escape_ascii_non_printable = false;
				args                              = args.substr(1U);
			}
			else if (args[0] == CHAR('H'))
			{
				result.escape_ascii_printable     = true;
				result.escape_ascii_non_printable = true;
				args                              = args.substr(1U);
			}
			else if (args[0] == CHAR('A'))
			{
				result.escape_ascii_printable     = false;
				result.escape_ascii_non_printable = true;
				args                              = args.substr(1U);
			}

			if (args.empty()) return;

			if (args[0] == CHAR('x') || args[0] == CHAR('X') ||
			    args[0] == CHAR('u') || args[0] == CHAR('U'))
			{
				result.hex_char = args[0];
				args            = args.substr(1U);
			}
		}

		static consteval char_format_args parse_args(lak::string_view<CHAR> args)
		{
			char_format_args result;
			_parse_args(result, args);
			if (!args.empty()) throw "invalid format arguments";
			return result;
		}
	};

	template<typename CHAR>
	struct string_format_args : public char_format_args<CHAR>
	{
		size_t precision = 0U; // .ddd

		static consteval string_format_args parse_args(lak::string_view<CHAR> args)
		{
			string_format_args result;

			char_format_args<CHAR>::_parse_args(result, args);

			if (args.empty()) return result;

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

			if (!args.empty()) throw "invalid format arguments";

			return result;
		}
	};
}

template<lak::concepts::one_of<LAK_ALL_CHARS> T, typename CHAR>
struct lak::format_traits<T, CHAR>
{
	using format_args = lak::char_format_args<CHAR>;

	static consteval format_args parse_args(lak::string_view<CHAR> args)
	{
		return format_args::parse_args(args);
	}

	static lak::string<CHAR> to_string(const format_args &args, const T &value)
	{
		bool escape =
		  ((args.escape_ascii_printable ^ args.escape_ascii_non_printable) &&
		   (args.escape_ascii_printable ^
		    !lak::is_ascii_printable(char32_t(value)))) ||
		  args.escape_ascii_printable /* = args.escape_ascii_non_printable */;

		if (escape) switch (args.hex_char)
			{
				case CHAR('x'):
					if constexpr (sizeof(T) == sizeof(char8_t))
						return lak::fmt<CHAR, "\\x{:0.2X}">(
						  uintmax_t(static_cast<uint8_t>(value)));
					else if constexpr (sizeof(T) == sizeof(char16_t))
						return lak::fmt<CHAR, "\\x{:0.4X}">(
						  uintmax_t(static_cast<uint16_t>(value)));
					else if constexpr (sizeof(T) == sizeof(char32_t))
						return lak::fmt<CHAR, "\\x{:0.8X}">(
						  uintmax_t(static_cast<uint32_t>(value)));
					else
						return lak::fmt<CHAR, "\\x{:0X}">(uintmax_t(value));
					break;
				case CHAR('X'):
					if constexpr (sizeof(T) == sizeof(char8_t))
						return lak::fmt<CHAR, "\\X{:0.2X}">(
						  uintmax_t(static_cast<uint8_t>(value)));
					else if constexpr (sizeof(T) == sizeof(char16_t))
						return lak::fmt<CHAR, "\\X{:0.4X}">(
						  uintmax_t(static_cast<uint16_t>(value)));
					else if constexpr (sizeof(T) == sizeof(char32_t))
						return lak::fmt<CHAR, "\\X{:0.8X}">(
						  uintmax_t(static_cast<uint32_t>(value)));
					else
						return lak::fmt<CHAR, "\\X{:0X}">(uintmax_t(value));
					break;
				case CHAR('u'):
					if constexpr (sizeof(T) == sizeof(char8_t))
						return lak::fmt<CHAR, "\\u{:0.2X}">(
						  uintmax_t(static_cast<uint8_t>(value)));
					else if constexpr (sizeof(T) == sizeof(char16_t))
						return lak::fmt<CHAR, "\\u{:0.4X}">(
						  uintmax_t(static_cast<uint16_t>(value)));
					else if constexpr (sizeof(T) == sizeof(char32_t))
						return lak::fmt<CHAR, "\\u{:0.8X}">(
						  uintmax_t(static_cast<uint32_t>(value)));
					else
						return lak::fmt<CHAR, "\\u{:0X}">(uintmax_t(value));
					break;
				case CHAR('U'):
					if constexpr (sizeof(T) == sizeof(char8_t))
						return lak::fmt<CHAR, "\\U{:0.2X}">(
						  uintmax_t(static_cast<uint8_t>(value)));
					else if constexpr (sizeof(T) == sizeof(char16_t))
						return lak::fmt<CHAR, "\\U{:0.4X}">(
						  uintmax_t(static_cast<uint16_t>(value)));
					else if constexpr (sizeof(T) == sizeof(char32_t))
						return lak::fmt<CHAR, "\\U{:0.8X}">(
						  uintmax_t(static_cast<uint32_t>(value)));
					else
						return lak::fmt<CHAR, "\\U{:0X}">(uintmax_t(value));
					break;
				default: BOUNDS_ASSERT_UNREACHABLE(); break;
			}
		else
			return lak::strconv<CHAR>(lak::string_view(&value, 1U));
	}
};
static_assert(!lak::concepts::static_formattable<wchar_t, char8_t>);
static_assert(lak::concepts::dynamic_formattable<wchar_t, char8_t>);
static_assert(lak::concepts::formattable<wchar_t, char8_t>);

template<typename T, typename CHAR>
struct lak::format_traits<lak::string_view<T>, CHAR>
{
	using format_args = lak::string_format_args<CHAR>;

	static consteval format_args parse_args(lak::string_view<CHAR> args)
	{
		return format_args::parse_args(args);
	}

	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::string_view<T> &value)
	{
		lak::string<CHAR> result;
		if (!(args.escape_ascii_printable || args.escape_ascii_non_printable))
			if constexpr (lak::is_same_v<T, CHAR>)
			{
				result.reserve(std::min<size_t>(value.size(), args.precision));
				result.insert(result.end(), value.begin(), value.end());
			}
			else
			{
				result = lak::strconv<CHAR>(value);
				result.reserve(args.precision);
			}
		else
		{
			result.reserve(std::min<size_t>(
			  args.escape_ascii_printable ? value.size() * (2U + (2U * sizeof(T)))
			                              : value.size(),
			  args.precision));
			for (const T &c : value)
				result += lak::format_traits<T, CHAR>::to_string(args, c);
		}
		if (result.size() < args.precision)
			result.append(args.precision - result.size(), CHAR(' '));
		return result;
	}
};
static_assert(lak::concepts::dynamic_formattable<lak::u8string_view, char>);
static_assert(
  lak::concepts::dynamic_formattable<lak::u16string_view, char32_t>);
static_assert(lak::concepts::dynamic_formattable<lak::u8string_view, char8_t>);
static_assert(!lak::concepts::static_formattable<lak::u8string_view, char8_t>);
static_assert(lak::concepts::formattable<lak::u8string_view, char8_t>);

template<typename C, typename CHAR>
struct lak::format_traits<lak::string<C>, CHAR>
: public lak::format_traits<lak::string_view<C>, CHAR>
{
	using typename lak::format_traits<lak::string_view<C>, CHAR>::format_args;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::string<C> &value)
	{
		return lak::format_traits<lak::string_view<C>, CHAR>::to_string(
		  args, lak::string_view<C>(value));
	}
};
static_assert(lak::concepts::dynamic_formattable<lak::u8string, char8_t>);
static_assert(!lak::concepts::static_formattable<lak::u8string, char8_t>);
static_assert(lak::concepts::formattable<lak::u8string, char8_t>);

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
			result.fill = '0';
			args        = args.substr(1U);
			if (args.empty()) return result;
		}

		if (args[0] >= CHAR('1') && args[0] <= CHAR('9'))
		{
			result.min_width = args[0] - CHAR('0');
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

		lak::astring prefix;

		if (std::signbit(value))
			prefix += "-";
		else if (args.force_sign)
			prefix += "+";
		else if (args.sign_pad)
			prefix += " ";

		if (args.show_base)
		{
			switch (args.base)
			{
				case lak::numeric_base::dec: break;
				case lak::numeric_base::hex: prefix += "0x"; break;
				case lak::numeric_base::oct: prefix += "0"; break;
				case lak::numeric_base::bin: prefix += "0b"; break;
			}
		}

		if (args.uppercase) strm << std::uppercase;
		strm << std::noshowbase << std::fixed << std::setprecision(args.precision)
		     << (std::signbit(value) ? -value : value);

		const auto view = lak::string_view<char>(strm.view());

		if (const size_t width = view.size() + prefix.size();
		    args.min_width < width)
			return lak::fmt<CHAR, "{}{}">(prefix, view);
		else if (const auto fill =
		           lak::astring(size_t(args.min_width - width), args.fill);
		         args.left_justified)
			return lak::fmt<CHAR, "{}{}{}">(prefix, view, fill);
		else if (args.fill == '0')
			return lak::fmt<CHAR, "{}{}{}">(prefix, fill, view);
		else
			return lak::fmt<CHAR, "{}{}{}">(fill, prefix, view);
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
