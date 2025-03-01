#include "lak/test.hpp"

#include "lak/ebpf/vm.hpp"

BEGIN_TEST(ebpf_vm)
{
#define UNPAIR(...) __VA_ARGS__.first, __VA_ARGS__.second

	{
		SCOPED_CHECKPOINT("basic load");

		lak::array<lak::ebpf::instruction_t> program = {
		  UNPAIR(
		    lak::ebpf::instruction::make_load(lak::ebpf::register_t::R0, 132U)),
		  lak::ebpf::instruction::make_exit(),
		};

		lak::ebpf::vm vm;

		ASSERT_EQUAL(vm.run_program(program).UNWRAP(), 132U);
	}

	{
		SCOPED_CHECKPOINT("basic math");

		lak::array<lak::ebpf::instruction_t> program = {
		  UNPAIR(
		    lak::ebpf::instruction::make_load(lak::ebpf::register_t::R1, 179U)),
		  UNPAIR(lak::ebpf::instruction::make_load(lak::ebpf::register_t::R2,
		                                           179U - 132U)),
		  lak::ebpf::instruction::make(lak::ebpf::opcode_class_alu::ALU64,
		                               lak::ebpf::opcode_alu::SUB,
		                               lak::ebpf::register_t::R1,
		                               lak::ebpf::register_t::R2),
		  lak::ebpf::instruction::make(lak::ebpf::opcode_class_alu::ALU64,
		                               lak::ebpf::opcode_alu::MOV,
		                               lak::ebpf::register_t::R0,
		                               lak::ebpf::register_t::R1),
		  lak::ebpf::instruction::make_exit(),
		};

		lak::ebpf::vm vm;

		ASSERT_EQUAL(vm.run_program(program).UNWRAP(), 132U);
	}

	{
		SCOPED_CHECKPOINT("external call");

		lak::array<lak::ebpf::instruction_t> program = {
		  UNPAIR(
		    lak::ebpf::instruction::make_load(lak::ebpf::register_t::R1, 100U)),
		  UNPAIR(
		    lak::ebpf::instruction::make_load(lak::ebpf::register_t::R2, 30U)),
		  UNPAIR(lak::ebpf::instruction::make_load(lak::ebpf::register_t::R3, 2U)),
		  lak::ebpf::instruction::make_agnostic_helper_call(0U),
		  lak::ebpf::instruction::make_exit(),
		};

		lak::ebpf::vm vm;

		vm.agnostic_helper.push_back(+[](lak::ebpf::vm *,
		                                 uint64_t &r0,
		                                 uint64_t &r1,
		                                 uint64_t &r2,
		                                 uint64_t &r3,
		                                 uint64_t &,
		                                 uint64_t &,
		                                 uint64_t,
		                                 uint64_t,
		                                 uint64_t,
		                                 uint64_t,
		                                 uint64_t) { r0 = r1 + r2 + r3; });

		ASSERT_EQUAL(vm.run_program(program).UNWRAP(), 132U);
	}

	return 0;
}
END_TEST()
