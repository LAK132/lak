#ifndef LAK_COMPILER_HPP
#define LAK_COMPILER_HPP

#if defined(__clang__)
#  define force_inline inline __attribute__((always_inline))
#  define DEBUG_BREAK()
#elif defined(__GNUC__)
#  define force_inline inline __attribute__((always_inline))
#  define DEBUG_BREAK()
#elif defined(_MSC_VER)
#  define force_inline  inline __forceinline
#  define DEBUG_BREAK() DebugBreak()
#else
#  define force_inline inline
#  define DEBUG_BREAK()
#endif

#endif