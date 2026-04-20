#include "lak/ebpf/ebpf.hpp"

#define LAK_EBPF_ENUM_CMP(VAL, NAME, LHS, TYPE, ...) LHS == TYPE::NAME ||

constexpr bool lak::ebpf::instruction::is_load() const
{
	return LAK_FOREACH_EBPF_OPCODE_CLASS_LOAD_TYPE(
	  LAK_EBPF_ENUM_CMP, opcode_class, lak::ebpf::opcode_class_t) false;
}

constexpr bool lak::ebpf::instruction::is_store() const
{
	return LAK_FOREACH_EBPF_OPCODE_CLASS_STORE_TYPE(
	  LAK_EBPF_ENUM_CMP, opcode_class, lak::ebpf::opcode_class_t) false;
}

constexpr bool lak::ebpf::instruction::is_alu() const
{
	return LAK_FOREACH_EBPF_OPCODE_CLASS_ALU_TYPE(
	  LAK_EBPF_ENUM_CMP, opcode_class, lak::ebpf::opcode_class_t) false;
}

constexpr bool lak::ebpf::instruction::is_jump() const
{
	return LAK_FOREACH_EBPF_OPCODE_CLASS_JUMP_TYPE(
	  LAK_EBPF_ENUM_CMP, opcode_class, lak::ebpf::opcode_class_t) false;
}

#undef LAK_EBPF_ENUM_CMP

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

