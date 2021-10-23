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
#	define force_inline  inline __attribute__((always_inline))
#	define packed_struct struct [[gnu::packed]]
#	define DEBUG_BREAK()
#	define DLL_EXPORT [[gnu::dllexport]]
#elif defined(LAK_COMPILER_CLANG) || defined(LAK_COMPILER_GNUC)
#	define force_inline  inline __attribute__((always_inline))
#	define packed_struct struct [[gnu::packed]]
#	define DEBUG_BREAK() asm("int $3")
#	define DLL_EXPORT    [[gnu::dllexport]]
#	if defined(__x86_64__)
#		define LAK_ARCH_X86_64
#	endif
#	if defined(__i386__)
#		define LAK_ARCH_X86
#	endif
#	if defined(__ia64__)
#		define LAK_ARCH_IA64
#	endif
#	if defined(__arm__)
#		define LAK_ARCH_ARM
#	endif
#	if defined(__aarch64__)
#		define LAK_ARCH_ARM64
#	endif
#elif defined(LAK_COMPILER_MSVC)
#	include <intrin.h>
#	define force_inline  inline __forceinline
#	define packed_struct struct [[gnu::packed]]
#	define DEBUG_BREAK() __debugbreak()
#	define DLL_EXPORT    __declspec(dllexport)
#	if defined(_M_X64) || defined(_M_AMD64)
#		define LAK_ARCH_X86_64
#	endif
#	if defined(_M_IX86)
#		define LAK_ARCH_X86
#	endif
#	if defined(_M_IA64)
#		define LAK_ARCH_IA64
#	endif
#	if defined(_M_ARM)
#		define LAK_ARCH_ARM
#	endif
#	if defined(_M_ARM64)
#		define LAK_ARCH_ARM64
#	endif
#	if defined(__AVX__)
#		define LAK_ARCH_AVX
#	endif
#	if defined(__AVX2__)
#		define LAK_ARCH_AVX2
#	endif
#	if defined(__AVX512__)
#		define LAK_ARCH_AVX512
#	endif
#else
#	error "Compiler not supported"
#endif

#if defined(LAK_ARCH_X86_64)
#	define LAK_ARCH x86_64
#endif
#if defined(LAK_ARCH_X86)
#	define LAK_ARCH x86
#endif
#if defined(LAK_ARCH_IA64)
#	define LAK_ARCH ia64
#endif
#if defined(LAK_ARCH_ARM)
#	define LAK_ARCH arm
#endif
#if defined(LAK_ARCH_ARM64)
#	define LAK_ARCH aarch64
#endif

#endif
