#include "lak/architecture.hpp"

#if defined(LAK_COMPILER_GNUC) || defined(LAK_COMPILER_CLANG)
#	include <cpuid.h>
#	include <immintrin.h>
#elif defined(LAK_COMPILER_MSVC)
#	include <immintrin.h>
#else
#	error "Compiler not supported"
#endif

lak::architecture lak::host_cpu_architecture()
{
	if constexpr (lak::architecture::compiled == lak::architecture::x86)
	{
		// check if we're actually running on a different CPU that supports x86

		if (((lak::cpuid(1U)[3] >> 30U) & 1U) != 0U)
			// IA-64 processor emulating x86
			return lak::architecture::ia64;

		if (lak::max_extended_cpuid_index() >= 0x80000001U &&
		    ((lak::cpuid(0x80000001U)[3] >> 29U) & 1U) != 0U)
			// CPU is in long-mode (x86_64)
			return lak::architecture::x86_64;
	}

	return lak::architecture::compiled;
}

// https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=msvc-160

lak::instruction_set lak::instruction_set::get()
{
	unsigned int max_index = lak::max_cpuid_index();

	lak::instruction_set result = {};

	if (max_index >= 1)
	{
		auto cpu_info = lak::cpuid(1);
		result.MMX    = ((cpu_info[3] >> 23) & 1U) != 0U;
		result.SSE    = ((cpu_info[3] >> 25) & 1U) != 0U;
		result.SSE2   = ((cpu_info[3] >> 26) & 1U) != 0U;
		result.SSE3   = ((cpu_info[2] >> 0) & 1U) != 0U;
		result.SSSE3  = ((cpu_info[2] >> 9) & 1U) != 0U;
		result.SSE41  = ((cpu_info[2] >> 19) & 1U) != 0U;
		result.SSE42  = ((cpu_info[2] >> 20) & 1U) != 0U;
		result.AVX    = ((cpu_info[2] >> 28) & 1U) != 0U;
	}

	if (max_index >= 7)
	{
		auto cpu_info   = lak::cpuid(7);
		result.AVX2     = ((cpu_info[1] >> 5) & 1U) != 0U;
		result.AVX512F  = ((cpu_info[1] >> 16) & 1U) != 0U;
		result.AVX512PF = ((cpu_info[1] >> 26) & 1U) != 0U;
		result.AVX512ER = ((cpu_info[1] >> 27) & 1U) != 0U;
		result.AVX512CD = ((cpu_info[1] >> 28) & 1U) != 0U;
	}

	return result;
}

unsigned int lak::max_cpuid_index() { return lak::cpuid(0U)[0]; }

unsigned int lak::max_extended_cpuid_index()
{
	return lak::max_cpuid_index() >= 0x80000000U ? lak::cpuid(0x80000000U)[0]
	                                             : 0;
}

lak::array<unsigned int, 4> lak::cpuid(unsigned int index)
{
#if !(defined(LAK_ARCH_X86_64) || defined(LAK_ARCH_X86) ||                    \
      defined(LAK_ARCH_IA64))
	return {0U, 0U, 0U, 0U};
#elif defined(LAK_COMPILER_GNUC) || defined(LAK_COMPILER_CLANG)
	lak::array<unsigned int, 4> info;
	__cpuid_count(index, 0, info[0], info[1], info[2], info[3]);
	return info;
#elif defined(LAK_COMPILER_MSVC)
	int info[4];
	__cpuidex(info, index, 0);
	return {static_cast<unsigned int>(info[0]),
	        static_cast<unsigned int>(info[1]),
	        static_cast<unsigned int>(info[2]),
	        static_cast<unsigned int>(info[3])};
#else
#	error "Compiler not supported"
#endif
}

#ifndef LAK_DONT_AUTO_COMPILE_PLATFORM_SPECIFICS
#	if defined(LAK_OS_WINDOWS)
#		include "win32/architecture.cpp"
#	endif

#	if defined(LAK_OS_LINUX)
#		include "linux/architecture.cpp"
#	endif

#	if defined(LAK_OS_APPLE)
#		include "macos/architecture.cpp"
#	endif
#endif
