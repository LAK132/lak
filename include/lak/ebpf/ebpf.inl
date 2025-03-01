#include "lak/ebpf/ebpf.hpp"

constexpr lak::ebpf::opcode_load_store_size lak::ebpf::instruction::size()
  const
{
	return static_cast<opcode_load_store_size>(uint8_t(ld_st.size) << 3U);
}

constexpr lak::ebpf::opcode_load_store_mode lak::ebpf::instruction::mode()
  const
{
	return static_cast<opcode_load_store_mode>(uint8_t(ld_st.mode) << 5U);
}

constexpr lak::ebpf::opcode_alu_jump_source lak::ebpf::instruction::source()
  const
{
	return static_cast<opcode_alu_jump_source>(uint8_t(alu_jmp.source) << 3U);
}

constexpr uint8_t lak::ebpf::instruction::code() const
{
	return uint8_t(alu_jmp.code) << 4U;
}

constexpr lak::ebpf::opcode_alu lak::ebpf::instruction::alu_code() const
{
	return static_cast<opcode_alu>(code());
}

constexpr lak::ebpf::opcode_jump lak::ebpf::instruction::jump_code() const
{
	return static_cast<opcode_jump>(code());
}

constexpr lak::ebpf::opcode_atomic lak::ebpf::instruction::atomic_code() const
{
	return static_cast<opcode_atomic>(immediate);
}

constexpr lak::ebpf::register_t lak::ebpf::instruction::dst_reg() const
{
	return static_cast<register_t>(dst);
}

