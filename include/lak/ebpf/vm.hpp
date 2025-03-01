#ifndef LAK_EBPF_VM_HPP
#define LAK_EBPF_VM_HPP

#include "lak/ebpf/ebpf.hpp"

#include "lak/array.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

namespace lak
{
	namespace ebpf
	{
		struct vm
		{
			uint32_t stack_size = 1024U;
			using platform_func = void (*)(vm *,
			                               uint64_t &r0,
			                               uint64_t &r1,
			                               uint64_t &r2,
			                               uint64_t &r3,
			                               uint64_t &r4,
			                               uint64_t &r5,
			                               uint64_t r6,
			                               uint64_t r7,
			                               uint64_t r8,
			                               uint64_t r9,
			                               uint64_t r10);
			lak::array<platform_func> agnostic_helper;
			lak::array<platform_func> specific_helper;

			lak::result<uint64_t, size_t> run_program(
			  lak::span<instruction_t> program,
			  uint64_t r1 = 0,
			  uint64_t r2 = 0,
			  uint64_t r3 = 0,
			  uint64_t r4 = 0,
			  uint64_t r5 = 0);
		};
	}
}

#endif
