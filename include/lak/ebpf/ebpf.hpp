#ifndef LAK_EBPF_EBPF_HPP
#define LAK_EBPF_EBPF_HPP

#include "lak/stdint.hpp"
#include "lak/string.hpp"
#include "lak/tuple.hpp"

namespace lak
{
	namespace ebpf
	{
#define LAK_FOREACH_EBPF_OPCODE_CLASS_LOAD_TYPE(MACRO, ...)                   \
	MACRO(0x00U, LD, __VA_ARGS__)  /*non-standard load operations*/             \
	MACRO(0x01U, LDX, __VA_ARGS__) /*load into register operations*/
#define LAK_FOREACH_EBPF_OPCODE_CLASS_STORE_TYPE(MACRO, ...)                  \
	MACRO(0x02U, ST, __VA_ARGS__)  /*store from immediate operations*/          \
	MACRO(0x03U, STX, __VA_ARGS__) /*store from register operations*/
#define LAK_FOREACH_EBPF_OPCODE_CLASS_ALU_TYPE(MACRO, ...)                    \
	MACRO(0x04U, ALU, __VA_ARGS__)                                              \
	MACRO(0x07U, ALU64, __VA_ARGS__)
#define LAK_FOREACH_EBPF_OPCODE_CLASS_JUMP_TYPE(MACRO, ...)                   \
	MACRO(0x05U, JMP, __VA_ARGS__)                                              \
	MACRO(0x06U, JMP32, __VA_ARGS__)

#define LAK_FOREACH_EBPF_OPCODE_LOAD_STORE_SIZE_TYPE(MACRO, ...)              \
	MACRO(0x00U, W, __VA_ARGS__)  /*word (uint32_t)*/                           \
	MACRO(0x08U, H, __VA_ARGS__)  /*half word (uint16_t)*/                      \
	MACRO(0x10U, B, __VA_ARGS__)  /*byte (uint8_t)*/                            \
	MACRO(0x18U, DW, __VA_ARGS__) /*double word (uint64_t)*/
#define LAK_FOREACH_EBPF_OPCODE_LOAD_STORE_MODE_TYPE(MACRO, ...)              \
	MACRO(0x00U, IMM, __VA_ARGS__)                                              \
	MACRO(0x20U, ABS, __VA_ARGS__)                                              \
	MACRO(0x40U, IND, __VA_ARGS__)                                              \
	MACRO(0x60U, MEM, __VA_ARGS__)                                              \
	MACRO(0x80U, MEMSX, __VA_ARGS__)                                            \
	MACRO(0xC0U, ATOMIC, __VA_ARGS__)

#define LAK_FOREACH_EBPF_OPCODE_ATOMIC_TYPE(MACRO, ...)                       \
	MACRO(0x00U, ADD, __VA_ARGS__)                                              \
	MACRO(0x40U, OR, __VA_ARGS__)                                               \
	MACRO(0x50U, AND, __VA_ARGS__)                                              \
	MACRO(0xA0U, XOR, __VA_ARGS__)                                              \
	MACRO(0x01U, FETCH_ADD, __VA_ARGS__)                                        \
	MACRO(0x41U, FETCH_OR, __VA_ARGS__)                                         \
	MACRO(0x51U, FETCH_AND, __VA_ARGS__)                                        \
	MACRO(0xA1U, FETCH_XOR, __VA_ARGS__)                                        \
	MACRO(0xE1U, XCHG, __VA_ARGS__)                                             \
	MACRO(0xF1U, CMPXCHG, __VA_ARGS__)

#define LAK_FOREACH_EBPF_OPCODE_ALU_JUMP_SOURCE_TYPE(MACRO, ...)              \
	MACRO(0x00U, K, __VA_ARGS__) /*use immediate as source*/                    \
	MACRO(0x08U, X, __VA_ARGS__) /*use source register as source*/

#define LAK_FOREACH_EBPF_OPCODE_ALU_ENDIAN_TYPE(MACRO, ...)                   \
	MACRO(0x00U, TO_LE, __VA_ARGS__)                                            \
	MACRO(0x08U, TO_BE, __VA_ARGS__)

#define LAK_FOREACH_EBPF_OPCODE_ALU_TYPE(MACRO, ...)                          \
	MACRO(0x00U, ADD, __VA_ARGS__)                                              \
	MACRO(0x10U, SUB, __VA_ARGS__)                                              \
	MACRO(0x20U, MUL, __VA_ARGS__)                                              \
	MACRO(0x30U, DIV, __VA_ARGS__)                                              \
	MACRO(0x40U, OR, __VA_ARGS__)                                               \
	MACRO(0x50U, AND, __VA_ARGS__)                                              \
	MACRO(0x60U, LSH, __VA_ARGS__)                                              \
	MACRO(0x70U, RSH, __VA_ARGS__)                                              \
	MACRO(0x80U, NEG, __VA_ARGS__)                                              \
	MACRO(0x90U, MOD, __VA_ARGS__)                                              \
	MACRO(0xA0U, XOR, __VA_ARGS__)                                              \
	MACRO(0xB0U, MOV, __VA_ARGS__)                                              \
	MACRO(0xC0U, ARSH, __VA_ARGS__)                                             \
	MACRO(0xD0U, END, __VA_ARGS__)

#define LAK_FOREACH_EBPF_OPCODE_JUMP_TYPE(MACRO, ...)                         \
	MACRO(0x00U, JA, __VA_ARGS__)                                               \
	MACRO(0x10U, JEQ, __VA_ARGS__)                                              \
	MACRO(0x20U, JGT, __VA_ARGS__)                                              \
	MACRO(0x30U, JGE, __VA_ARGS__)                                              \
	MACRO(0x40U, JSET, __VA_ARGS__)                                             \
	MACRO(0x50U, JNE, __VA_ARGS__)                                              \
	MACRO(0x60U, JSGT, __VA_ARGS__)                                             \
	MACRO(0x70U, JSGE, __VA_ARGS__)                                             \
	MACRO(0x80U, CALL, __VA_ARGS__)                                             \
	MACRO(0x90U, EXIT, __VA_ARGS__)                                             \
	MACRO(0xA0U, JLT, __VA_ARGS__)                                              \
	MACRO(0xB0U, JLE, __VA_ARGS__)                                              \
	MACRO(0xC0U, JSLT, __VA_ARGS__)                                             \
	MACRO(0xD0U, JSLE, __VA_ARGS__)
#define LAK_FOREACH_EBPF_OPCODE_CALL_TYPE(MACRO, ...)                         \
	MACRO(0x00U, AGNOSTIC, __VA_ARGS__)                                         \
	MACRO(0x01U, LOCAL, __VA_ARGS__)                                            \
	MACRO(0x02U, SPECIFIC, __VA_ARGS__)

#define LAK_FOREACH_EBPF_REGISTER_TYPE(MACRO, ...)                            \
	/* return value */                                                          \
	MACRO(0x00U, R0, __VA_ARGS__)                                               \
	/* function call arguments */                                               \
	MACRO(0x01U, R1, __VA_ARGS__)                                               \
	MACRO(0x02U, R2, __VA_ARGS__)                                               \
	MACRO(0x03U, R3, __VA_ARGS__)                                               \
	MACRO(0x04U, R4, __VA_ARGS__)                                               \
	MACRO(0x05U, R5, __VA_ARGS__)                                               \
	/* callee saved registers that function calls will preserve */              \
	MACRO(0x06U, R6, __VA_ARGS__)                                               \
	MACRO(0x07U, R7, __VA_ARGS__)                                               \
	MACRO(0x08U, R8, __VA_ARGS__)                                               \
	MACRO(0x09U, R9, __VA_ARGS__)                                               \
	/* read-only frame pointer to access stack */                               \
	MACRO(0x0AU, R10, __VA_ARGS__)

#define LAK_EBPF_ENUM(VAL, NAME, ...) NAME = VAL,

