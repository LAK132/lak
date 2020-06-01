// This library assumes that char only holds 7bit ASCII characters, and that
// wchar_t will hold Unicode characters. wchar_t encoding is determined based
// on its size.

#ifndef LAK_STRCONV_HPP
#define LAK_STRCONV_HPP

#include "lak/span.hpp"
#include "lak/string.hpp"

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

    inline constexpr const char32_t &operator*() const noexcept;

    inline constexpr codepoint_iterator &operator++() noexcept;

    inline constexpr bool operator==(char32_t c) const noexcept;

    inline constexpr bool operator!=(char32_t c) const noexcept;
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

    inline constexpr codepoint_iterator<CHAR> begin() const noexcept;

    inline constexpr char32_t end() const noexcept;
  };

  extern template struct codepoint_range<char>;
  extern template struct codepoint_range<wchar_t>;
  extern template struct codepoint_range<char8_t>;
  extern template struct codepoint_range<char16_t>;
  extern template struct codepoint_range<char32_t>;

  template<typename TO, typename FROM>
  inline std::basic_string<TO> strconv(const std::basic_string<FROM> &str);
  template<typename FROM>
  inline std::string to_astring(const std::basic_string<FROM> &str);
  template<typename FROM>
  inline std::wstring to_wstring(const std::basic_string<FROM> &str);
  template<typename FROM>
  inline std::u8string to_u8string(const std::basic_string<FROM> &str);
  template<typename FROM>
  inline std::u16string to_u16string(const std::basic_string<FROM> &str);
  template<typename FROM>
  inline std::u32string to_u32string(const std::basic_string<FROM> &str);

  template<typename TO, typename FROM>
  inline std::basic_string<TO> strconv(span<const FROM> str);
  template<typename FROM>
  inline std::string to_astring(span<const FROM> str);
  template<typename FROM>
  inline std::wstring to_wstring(span<const FROM> str);
  template<typename FROM>
  inline std::u8string to_u8string(span<const FROM> str);
  template<typename FROM>
  inline std::u16string to_u16string(span<const FROM> str);
  template<typename FROM>
  inline std::u32string to_u32string(span<const FROM> str);

  template<typename FROM>
  inline std::string to_astring(const FROM *str);
  template<typename FROM>
  inline std::wstring to_wstring(const FROM *str);
  template<typename FROM>
  inline std::u8string to_u8string(const FROM *str);
  template<typename FROM>
  inline std::u16string to_u16string(const FROM *str);
  template<typename FROM>
  inline std::u32string to_u32string(const FROM *str);
}

#include "lak/strconv.inl"

#endif
