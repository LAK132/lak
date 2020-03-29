#ifndef LAK_STRING_HPP
#define LAK_STRING_HPP

#include <string>

// char8_t typdef for C++ < 20
#if __cplusplus <= 201703L
using char8_t = uint_least8_t;
namespace std
{
  using u8string = std::basic_string<char8_t>;
}
#endif

#endif // LAK_STRING_HPP