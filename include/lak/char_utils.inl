#include "lak/char_utils.hpp"

#include "lak/span_manip.hpp"

namespace
{
	namespace impl
	{
		constexpr char8_t alphanum[] = {
		  u8'0',
		  u8'1',
		  u8'2',
		  u8'3',
		  u8'4',
		  u8'5',
		  u8'6',
		  u8'7',
		  u8'8',
		  u8'9',
		};

		constexpr char8_t hex_alphanum[] = {
		  u8'0', u8'1', u8'2', u8'3', u8'4', u8'5', u8'6', u8'7',
		  u8'8', u8'9', u8'A', u8'B', u8'C', u8'D', u8'E', u8'F',
		  u8'a', u8'b', u8'c', u8'd', u8'e', u8'f',
		};

		constexpr char32_t control_codes[0x41] = {
		  U'\u0000', // null
		  U'\u0001', // start of heading
		  U'\u0002', // start of text
		  U'\u0003', // end of text
		  U'\u0004', // end of transmission
		  U'\u0005', // enquiry
		  U'\u0006', // acknowledge
		  U'\u0007', // bell/alert
		  U'\u0008', // backspace
		  U'\u0009', // horizontal tab
		  U'\u000A', // line feed
		  U'\u000B', // vertical tab
		  U'\u000C', // form feed
		  U'\u000D', // carriage return
		  U'\u000E', // shift out
		  U'\u000F', // shift in
		  U'\u0010', // data link escape
		  U'\u0011', // device control one (XON)
		  U'\u0012', // device control two
		  U'\u0013', // device control three (XOFF)
		  U'\u0014', // device control four
		  U'\u0015', // negative acknowledge
		  U'\u0016', // synchronour idle
		  U'\u0017', // end of transmission block
		  U'\u0018', // cancel
		  U'\u0019', // end of medium
		  U'\u001A', // substitute
		  U'\u001B', // escape
		  U'\u001C', // file separator
		  U'\u001D', // group separator
		  U'\u001E', // record separator
		  U'\u001F', // unit separator
		  U'\u007F', // delete
		  U'\u0080', // padding character
		  U'\u0081', // high octet preset
		  U'\u0082', // break permitted here
		  U'\u0083', // no break here
		  U'\u0084', // index
		  U'\u0085', // next line
		  U'\u0086', // start of selected area
		  U'\u0087', // end of selected area
		  U'\u0088', // horizontal tab set
		  U'\u0089', // horizontal tabl with justification
		  U'\u008A', // vertical tab set
		  U'\u008B', // partial line down
		  U'\u008C', // partial line up
		  U'\u008D', // reverse line feed
		  U'\u008E', // single-shift 2
		  U'\u008F', // single-shift 3
		  U'\u0090', // device control string
		  U'\u0091', // private use 1
		  U'\u0092', // pricate use 2
		  U'\u0093', // set transmit state
		  U'\u0094', // cancel character
		  U'\u0095', // message waiting
		  U'\u0096', // start of protected area
		  U'\u0097', // end of protected area
		  U'\u0098', // start of string
		  U'\u0099', // single graphic character introducer
		  U'\u009A', // single character introducer
		  U'\u009B', // control sequence introducer
		  U'\u009C', // string terminator
		  U'\u009D', // operator system command
		  U'\u009E', // privary message
		  U'\u009F', // application program command
		};

		constexpr char8_t ascii_spaces[] = {
		  u8'\x09', // tab
		  u8'\x0A', // line feed
		  u8'\x0B', // line tab
		  u8'\x0C', // form feed
		  u8'\x0D', // carriage return
		  u8'\x20', // space
		};

		constexpr char32_t spaces[25] = {
		  U'\u0009', // tab
		  U'\u000A', // line feed
		  U'\u000B', // line tab
		  U'\u000C', // form feed
		  U'\u000D', // carriage return
		  U'\u0020', // space
		  U'\u0085', // next line
		  U'\u00A0', // no-break space
		  U'\u1680', // ogham space mark
		  U'\u2000', // en quad
		  U'\u2001', // em quad
		  U'\u2002', // en space
		  U'\u2003', // em space
		  U'\u2004', // three-per-em space
		  U'\u2005', // four-per-em space
		  U'\u2006', // six-per-em space
		  U'\u2007', // figure space
		  U'\u2008', // punctuation space
		  U'\u2009', // thin space
		  U'\u200A', // hair space
		  U'\u2028', // line separator
		  U'\u2029', // paragraph separator
		  U'\u202F', // narrow no-break space
		  U'\u205F', // medium mathematical space
		  U'\u3000', // ideographic space
		};
	}
}

/* --- alphanumeric --- */

inline constexpr lak::span<const char8_t> lak::alphanumerics()
{
	return lak::span(impl::alphanum);
}

inline constexpr bool lak::is_alphanumeric(char8_t c)
{
	return c >= u8'0' && c <= u8'9';
}

inline lak::result<uint8_t> lak::from_alphanumeric(char8_t c)
{
	if (lak::is_alphanumeric(c))
		return lak::ok_t{uint8_t(c - u8'0')};
	else
		return lak::err_t{};
}

/* --- hex_alphanumeric --- */

inline constexpr lak::span<const char8_t> lak::hex_alphanumerics()
{
	return lak::span(impl::hex_alphanum);
}

inline constexpr bool lak::is_hex_alphanumeric(char8_t c)
{
	return (c >= u8'0' && c <= u8'9') || (c >= u8'A' && c <= u8'F') ||
	       (c >= u8'a' && c <= u8'f');
}

inline lak::result<uint8_t> lak::from_hex_alphanumeric(char8_t c)
{
	if (c >= u8'0' && c <= u8'9')
		return lak::ok_t{uint8_t(c - u8'0')};
	else if (c >= u8'A' && c <= u8'F')
		return lak::ok_t{uint8_t((c - u8'A') + 0xA)};
	else if (c >= u8'a' && c <= u8'f')
		return lak::ok_t{uint8_t((c - u8'a') + 0xA)};
	else
		return lak::err_t{};
}

/* --- control_codes --- */

inline constexpr lak::span<const char32_t> lak::control_codes()
{
	return lak::span(impl::control_codes);
}

inline constexpr bool lak::is_control_code(char32_t c)
{
	return (c <= U'\u001F') || (c >= U'\u007F' && c <= U'\u009F');
}

/* --- whitespace --- */

inline constexpr lak::span<const char32_t> lak::whitespaces()
{
	return lak::span(impl::spaces);
}

inline constexpr bool lak::is_whitespace(char32_t c)
{
	return lak::contains(lak::whitespaces(), c);
}

/* --- ascii_whitespace --- */

inline constexpr lak::span<const char8_t> lak::ascii_whitespaces()
{
	return lak::span(impl::ascii_spaces);
}

inline constexpr bool lak::is_ascii_whitespace(char8_t c)
{
	return lak::contains(lak::ascii_whitespaces(), c);
}