constexpr lak::ebpf::opcode_alu_endian lak::ebpf::instruction::endian() const
{
	return static_cast<opcode_alu_endian>(uint8_t(alu_jmp.source) << 3U);
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

constexpr lak::ebpf::opcode_jump_call lak::ebpf::instruction::call_src() const
{
	return static_cast<opcode_jump_call>(src);
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
	uint64_t inst     = static_cast<uint64_t>(packed);
	auto opcode_class = static_cast<opcode_class_t>(inst & 0b111U);
	if (static_cast<uint8_t>(opcode_class) <= 0x03U)
		return lak::ebpf::instruction{
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
		return lak::ebpf::instruction{
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
	return lak::ebpf::instruction{
	  .opcode_class = opcode_class_t::LD,
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
	return lak::ebpf::instruction{
	  .opcode_class = static_cast<opcode_class_t>(load),
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
	return lak::ebpf::instruction{
	  .opcode_class = static_cast<opcode_class_t>(store),
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
  uint16_t offset,
  uint32_t immediate)
{
	return lak::ebpf::instruction{
	  .opcode_class = static_cast<opcode_class_t>(alu),
	  .alu_jmp =
	    {
	      .source = static_cast<uint8_t>(static_cast<uint8_t>(source) >> 3U),
	      .code   = static_cast<uint8_t>(static_cast<uint8_t>(code) >> 4U),
	    },
	  .dst       = static_cast<uint8_t>(dst),
	  .src       = static_cast<uint8_t>(src),
	  .offset    = offset,
	  .immediate = immediate,
	};
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_alu alu,
  opcode_alu code,
  register_t dst,
  register_t src,
  uint16_t offset)
{
	return make(alu, opcode_alu_jump_source::X, code, dst, src, offset, 0U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_alu alu,
  opcode_alu code,
  register_t dst,
  uint16_t offset,
  uint32_t immediate)
{
	return make(alu,
	            opcode_alu_jump_source::K,
	            code,
	            dst,
	            register_t::R0,
	            offset,
	            immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_alu alu, opcode_alu code, register_t dst, register_t src)
{
	return make(alu, opcode_alu_jump_source::X, code, dst, src, 0U, 0U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_alu alu, opcode_alu code, register_t dst, uint32_t immediate)
{
	return make(
	  alu, opcode_alu_jump_source::K, code, dst, register_t::R0, 0U, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_end(
  opcode_alu_endian endian, register_t dst, uint32_t immediate)
{
	return make(opcode_class_alu::ALU,
	            static_cast<opcode_alu_jump_source>(endian),
	            opcode_alu::END,
	            dst,
	            register_t::R0,
	            0U,
	            immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_end16(
  opcode_alu_endian endian, register_t dst)
{
	return make_end(endian, dst, 16U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_end32(
  opcode_alu_endian endian, register_t dst)
{
	return make_end(endian, dst, 32U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_end64(
  opcode_alu_endian endian, register_t dst)
{
	return make_end(endian, dst, 64U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_bswap(
  register_t dst, uint32_t immediate)
{
	return make(opcode_class_alu::ALU64,
	            opcode_alu_jump_source::K,
	            opcode_alu::END,
	            dst,
	            register_t::R0,
	            0U,
	            immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_bswap16(
  register_t dst)
{
	return make_bswap(dst, 16U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_bswap32(
  register_t dst)
{
	return make_bswap(dst, 32U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_bswap64(
  register_t dst)
{
	return make_bswap(dst, 64U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_sdiv(
  opcode_class_alu alu, register_t dst, register_t src)
{
	return make(alu, opcode_alu::DIV, dst, src, 1U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_sdiv(
  opcode_class_alu alu, register_t dst, uint32_t immediate)
{
	return make(alu, opcode_alu::DIV, dst, 1U, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_smod(
  opcode_class_alu alu, register_t dst, register_t src)
{
	return make(alu, opcode_alu::MOD, dst, src, 1U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_smod(
  opcode_class_alu alu, register_t dst, uint32_t immediate)
{
	return make(alu, opcode_alu::MOD, dst, 1U, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_movsx(
  opcode_class_alu alu, register_t dst, register_t src, uint16_t offset)
{
	return make(alu, opcode_alu::MOV, dst, src, offset);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_movsx8(
  opcode_class_alu alu, register_t dst, register_t src)
{
	return make_movsx(alu, dst, src, 8U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_movsx16(
  opcode_class_alu alu, register_t dst, register_t src)
{
	return make_movsx(alu, dst, src, 16U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_movsx32(
  opcode_class_alu alu, register_t dst, register_t src)
{
	return make_movsx(alu, dst, src, 32U);
}

/* --- jump -- */

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_jump jump,
  opcode_alu_jump_source source,
  opcode_jump code,
  register_t dst,
  register_t src,
  uint16_t offset,
  uint32_t immediate)
{
	return lak::ebpf::instruction{
	  .opcode_class = static_cast<opcode_class_t>(jump),
	  .alu_jmp =
	    {
	      .source = static_cast<uint8_t>(static_cast<uint8_t>(source) >> 3U),
	      .code   = static_cast<uint8_t>(static_cast<uint8_t>(code) >> 4U),
	    },
	  .dst       = static_cast<uint8_t>(dst),
	  .src       = static_cast<uint8_t>(src),
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
	return make(jump, opcode_alu_jump_source::X, code, dst, src, offset, 0U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_class_jump jump,
  opcode_jump code,
  register_t dst,
  uint16_t offset,
  uint32_t immediate)
{
	return make(jump,
	            opcode_alu_jump_source::K,
	            code,
	            dst,
	            register_t::R0,
	            offset,
	            immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_jump16(
  uint16_t offset)
{
	return make(opcode_class_jump::JMP,
	            opcode_alu_jump_source::K,
	            opcode_jump::JA,
	            register_t::R0,
	            register_t::R0,
	            offset,
	            0U);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_jump32(
  uint32_t immediate)
{
	return make(opcode_class_jump::JMP32,
	            opcode_alu_jump_source::K,
	            opcode_jump::JA,
	            register_t::R0,
	            register_t::R0,
	            0U,
	            immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make(
  opcode_jump_call src, uint32_t immediate)
{
	return make(opcode_class_jump::JMP,
	            opcode_alu_jump_source::K,
	            opcode_jump::CALL,
	            register_t::R0,
	            static_cast<register_t>(src),
	            0U,
	            immediate);
}

constexpr lak::ebpf::instruction
lak::ebpf::instruction::make_agnostic_helper_call(uint32_t immediate)
{
	return make(lak::ebpf::opcode_jump_call::AGNOSTIC, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_local_call(
  uint32_t immediate)
{
	return make(lak::ebpf::opcode_jump_call::LOCAL, immediate);
}

constexpr lak::ebpf::instruction
lak::ebpf::instruction::make_specific_helper_call(uint32_t immediate)
{
	return make(lak::ebpf::opcode_jump_call::SPECIFIC, immediate);
}

constexpr lak::ebpf::instruction lak::ebpf::instruction::make_exit()
{
	return make(opcode_class_jump::JMP,
	            opcode_alu_jump_source::K,
	            opcode_jump::EXIT,
	            register_t::R0,
	            register_t::R0,
	            0U,
	            0U);
}

constexpr lak::ebpf::instruction::operator lak::ebpf::instruction_t() const
{
	uint8_t opcode =
	  static_cast<uint8_t>(opcode_class) <= 0x03U
	    ? static_cast<uint8_t>(opcode_class) | (uint8_t(ld_st.size) << 3U) |
	        (uint8_t(ld_st.mode) << 5U)
	    : static_cast<uint8_t>(opcode_class) | (uint8_t(alu_jmp.source) << 3U) |
	        (uint8_t(alu_jmp.code) << 4U);
	return static_cast<instruction_t>((static_cast<uint64_t>(immediate) << 32U) |
	                                  (static_cast<uint64_t>(offset) << 16U) |
	                                  ((static_cast<uint64_t>(src)) << 12U) |
	                                  ((static_cast<uint64_t>(dst)) << 8U) |
	                                  opcode);
}

#include "lak/debug.hpp"
#include "lak/format_traits.hpp"
#include "lak/string_literals/view.hpp"

#define LAK_EBPF_ENUM_STR(VAL, NAME, TYPE, ...)                               \
	case TYPE::NAME: return lak::strconv<CHAR>(#NAME ""_view);

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_class_load, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::opcode_class_load &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_CLASS_LOAD_TYPE(LAK_EBPF_ENUM_STR,
			                                        lak::ebpf::opcode_class_load);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::ebpf::opcode_class_load &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_class_store, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(
	  const lak::ebpf::opcode_class_store &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_CLASS_STORE_TYPE(LAK_EBPF_ENUM_STR,
			                                         lak::ebpf::opcode_class_store);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(
	  const format_args &args, const lak::ebpf::opcode_class_store &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_load_store_size, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(
	  const lak::ebpf::opcode_load_store_size &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_LOAD_STORE_SIZE_TYPE(
			  LAK_EBPF_ENUM_STR, lak::ebpf::opcode_load_store_size);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(
	  const format_args &args, const lak::ebpf::opcode_load_store_size &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_load_store_mode, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(
	  const lak::ebpf::opcode_load_store_mode &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_LOAD_STORE_MODE_TYPE(
			  LAK_EBPF_ENUM_STR, lak::ebpf::opcode_load_store_mode);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(
	  const format_args &args, const lak::ebpf::opcode_load_store_mode &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_atomic, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::opcode_atomic &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_ATOMIC_TYPE(LAK_EBPF_ENUM_STR,
			                                    lak::ebpf::opcode_atomic);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::ebpf::opcode_atomic &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_class_alu, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::opcode_class_alu &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_CLASS_ALU_TYPE(LAK_EBPF_ENUM_STR,
			                                       lak::ebpf::opcode_class_alu);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::ebpf::opcode_class_alu &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_class_jump, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::opcode_class_jump &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_CLASS_JUMP_TYPE(LAK_EBPF_ENUM_STR,
			                                        lak::ebpf::opcode_class_jump);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::ebpf::opcode_class_jump &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_alu_jump_source, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(
	  const lak::ebpf::opcode_alu_jump_source &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_ALU_JUMP_SOURCE_TYPE(
			  LAK_EBPF_ENUM_STR, lak::ebpf::opcode_alu_jump_source);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(
	  const format_args &args, const lak::ebpf::opcode_alu_jump_source &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_alu_endian, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::opcode_alu_endian &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_ALU_ENDIAN_TYPE(LAK_EBPF_ENUM_STR,
			                                        lak::ebpf::opcode_alu_endian);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::ebpf::opcode_alu_endian &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_alu, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::opcode_alu &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_ALU_TYPE(LAK_EBPF_ENUM_STR,
			                                 lak::ebpf::opcode_alu);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::ebpf::opcode_alu &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_jump, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::opcode_jump &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_JUMP_TYPE(LAK_EBPF_ENUM_STR,
			                                  lak::ebpf::opcode_jump);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::ebpf::opcode_jump &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::opcode_class_t, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::opcode_class_t &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_OPCODE_CLASS_LOAD_TYPE(LAK_EBPF_ENUM_STR,
			                                        lak::ebpf::opcode_class_t);
			LAK_FOREACH_EBPF_OPCODE_CLASS_STORE_TYPE(LAK_EBPF_ENUM_STR,
			                                         lak::ebpf::opcode_class_t);
			LAK_FOREACH_EBPF_OPCODE_CLASS_ALU_TYPE(LAK_EBPF_ENUM_STR,
			                                       lak::ebpf::opcode_class_t);
			LAK_FOREACH_EBPF_OPCODE_CLASS_JUMP_TYPE(LAK_EBPF_ENUM_STR,
			                                        lak::ebpf::opcode_class_t);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::ebpf::opcode_class_t &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::register_t, CHAR>
: public lak::format_traits<lak::string_view<CHAR>, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::register_t &value)
	{
		switch (value)
		{
			LAK_FOREACH_EBPF_REGISTER_TYPE(LAK_EBPF_ENUM_STR, lak::ebpf::register_t);
			default: BOUNDS_ASSERT_UNREACHABLE(return {});
		}
	}

	using typename lak::format_traits<lak::string_view<CHAR>, CHAR>::format_args;
	using lak::format_traits<lak::string_view<CHAR>, CHAR>::to_string;
	static lak::string<CHAR> to_string(const format_args &args,
	                                   const lak::ebpf::register_t &value)
	{
		return to_string(args, lak::string_view(to_string(value)));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::instruction, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::instruction &value)
	{
		lak::string<CHAR> result;
		result += lak::fmt<CHAR, "{:.5} | ">(value.opcode_class);

		if (value.is_load() || value.is_store())
			result += lak::fmt<CHAR, "{:.5} | {:.5} ">(value.size(), value.mode());
		else if (value.is_jump())
			result +=
			  lak::fmt<CHAR, "{:.5} | {:.5} ">(value.source(), value.jump_code());
		else if (value.is_alu())
		{
			if (value.alu_code() != lak::ebpf::opcode_alu::END)
				result += lak::fmt<CHAR, "{:.5} | ">(value.source());
			else if (value.opcode_class == lak::ebpf::opcode_class_t::ALU)
				result += lak::fmt<CHAR, "{:.5} | ">(value.endian());
			else
				result += lak::strconv<CHAR>("BSWAP | "_view);

			switch (value.alu_code())
			{
				case lak::ebpf::opcode_alu::DIV:
					result += value.offset == 1 ? lak::strconv<CHAR>("SDIV  "_view)
					                            : lak::strconv<CHAR>("DIV   "_view);
					break;

				case lak::ebpf::opcode_alu::MOD:
					result += value.offset == 1 ? lak::strconv<CHAR>("SMOD  "_view)
					                            : lak::strconv<CHAR>("MOD   "_view);
					break;

				case lak::ebpf::opcode_alu::MOV:
					result +=
					  (value.offset == 8 || value.offset == 16 || value.offset == 32)
					    ? lak::strconv<CHAR>("MOVSX "_view)
					    : lak::strconv<CHAR>("MOV   "_view);
					break;

				default: result += lak::fmt<CHAR, "{:.5} ">(value.alu_code());
			}
		}
		else
			BOUNDS_ASSERT_UNREACHABLE();

		result +=
		  lak::fmt<u8".dst={:0.2d} .src={:0.2d} .off={:#0.4X} .imm={:#0.8X}">(
		    value.dst, value.src, value.offset, value.immediate);

		return result;
	}
};

template<typename CHAR>
struct lak::format_traits<lak::ebpf::instruction_t, CHAR>
{
	static lak::string<CHAR> to_string(const lak::ebpf::instruction_t &value)
	{
		return lak::format_traits<lak::ebpf::instruction, CHAR>::to_string(
		  lak::ebpf::instruction::make(value));
	}
};

#undef LAK_EBPF_ENUM_STR
