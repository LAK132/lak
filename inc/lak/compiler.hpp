#ifndef LAK_COMPILER_HPP
#define LAK_COMPILER_HPP

#if __cplusplus >= 202002L
#  define LAK_COMPILER_CPP20
#endif
#if __cplusplus >= 201703L
#  define LAK_COMPILER_CPP17
#endif
#if __cplusplus >= 201402L
#  define LAK_COMPILER_CPP14
#endif
#if __cplusplus >= 201103L
#  define LAK_COMPILER_CPP11
#endif
#if __cplusplus >= 199711L
#  define LAK_COMPILER_CPP98
#endif

#ifndef LAK_COMPILER_CPP17
#  error Expected at least a C++17 compiler
#endif

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