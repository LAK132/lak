#include "lak/ebpf/assembler.hpp"

constexpr auto register_parser =
  lak::dsl::replace_str_literal<u8"r10", lak::ebpf::register_t::R10> |
  lak::dsl::replace_str_literal<u8"r0", lak::ebpf::register_t::R0> |
  lak::dsl::replace_str_literal<u8"r1", lak::ebpf::register_t::R1> |
  lak::dsl::replace_str_literal<u8"r2", lak::ebpf::register_t::R2> |
  lak::dsl::replace_str_literal<u8"r3", lak::ebpf::register_t::R3> |
  lak::dsl::replace_str_literal<u8"r4", lak::ebpf::register_t::R4> |
  lak::dsl::replace_str_literal<u8"r5", lak::ebpf::register_t::R5> |
  lak::dsl::replace_str_literal<u8"r6", lak::ebpf::register_t::R6> |
  lak::dsl::replace_str_literal<u8"r7", lak::ebpf::register_t::R7> |
  lak::dsl::replace_str_literal<u8"r8", lak::ebpf::register_t::R8> |
  lak::dsl::replace_str_literal<u8"r9", lak::ebpf::register_t::R9>;

constexpr auto load_store_size_parser =
  lak::dsl::replace_str_literal<u8"dw",
                                lak::ebpf::opcode_load_store_size::DW> |
  lak::dsl::replace_str_literal<u8"w", lak::ebpf::opcode_load_store_size::W> |
  lak::dsl::replace_str_literal<u8"h", lak::ebpf::opcode_load_store_size::H> |
  lak::dsl::replace_str_literal<u8"b", lak::ebpf::opcode_load_store_size::B>;

constexpr auto _offset_parser =
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"0b">,
                        lak::dsl::parsed_bin_uint<uint16_t>> |
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"0x">,
                        lak::dsl::parsed_hex_uint<uint16_t>> |
  lak::dsl::parsed_dec_uint<uint16_t>;

constexpr auto offset_parser =
  lak::dsl::conditional<lak::dsl::char_literal<U'-'>,
                        lak::dsl::transform<_offset_parser,
                                            [](uint16_t v)
                                            {
	                                            return static_cast<uint16_t>(
	                                              -static_cast<int16_t>(v));
                                            }>,
                        _offset_parser>;

constexpr auto _immediate_parser =
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"0b">,
                        lak::dsl::parsed_bin_uint<uint32_t>> |
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"0x">,
                        lak::dsl::parsed_hex_uint<uint32_t>> |
  lak::dsl::parsed_dec_uint<uint32_t>;

constexpr auto immediate_parser =
  lak::dsl::conditional<lak::dsl::char_literal<U'-'>,
                        lak::dsl::transform<_immediate_parser,
                                            [](uint32_t v)
                                            {
	                                            return static_cast<uint32_t>(
	                                              -static_cast<int32_t>(v));
                                            }>,
                        _immediate_parser>;

constexpr auto _immediate64_parser =
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"0b">,
                        lak::dsl::parsed_bin_uint<uint64_t>> |
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"0x">,
                        lak::dsl::parsed_hex_uint<uint64_t>> |
  lak::dsl::parsed_dec_uint<uint64_t>;

constexpr auto immediate64_parser =
  lak::dsl::conditional<lak::dsl::char_literal<U'-'>,
                        lak::dsl::transform<_immediate64_parser,
                                            [](uint64_t v)
                                            {
	                                            return static_cast<uint64_t>(
	                                              -static_cast<int64_t>(v));
                                            }>,
                        _immediate64_parser>;

constexpr auto alu_jump_source_parser =
  lak::dsl::replace_str_literal<u8"k", lak::ebpf::opcode_alu_jump_source::K> |
  lak::dsl::replace_str_literal<u8"x", lak::ebpf::opcode_alu_jump_source::X>;

