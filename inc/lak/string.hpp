#ifndef LAK_STRING_HPP
#define LAK_STRING_HPP

#include "lak/char.hpp"

#include <string>

namespace lak
{
  template<typename CHAR>
  using string    = std::basic_string<CHAR>;
  using astring   = lak::string<char>;
  using wstring   = lak::string<wchar_t>;
  using u8string  = lak::string<char8_t>;
  using u16string = lak::string<char16_t>;
  using u32string = lak::string<char32_t>;
}

#endif