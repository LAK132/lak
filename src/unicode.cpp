#include "lak/unicode.hpp"
#include "lak/debug.hpp"

namespace lak
{
  /* --- string_length --- */

  size_t string_length(const char *str)
  {
    size_t length = 0;
    while (*(str++) != 0) ++length;
    return length;
  }

  size_t string_length(const wchar_t *str)
  {
    size_t length = 0;
    while (*(str++) != 0) ++length;
    return length;
  }

  size_t string_length(const char8_t *str)
  {
    size_t length = 0;
    while (*(str++) != 0) ++length;
    return length;
  }

  size_t string_length(const char16_t *str)
  {
    size_t length = 0;
    while (*(str++) != 0) ++length;
    return length;
  }

  size_t string_length(const char32_t *str)
  {
    size_t length = 0;
    while (*(str++) != 0) ++length;
    return length;
  }

  /* --- character_length --- */

  uint8_t character_length(lak::span<const char> str)
  {
    if (str.size() >= 1 && str[0] <= 0x7F)
    {
      return 1;
    }
    else
    {
      if (str.size()) WARNING("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  uint8_t character_length(lak::span<const wchar_t> str)
  {
    return lak::character_length(lak::span<const wchar_unicode_t>(str));
  }

  uint8_t character_length(lak::span<const char8_t> str)
  {
    if (str.size() >= 1 && (str[0] & 0b1000'0000U) == 0b0000'0000U)
    {
      return 1;
    }
    else if (str.size() >= 2 && (str[0] & 0b1110'0000U) == 0b1100'0000U &&
             (str[1] & 0b1100'0000U) == 0b1000'0000U)
    {
      return 2;
    }
    else if (str.size() >= 3 && (str[0] & 0b1111'0000U) == 0b1110'0000U &&
             (str[1] & 0b1100'0000U) == 0b1000'0000U &&
             (str[2] & 0b1100'0000U) == 0b1000'0000U)
    {
      return 3;
    }
    else if (str.size() >= 4 && (str[0] & 0b1111'1000U) == 0b1111'0000U &&
             (str[1] & 0b1100'0000U) == 0b1000'0000U &&
             (str[2] & 0b1100'0000U) == 0b1000'0000U &&
             (str[3] & 0b1100'0000U) == 0b1000'0000U)
    {
      return 4;
    }
    else
    {
      if (str.size()) WARNING("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  uint8_t character_length(lak::span<const char16_t> str)
  {
    if (str.size() >= 1 &&
        (str[0] & 0b1111'1000'0000'0000U) != 0b1101'1000'0000'0000U)
    {
      return 1;
    }
    else if (str.size() >= 2 &&
             (str[0] & 0b1111'1100'0000'0000U) == 0b1101'1000'0000'0000U &&
             (str[1] & 0b1111'1100'0000'0000U) == 0b1101'1100'0000'0000U)
    {
      return 2;
    }
    else
    {
      if (str.size()) WARNING("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  uint8_t character_length(lak::span<const char32_t> str)
  {
    if (str.size() >= 1 && str[0] < 0x110000 &&
        (str[0] & 0b1111'1000'0000'0000U) != 0b1101'1000'0000'0000U)
    {
      return 1;
    }
    else
    {
      if (str.size()) WARNING("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  /* --- codepoint --- */

  char32_t codepoint(lak::span<const char> str)
  {
    if (str.size() >= 1 && str[0] < 0x80)
    {
      return str[0];
    }
    else
    {
      if (str.size()) FATAL("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  char32_t codepoint(lak::span<const wchar_t> str)
  {
    return lak::codepoint(lak::span<const wchar_unicode_t>(str));
  }

  char32_t codepoint(lak::span<const char8_t> str)
  {
    if (str.size() >= 1 &&
        (str[0] < 0x80 || (str[0] & 0b1100'0000U) == 0b0100'0000U))
    {
      return str[0];
    }
    else if (str.size() >= 2 && (str[0] & 0b1110'0000U) == 0b1100'0000U &&
             (str[1] & 0b1100'0000U) == 0b1000'0000U)
    {
      return ((static_cast<char32_t>(str[0]) - 0xC0UL) << 6) |
             ((static_cast<char32_t>(str[1]) - 0x80UL) << 0);
    }
    else if (str.size() >= 3 && (str[0] & 0b1111'0000U) == 0b1110'0000U &&
             (str[1] & 0b1100'0000U) == 0b1000'0000U &&
             (str[2] & 0b1100'0000U) == 0b1000'0000U)
    {
      return ((static_cast<char32_t>(str[0]) - 0xE0UL) << 12) |
             ((static_cast<char32_t>(str[1]) - 0x80UL) << 6) |
             ((static_cast<char32_t>(str[2]) - 0x80UL) << 0);
    }
    else if (str.size() >= 4 && (str[0] & 0b1111'1000U) == 0b1111'0000U &&
             (str[1] & 0b1100'0000U) == 0b1000'0000U &&
             (str[2] & 0b1100'0000U) == 0b1000'0000U &&
             (str[3] & 0b1100'0000U) == 0b1000'0000U)
    {
      return ((static_cast<char32_t>(str[0]) - 0xF0UL) << 18) |
             ((static_cast<char32_t>(str[1]) - 0x80UL) << 12) |
             ((static_cast<char32_t>(str[2]) - 0x80UL) << 6) |
             ((static_cast<char32_t>(str[3]) - 0x80UL) << 0);
    }
    else
    {
      if (str.size()) FATAL("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  char32_t codepoint(lak::span<const char16_t> str)
  {
    if (str.size() >= 1 &&
        (str[0] & 0b1111'1000'0000'0000U) != 0b1101'1000'0000'0000U)
    {
      return str[0];
    }
    else if (str.size() >= 2 &&
             (str[0] & 0b1111'1100'0000'0000U) == 0b1101'1000'0000'0000U &&
             (str[1] & 0b1111'1100'0000'0000U) == 0b1101'1100'0000'0000U)
    {
      return 0x010000U +
             (((static_cast<char32_t>(str[0]) & 0b11'1111'1111) << 10) |
              ((static_cast<char32_t>(str[1]) & 0b11'1111'1111) << 0));
    }
    else
    {
      if (str.size()) FATAL("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  char32_t codepoint(lak::span<const char32_t> str)
  {
    if (str.size() >= 1 && str[0] < 0x110000 &&
        (str[0] & 0b1111'1000'0000'0000U) != 0b1101'1000'0000'0000U)
    {
      return str[0];
    }
    else
    {
      if (str.size()) FATAL("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  /* --- from_codepoint --- */

  lak::span<char> from_codepoint(lak::codepoint_buffer<char> c, char32_t code)
  {
    if (code <= 0x7FU)
    {
      c[0] = static_cast<char>(code);
      return c;
    }
    else
    {
      // Codepoint is outside the ASCII range
      c[0] = '\0';
      return c.first(0);
    }
  }

  lak::span<wchar_t> from_codepoint(lak::codepoint_buffer<wchar_t> c,
                                    char32_t code)
  {
    return lak::span<wchar_t>(
      lak::from_codepoint(lak::codepoint_buffer<wchar_unicode_t>(c), code));
  }

  lak::span<char8_t> from_codepoint(lak::codepoint_buffer<char8_t> c,
                                    char32_t code)
  {
    if (code <= 0x7FU)
    {
      c[0] = static_cast<char8_t>(code);
      c[1] = c[2] = c[3] = u8'\0';
      return c.first(1);
    }
    else if (code >= 0x80U && code <= 0x07FFU)
    {
      c[0] = 0xC0U | static_cast<char8_t>(code >> 6);
      c[1] = 0x80U | static_cast<char8_t>(code & 0x3FU);
      c[2] = c[3] = u8'\0';
      return c.first(2);
    }
    else if (code >= 0x0800U && code <= 0xFFFFU)
    {
      c[0] = 0xE0U | static_cast<char8_t>(code >> 12);
      c[1] = 0x80U | static_cast<char8_t>((code >> 6) & 0x3FU);
      c[2] = 0x80U | static_cast<char8_t>(code & 0x3FU);
      c[3] = u8'\0';
      return c.first(3);
    }
    else if (code >= 0x00010000U && code <= 0x00010FFFFU)
    {
      c[0] = 0xF0U | static_cast<char8_t>(code >> 18);
      c[1] = 0x80U | static_cast<char8_t>((code >> 12) & 0x3FU);
      c[2] = 0x80U | static_cast<char8_t>((code >> 6) & 0x3FU);
      c[3] = 0x80U | static_cast<char8_t>(code & 0x3FU);
      return c;
    }
    else
    {
      // Invalid codepoint
      FATAL("Bad encoding (" << (size_t)code << ")");
      c[0] = c[1] = c[2] = c[3] = u8'\0';
      return c.first(0);
    }
  }

  lak::span<char16_t> from_codepoint(lak::codepoint_buffer<char16_t> c,
                                     char32_t code)
  {
    if (code <= 0xD7FF || (code >= 0xE000 && code <= 0xFFFFU))
    {
      c[0] = static_cast<char16_t>(code);
      c[1] = u'\0';
      return c.first(1);
    }
    else if (code <= 0x0010FFFF)
    {
      const char32_t _code = code - 0x00010000U;
      c[0] = static_cast<char16_t>(0xD800U | ((_code >> 10) & 0x03FFU));
      c[1] = static_cast<char16_t>(0xDC00U | (_code & 0x03FFU));
      return c;
    }
    else
    {
      // Invalid codepoint
      FATAL("Bad encoding (" << (size_t)code << ")");
      c[0] = c[1] = u'\0';
      return c.first(0);
    }
  }

  lak::span<char32_t> from_codepoint(lak::codepoint_buffer<char32_t> c,
                                     char32_t code)
  {
    c[0] = code;
    return c;
  }

  bool is_whitespace(char32_t c)
  {
    return c == U'\u0009'    // tab
           || c == U'\u000A' // line feed
           || c == U'\u000B' // line tab
           || c == U'\u000C' // form feed
           || c == U'\u000D' // carriage return
           || c == U'\u0020' // space
           || c == U'\u0085' // next line
           || c == U'\u00A0' // no-break space
           || c == U'\u1680' // ogham space mark
           || c == U'\u2000' // en quad
           || c == U'\u2001' // em quad
           || c == U'\u2002' // en space
           || c == U'\u2003' // em space
           || c == U'\u2004' // three-per-em space
           || c == U'\u2005' // four-per-em space
           || c == U'\u2006' // six-per-em space
           || c == U'\u2007' // figure space
           || c == U'\u2008' // punctuation space
           || c == U'\u2009' // thin space
           || c == U'\u200A' // hair space
           || c == U'\u2028' // line separator
           || c == U'\u2029' // paragraph separator
           || c == U'\u202F' // narrow no-break space
           || c == U'\u205F' // medium mathematical space
           || c == U'\u3000' // ideographic space
      ;
  }
}

std::ostream &operator<<(std::ostream &strm, lak::span<const char> str)
{
  for (const auto &c : str) strm << c;
  return strm;
}

std::ostream &operator<<(std::ostream &strm, lak::span<const wchar_t> str)
{
  for (const auto &c : str) strm << c;
  return strm;
}

std::ostream &operator<<(std::ostream &strm, lak::span<const char8_t> str)
{
#if __cplusplus <= 201703L
  for (const auto &c : str) strm << static_cast<char>(c);
#else
  for (const auto &c : str) strm << c;
#endif
  return strm;
}

std::ostream &operator<<(std::ostream &strm, lak::span<const char16_t> str)
{
  for (const auto &c : str) strm << c;
  return strm;
}

std::ostream &operator<<(std::ostream &strm, lak::span<const char32_t> str)
{
  for (const auto &c : str) strm << c;
  return strm;
}