constexpr auto alu_code_parser =
  lak::dsl::replace_str_literal<u8"add", lak::ebpf::opcode_alu::ADD> |
  lak::dsl::replace_str_literal<u8"sub", lak::ebpf::opcode_alu::SUB> |
  lak::dsl::replace_str_literal<u8"mul", lak::ebpf::opcode_alu::MUL> |
  lak::dsl::replace_str_literal<u8"div", lak::ebpf::opcode_alu::DIV> |
  lak::dsl::replace_str_literal<u8"or", lak::ebpf::opcode_alu::OR> |
  lak::dsl::replace_str_literal<u8"and", lak::ebpf::opcode_alu::AND> |
  lak::dsl::replace_str_literal<u8"lsh", lak::ebpf::opcode_alu::LSH> |
  lak::dsl::replace_str_literal<u8"rsh", lak::ebpf::opcode_alu::RSH> |
  lak::dsl::replace_str_literal<u8"neg", lak::ebpf::opcode_alu::NEG> |
  lak::dsl::replace_str_literal<u8"mod", lak::ebpf::opcode_alu::MOD> |
  lak::dsl::replace_str_literal<u8"xor", lak::ebpf::opcode_alu::XOR> |
  lak::dsl::replace_str_literal<u8"mov", lak::ebpf::opcode_alu::MOV> |
  lak::dsl::replace_str_literal<u8"arsh", lak::ebpf::opcode_alu::ARSH>;

constexpr auto jump32_code_parser =
  lak::dsl::replace_str_literal<u8"jeq", lak::ebpf::opcode_jump::JEQ> |
  lak::dsl::replace_str_literal<u8"jgt", lak::ebpf::opcode_jump::JGT> |
  lak::dsl::replace_str_literal<u8"jge", lak::ebpf::opcode_jump::JGE> |
  lak::dsl::replace_str_literal<u8"jset", lak::ebpf::opcode_jump::JSET> |
  lak::dsl::replace_str_literal<u8"jne", lak::ebpf::opcode_jump::JNE> |
  lak::dsl::replace_str_literal<u8"jsgt", lak::ebpf::opcode_jump::JSGT> |
  lak::dsl::replace_str_literal<u8"jsge", lak::ebpf::opcode_jump::JSGE> |
  lak::dsl::replace_str_literal<u8"jlt", lak::ebpf::opcode_jump::JLT> |
  lak::dsl::replace_str_literal<u8"jle", lak::ebpf::opcode_jump::JLE> |
  lak::dsl::replace_str_literal<u8"jslt", lak::ebpf::opcode_jump::JSLT> |
  lak::dsl::replace_str_literal<u8"jsle", lak::ebpf::opcode_jump::JSLE>;

constexpr auto jump_code_parser =
  lak::dsl::replace_str_literal<u8"ja", lak::ebpf::opcode_jump::JA> |
  lak::dsl::replace_str_literal<u8"call", lak::ebpf::opcode_jump::CALL> |
  lak::dsl::replace_str_literal<u8"exit", lak::ebpf::opcode_jump::EXIT> |
  jump32_code_parser;

constexpr auto ws = +lak::dsl::ascii_nonnewline_whitespace;

/* --- ld --- */

constexpr auto ld_imm_prefix =
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"ld"> + ws +
                          lak::dsl::str_literal<u8"dw"> + ws +
                          lak::dsl::str_literal<u8"imm"> + ws,
                        register_parser,
                        ws>;

constexpr auto ld_mfd_parser = lak::dsl::transform<
  ld_imm_prefix +
    lak::dsl::capture_2nd<lak::dsl::str_literal<u8"mfd">, immediate_parser>,
  [](const lak::tuple<lak::ebpf::register_t, uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[dst, imm] = result;
	  return {lak::ebpf::instruction::make_load(dst, 1U, imm), lak::nullopt};
  }>;

constexpr auto ld_mva_mfd_parser = lak::dsl::transform<
  ld_imm_prefix +
    lak::dsl::capture_2nd<lak::dsl::str_literal<u8"mva"> + ws +
                            lak::dsl::str_literal<u8"mfd"> + ws,
                          immediate_parser> +
    lak::dsl::capture_2nd<ws, immediate_parser>,
  [](const lak::tuple<lak::ebpf::register_t, uint32_t, uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[dst, imm1, imm2] = result;
	  auto inst = lak::ebpf::instruction::make_load(dst, 2U, imm1, imm2);
	  return {inst.first, lak::var_t<0>(inst.second)};
  }>;

