#ifndef LAK_EBPF_EBPF_HPP
#define LAK_EBPF_EBPF_HPP

#include "lak/stdint.hpp"
#include "lak/string.hpp"
#include "lak/tuple.hpp"

namespace lak
{
	namespace ebpf
	{
		enum struct opcode_class_load : uint8_t
		{
			LD  = 0x00U, // non-standard load operations
			LDX = 0x01U, // load into register operations
		};
		enum struct opcode_class_store : uint8_t
		{
			ST  = 0x02U, // store from immediate operations
			STX = 0x03U, // store from register operations
		};

		enum struct opcode_load_store_size : uint8_t
		{
			W  = 0x00U, // word (uint32_t)
			H  = 0x08U, // half word (uint16_t)
			B  = 0x10U, // byte (uint8_t)
			DW = 0x18U, // double word (uint64_t)
		};
		enum struct opcode_load_store_mode : uint8_t
		{
			IMM    = 0x00U,
			ABS    = 0x20U,
			IND    = 0x40U,
			MEM    = 0x60U,
			MEMSX  = 0x80U,
			ATOMIC = 0xC0U,
		};
		enum struct opcode_atomic : uint8_t
		{
			ADD = 0x00U,
			OR  = 0x40U,
			AND = 0x50U,
			XOR = 0xA0U,

			FETCH = 0x01U,

			FETCH_ADD = ADD | FETCH,
			FETCH_OR  = OR | FETCH,
			FETCH_AND = AND | FETCH,
			FETCH_XOR = XOR | FETCH,

			XCHG    = 0xE0U | FETCH,
			CMPXCHG = 0xF0U | FETCH,
		};

		enum struct opcode_class_alu : uint8_t
		{
			ALU   = 0x04U,
			ALU64 = 0x07U,
		};
		enum struct opcode_class_jump : uint8_t
		{
			JMP   = 0x05U,
			JMP32 = 0x06U,
		};

		enum struct opcode_alu_jump_source : uint8_t
		{
			K = 0x00U, // use immediate as source
			X = 0x08U, // use source register as source
		};
		enum struct opcode_alu : uint8_t
		{
			ADD  = 0x00U,
			SUB  = 0x10U,
			MUL  = 0x20U,
			DIV  = 0x30U,
			OR   = 0x40U,
			AND  = 0x50U,
			LSH  = 0x60U,
			RSH  = 0x70U,
			NEG  = 0x80U,
			MOD  = 0x90U,
			XOR  = 0xA0U,
			MOV  = 0xB0U,
			ARSH = 0xC0U,
			END  = 0xD0U,
		};
		enum struct opcode_jump : uint8_t
		{
			JA   = 0x00U,
			JEQ  = 0x10U,
			JGT  = 0x20U,
			JGE  = 0x30U,
			JSET = 0x40U,
			JNE  = 0x50U,
			JSGT = 0x60U,
			JSGE = 0x70U,
			CALL = 0x80U,
			EXIT = 0x90U,
			JLT  = 0xA0U,
			JLE  = 0xB0U,
			JSLT = 0xC0U,
			JSLE = 0xD0U,
		};

		enum struct opcode_t : uint8_t
		{
		};

		enum struct register_t : uint8_t
		{
			// return value
			R0 = 0,

			// function call arguments
			R1 = 1,
			R2 = 2,
			R3 = 3,
			R4 = 4,
			R5 = 5,

			// callee saved registers that function calls will preserve
			R6 = 6,
			R7 = 7,
			R8 = 8,
			R9 = 9,

			// read-only frame pointer to access stack
			R10 = 10,
		};

		enum struct instruction_t : uint64_t
		{
			// instruction layout:
			// LSB                                    MSB
			// u8 opcode, u4 dst, u4 src, u16 offset, u32 immediate
		};

		struct instruction
		{
			struct _ld_st
			{
				uint8_t size : 2U;
				uint8_t mode : 3U;
			};
			struct _alu_jmp
			{
				uint8_t source : 1U;
				uint8_t code : 4U;
			};

			uint8_t opcode_class : 3U;
			union
			{
				_ld_st ld_st;
				_alu_jmp alu_jmp;
			};
			uint8_t dst : 4U;
			uint8_t src : 4U;
			uint16_t offset;
			uint32_t immediate;

