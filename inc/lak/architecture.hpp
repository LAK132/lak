#ifndef LAK_ARCHITECTURE_HPP
#define LAK_ARCHITECTURE_HPP

#include "lak/array.hpp"
#include "lak/compiler.hpp"

// https://sourceforge.net/p/predef/wiki/Architectures/

namespace lak
{
	enum struct architecture
	{
		unknown = 0,
		x86     = 1,
		x86_64  = 2,
		ia64    = 3,
		arm     = 4,
		aarch64 = 5
#ifdef LAK_ARCH
		,
		compiled = LAK_ARCH
#endif
	};

	lak::architecture host_cpu_architecture();

	lak::result<lak::architecture> host_os_architecture();

	struct instruction_set
	{
		bool MMX;
		bool SSE;
		bool SSE2;
		bool SSE3;
		bool SSSE3;
		bool SSE41;
		bool SSE42;
		bool AVX;
		bool AVX2;
		bool AVX512F;
		bool AVX512PF;
		bool AVX512ER;
		bool AVX512CD;

		static instruction_set get();
	};

	unsigned int max_cpuid_index();

	unsigned int max_extended_cpuid_index();

	// Returns all 0s on non-(x86|x86_64|IA-64) architectures.
	lak::array<unsigned int, 4> cpuid(unsigned int index);
}

#endif
