// This library assumes that char only holds 7bit ASCII characters, and that
// wchar_t will hold Unicode characters. wchar_t encoding is determined based
// on its size.

#include "lak/span.hpp"
#include "lak/tuple.hpp"

#ifndef LAK_UNICODE_HPP
#  define LAK_UNICODE_HPP

#  include "lak/string.hpp"

namespace lak
{
  // Length of the null terminated string not including the null terminator.
  size_t string_length(const char *str);
  size_t string_length(const wchar_t *str);
  size_t string_length(const char8_t *str);
  size_t string_length(const char16_t *str);
  size_t string_length(const char32_t *str);

  // The length of the character length (not codepoint length) of the string if
  // it was converted to TO type characters.
  template<typename TO, typename FROM>
  size_t converted_string_length(lak::span<FROM> str);

  template<typename CHAR>
  lak::span<CHAR> string_view(CHAR *str);
  template<typename CHAR>
  lak::span<const CHAR> string_view(const lak::string<CHAR> &str);

  // Returns the length of the multi-byte first/offset character (always 1 for
  // non-multi-byte string types). Returns 0 if character has a bad encoding
  // or the string is empty.
  uint8_t character_length(lak::span<const char> str);
  uint8_t character_length(lak::span<const wchar_t> str);
  uint8_t character_length(lak::span<const char8_t> str);
  uint8_t character_length(lak::span<const char16_t> str);
  uint8_t character_length(lak::span<const char32_t> str);
  template<typename CHAR>
  uint8_t character_length(const lak::string<CHAR> &str, size_t offset);

  // Returns the Unicode character code representing the first/offset
  // character. Returns NUL on error (use character_length to get more
  // information).
  char32_t codepoint(lak::span<const char> str);
  char32_t codepoint(lak::span<const wchar_t> str);
  char32_t codepoint(lak::span<const char8_t> str);
  char32_t codepoint(lak::span<const char16_t> str);
  char32_t codepoint(lak::span<const char32_t> str);
  template<typename CHAR>
  char32_t codepoint(const lak::string<CHAR> &str, size_t offset);

  template<typename CHAR>
  using codepoint_buffer_t = lak::span<CHAR, lak::chars_per_codepoint_v<CHAR>>;
  template<typename CHAR,
           size_t SIZE,
           lak::enable_if_i<(SIZE >= lak::chars_per_codepoint_v<CHAR>)> = 0>
  force_inline constexpr lak::codepoint_buffer_t<CHAR> codepoint_buffer(
    lak::span<CHAR, SIZE> buf)
  {
    return buf.template first<lak::chars_per_codepoint_v<CHAR>>();
  }

  template<typename CHAR>
  uint8_t codepoint_length(char32_t code);

  lak::span<char> from_codepoint(lak::codepoint_buffer_t<char> buffer,
                                 char32_t code);
  lak::span<wchar_t> from_codepoint(lak::codepoint_buffer_t<wchar_t> buffer,
                                    char32_t code);
  lak::span<char8_t> from_codepoint(lak::codepoint_buffer_t<char8_t> buffer,
                                    char32_t code);
  lak::span<char16_t> from_codepoint(lak::codepoint_buffer_t<char16_t> buffer,
                                     char32_t code);
  lak::span<char32_t> from_codepoint(lak::codepoint_buffer_t<char32_t> buffer,
                                     char32_t code);

  template<typename CHAR>
  void append_codepoint(lak::string<CHAR> &str, char32_t code);

  inline constexpr lak::span<const char32_t> hex_alphanumerics();
  inline constexpr bool is_hex_alphanumeric(char32_t c);

  inline constexpr lak::span<const char32_t> control_codes();
  inline constexpr bool is_control_code(char32_t c);

  inline constexpr lak::span<const char32_t> whitespaces();
  inline constexpr bool is_whitespace(char32_t c);

  template<typename CHAR>
  struct codepoint_iterator
  {
  private:
    lak::span<const CHAR> _data;
    lak::pair<char32_t, uint8_t> _current;

  public:
    inline constexpr codepoint_iterator(lak::span<CHAR> str) noexcept
    : _data(str)
    {
      operator++();
    }

    inline constexpr codepoint_iterator(lak::span<const CHAR> str) noexcept
    : _data(str)
    {
      operator++();
    }

    inline constexpr const lak::pair<char32_t, uint8_t> &operator*()
      const noexcept;

    inline constexpr codepoint_iterator &operator++() noexcept;

    inline constexpr bool operator==(char32_t c) const noexcept;

    inline constexpr bool operator!=(char32_t c) const noexcept;
  };

  template<typename CHAR>
  struct codepoint_range
  {
  private:
    lak::span<const CHAR> _data;

  public:
    inline constexpr codepoint_range(lak::span<CHAR> str) noexcept : _data(str)
    {
    }

    inline constexpr codepoint_range(lak::span<const CHAR> str) noexcept
    : _data(str)
    {
    }

    inline constexpr codepoint_range(const lak::string<CHAR> &str) noexcept
    : _data(lak::span(str.c_str(), str.size()))
    {
    }

    inline constexpr codepoint_iterator<CHAR> begin() const noexcept;

    inline constexpr char32_t end() const noexcept;
  };
}

#  ifndef LAK_NO_STD
#    include <ostream>

std::ostream &operator<<(std::ostream &strm, const lak::span<const char> &str);

std::ostream &operator<<(std::ostream &strm,
                         const lak::span<const wchar_t> &str);

std::ostream &operator<<(std::ostream &strm,
                         const lak::span<const char8_t> &str);

std::ostream &operator<<(std::ostream &strm,
                         const lak::span<const char16_t> &str);

std::ostream &operator<<(std::ostream &strm,
                         const lak::span<const char32_t> &str);
#  endif

#  include "lak/unicode.inl"

#endif