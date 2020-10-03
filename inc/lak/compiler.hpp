#ifndef LAK_COMPILER_HPP
#define LAK_COMPILER_HPP

#if defined(__clang__)
#  define force_inline  inline __attribute__((always_inline))
#  define packed_struct struct __attribute__((packed))
#  define DEBUG_BREAK()
#elif defined(__GNUC__)
#  define force_inline  inline __attribute__((always_inline))
#  define packed_struct struct __attribute__((packed))
#  define DEBUG_BREAK()
#elif defined(_MSC_VER)
#  define force_inline  inline __forceinline
#  define packed_struct __pragma(pack(1)) struct
#  define DEBUG_BREAK() __debugbreak()
#else
#  error "Compiler not supported"
#endif

#endif