constexpr auto ld_var_parser = lak::dsl::transform<
  ld_imm_prefix +
    lak::dsl::capture_2nd<lak::dsl::str_literal<u8"var">, immediate_parser>,
  [](const lak::tuple<lak::ebpf::register_t, uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[dst, imm] = result;
	  return {lak::ebpf::instruction::make_load(dst, 3U, imm), lak::nullopt};
  }>;

constexpr auto ld_code_parser = lak::dsl::transform<
  ld_imm_prefix +
    lak::dsl::capture_2nd<lak::dsl::str_literal<u8"code">, immediate_parser>,
  [](const lak::tuple<lak::ebpf::register_t, uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[dst, imm] = result;
	  return {lak::ebpf::instruction::make_load(dst, 4U, imm), lak::nullopt};
  }>;

constexpr auto ld_mid_parser = lak::dsl::transform<
  ld_imm_prefix +
    lak::dsl::capture_2nd<lak::dsl::str_literal<u8"mid">, immediate_parser>,
  [](const lak::tuple<lak::ebpf::register_t, uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[dst, imm] = result;
	  return {lak::ebpf::instruction::make_load(dst, 5U, imm), lak::nullopt};
  }>;

constexpr auto ld_mva_mid_parser = lak::dsl::transform<
  ld_imm_prefix +
    lak::dsl::capture_2nd<lak::dsl::str_literal<u8"mva"> + ws +
                            lak::dsl::str_literal<u8"mid"> + ws,
                          immediate_parser> +
    lak::dsl::capture_2nd<ws, immediate_parser>,
  [](const lak::tuple<lak::ebpf::register_t, uint32_t, uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[dst, imm1, imm2] = result;
	  auto inst = lak::ebpf::instruction::make_load(dst, 6U, imm1, imm2);
	  return {inst.first, lak::var_t<0>(inst.second)};
  }>;

constexpr auto ld_imm64_parser = lak::dsl::transform<
  ld_imm_prefix + immediate64_parser,
  [](const lak::tuple<lak::ebpf::register_t, uint64_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[dst, imm] = result;
	  auto inst              = lak::ebpf::instruction::make_load(dst, imm);
	  return {inst.first, lak::var_t<0>(inst.second)};
  }>;

constexpr auto ld_parser = ld_mfd_parser | ld_mva_mfd_parser | ld_var_parser |
                           ld_code_parser | ld_mid_parser | ld_mva_mid_parser |
                           ld_imm64_parser;

/* --- ldx --- */

constexpr auto ldx_parser = lak::dsl::transform<
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"ldx"> + ws,
                        load_store_size_parser> +
    lak::dsl::capture_2nd<ws + lak::dsl::str_literal<u8"mem"> + ws,
                          register_parser> +
    lak::dsl::capture_2nd<ws, register_parser> +
    lak::dsl::capture_2nd<ws, offset_parser>,
  [](const lak::tuple<lak::ebpf::opcode_load_store_size,
                      lak::ebpf::register_t,
                      lak::ebpf::register_t,
                      uint16_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[size, dst, src, offset] = result;
	  return {lak::ebpf::instruction::make_load(size, dst, src, offset),
	          lak::nullopt};
  }>;

/* --- st --- */

constexpr auto st_parser = lak::dsl::transform<
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"st"> + ws,
                        load_store_size_parser> +
    lak::dsl::capture_2nd<ws + lak::dsl::str_literal<u8"mem"> + ws,
                          register_parser> +
    lak::dsl::capture_2nd<ws, offset_parser> +
    lak::dsl::capture_2nd<ws, immediate_parser>,
  [](const lak::tuple<lak::ebpf::opcode_load_store_size,
                      lak::ebpf::register_t,
                      uint16_t,
                      uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[size, dst, offset, imm] = result;
	  return {lak::ebpf::instruction::make_store(size, dst, offset, imm),
	          lak::nullopt};
  }>;

/* --- stx --- */