		enum struct opcode_class_load : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_CLASS_LOAD_TYPE(LAK_EBPF_ENUM)
		};
		enum struct opcode_class_store : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_CLASS_STORE_TYPE(LAK_EBPF_ENUM)
		};

		enum struct opcode_load_store_size : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_LOAD_STORE_SIZE_TYPE(LAK_EBPF_ENUM)
		};
		enum struct opcode_load_store_mode : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_LOAD_STORE_MODE_TYPE(LAK_EBPF_ENUM)
		};
		enum struct opcode_atomic : uint8_t
		{
			// clang-format off
			LAK_FOREACH_EBPF_OPCODE_ATOMIC_TYPE(LAK_EBPF_ENUM)
			FETCH = FETCH_ADD,
			// clang-format on
		};

		enum struct opcode_class_alu : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_CLASS_ALU_TYPE(LAK_EBPF_ENUM)
		};
		enum struct opcode_class_jump : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_CLASS_JUMP_TYPE(LAK_EBPF_ENUM)
		};

		enum struct opcode_alu_jump_source : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_ALU_JUMP_SOURCE_TYPE(LAK_EBPF_ENUM)
		};
		enum struct opcode_alu_endian : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_ALU_ENDIAN_TYPE(LAK_EBPF_ENUM)
		};
		enum struct opcode_alu : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_ALU_TYPE(LAK_EBPF_ENUM)
		};
		enum struct opcode_jump : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_JUMP_TYPE(LAK_EBPF_ENUM)
		};
		enum struct opcode_jump_call : uint8_t
		{
			LAK_FOREACH_EBPF_OPCODE_CALL_TYPE(LAK_EBPF_ENUM)
		};

		enum struct opcode_class_t : uint8_t
		{
			// clang-format off
			LAK_FOREACH_EBPF_OPCODE_CLASS_LOAD_TYPE(LAK_EBPF_ENUM)
			LAK_FOREACH_EBPF_OPCODE_CLASS_STORE_TYPE(LAK_EBPF_ENUM)
			LAK_FOREACH_EBPF_OPCODE_CLASS_ALU_TYPE(LAK_EBPF_ENUM)
			LAK_FOREACH_EBPF_OPCODE_CLASS_JUMP_TYPE(LAK_EBPF_ENUM)
			// clang-format on
		};

		enum struct opcode_t : uint8_t
		{
		};

		enum struct register_t : uint8_t
		{
			LAK_FOREACH_EBPF_REGISTER_TYPE(LAK_EBPF_ENUM)
		};

		enum struct instruction_t : uint64_t
		{
			// instruction layout:
			// LSB                                    MSB
			// u8 opcode, u4 dst, u4 src, u16 offset, u32 immediate
		};