constexpr lak::ebpf::register_t lak::ebpf::instruction::src_reg() const
{
	return static_cast<register_t>(src);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  lak::ebpf::instruction_t packed)
{
	uint64_t inst        = static_cast<uint64_t>(packed);
	uint8_t opcode_class = static_cast<uint8_t>(inst) & 0b111U;
	if (opcode_class <= 0x03U)
		return {
		  .opcode_class = opcode_class,
		  .ld_st =
		    {
		      .size = static_cast<uint8_t>((inst >> 3U) & 0b11U),
		      .mode = static_cast<uint8_t>((inst >> 5U) & 0b111U),
		    },
		  .dst       = static_cast<uint8_t>((inst >> 8U) & 0b1111U),
		  .src       = static_cast<uint8_t>((inst >> 12U) & 0b1111U),
		  .offset    = static_cast<uint16_t>(inst >> 16U),
		  .immediate = static_cast<uint32_t>(inst >> 32U),
		};
	else
		return {
		  .opcode_class = opcode_class,
		  .alu_jmp =
		    {
		      .source = static_cast<uint8_t>((inst >> 3U) & 0b1U),
		      .code   = static_cast<uint8_t>((inst >> 4U) & 0b1111U),
		    },
		  .dst       = static_cast<uint8_t>((inst >> 8U) & 0b1111U),
		  .src       = static_cast<uint8_t>((inst >> 12U) & 0b1111U),
		  .offset    = static_cast<uint16_t>(inst >> 16U),
		  .immediate = static_cast<uint32_t>(inst >> 32U),
		};
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_immediate(
  uint32_t immediate)
{
	return {
	  .opcode_class = 0U,
	  .ld_st =
	    {
	      .size = 0U,
	      .mode = 0U,
	    },
	  .dst       = 0U,
	  .src       = 0U,
	  .offset    = 0U,
	  .immediate = immediate,
	};
}

/* --- load --- */

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_load load,
  opcode_load_store_size size,
  opcode_load_store_mode mode,
  register_t dst,
  uint8_t src,
  uint16_t offset,
  uint32_t immediate)
{
	return {
	  .opcode_class = static_cast<uint8_t>(load),
	  .ld_st =
	    {
	      .size = static_cast<uint8_t>(static_cast<uint8_t>(size) >> 3U),
	      .mode = static_cast<uint8_t>(static_cast<uint8_t>(mode) >> 5U),
	    },
	  .dst       = static_cast<uint8_t>(dst),
	  .src       = src,
	  .offset    = offset,
	  .immediate = immediate,
	};
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_load load,
  opcode_load_store_size size,
  opcode_load_store_mode mode,
  register_t dst,
  register_t src,
  uint16_t offset,
  uint32_t immediate)
{
	return make(
	  load, size, mode, dst, static_cast<uint8_t>(src), offset, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_load(
  opcode_load_store_size size, register_t dst, register_t src, uint16_t offset)
{
	return make(opcode_class_load::LDX,
	            size,
	            opcode_load_store_mode::MEM,
	            dst,
	            src,
	            offset,
	            0U);
}

constexpr lak::pair<lak::ebpf::instruction, lak::ebpf::instruction>
lak::ebpf::instruction::make_load(register_t dst, uint64_t immediate)
{
	return {make(opcode_class_load::LD,
	             opcode_load_store_size::DW,
	             opcode_load_store_mode::IMM,
	             dst,
	             0U,
	             0U,
	             static_cast<uint32_t>(immediate)),
	        make_immediate(static_cast<uint32_t>(immediate >> 32U))};
}

constexpr lak::pair<lak::ebpf::instruction, lak::ebpf::instruction>
lak::ebpf::instruction::make_load(register_t dst,
                                  uint8_t src,
                                  uint32_t immediate,
                                  uint32_t next_immediate)
{
	return {make(opcode_class_load::LD,
	             opcode_load_store_size::DW,
	             opcode_load_store_mode::IMM,
	             dst,
	             src,
	             0U,
	             immediate),
	        make_immediate(next_immediate)};
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_load(
  register_t dst, uint8_t src, uint32_t immediate)
{
	return make(opcode_class_load::LD,
	            opcode_load_store_size::DW,
	            opcode_load_store_mode::IMM,
	            dst,
	            src,
	            0U,
	            immediate);
}

/* --- store --- */

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_store store,
  opcode_load_store_size size,
  opcode_load_store_mode mode,
  register_t dst,
  uint8_t src,
  uint16_t offset,
  uint32_t immediate)
{
	return {
	  .opcode_class = static_cast<uint8_t>(store),
	  .ld_st =
	    {
	      .size = static_cast<uint8_t>(static_cast<uint8_t>(size) >> 3U),
	      .mode = static_cast<uint8_t>(static_cast<uint8_t>(mode) >> 5U),
	    },
	  .dst       = static_cast<uint8_t>(dst),
	  .src       = src,
	  .offset    = offset,
	  .immediate = immediate,
	};
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_store store,
  opcode_load_store_size size,
  opcode_load_store_mode mode,
  register_t dst,
  register_t src,
  uint16_t offset,
  uint32_t immediate)
{
	return make(
	  store, size, mode, dst, static_cast<uint8_t>(src), offset, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_store(
  opcode_load_store_size size, register_t dst, uint16_t offset, register_t src)
{
	return make(opcode_class_store::STX,
	            size,
	            opcode_load_store_mode::MEM,
	            dst,
	            src,
	            offset,
	            0U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_store(
  opcode_load_store_size size,
  register_t dst,
  uint16_t offset,
  uint32_t immediate)
{
	return make(opcode_class_store::ST,
	            size,
	            opcode_load_store_mode::MEM,
	            dst,
	            0U,
	            offset,
	            immediate);
}

/* --- alu --- */

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_alu alu,
  opcode_alu_jump_source source,
  opcode_alu code,
  register_t dst,
  register_t src,
  uint32_t immediate)
{
	return {
	  .opcode_class = static_cast<uint8_t>(alu),
	  .alu_jmp =
	    {
	      .source = static_cast<uint8_t>(static_cast<uint8_t>(source) >> 3U),
	      .code   = static_cast<uint8_t>(static_cast<uint8_t>(code) >> 4U),
	    },
	  .dst       = static_cast<uint8_t>(dst),
	  .src       = static_cast<uint8_t>(src),
	  .offset    = 0U,
	  .immediate = immediate,
	};
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_alu alu, opcode_alu code, register_t dst, register_t src)
{
	return make(alu, opcode_alu_jump_source::X, code, dst, src, 0U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_alu alu, opcode_alu code, register_t dst, uint32_t immediate)
{
	return make(
	  alu, opcode_alu_jump_source::K, code, dst, register_t::R0, immediate);
}

/* --- jump -- */

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_jump jump,
  opcode_alu_jump_source source,
  opcode_jump code,
  register_t dst,
  uint8_t src,
  uint16_t offset,
  uint32_t immediate)
{
	return {
	  .opcode_class = static_cast<uint8_t>(jump),
	  .alu_jmp =
	    {
	      .source = static_cast<uint8_t>(static_cast<uint8_t>(source) >> 3U),
	      .code   = static_cast<uint8_t>(static_cast<uint8_t>(code) >> 4U),
	    },
	  .dst       = static_cast<uint8_t>(dst),
	  .src       = src,
	  .offset    = offset,
	  .immediate = immediate,
	};
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_jump jump,
  opcode_jump code,
  register_t dst,
  register_t src,
  uint16_t offset)
{
	return make(jump,
	            opcode_alu_jump_source::X,
	            code,
	            dst,
	            static_cast<uint8_t>(src),
	            offset,
	            0U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_jump jump,
  opcode_jump code,
  register_t dst,
  uint32_t immediate,
  uint16_t offset)
{
	return make(
	  jump, opcode_alu_jump_source::K, code, dst, 0U, offset, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_jump(
  uint16_t offset)
{
	return make(opcode_class_jump::JMP,
	            opcode_alu_jump_source::K,
	            opcode_jump::JA,
	            register_t::R0,
	            0U,
	            offset,
	            0U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_jump(
  uint32_t immediate)
{
	return make(opcode_class_jump::JMP32,
	            opcode_alu_jump_source::K,
	            opcode_jump::JA,
	            register_t::R0,
	            0U,
	            0U,
	            immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_call(
  uint8_t src, uint32_t immediate)
{
	return make(opcode_class_jump::JMP,
	            opcode_alu_jump_source::K,
	            opcode_jump::CALL,
	            register_t::R0,
	            src,
	            0U,
	            immediate);
}

constexpr lak::ebpf::instruction
lak::ebpf::instruction::make_agnostic_helper_call(uint32_t immediate)
{
	return make_call(0U, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_local_call(
  uint32_t immediate)
{
	return make_call(1U, immediate);
}

constexpr lak::ebpf::instruction
lak::ebpf::instruction::make_specific_helper_call(uint32_t immediate)
{
	return make_call(2U, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_exit()
{
	return make(opcode_class_jump::JMP,
	            opcode_alu_jump_source::K,
	            opcode_jump::EXIT,
	            register_t::R0,
	            0U,
	            0U,
	            0U);
}

constexpr lak::ebpf::instruction::operator lak::ebpf::instruction_t() const
{
	uint8_t opcode = opcode_class <= 0x03U
	                   ? uint8_t(opcode_class) | (uint8_t(ld_st.size) << 3U) |
	                       (uint8_t(ld_st.mode) << 5U)
	                   : uint8_t(opcode_class) |
	                       (uint8_t(alu_jmp.source) << 3U) |
	                       (uint8_t(alu_jmp.code) << 4U);
	return static_cast<instruction_t>((static_cast<uint64_t>(immediate) << 32U) |
	                                  (static_cast<uint64_t>(offset) << 16U) |
	                                  ((static_cast<uint64_t>(src)) << 12U) |
	                                  ((static_cast<uint64_t>(dst)) << 8U) |
	                                  opcode);
}
