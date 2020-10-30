#ifndef LAK_COMPILER_HPP
#define LAK_COMPILER_HPP

#ifdef __clang__
#  define LAK_COMPILER_CLANG
#endif

#ifdef __GNUC__
#  define LAK_COMPILER_GNUC
#endif

#ifdef _MSC_VER
#  define LAK_COMPILER_MSVC
#endif

#if defined(LAK_COMPILER_CLANG)
#  define force_inline  inline __attribute__((always_inline))
#  define packed_struct struct __attribute__((packed))
#  define DEBUG_BREAK() asm("int $3")
#elif defined(LAK_COMPILER_GNUC)
#  define force_inline  inline __attribute__((always_inline))
#  define packed_struct struct __attribute__((packed))
#  define DEBUG_BREAK() asm("int $3")
#elif defined(LAK_COMPILER_MSVC)
#  include <intrin.h>
#  define force_inline  inline __forceinline
#  define packed_struct __pragma(pack(1)) struct
#  define DEBUG_BREAK() __debugbreak()
#else
#  error "Compiler not supported"
#endif

#endif