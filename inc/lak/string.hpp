#ifndef LAK_STRING_HPP
#define LAK_STRING_HPP

#include "lak/char.hpp"

#include <string>

// u8string typedef for C++ < 20
#if __cplusplus <= 201703L
namespace std
{
  using u8string = std::basic_string<char8_t>;
}
#endif

namespace lak
{
  using astring   = std::string;
  using wstring   = std::wstring;
  using u8string  = std::u8string;
  using u16string = std::u16string;
  using u32string = std::u32string;
}

#endif