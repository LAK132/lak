#ifndef LAK_STRCONV_HPP
#define LAK_STRCONV_HPP

#include "string.hpp"

#include <type_traits>

namespace lak
{
  template<typename FROM>
  struct string_conv_iterator
  {
  private:
    using _string   = std::basic_string<FROM>;
    using _iterator = typename _string::const_iterator;
    _iterator _begin;
    _iterator _end;
    char32_t _current = 0;

  public:
    string_conv_iterator(const _string &str)
    : _begin(str.begin()), _end(str.end())
    {
      operator++();
    }
    char32_t operator*() const { return _current; }
    string_conv_iterator &operator++();
  };

  using ascii_code_iterator_t = string_conv_iterator<char>;
  using wide_code_iterator_t  = string_conv_iterator<wchar_t>;
  using utf8_code_iterator_t  = string_conv_iterator<char8_t>;
  using utf16_code_iterator_t = string_conv_iterator<char16_t>;
  using utf32_code_iterator_t = string_conv_iterator<char32_t>;

  extern template struct string_conv_iterator<char>;
  extern template struct string_conv_iterator<wchar_t>;
  extern template struct string_conv_iterator<char8_t>;
  extern template struct string_conv_iterator<char16_t>;
  extern template struct string_conv_iterator<char32_t>;

  void append_code(std::string &str, const char32_t code);
  void append_code(std::wstring &str, const char32_t code);
  void append_code(std::u8string &str, const char32_t code);
  void append_code(std::u16string &str, const char32_t code);
  void append_code(std::u32string &str, const char32_t code);

  template<typename TO, typename FROM>
  std::basic_string<TO> strconv(const std::basic_string<FROM> &str)
  {
    std::basic_string<TO> result;

    for (auto it = string_conv_iterator<FROM>(str); *it != 0; ++it)
      append_code(result, *it);

    return result;
  }

  template<typename FROM>
  inline std::string strconv_ascii(const std::basic_string<FROM> &str)
  {
    return strconv<char>(str);
  }

  template<typename FROM>
  inline std::wstring strconv_wide(const std::basic_string<FROM> &str)
  {
    return strconv<wchar_t>(str);
  }

  template<typename FROM>
  inline std::u8string strconv_u8(const std::basic_string<FROM> &str)
  {
    return strconv<char8_t>(str);
  }

  template<typename FROM>
  inline std::u16string strconv_u16(const std::basic_string<FROM> &str)
  {
    return strconv<char16_t>(str);
  }

  template<typename FROM>
  inline std::u32string strconv_u32(const std::basic_string<FROM> &str)
  {
    return strconv<char32_t>(str);
  }
}

#endif // LAK_STRCONV_HPP