// :TODO: atomics
constexpr auto stx_parser = lak::dsl::transform<
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"stx"> + ws,
                        load_store_size_parser> +
    lak::dsl::capture_2nd<ws + lak::dsl::str_literal<u8"mem"> + ws,
                          register_parser> +
    lak::dsl::capture_2nd<ws, offset_parser> +
    lak::dsl::capture_2nd<ws, register_parser>,
  [](const lak::tuple<lak::ebpf::opcode_load_store_size,
                      lak::ebpf::register_t,
                      uint16_t,
                      lak::ebpf::register_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[size, dst, offset, src] = result;
	  return {lak::ebpf::instruction::make_store(size, dst, offset, src),
	          lak::nullopt};
  }>;

/* --- alu --- */

constexpr auto alu_k_parser = lak::dsl::transform<
  (lak::dsl::replace_str_literal<u8"alu64",
                                 lak::ebpf::opcode_class_alu::ALU64> |
   lak::dsl::replace_str_literal<u8"alu",
                                 lak::ebpf::opcode_class_alu::ALU>)+lak::dsl::
      capture_2nd<ws + lak::dsl::char_literal<U'k'> + ws, alu_code_parser> +
    lak::dsl::capture_2nd<ws, register_parser> +
    lak::dsl::capture_2nd<ws, immediate_parser>,
  [](const lak::tuple<lak::ebpf::opcode_class_alu,
                      lak::ebpf::opcode_alu,
                      lak::ebpf::register_t,
                      uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[op, code, dst, imm] = result;
	  return {lak::ebpf::instruction::make(op, code, dst, imm), lak::nullopt};
  }>;

constexpr auto alu_x_parser = lak::dsl::transform<
  (lak::dsl::replace_str_literal<u8"alu64",
                                 lak::ebpf::opcode_class_alu::ALU64> |
   lak::dsl::replace_str_literal<u8"alu",
                                 lak::ebpf::opcode_class_alu::ALU>)+lak::dsl::
      capture_2nd<ws + lak::dsl::char_literal<U'x'> + ws, alu_code_parser> +
    lak::dsl::capture_2nd<ws, register_parser> +
    lak::dsl::capture_2nd<ws, register_parser>,
  [](const lak::tuple<lak::ebpf::opcode_class_alu,
                      lak::ebpf::opcode_alu,
                      lak::ebpf::register_t,
                      lak::ebpf::register_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[op, code, dst, src] = result;
	  return {lak::ebpf::instruction::make(op, code, dst, src), lak::nullopt};
  }>;

constexpr auto alu_endian_parser = lak::dsl::transform<
  (lak::dsl::replace_str_literal<u8"alu64",
                                 lak::ebpf::opcode_class_alu::ALU64> |
   lak::dsl::replace_str_literal<u8"alu",
                                 lak::ebpf::opcode_class_alu::ALU>)+lak::dsl::
      capture_2nd<
        ws,
        lak::dsl::replace_str_literal<u8"to_le",
                                      lak::ebpf::opcode_alu_jump_source::K> |
          lak::dsl::replace_str_literal<
            u8"to_be",
            lak::ebpf::opcode_alu_jump_source::X>> +
    lak::dsl::capture_2nd<ws + lak::dsl::str_literal<u8"end">,
                          register_parser> +
    lak::dsl::capture_2nd<
      ws,
      lak::dsl::replace_str_literal<u8"16", uint32_t(16)> |
        lak::dsl::replace_str_literal<u8"32", uint32_t(32)> |
        lak::dsl::replace_str_literal<u8"64", uint32_t(64)>>,
  [](const lak::tuple<lak::ebpf::opcode_class_alu,
                      lak::ebpf::opcode_alu_jump_source,
                      lak::ebpf::register_t,
                      uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[op, source, dst, imm] = result;
	  return {lak::ebpf::instruction::make(op,
	                                       source,
	                                       lak::ebpf::opcode_alu::END,
	                                       dst,
	                                       lak::ebpf::register_t::R0,
	                                       imm),
	          lak::nullopt};
  }>;

constexpr auto alu_parser = alu_k_parser | alu_x_parser | alu_endian_parser;

/* --- jmp --- */