#undef LAK_EBPF_ENUM

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

			opcode_class_t opcode_class : 3U;
			union
			{
				_ld_st ld_st;
				_alu_jmp alu_jmp;
			};
			uint8_t dst : 4U;
			uint8_t src : 4U;
			uint16_t offset;
			uint32_t immediate;

			constexpr bool is_load() const;
			constexpr bool is_store() const;
			constexpr bool is_alu() const;
			constexpr bool is_jump() const;

			constexpr opcode_load_store_size size() const;
			constexpr opcode_load_store_mode mode() const;
			constexpr opcode_alu_jump_source source() const;
			constexpr opcode_alu_endian endian() const;
			constexpr uint8_t code() const;
			constexpr opcode_alu alu_code() const;
			constexpr opcode_jump jump_code() const;
			constexpr opcode_jump_call call_src() const;
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
			                                  uint16_t offset,
			                                  uint32_t immediate);

			static constexpr instruction make(opcode_class_alu alu,
			                                  opcode_alu code,
			                                  register_t dst,
			                                  register_t src,
			                                  uint16_t offset);
			static constexpr instruction make(opcode_class_alu alu,
			                                  opcode_alu code,
			                                  register_t dst,
			                                  uint16_t offset,
			                                  uint32_t immediate);

			static constexpr instruction make(opcode_class_alu alu,
			                                  opcode_alu code,
			                                  register_t dst,
			                                  register_t src);
			static constexpr instruction make(opcode_class_alu alu,
			                                  opcode_alu code,
			                                  register_t dst,
			                                  uint32_t immediate);

			static constexpr instruction make_end(opcode_alu_endian endian,
			                                      register_t dst,
			                                      uint32_t immediate);
			static constexpr instruction make_end16(opcode_alu_endian endian,
			                                        register_t dst);
			static constexpr instruction make_end32(opcode_alu_endian endian,
			                                        register_t dst);
			static constexpr instruction make_end64(opcode_alu_endian endian,
			                                        register_t dst);
			static constexpr instruction make_bswap(register_t dst,
			                                        uint32_t immediate);
			static constexpr instruction make_bswap16(register_t dst);
			static constexpr instruction make_bswap32(register_t dst);
			static constexpr instruction make_bswap64(register_t dst);

			static constexpr instruction make_sdiv(opcode_class_alu alu,
			                                       register_t dst,
			                                       register_t src);
			static constexpr instruction make_sdiv(opcode_class_alu alu,
			                                       register_t dst,
			                                       uint32_t immediate);

			static constexpr instruction make_smod(opcode_class_alu alu,
			                                       register_t dst,
			                                       register_t src);
			static constexpr instruction make_smod(opcode_class_alu alu,
			                                       register_t dst,
			                                       uint32_t immediate);

			static constexpr instruction make_movsx(opcode_class_alu alu,
			                                        register_t dst,
			                                        register_t src,
			                                        uint16_t offset);
			static constexpr instruction make_movsx8(opcode_class_alu alu,
			                                         register_t dst,
			                                         register_t src);
			static constexpr instruction make_movsx16(opcode_class_alu alu,
			                                          register_t dst,
			                                          register_t src);
			static constexpr instruction make_movsx32(opcode_class_alu alu,
			                                          register_t dst,
			                                          register_t src);

			/* --- jump -- */

			static constexpr instruction make(opcode_class_jump jump,
			                                  opcode_alu_jump_source source,
			                                  opcode_jump code,
			                                  register_t dst,
			                                  register_t src,
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
			                                  uint16_t offset,
			                                  uint32_t immediate);
			static constexpr instruction make_jump16(uint16_t offset);
			static constexpr instruction make_jump32(uint32_t immediate);
			static constexpr instruction make(opcode_jump_call src,
			                                  uint32_t immediate);
			static constexpr instruction make_agnostic_helper_call(
			  uint32_t immediate);
			static constexpr instruction make_local_call(uint32_t immedate);
			static constexpr instruction make_specific_helper_call(
			  uint32_t immediate);
			static constexpr instruction make_exit();

			constexpr operator instruction_t() const;

			constexpr instruction_t pack() const { return operator instruction_t(); }

			operator lak::u8string() const;
		};
	}
}

#include "lak/ebpf/ebpf.inl"

#endif
