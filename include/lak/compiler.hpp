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

#ifdef __INTEL_COMPILER
#	define LAK_COMPILER_INTEL
#endif

// https://github.com/nemequ/portable-snippets/blob/master/debug-trap/debug-trap.h

#if defined(__has_builtin) && !defined(__ibmxl__)
#	if __has_builtin(__builtin_debugtrap)
#		define DEBUG_BREAK() __builtin_debugtrap()
#	elif __has_builtin(__debugbreak)
#		define DEBUG_BREAK() __debugbreak()
#	endif
#endif
#if !defined(DEBUG_BREAK)
#	if defined(LAK_COMPILER_MSVC) || defined(LAK_COMPILER_INTEL)
#		define DEBUG_BREAK() __debugbreak()
#	elif defined(__ARMCC_VERSION)
#		define DEBUG_BREAK() __breakpoint(42)
#	elif defined(__ibmxl__) || defined(__xlC__)
#		include <builtins.h>
#		define DEBUG_BREAK() __trap(42)
#	elif defined(__DMC__) && defined(_M_IX86)
static inline void lak_debug_break() { __asm int 3h; }
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(__i386__) || defined(__x86_64__)
static inline void lak_debug_break() { __asm__ __volatile__("int3"); }
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(__thumb__)
static inline void lak_debug_break() { __asm__ __volatile__(".inst 0xde01"); }
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(__aarch64__)
static inline void lak_debug_break()
{
	__asm__ __volatile__(".inst 0xd4200000");
}
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(__arm__)
static inline void lak_debug_break()
{
	__asm__ __volatile__(".inst 0xe7f001f0");
}
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(__alpha__) && !defined(__osf__)
static inline void lak_debug_break() { __asm__ __volatile__("bpt"); }
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(_54_)
static inline void lak_debug_break() { __asm__ __volatile__("ESTOP"); }
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(_55_)
static inline void lak_debug_break()
{
	__asm__ __volatile__(
	  ";\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP");
}
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(_64P_)
static inline void lak_debug_break() { __asm__ __volatile__("SWBP 0"); }
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(_6x_)
static inline void lak_debug_break()
{
	__asm__ __volatile__("NOP\n .word 0x10000000");
}
#		define DEBUG_BREAK() lak_debug_break()
#	elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(__GNUC__)
#		define DEBUG_BREAK() __builtin_trap()
#	else
#		include <signal.h>
#		if defined(SIGTRAP)
#			define DEBUG_BREAK() raise(SIGTRAP)
#		else
#			define DEBUG_BREAK() raise(SIGABRT)
#		endif
#	endif
#endif

#if defined(LAK_COMPILER_EMSCRIPTEN)
#	define force_inline  inline __attribute__((always_inline))
#	define packed_struct struct [[gnu::packed]]
#	define DLL_EXPORT    [[gnu::dllexport]]
#	define UNREACHABLE() __builtin_unreachable()
#elif defined(LAK_COMPILER_CLANG) || defined(LAK_COMPILER_GNUC)
#	define force_inline  inline __attribute__((always_inline))
#	define packed_struct struct [[gnu::packed]]
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
#	define UNREACHABLE() __builtin_unreachable()
#elif defined(LAK_COMPILER_MSVC)
#	include <intrin.h>
#	define force_inline  inline __forceinline
#	define packed_struct struct [[gnu::packed]]
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
#	define UNREACHABLE() __assume(false)
#else
#	error "Compiler not supported"
#endif

#if defined(LAK_ARCH_X86) || defined(LAK_ARCH_X86_64) || defined(LAK_ARCH_IA64)
#	define LAK_ARCH_X86_COMPAT
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
