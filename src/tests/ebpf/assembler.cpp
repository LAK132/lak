#include "lak/test.hpp"

#include "lak/binary_reader.hpp"
#include "lak/dsl/binary_reader.hpp"

#include "lak/string_literals/string.hpp"

#include "lak/ebpf/assembler.hpp"
#include "lak/ebpf/vm.hpp"

BEGIN_TEST(ebpf_assembler)
{
	{
		auto str =
		  u8R"(
alu   k  add r0 0x1234
jmp   ja        0x1234
alu64 k  add r0 0x1234
alu   x  add r1 r2
ld    dw imm r0 0x1234
)"_str;

		lak::binary_reader strm{lak::span<byte_t>(lak::span(str))};
		lak::dsl::binary_reader parser{strm};

		auto line_parser = lak::dsl::capture_2nd<*lak::dsl::ascii_whitespace +
		                                           *(lak::dsl::slash_line_comment +
		                                             *lak::dsl::ascii_whitespace),
		                                         lak::ebpf::instruction_parser>;

		// ---

		lak::pair<lak::ebpf::instruction_t,
		          lak::optional<
		            lak::variant<lak::ebpf::instruction_t, lak::u8string_view>>>
		  res = parser.read(line_parser).UNWRAP();

		ASSERT_EQUAL(static_cast<uint64_t>(res.first), 0x00001234'0000'0'0'04U);
		ASSERT(!res.second.has_value());

		// ---

		res = parser.read(line_parser).UNWRAP();

		ASSERT_EQUAL(static_cast<uint64_t>(res.first), 0x00000000'1234'0'0'05U);
		ASSERT(!res.second.has_value());

		// ---

		res = parser.read(line_parser).UNWRAP();

		ASSERT_EQUAL(static_cast<uint64_t>(res.first), 0x00001234'0000'0'0'07U);
		ASSERT(!res.second.has_value());

		// ---

		res = parser.read(line_parser).UNWRAP();

		ASSERT_EQUAL(static_cast<uint64_t>(res.first), 0x00000000'0000'2'1'0CU);
		ASSERT(!res.second.has_value());

		// ---

		res = parser.read(line_parser).UNWRAP();

		ASSERT_EQUAL(static_cast<uint64_t>(res.first), 0x00001234'0000'0'0'18U);
		ASSERT(res.second.has_value());
		ASSERT_EQUAL(static_cast<uint64_t>(*res.second->template get<0>()),
		             0x00000000'0000'0'0'00U);
	}

	{
		auto str =
		  u8R"(
ld dw imm r3 0

loop:
jmp k jeq r1 0 end
alu k sub r1 1
alu x add r3 r2
jmp ja loop

end:
stx dw mem r10 -8 r3
ldx dw mem r0 r10 -8
jmp exit
)"_str;

		constexpr auto ws =
		  lak::dsl::ascii_whitespace | lak::dsl::slash_line_comment;

		auto block = lak::ebpf::program_parser<ws>.parse(str).UNWRAP().value;

		lak::ebpf::vm vm;

		ASSERT_EQUAL(vm.run_program(block, 2, 66).UNWRAP(), 132U);
	}

	return 0;
}
END_TEST()
