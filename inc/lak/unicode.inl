#include "lak/strcast.hpp"
#include "lak/strconv.hpp"
#include "lak/streamify.hpp"

template<typename TO, typename FROM>
size_t lak::converted_string_length(lak::span<FROM> str)
{
  size_t result = 0;
  while (str.size() > 0)
  {
    auto char_len = lak::character_length(str);
    ASSERT_NOT_EQUAL(char_len, 0);
    result += lak::codepoint_length<TO>(lak::codepoint(str));
    str = str.subspan(char_len);
  }
  return result;
}

template<typename CHAR>
lak::span<CHAR> lak::string_view(CHAR *str)
{
  return {str, lak::string_length(str)};
}

template<typename CHAR>
lak::span<const CHAR> lak::string_view(const lak::string<CHAR> &str)
{
  return {str.c_str(), str.length()};
}

template<typename CHAR>
uint8_t lak::character_length(const lak::string<CHAR> &str, size_t offset)
{
  return lak::character_length(lak::string_view(str).subspan(offset));
}

template<typename CHAR>
char32_t lak::codepoint(const lak::string<CHAR> &str, size_t offset)
{
  return lak::codepoint(lak::string_view(str).subspan(offset));
}

template<typename CHAR>
uint8_t lak::codepoint_length(char32_t code)
{
  if constexpr (std::is_same_v<CHAR, char>)
  {
    return code <= 0x7FU ? 1 : 0;
  }
  else if constexpr (std::is_same_v<CHAR, wchar_t>)
  {
    return lak::codepoint_length<wchar_unicode_t>(code);
  }
  else if constexpr (std::is_same_v<CHAR, char8_t>)
  {
    if (code <= 0x7FU)
      return 1;
    else if (code >= 0x80U && code <= 0x07FFU)
      return 2;
    else if (code >= 0x0800U && code <= 0xFFFFU)
      return 3;
    else if (code >= 0x00010000U && code <= 0x0010FFFFU)
      return 4;
    else
      return 0;
  }
  else if constexpr (std::is_same_v<CHAR, char16_t>)
  {
    if (code <= 0xD7FFU || (code >= 0xE000U && code <= 0xFFFFU))
      return 1;
    else if (code <= 0x0010FFFFU)
      return 2;
    else
      return 0;
  }
  else if constexpr (std::is_same_v<CHAR, char32_t>)
  {
    return (code <= 0xD7FFU || (code >= 0xE000U && code <= 0x0010FFFFU)) ? 1
                                                                         : 0;
  }
}

template<typename CHAR>
void lak::append_codepoint(lak::string<CHAR> &str, char32_t code)
{
  constexpr size_t buffer_size = lak::chars_per_codepoint_v<CHAR>;
  CHAR buffer[buffer_size + 1] = {};
  lak::from_codepoint(lak::codepoint_buffer(lak::span(buffer)), code);
  str += buffer;
}

namespace
{
  namespace impl
  {
    constexpr char32_t hex_alphanum[] = {
      U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9', U'A',
      U'B', U'C', U'D', U'E', U'F', U'a', U'b', U'c', U'd', U'e', U'f'};

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
      U'\u009F'  // application program command
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
      U'\u3000'  // ideographic space
    };
  }
}

inline constexpr lak::span<const char32_t> lak::hex_alphanumerics()
{
  return lak::span(impl::spaces);
}

inline constexpr bool lak::is_hex_alphanumeric(char32_t c)
{
  return (c >= U'0' && c <= U'9') || (c >= U'A' && c <= U'F') ||
         (c >= U'a' && c <= U'f');
}

inline constexpr lak::span<const char32_t> lak::control_codes()
{
  return lak::span(impl::spaces);
}

inline constexpr bool lak::is_control_code(char32_t c)
{
  return (c <= U'\u001F') || (c >= U'\u007F' && c <= U'\u009F');
}

inline constexpr lak::span<const char32_t> lak::whitespaces()
{
  return lak::span(impl::spaces);
}

inline constexpr bool lak::is_whitespace(char32_t c)
{
  return lak::contains(lak::whitespaces(), c);
}

template<typename CHAR>
inline constexpr const lak::pair<char32_t, uint8_t>
  &lak::codepoint_iterator<CHAR>::operator*() const noexcept
{
  return _current;
}

template<typename CHAR>
inline constexpr lak::codepoint_iterator<CHAR>
  &lak::codepoint_iterator<CHAR>::operator++() noexcept
{
  _current.second = lak::character_length(_data);
  _current.first  = _current.second ? codepoint(_data) : 0;
  _data           = _data.subspan(_current.second);
  return *this;
}

template<typename CHAR>
inline constexpr bool lak::codepoint_iterator<CHAR>::operator==(
  char32_t c) const noexcept
{
  return _current.first == c;
}

template<typename CHAR>
inline constexpr bool lak::codepoint_iterator<CHAR>::operator!=(
  char32_t c) const noexcept
{
  return _current.first != c;
}

template<typename CHAR>
inline constexpr lak::codepoint_iterator<CHAR>
lak::codepoint_range<CHAR>::begin() const noexcept
{
  return {_data};
}

template<typename CHAR>
inline constexpr char32_t lak::codepoint_range<CHAR>::end() const noexcept
{
  return 0;
}
