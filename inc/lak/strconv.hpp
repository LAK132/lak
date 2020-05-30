#ifndef LAK_STRCONV_HPP
#define LAK_STRCONV_HPP

// This library assumes that char only holds 7bit ASCII characters, and that
// wchar_t will hold Unicode characters. wchar_t encoding is determined based
// on its size.

#include "string.hpp"

#include <type_traits>

namespace lak
{
  template<typename CHAR>
  struct codepoint_iterator
  {
  private:
    span<const CHAR> _data;
    char32_t _current;

  public:
    inline constexpr codepoint_iterator(span<const CHAR> str) noexcept
    : _data(str)
    {
      operator++();
    }

    inline constexpr const char32_t &operator*() const noexcept
    {
      return _current;
    }

    inline constexpr codepoint_iterator &operator++() noexcept
    {
      auto len = character_length(_data);
      _current = len ? codepoint(_data) : 0;
      _data    = _data.subspan(len);
      return *this;
    }

    inline constexpr bool operator==(char32_t c) const noexcept
    {
      return _current == c;
    }

    inline constexpr bool operator!=(char32_t c) const noexcept
    {
      return _current != c;
    }
  };

  extern template struct codepoint_iterator<char>;
  extern template struct codepoint_iterator<wchar_t>;
  extern template struct codepoint_iterator<char8_t>;
  extern template struct codepoint_iterator<char16_t>;
  extern template struct codepoint_iterator<char32_t>;

  template<typename CHAR>
  struct codepoint_range
  {
  private:
    span<const CHAR> _data;

  public:
    inline constexpr codepoint_range(span<const CHAR> str) noexcept
    : _data(str)
    {
    }

    inline constexpr codepoint_range(
      const std::basic_string<CHAR> &str) noexcept
    : _data(str.c_str(), str.size())
    {
    }

    inline constexpr codepoint_iterator<CHAR> begin() const noexcept
    {
      return {_data};
    }

    inline constexpr char32_t end() const noexcept { return 0; }
  };

  extern template struct codepoint_range<char>;
  extern template struct codepoint_range<wchar_t>;
  extern template struct codepoint_range<char8_t>;
  extern template struct codepoint_range<char16_t>;
  extern template struct codepoint_range<char32_t>;

  template<typename TO, typename FROM>
  std::basic_string<TO> strconv(const std::basic_string<FROM> &str)
  {
    std::basic_string<TO> result;

    for (const auto &c : codepoint_range(str)) append_codepoint(result, c);

    return result;
  }

  template<typename FROM>
  inline std::string to_astring(const std::basic_string<FROM> &str)
  {
    return strconv<char>(str);
  }

  template<typename FROM>
  inline std::wstring to_wstring(const std::basic_string<FROM> &str)
  {
    return strconv<wchar_t>(str);
  }

  template<typename FROM>
  inline std::u8string to_u8string(const std::basic_string<FROM> &str)
  {
    return strconv<char8_t>(str);
  }

  template<typename FROM>
  inline std::u16string to_u16string(const std::basic_string<FROM> &str)
  {
    return strconv<char16_t>(str);
  }

  template<typename FROM>
  inline std::u32string to_u32string(const std::basic_string<FROM> &str)
  {
    return strconv<char32_t>(str);
  }
}

#endif // LAK_STRCONV_HPP