constexpr auto jmp_k_parser = lak::dsl::transform<
  (lak::dsl::replace_str_literal<u8"jmp32",
                                 lak::ebpf::opcode_class_jump::JMP32> |
   lak::dsl::replace_str_literal<u8"jmp",
                                 lak::ebpf::opcode_class_jump::JMP>)+lak::dsl::
      capture_2nd<ws + lak::dsl::str_literal<u8"k"> + ws, jump32_code_parser> +
    lak::dsl::capture_2nd<ws, register_parser> +
    lak::dsl::capture_2nd<ws, immediate_parser> +
    lak::dsl::capture_2nd<ws, offset_parser>,
  [](const lak::tuple<lak::ebpf::opcode_class_jump,
                      lak::ebpf::opcode_jump,
                      lak::ebpf::register_t,
                      uint32_t,
                      uint16_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[op, jmp, dst, imm, offset] = result;
	  return {lak::ebpf::instruction::make(op, jmp, dst, imm, offset),
	          lak::nullopt};
  }>;

constexpr auto jmp_x_parser = lak::dsl::transform<
  (lak::dsl::replace_str_literal<u8"jmp32",
                                 lak::ebpf::opcode_class_jump::JMP32> |
   lak::dsl::replace_str_literal<u8"jmp",
                                 lak::ebpf::opcode_class_jump::JMP>)+lak::dsl::
      capture_2nd<ws + lak::dsl::str_literal<u8"x"> + ws, jump32_code_parser> +
    lak::dsl::capture_2nd<ws, register_parser> +
    lak::dsl::capture_2nd<ws, register_parser> +
    lak::dsl::capture_2nd<ws, offset_parser>,
  [](const lak::tuple<lak::ebpf::opcode_class_jump,
                      lak::ebpf::opcode_jump,
                      lak::ebpf::register_t,
                      lak::ebpf::register_t,
                      uint16_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[op, jmp, dst, src, offset] = result;
	  return {lak::ebpf::instruction::make(op, jmp, dst, src, offset),
	          lak::nullopt};
  }>;

constexpr auto jmp_k_label_parser = lak::dsl::transform<
  (lak::dsl::replace_str_literal<u8"jmp32",
                                 lak::ebpf::opcode_class_jump::JMP32> |
   lak::dsl::replace_str_literal<u8"jmp",
                                 lak::ebpf::opcode_class_jump::JMP>)+lak::dsl::
      capture_2nd<ws + lak::dsl::str_literal<u8"k"> + ws, jump32_code_parser> +
    lak::dsl::capture_2nd<ws, register_parser> +
    lak::dsl::capture_2nd<ws, immediate_parser> +
    lak::dsl::capture_2nd<ws, lak::ebpf::label_token_parser>,
  [](const lak::tuple<lak::ebpf::opcode_class_jump,
                      lak::ebpf::opcode_jump,
                      lak::ebpf::register_t,
                      uint32_t,
                      lak::u8string_view> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[op, jmp, dst, imm, label] = result;
	  return {lak::ebpf::instruction::make(op, jmp, dst, imm, UINT16_MAX),
	          lak::var_t<1>(label)};
  }>;

constexpr auto jmp_x_label_parser = lak::dsl::transform<
  (lak::dsl::replace_str_literal<u8"jmp32",
                                 lak::ebpf::opcode_class_jump::JMP32> |
   lak::dsl::replace_str_literal<u8"jmp",
                                 lak::ebpf::opcode_class_jump::JMP>)+lak::dsl::
      capture_2nd<ws + lak::dsl::str_literal<u8"x"> + ws, jump32_code_parser> +
    lak::dsl::capture_2nd<ws, register_parser> +
    lak::dsl::capture_2nd<ws, register_parser> +
    lak::dsl::capture_2nd<ws, lak::ebpf::label_token_parser>,
  [](const lak::tuple<lak::ebpf::opcode_class_jump,
                      lak::ebpf::opcode_jump,
                      lak::ebpf::register_t,
                      lak::ebpf::register_t,
                      lak::u8string_view> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[op, jmp, dst, src, label] = result;
	  return {lak::ebpf::instruction::make(op, jmp, dst, src, UINT16_MAX),
	          lak::var_t<1>(label)};
  }>;

