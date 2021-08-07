#ifndef LAK_COMPILER_HPP
#define LAK_COMPILER_HPP

#if __cplusplus >= 202002L
#	define LAK_COMPILER_CPP20
#endif
#if __cplusplus >= 201703L
#	define LAK_COMPILER_CPP17
#endif
#if __cplusplus >= 201402L
#	define LAK_COMPILER_CPP14
#endif
#if __cplusplus >= 201103L
#	define LAK_COMPILER_CPP11
#endif
#if __cplusplus >= 199711L
#	define LAK_COMPILER_CPP98
#endif

#ifndef LAK_COMPILER_CPP20
#	error Expected at least a C++20 compiler
#endif

#ifdef __EMSCRIPTEN__
#	define LAK_COMPILER_EMSCRIPTEN
#endif

#ifdef __clang__
#	define LAK_COMPILER_CLANG
#endif

#ifdef __GNUC__
#	define LAK_COMPILER_GNUC
#endif

#ifdef _MSC_VER
#	define LAK_COMPILER_MSVC
#endif

#if defined(LAK_COMPILER_EMSCRIPTEN)
#	define force_inline  inline [[gnu::always_inline]]
#	define packed_struct struct [[gnu::packed]]
#	define DEBUG_BREAK()
#	define LAK_OPT_ARGS(...) , __VA_ARGS__
#	define DLL_EXPORT        [[gnu::dllexport]]
#elif defined(LAK_COMPILER_CLANG)
#	define force_inline      inline [[gnu::always_inline]]
#	define packed_struct     struct [[gnu::packed]]
#	define DEBUG_BREAK()     asm("int $3")
#	define LAK_OPT_ARGS(...) , __VA_ARGS__
#	define DLL_EXPORT        [[gnu::dllexport]]
#elif defined(LAK_COMPILER_GNUC)
#	define force_inline      inline [[gnu::always_inline]]
#	define packed_struct     struct [[gnu::packed]]
#	define DEBUG_BREAK()     asm("int $3")
#	define LAK_OPT_ARGS(...) , ##__VA_ARGS__
#	define DLL_EXPORT        [[gnu::dllexport]]
#elif defined(LAK_COMPILER_MSVC)
#	include <intrin.h>
#	define force_inline      inline __forceinline
#	define packed_struct     __pragma(pack(1)) struct
#	define DEBUG_BREAK()     __debugbreak()
#	define LAK_OPT_ARGS(...) , __VA_ARGS__
#	define DLL_EXPORT        __declspec(dllexport)
#else
#	error "Compiler not supported"
#endif

#endif