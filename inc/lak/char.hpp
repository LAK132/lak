#ifndef LAK_CHAR_HPP
#define LAK_CHAR_HPP

// char8_t typedef for C++ < 20
#if __cplusplus <= 201703L
#  include <cstdint>
using char8_t = uint_least8_t;
#endif

#endif