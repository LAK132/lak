#include "lak/unicode.hpp"
#include "lak/debug.hpp"

/* --- string_length --- */

size_t lak::string_length(const char *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}

size_t lak::string_length(const wchar_t *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}

size_t lak::string_length(const char8_t *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}

size_t lak::string_length(const char16_t *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}

size_t lak::string_length(const char32_t *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}

/* --- character_length --- */

uint8_t lak::character_length(lak::span<const char> str)
{
  if (str.size() >= 1 && str[0] <= 0x7F)
  {
    return 1;
  }
  else
  {
    if (str.size()) WARNING("Bad encoding (", (size_t)str[0], ")");
    return 0;
  }
}

uint8_t lak::character_length(lak::span<const wchar_t> str)
{
  return lak::character_length(lak::span<const wchar_unicode_t>(str));
}

uint8_t lak::character_length(lak::span<const char8_t> str)
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
    if (str.size()) WARNING("Bad encoding (", (size_t)str[0], ")");
    return 0;
  }
}

uint8_t lak::character_length(lak::span<const char16_t> str)
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
    if (str.size()) WARNING("Bad encoding (", (size_t)str[0], ")");
    return 0;
  }
}

uint8_t lak::character_length(lak::span<const char32_t> str)
{
  if (str.size() >= 1 && str[0] < 0x110000 &&
      (str[0] & 0b1111'1000'0000'0000U) != 0b1101'1000'0000'0000U)
  {
    return 1;
  }
  else
  {
    if (str.size()) WARNING("Bad encoding (", (size_t)str[0], ")");
    return 0;
  }
}

/* --- codepoint --- */

char32_t lak::codepoint(lak::span<const char> str)
{
  if (str.size() >= 1 && str[0] < 0x80)
  {
    return str[0];
  }
  else
  {
    if (str.size()) WARNING("Bad encoding (", (size_t)str[0], ")");
    return 0;
  }
}

char32_t lak::codepoint(lak::span<const wchar_t> str)
{
  return lak::codepoint(lak::span<const wchar_unicode_t>(str));
}

char32_t lak::codepoint(lak::span<const char8_t> str)
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
    if (str.size()) WARNING("Bad encoding (", (size_t)str[0], ")");
    return 0;
  }
}

char32_t lak::codepoint(lak::span<const char16_t> str)
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
    if (str.size()) WARNING("Bad encoding (", (size_t)str[0], ")");
    return 0;
  }
}

char32_t lak::codepoint(lak::span<const char32_t> str)
{
  if (str.size() >= 1 && str[0] < 0x110000 &&
      (str[0] & 0b1111'1000'0000'0000U) != 0b1101'1000'0000'0000U)
  {
    return str[0];
  }
  else
  {
    if (str.size()) WARNING("Bad encoding (", (size_t)str[0], ")");
    return 0;
  }
}

/* --- from_codepoint --- */

lak::span<char> lak::from_codepoint(lak::codepoint_buffer_t<char> c,
                                    char32_t code)
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

lak::span<wchar_t> lak::from_codepoint(lak::codepoint_buffer_t<wchar_t> c,
                                       char32_t code)
{
  return lak::span<wchar_t>(
    lak::from_codepoint(lak::codepoint_buffer_t<wchar_unicode_t>(c), code));
}

lak::span<char8_t> lak::from_codepoint(lak::codepoint_buffer_t<char8_t> c,
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
    WARNING("Bad encoding (", (size_t)code, ")");
    c[0] = c[1] = c[2] = c[3] = u8'\0';
    return c.first(0);
  }
}

lak::span<char16_t> lak::from_codepoint(lak::codepoint_buffer_t<char16_t> c,
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
    WARNING("Bad encoding (", (size_t)code, ")");
    c[0] = c[1] = u'\0';
    return c.first(0);
  }
}

lak::span<char32_t> lak::from_codepoint(lak::codepoint_buffer_t<char32_t> c,
                                        char32_t code)
{
  if (code <= 0xD7FFU || (code >= 0xE000U && code <= 0x0010FFFFU))
  {
    c[0] = code;
    return c;
  }
  else
  {
    WARNING("Bad encoding (", (size_t)code, ")");
    c[0] = U'\0';
    return c.first(0);
  }
}

std::ostream &operator<<(std::ostream &strm, const lak::span<const char> &str)
{
  return strm << std::string_view(str.data(), str.size());
}

std::ostream &operator<<(std::ostream &strm,
                         const lak::span<const wchar_t> &str)
{
  return strm << lak::as_astring(lak::to_u8string(str));
}

std::ostream &operator<<(std::ostream &strm,
                         const lak::span<const char8_t> &str)
{
  return strm << lak::as_astring(str);
}

std::ostream &operator<<(std::ostream &strm,
                         const lak::span<const char16_t> &str)
{
  return strm << lak::as_astring(lak::to_u8string(str));
}

std::ostream &operator<<(std::ostream &strm,
                         const lak::span<const char32_t> &str)
{
  return strm << lak::as_astring(lak::to_u8string(str));
}
