#include "lak/unicode.hpp"

namespace lak
{
  template<typename CHAR>
  inline constexpr const std::pair<char32_t, uint8_t>
    &codepoint_iterator<CHAR>::operator*() const noexcept
  {
    return _current;
  }

  template<typename CHAR>
  inline constexpr codepoint_iterator<CHAR>
    &codepoint_iterator<CHAR>::operator++() noexcept
  {
    _current.second = character_length(_data);
    _current.first  = _current.second ? codepoint(_data) : 0;
    _data           = _data.subspan(_current.second);
    return *this;
  }

  template<typename CHAR>
  inline constexpr bool codepoint_iterator<CHAR>::operator==(
    char32_t c) const noexcept
  {
    return _current.first == c;
  }

  template<typename CHAR>
  inline constexpr bool codepoint_iterator<CHAR>::operator!=(
    char32_t c) const noexcept
  {
    return _current.first != c;
  }

  extern template struct codepoint_iterator<char>;
  extern template struct codepoint_iterator<wchar_t>;
  extern template struct codepoint_iterator<char8_t>;
  extern template struct codepoint_iterator<char16_t>;
  extern template struct codepoint_iterator<char32_t>;

  template<typename CHAR>
  inline constexpr codepoint_iterator<CHAR> codepoint_range<CHAR>::begin()
    const noexcept
  {
    return {_data};
  }

  template<typename CHAR>
  inline constexpr char32_t codepoint_range<CHAR>::end() const noexcept
  {
    return 0;
  }

  extern template struct codepoint_range<char>;
  extern template struct codepoint_range<wchar_t>;
  extern template struct codepoint_range<char8_t>;
  extern template struct codepoint_range<char16_t>;
  extern template struct codepoint_range<char32_t>;

  template<typename TO, typename FROM>
  inline std::basic_string<TO> strconv(const std::basic_string<FROM> &str)
  {
    std::basic_string<TO> result;

    for (const auto &[c, len] : codepoint_range(str))
      append_codepoint(result, c);

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

  template<typename TO, typename FROM>
  inline std::basic_string<TO> strconv(span<const FROM> str)
  {
    std::basic_string<TO> result;

    for (const auto &[c, len] : codepoint_range(str))
      append_codepoint(result, c);

    return result;
  }

  template<typename FROM>
  inline std::string to_astring(span<const FROM> str)
  {
    return strconv<char>(str);
  }

  template<typename FROM>
  inline std::wstring to_wstring(span<const FROM> str)
  {
    return strconv<wchar_t>(str);
  }

  template<typename FROM>
  inline std::u8string to_u8string(span<const FROM> str)
  {
    return strconv<char8_t>(str);
  }

  template<typename FROM>
  inline std::u16string to_u16string(span<const FROM> str)
  {
    return strconv<char16_t>(str);
  }

  template<typename FROM>
  inline std::u32string to_u32string(span<const FROM> str)
  {
    return strconv<char32_t>(str);
  }

  template<typename FROM>
  inline std::string to_astring(const FROM *str)
  {
    return strconv<char>(lak::span(str, lak::string_length(str)));
  }

  template<typename FROM>
  inline std::wstring to_wstring(const FROM *str)
  {
    return strconv<wchar_t>(lak::span(str, lak::string_length(str)));
  }

  template<typename FROM>
  inline std::u8string to_u8string(const FROM *str)
  {
    return strconv<char8_t>(lak::span(str, lak::string_length(str)));
  }

  template<typename FROM>
  inline std::u16string to_u16string(const FROM *str)
  {
    return strconv<char16_t>(lak::span(str, lak::string_length(str)));
  }

  template<typename FROM>
  inline std::u32string to_u32string(const FROM *str)
  {
    return strconv<char32_t>(lak::span(str, lak::string_length(str)));
  }

}
