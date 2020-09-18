#include "lak/strconv.hpp"

template<typename TO, typename FROM>
size_t lak::converted_string_length(lak::span<FROM> str)
{
  size_t result = 0;
  while (str.size() > 0)
  {
    auto char_len = lak::character_length(str);
    ASSERT_NOT_EQUAL(char_len, 0);
    result += lak::codepoint_length<TO>(lak::codepoint(str));
    str = str.subspan(char_len);
  }
  return result;
}

template<typename CHAR>
lak::span<CHAR> lak::string_view(CHAR *str)
{
  return {str, lak::string_length(str)};
}

template<typename CHAR>
lak::span<const CHAR> lak::string_view(const lak::string<CHAR> &str)
{
  return {str.c_str(), str.length()};
}

template<typename CHAR>
uint8_t lak::character_length(const lak::string<CHAR> &str, size_t offset)
{
  return lak::character_length(lak::string_view(str).subspan(offset));
}

template<typename CHAR>
char32_t lak::codepoint(const lak::string<CHAR> &str, size_t offset)
{
  return lak::codepoint(lak::string_view(str).subspan(offset));
}

template<typename CHAR>
uint8_t lak::codepoint_length(char32_t code)
{
  if constexpr (std::is_same_v<CHAR, char>)
  {
    return code <= 0x7FU ? 1 : 0;
  }
  else if constexpr (std::is_same_v<CHAR, wchar_t>)
  {
    return lak::codepoint_length<wchar_unicode_t>(code);
  }
  else if constexpr (std::is_same_v<CHAR, char8_t>)
  {
    if (code <= 0x7FU)
      return 1;
    else if (code >= 0x80U && code <= 0x07FFU)
      return 2;
    else if (code >= 0x0800U && code <= 0xFFFFU)
      return 3;
    else if (code >= 0x00010000U && code <= 0x00010FFFFU)
      return 4;
    else
      return 0;
  }
  else if constexpr (std::is_same_v<CHAR, char16_t>)
  {
    if (code <= 0xD7FFU || (code >= 0xE000U && code <= 0xFFFFU))
      return 1;
    else if (code <= 0x0010FFFFU)
      return 2;
    else
      return 0;
  }
  else if constexpr (std::is_same_v<CHAR, char32_t>)
  {
    return (code <= 0xD7FFU || (code >= 0xE000U && code <= 0x0010FFFFU)) ? 1
                                                                         : 0;
  }
}

template<typename CHAR>
void lak::append_codepoint(lak::string<CHAR> &str, char32_t code)
{
  constexpr size_t buffer_size = lak::chars_per_codepoint_v<CHAR>;
  CHAR buffer[buffer_size + 1] = {};
  lak::from_codepoint(lak::codepoint_buffer<CHAR>((CHAR *)buffer), code);
  str += buffer;
}
