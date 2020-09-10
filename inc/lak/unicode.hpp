// This library assumes that char only holds 7bit ASCII characters, and that
// wchar_t will hold Unicode characters. wchar_t encoding is determined based
// on its size.

#ifndef LAK_UNICODE_HPP
#define LAK_UNICODE_HPP

#include "lak/span.hpp"
#include "lak/string.hpp"

namespace lak
{
  size_t string_length(const char *str);
  size_t string_length(const wchar_t *str);
  size_t string_length(const char8_t *str);
  size_t string_length(const char16_t *str);
  size_t string_length(const char32_t *str);

  // Returns the length of the multi-byte first/offset character (always 1 for
  // non-multi-byte string types). Returns 0 if character has a bad encoding
  // or the string is empty.
  uint8_t character_length(lak::span<const char> str);
  uint8_t character_length(lak::span<const wchar_t> str);
  uint8_t character_length(lak::span<const char8_t> str);
  uint8_t character_length(lak::span<const char16_t> str);
  uint8_t character_length(lak::span<const char32_t> str);
  template<typename CHAR>
  uint8_t character_length(const std::basic_string<CHAR> &str, size_t offset);

  // Returns the Unicode character code representing the first/offset
  // character. Returns NUL on error (use character_length to get more
  // information).
  char32_t codepoint(lak::span<const char> str);
  char32_t codepoint(lak::span<const wchar_t> str);
  char32_t codepoint(lak::span<const char8_t> str);
  char32_t codepoint(lak::span<const char16_t> str);
  char32_t codepoint(lak::span<const char32_t> str);
  template<typename CHAR>
  uint8_t codepoint(const std::basic_string<CHAR> &str, size_t offset);

  template<typename CHAR>
  using codepoint_buffer = lak::span<CHAR, chars_per_codepoint_v<CHAR>>;

  lak::span<char> from_codepoint(lak::codepoint_buffer<char> buffer,
                                 char32_t code);
  lak::span<wchar_t> from_codepoint(lak::codepoint_buffer<wchar_t> buffer,
                                    char32_t code);
  lak::span<char8_t> from_codepoint(lak::codepoint_buffer<char8_t> buffer,
                                    char32_t code);
  lak::span<char16_t> from_codepoint(lak::codepoint_buffer<char16_t> buffer,
                                     char32_t code);
  lak::span<char32_t> from_codepoint(lak::codepoint_buffer<char32_t> buffer,
                                     char32_t code);

  template<typename CHAR>
  void append_codepoint(std::basic_string<CHAR> &str, char32_t code);

  bool is_whitespace(char32_t c);
}

#include "lak/unicode.inl"

#endif