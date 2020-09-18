#include "lak/unicode.hpp"

template<typename CHAR>
inline constexpr const std::pair<char32_t, uint8_t>
  &lak::codepoint_iterator<CHAR>::operator*() const noexcept
{
  return _current;
}

template<typename CHAR>
inline constexpr lak::codepoint_iterator<CHAR>
  &lak::codepoint_iterator<CHAR>::operator++() noexcept
{
  _current.second = lak::character_length(_data);
  _current.first  = _current.second ? codepoint(_data) : 0;
  _data           = _data.subspan(_current.second);
  return *this;
}

template<typename CHAR>
inline constexpr bool lak::codepoint_iterator<CHAR>::operator==(
  char32_t c) const noexcept
{
  return _current.first == c;
}

template<typename CHAR>
inline constexpr bool lak::codepoint_iterator<CHAR>::operator!=(
  char32_t c) const noexcept
{
  return _current.first != c;
}

template<typename CHAR>
inline constexpr lak::codepoint_iterator<CHAR>
lak::codepoint_range<CHAR>::begin() const noexcept
{
  return {_data};
}

template<typename CHAR>
inline constexpr char32_t lak::codepoint_range<CHAR>::end() const noexcept
{
  return 0;
}

template<typename TO, typename FROM>
inline lak::string<TO> lak::strconv(const lak::string<FROM> &str)
{
  lak::string<TO> result;

  for (const auto &[c, len] : lak::codepoint_range(str))
    lak::append_codepoint(result, c);

  return result;
}

template<typename FROM>
inline lak::astring lak::to_astring(const lak::string<FROM> &str)
{
  return lak::strconv<char>(str);
}

template<typename FROM>
inline lak::wstring lak::to_wstring(const lak::string<FROM> &str)
{
  return lak::strconv<wchar_t>(str);
}

template<typename FROM>
inline lak::u8string lak::to_u8string(const lak::string<FROM> &str)
{
  return lak::strconv<char8_t>(str);
}

template<typename FROM>
inline lak::u16string lak::to_u16string(const lak::string<FROM> &str)
{
  return lak::strconv<char16_t>(str);
}

template<typename FROM>
inline lak::u32string lak::to_u32string(const lak::string<FROM> &str)
{
  return lak::strconv<char32_t>(str);
}

template<typename TO, typename FROM>
inline lak::string<TO> lak::strconv(lak::span<FROM> str)
{
  lak::string<TO> result;

  for (const auto &[c, len] : lak::codepoint_range(str))
    lak::append_codepoint(result, c);

  return result;
}

template<typename FROM>
inline lak::astring lak::to_astring(lak::span<FROM> str)
{
  return lak::strconv<char>(str);
}

template<typename FROM>
inline lak::wstring lak::to_wstring(lak::span<FROM> str)
{
  return lak::strconv<wchar_t>(str);
}

template<typename FROM>
inline lak::u8string lak::to_u8string(lak::span<FROM> str)
{
  return lak::strconv<char8_t>(str);
}

template<typename FROM>
inline lak::u16string lak::to_u16string(lak::span<FROM> str)
{
  return lak::strconv<char16_t>(str);
}

template<typename FROM>
inline lak::u32string lak::to_u32string(lak::span<FROM> str)
{
  return lak::strconv<char32_t>(str);
}

template<typename FROM>
inline lak::astring lak::to_astring(const FROM *str)
{
  return lak::strconv<char>(lak::span(str, lak::string_length(str)));
}

template<typename FROM>
inline lak::wstring lak::to_wstring(const FROM *str)
{
  return lak::strconv<wchar_t>(lak::span(str, lak::string_length(str)));
}

template<typename FROM>
inline lak::u8string lak::to_u8string(const FROM *str)
{
  return lak::strconv<char8_t>(lak::span(str, lak::string_length(str)));
}

template<typename FROM>
inline lak::u16string lak::to_u16string(const FROM *str)
{
  return lak::strconv<char16_t>(lak::span(str, lak::string_length(str)));
}

template<typename FROM>
inline lak::u32string lak::to_u32string(const FROM *str)
{
  return lak::strconv<char32_t>(lak::span(str, lak::string_length(str)));
}

inline lak::span<const char> lak::as_astring(const lak::u8string &str)
{
  return lak::span<const char>(lak::string_view(str));
}

inline lak::span<const char> lak::as_astring(lak::span<const char8_t> str)
{
  return lak::span<const char>(str);
}

inline lak::span<const char> lak::as_astring(const char8_t *str)
{
  return lak::span<const char>(lak::string_view(str));
}

inline lak::span<const char8_t> lak::as_u8string(const lak::astring &str)
{
  return lak::span<const char8_t>(lak::string_view(str));
}

inline lak::span<const char8_t> lak::as_u8string(lak::span<const char> str)
{
  return lak::span<const char8_t>(str);
}

inline lak::span<const char8_t> lak::as_u8string(const char *str)
{
  return lak::span<const char8_t>(lak::string_view(str));
}

LAK_EXTERN_TEMPLATE_FOREACH_CHAR(lak::codepoint_iterator)
LAK_EXTERN_TEMPLATE_FOREACH_CHAR(lak::codepoint_range)