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

#endif