			constexpr opcode_load_store_size size() const;
			constexpr opcode_load_store_mode mode() const;
			constexpr opcode_alu_jump_source source() const;
			constexpr uint8_t code() const;
			constexpr opcode_alu alu_code() const;
			constexpr opcode_jump jump_code() const;
			constexpr opcode_atomic atomic_code() const;
			constexpr register_t dst_reg() const;
			constexpr register_t src_reg() const;

			static constexpr instruction make(instruction_t packed);

			static constexpr instruction make_immediate(uint32_t immedate);

			/* --- load --- */

			static constexpr instruction make(opcode_class_load load,
			                                  opcode_load_store_size size,
			                                  opcode_load_store_mode mode,
			                                  register_t dst,
			                                  uint8_t src,
			                                  uint16_t offset,
			                                  uint32_t immediate);
			static constexpr instruction make(opcode_class_load load,
			                                  opcode_load_store_size size,
			                                  opcode_load_store_mode mode,
			                                  register_t dst,
			                                  register_t src,
			                                  uint16_t offset,
			                                  uint32_t immediate);
			static constexpr instruction make_load(opcode_load_store_size size,
			                                       register_t dst,
			                                       register_t src,
			                                       uint16_t offset);
			static constexpr lak::pair<instruction, instruction> make_load(
			  register_t dst, uint64_t immediate);
			static constexpr lak::pair<instruction, instruction> make_load(
			  register_t dst,
			  uint8_t src,
			  uint32_t immediate,
			  uint32_t next_immediate);
			static constexpr instruction make_load(register_t dst,
			                                       uint8_t src,
			                                       uint32_t immediate);

			/* --- store --- */

			static constexpr instruction make(opcode_class_store store,
			                                  opcode_load_store_size size,
			                                  opcode_load_store_mode mode,
			                                  register_t dst,
			                                  uint8_t src,
			                                  uint16_t offset,
			                                  uint32_t immediate);
			static constexpr instruction make(opcode_class_store store,
			                                  opcode_load_store_size size,
			                                  opcode_load_store_mode mode,
			                                  register_t dst,
			                                  register_t src,
			                                  uint16_t offset,
			                                  uint32_t immediate);
			static constexpr instruction make_store(opcode_load_store_size size,
			                                        register_t dst,
			                                        uint16_t offset,
			                                        register_t src);
			static constexpr instruction make_store(opcode_load_store_size size,
			                                        register_t dst,
			                                        uint16_t offset,
			                                        uint32_t immediate);

			/* --- alu --- */

			static constexpr instruction make(opcode_class_alu alu,
			                                  opcode_alu_jump_source source,
			                                  opcode_alu code,
			                                  register_t dst,
			                                  register_t src,
			                                  uint32_t immediate);
			static constexpr instruction make(opcode_class_alu alu,
			                                  opcode_alu code,
			                                  register_t dst,
			                                  register_t src);
			static constexpr instruction make(opcode_class_alu alu,
			                                  opcode_alu code,
			                                  register_t dst,
			                                  uint32_t immediate);

			/* --- jump -- */

			static constexpr instruction make(opcode_class_jump jump,
			                                  opcode_alu_jump_source source,
			                                  opcode_jump code,
			                                  register_t dst,
			                                  uint8_t src,
			                                  uint16_t offset,
			                                  uint32_t immediate);
			static constexpr instruction make(opcode_class_jump jump,
			                                  opcode_jump code,
			                                  register_t dst,
			                                  register_t src,
			                                  uint16_t offset);
			static constexpr instruction make(opcode_class_jump jump,
			                                  opcode_jump code,
			                                  register_t dst,
			                                  uint32_t immediate,
			                                  uint16_t offset);
			static constexpr instruction make_jump(uint16_t offset);
			static constexpr instruction make_jump(uint32_t immediate);
			static constexpr instruction make_call(uint8_t src, uint32_t immediate);
			static constexpr instruction make_agnostic_helper_call(
			  uint32_t immediate);
			static constexpr instruction make_local_call(uint32_t immedate);
			static constexpr instruction make_specific_helper_call(
			  uint32_t immediate);
			static constexpr instruction make_exit();

			constexpr operator instruction_t() const;

			constexpr instruction_t pack() const { return operator instruction_t(); }
		};
	}
}

#include "lak/ebpf/ebpf.inl"

#endif
