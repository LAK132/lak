#include "lak/string_view.hpp"
#include "lak/unicode.hpp"

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
inline lak::string<TO> lak::strconv(lak::string_view<FROM> str)
{
  lak::string<TO> result;

  for (const auto &[c, len] : lak::codepoint_range(str))
    lak::append_codepoint(result, c);

  return result;
}

template<typename FROM>
inline lak::astring lak::to_astring(lak::string_view<FROM> str)
{
  return lak::strconv<char>(str);
}

template<typename FROM>
inline lak::wstring lak::to_wstring(lak::string_view<FROM> str)
{
  return lak::strconv<wchar_t>(str);
}

template<typename FROM>
inline lak::u8string lak::to_u8string(lak::string_view<FROM> str)
{
  return lak::strconv<char8_t>(str);
}

template<typename FROM>
inline lak::u16string lak::to_u16string(lak::string_view<FROM> str)
{
  return lak::strconv<char16_t>(str);
}

template<typename FROM>
inline lak::u32string lak::to_u32string(lak::string_view<FROM> str)
{
  return lak::strconv<char32_t>(str);
}

template<typename FROM>
inline lak::astring lak::to_astring(const FROM *str)
{
  return lak::strconv<char>(lak::string_view(str, lak::string_length(str)));
}

template<typename FROM>
inline lak::wstring lak::to_wstring(const FROM *str)
{
  return lak::strconv<wchar_t>(lak::string_view(str, lak::string_length(str)));
}

template<typename FROM>
inline lak::u8string lak::to_u8string(const FROM *str)
{
  return lak::strconv<char8_t>(lak::string_view(str, lak::string_length(str)));
}

template<typename FROM>
inline lak::u16string lak::to_u16string(const FROM *str)
{
  return lak::strconv<char16_t>(
    lak::string_view(str, lak::string_length(str)));
}

template<typename FROM>
inline lak::u32string lak::to_u32string(const FROM *str)
{
  return lak::strconv<char32_t>(
    lak::string_view(str, lak::string_length(str)));
}

LAK_EXTERN_TEMPLATE_FOREACH_CHAR(lak::codepoint_iterator)
LAK_EXTERN_TEMPLATE_FOREACH_CHAR(lak::codepoint_range)