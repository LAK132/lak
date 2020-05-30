#ifndef LAK_STRING_HPP
#define LAK_STRING_HPP

// This library assumes that char only holds 7bit ASCII characters, and that
// wchar_t will hold Unicode characters. wchar_t encoding is determined based
// on its size.

#include "span.hpp"
#include "stdint.hpp"

#include <string>

// char8_t typdef for C++ < 20
#if __cplusplus <= 201703L
using char8_t = uint_least8_t;
namespace std
{
  using u8string = std::basic_string<char8_t>;
}
#endif

namespace lak
{
  // Returns the length of the multi-byte first/offset character (always 1 for
  // non-multi-byte string types). Returns 0 if character has a bad encoding
  // or the string is empty.
  uint8_t character_length(const std::string &str, size_t offset);
  uint8_t character_length(const std::wstring &str, size_t offset);
  uint8_t character_length(const std::u8string &str, size_t offset);
  uint8_t character_length(const std::u16string &str, size_t offset);
  uint8_t character_length(const std::u32string &str, size_t offset);
  uint8_t character_length(span<const char> str);
  uint8_t character_length(span<const wchar_t> str);
  uint8_t character_length(span<const char8_t> str);
  uint8_t character_length(span<const char16_t> str);
  uint8_t character_length(span<const char32_t> str);

  // Returns the Unicode character code representing the first/offset
  // character. Returns NUL on error (use character_length to get more
  // information).
  char32_t codepoint(const std::string &str, size_t offset);
  char32_t codepoint(const std::wstring &str, size_t offset);
  char32_t codepoint(const std::u8string &str, size_t offset);
  char32_t codepoint(const std::u16string &str, size_t offset);
  char32_t codepoint(const std::u32string &str, size_t offset);
  char32_t codepoint(span<const char> str);
  char32_t codepoint(span<const wchar_t> str);
  char32_t codepoint(span<const char8_t> str);
  char32_t codepoint(span<const char16_t> str);
  char32_t codepoint(span<const char32_t> str);

  void append_codepoint(std::string &str, char32_t code);
  void append_codepoint(std::wstring &str, char32_t code);
  void append_codepoint(std::u8string &str, char32_t code);
  void append_codepoint(std::u16string &str, char32_t code);
  void append_codepoint(std::u32string &str, char32_t code);
}

#endif