constexpr auto jmp16_ja_parser = lak::dsl::transform<
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"jmp"> + ws +
                          lak::dsl::str_literal<u8"ja"> + ws,
                        offset_parser>,
  [](uint16_t offset) -> lak::ebpf::instruction_parser_t::value_type
  { return {lak::ebpf::instruction::make_jump16(offset), lak::nullopt}; }>;

constexpr auto jmp32_ja_parser = lak::dsl::transform<
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"jmp32"> + ws +
                          lak::dsl::str_literal<u8"ja"> + ws,
                        immediate_parser>,
  [](uint32_t immediate) -> lak::ebpf::instruction_parser_t::value_type
  { return {lak::ebpf::instruction::make_jump32(immediate), lak::nullopt}; }>;

constexpr auto jmp16_ja_label_parser = lak::dsl::transform<
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"jmp"> + ws +
                          lak::dsl::str_literal<u8"ja"> + ws,
                        lak::ebpf::label_token_parser>,
  [](lak::u8string_view label) -> lak::ebpf::instruction_parser_t::value_type
  {
	  return {lak::ebpf::instruction::make_jump16(UINT16_MAX),
	          lak::var_t<1>(label)};
  }>;

constexpr auto jmp32_ja_label_parser = lak::dsl::transform<
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"jmp32"> + ws +
                          lak::dsl::str_literal<u8"ja"> + ws,
                        lak::ebpf::label_token_parser>,
  [](lak::u8string_view label) -> lak::ebpf::instruction_parser_t::value_type
  {
	  return {lak::ebpf::instruction::make_jump32(UINT32_MAX),
	          lak::var_t<1>(label)};
  }>;

constexpr auto jmp_call_parser = lak::dsl::transform<
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"jmp"> + ws +
                          lak::dsl::str_literal<u8"call"> + ws,
                        (lak::dsl::replace_char_literal<U'0', uint8_t(0)> |
                         lak::dsl::replace_char_literal<U'1', uint8_t(1)> |
                         lak::dsl::replace_char_literal<U'2', uint8_t(2)>)> +
    lak::dsl::capture_2nd<ws, immediate_parser>,
  [](const lak::tuple<uint8_t, uint32_t> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[src, imm] = result;
	  return {lak::ebpf::instruction::make_call(src, imm), lak::nullopt};
  }>;

constexpr auto jmp_call_label_parser = lak::dsl::transform<
  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"jmp"> + ws +
                          lak::dsl::str_literal<u8"call"> + ws,
                        lak::dsl::replace_char_literal<U'1', uint8_t(1)>> +
    lak::dsl::capture_2nd<ws, lak::ebpf::label_token_parser>,
  [](const lak::tuple<uint8_t, lak::u8string_view> &result)
    -> lak::ebpf::instruction_parser_t::value_type
  {
	  const auto &[src, label] = result;
	  return {lak::ebpf::instruction::make_call(src, UINT32_MAX),
	          lak::var_t<1>(label)};
  }>;

constexpr auto jmp_exit_parser = lak::dsl::transform<
  lak::dsl::str_literal<u8"jmp"> + ws + lak::dsl::str_literal<u8"exit">,
  [](lak::u8string_view) -> lak::ebpf::instruction_parser_t::value_type
  { return {lak::ebpf::instruction::make_exit(), lak::nullopt}; }>;

constexpr auto jmp_parser = jmp_k_parser | jmp_x_parser | jmp_k_label_parser |
                            jmp_x_label_parser | jmp16_ja_parser |
                            jmp32_ja_parser | jmp16_ja_label_parser |
                            jmp32_ja_label_parser | jmp_call_parser |
                            jmp_call_label_parser | jmp_exit_parser;

lak::dsl::result<lak::ebpf::instruction_parser_t::value_type>
lak::ebpf::instruction_parser_t::parse(lak::u8string_view str) const
{
	return (ldx_parser | ld_parser | stx_parser | st_parser | alu_parser |
	        jmp_parser)
	  .parse(str);
}
