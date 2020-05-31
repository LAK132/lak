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

  uint8_t character_length(const std::string &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return character_length(
      span<const char>(str.c_str() + offset, str.size() - offset));
  }

  uint8_t character_length(const std::wstring &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return character_length(
      span<const wchar_t>(str.c_str() + offset, str.size() - offset));
  }

  uint8_t character_length(const std::u8string &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return character_length(
      span<const char8_t>(str.c_str() + offset, str.size() - offset));
  }

  uint8_t character_length(const std::u16string &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return character_length(
      span<const char16_t>(str.c_str() + offset, str.size() - offset));
  }

  uint8_t character_length(const std::u32string &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return character_length(
      span<const char32_t>(str.c_str() + offset, str.size() - offset));
  }

  uint8_t character_length(span<const char> str)
  {
    if (str.size() >= 1 && str[0] <= 0x7F)
    {
      return 1;
    }
    else
    {
      if (str.size()) FATAL("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  uint8_t character_length(span<const wchar_t> str)
  {
    static_assert(sizeof(wchar_t) == sizeof(char8_t) ||
                    sizeof(wchar_t) == sizeof(char16_t) ||
                    sizeof(wchar_t) == sizeof(char32_t),
                  "wchar_t is not a known size.");

    if constexpr (sizeof(wchar_t) == sizeof(char8_t))
      return character_length(
        {reinterpret_cast<const char8_t *>(str.begin()), str.size()});
    else if constexpr (sizeof(wchar_t) == sizeof(char16_t))
      return character_length(
        {reinterpret_cast<const char16_t *>(str.begin()), str.size()});
    else
      return character_length(
        {reinterpret_cast<const char32_t *>(str.begin()), str.size()});
  }

  uint8_t character_length(span<const char8_t> str)
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
      if (str.size()) FATAL("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  uint8_t character_length(span<const char16_t> str)
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
      if (str.size()) FATAL("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  uint8_t character_length(span<const char32_t> str)
  {
    if (str.size() >= 1 && str[0] < 0x110000 &&
        (str[0] & 0b1111'1000'0000'0000U) != 0b1101'1000'0000'0000U)
    {
      return 1;
    }
    else
    {
      if (str.size()) FATAL("Bad encoding (" << (size_t)str[0] << ")");
      return 0;
    }
  }

  /* --- codepoint --- */

  char32_t codepoint(const std::string &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return codepoint(
      span<const char>(str.c_str() + offset, str.size() - offset));
  }

  char32_t codepoint(const std::wstring &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return codepoint(
      span<const wchar_t>(str.c_str() + offset, str.size() - offset));
  }

  char32_t codepoint(const std::u8string &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return codepoint(
      span<const char8_t>(str.c_str() + offset, str.size() - offset));
  }

  char32_t codepoint(const std::u16string &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return codepoint(
      span<const char16_t>(str.c_str() + offset, str.size() - offset));
  }

  char32_t codepoint(const std::u32string &str, size_t offset)
  {
    ASSERT(offset < str.size());
    return codepoint(
      span<const char32_t>(str.c_str() + offset, str.size() - offset));
  }

  char32_t codepoint(span<const char> str)
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

  char32_t codepoint(span<const wchar_t> str)
  {
    static_assert(sizeof(wchar_t) == sizeof(char8_t) ||
                    sizeof(wchar_t) == sizeof(char16_t) ||
                    sizeof(wchar_t) == sizeof(char32_t),
                  "wchar_t is not a known size.");

    if constexpr (sizeof(wchar_t) == sizeof(char8_t))
      return codepoint(
        {reinterpret_cast<const char8_t *>(str.begin()), str.size()});
    else if constexpr (sizeof(wchar_t) == sizeof(char16_t))
      return codepoint(
        {reinterpret_cast<const char16_t *>(str.begin()), str.size()});
    else
      return codepoint(
        {reinterpret_cast<const char32_t *>(str.begin()), str.size()});
  }

  char32_t codepoint(span<const char8_t> str)
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

  char32_t codepoint(span<const char16_t> str)
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

  char32_t codepoint(span<const char32_t> str)
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

  /* --- append_codepoint --- */

  void append_codepoint(std::string &str, const char32_t code)
  {
    if (code <= 0x7F) str += static_cast<char>(code);
  }

  void append_codepoint(std::wstring &str, const char32_t code)
  {
    static_assert(sizeof(wchar_t) == sizeof(char8_t) ||
                    sizeof(wchar_t) == sizeof(char16_t) ||
                    sizeof(wchar_t) == sizeof(char32_t),
                  "wchar_t is not a known size.");

    if constexpr (sizeof(wchar_t) == sizeof(char8_t))
    {
      if (code <= 0x7FU)
      {
        str += static_cast<wchar_t>(code);
      }
      else if (code >= 0x80U && code <= 0x7FFU)
      {
        str += 0xC0U | static_cast<wchar_t>(code >> 6);
        str += 0x80U | static_cast<wchar_t>(code & 0x3FU);
      }
      else if (code >= 0x800U && code <= 0xFFFFU)
      {
        str += 0xE0U | static_cast<wchar_t>(code >> 12);
        str += 0x80U | static_cast<wchar_t>((code >> 6) & 0x3FU);
        str += 0x80U | static_cast<wchar_t>(code & 0x3FU);
      }
      else if (code >= 0x10000U && code <= 0x10FFFFU)
      {
        str += 0xF0U | static_cast<wchar_t>(code >> 18);
        str += 0x80U | static_cast<wchar_t>((code >> 12) & 0x3FU);
        str += 0x80U | static_cast<wchar_t>((code >> 6) & 0x3FU);
        str += 0x80U | static_cast<wchar_t>(code & 0x3FU);
      }
    }
    else if constexpr (sizeof(wchar_t) == sizeof(char16_t))
    {
      if (code <= 0xFFFFU)
      {
        str += static_cast<wchar_t>(code);
      }
      else
      {
        const auto _code = code - 0x010000U;
        str += static_cast<wchar_t>(0xD800U | ((_code >> 10) & 0x3FFU));
        str += static_cast<wchar_t>(0xDC00U | (_code & 0x3FFU));
      }
    }
    else
    {
      str += static_cast<wchar_t>(code);
    }
  }

  void append_codepoint(std::u8string &str, const char32_t code)
  {
    if (code <= 0x7FU)
    {
      str += static_cast<char8_t>(code);
    }
    else if (code >= 0x80U && code <= 0x7FFU)
    {
      str += 0xC0U | static_cast<char8_t>(code >> 6);
      str += 0x80U | static_cast<char8_t>(code & 0x3FU);
    }
    else if (code >= 0x800U && code <= 0xFFFFU)
    {
      str += 0xE0U | static_cast<char8_t>(code >> 12);
      str += 0x80U | static_cast<char8_t>((code >> 6) & 0x3FU);
      str += 0x80U | static_cast<char8_t>(code & 0x3FU);
    }
    else if (code >= 0x10000U && code <= 0x10FFFFU)
    {
      str += 0xF0U | static_cast<char8_t>(code >> 18);
      str += 0x80U | static_cast<char8_t>((code >> 12) & 0x3FU);
      str += 0x80U | static_cast<char8_t>((code >> 6) & 0x3FU);
      str += 0x80U | static_cast<char8_t>(code & 0x3FU);
    }
  }

  void append_codepoint(std::u16string &str, const char32_t code)
  {
    if (code <= 0xFFFFU)
    {
      str += static_cast<char16_t>(code);
    }
    else
    {
      const auto _code = code - 0x01000U;
      str += static_cast<char16_t>(0xD800U | ((_code >> 10) & 0x3FFU));
      str += static_cast<char16_t>(0xDC00U | (_code & 0x3FFU));
    }
  }

  void append_codepoint(std::u32string &str, const char32_t code)
  {
    str += code;
  }
}