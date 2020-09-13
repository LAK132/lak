// This library assumes that char only holds 7bit ASCII characters, and that
// wchar_t will hold Unicode characters. wchar_t encoding is determined based
// on its size.

#ifndef LAK_STRCONV_HPP
#define LAK_STRCONV_HPP

#include "lak/span.hpp"
#include "lak/string.hpp"

#include <tuple>

namespace lak
{
  template<typename CHAR>
  struct codepoint_iterator
  {
  private:
    lak::span<const CHAR> _data;
    std::pair<char32_t, uint8_t> _current;

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

    inline constexpr const std::pair<char32_t, uint8_t> &operator*()
      const noexcept;

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
  inline lak::string<TO> strconv(const lak::string<FROM> &str);
  template<typename FROM>
  inline lak::astring to_astring(const lak::string<FROM> &str);
  template<typename FROM>
  inline lak::wstring to_wstring(const lak::string<FROM> &str);
  template<typename FROM>
  inline lak::u8string to_u8string(const lak::string<FROM> &str);
  template<typename FROM>
  inline lak::u16string to_u16string(const lak::string<FROM> &str);
  template<typename FROM>
  inline lak::u32string to_u32string(const lak::string<FROM> &str);

  template<typename TO, typename FROM>
  inline lak::string<TO> strconv(lak::span<FROM> str);
  template<typename FROM>
  inline lak::astring to_astring(lak::span<FROM> str);
  template<typename FROM>
  inline lak::wstring to_wstring(lak::span<FROM> str);
  template<typename FROM>
  inline lak::u8string to_u8string(lak::span<FROM> str);
  template<typename FROM>
  inline lak::u16string to_u16string(lak::span<FROM> str);
  template<typename FROM>
  inline lak::u32string to_u32string(lak::span<FROM> str);

  template<typename FROM>
  inline lak::astring to_astring(const FROM *str);
  template<typename FROM>
  inline lak::wstring to_wstring(const FROM *str);
  template<typename FROM>
  inline lak::u8string to_u8string(const FROM *str);
  template<typename FROM>
  inline lak::u16string to_u16string(const FROM *str);
  template<typename FROM>
  inline lak::u32string to_u32string(const FROM *str);
}

#include "lak/strconv.inl"

#endif
