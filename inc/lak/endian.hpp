#ifndef LAK_ENDIAN_HPP
#define LAK_ENDIAN_HPP

#if defined(__clang__) || defined(__GNUC__)
#  if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    define LAK_BIG_ENDIAN
#  elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define LAK_LITTLE_ENDIAN
#  else
#    error "Endianness not supported"
#  endif
#elif defined(_MSC_VER)
#  include "winnt.h"
#  if REG_DWORD == REG_DWORD_BIG_ENDIAN
#    define LAK_BIG_ENDIAN
#  elif REG_DWORD == REG_DWORD_LITTLE_ENDIAN
#    define LAK_LITTLE_ENDIAN
#  else
#    error "Endianness not supported"
#  endif
#else
#  error "Compiler not supported"
#endif

namespace lak
{
  enum struct endian
  {
    little = 0,
    big    = 1,
#if defined(LAK_LITTLE_ENDIAN)
    native = little
#elif defined(LAK_BIG_ENDIAN)
    native = big
#endif
  };
}

#endif