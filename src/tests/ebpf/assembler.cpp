#include "lak/test.hpp"

#include "lak/binary_reader.hpp"
#include "lak/dsl/binary_reader.hpp"

#include "lak/string_literals/string.hpp"

#include "lak/ebpf/vm.hpp"
#include "lak/file/ebpf.hpp"

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

		auto line_parser = lak::dsl::repeat_exact<
		  lak::dsl::capture_2nd<*lak::dsl::whitespace,
		                        lak::ebpf::instruction_parser>,
		  5U>;

		lak::array<lak::pair<lak::ebpf::instruction_t,
		                     lak::optional<lak::variant<lak::ebpf::instruction_t,
		                                                lak::u8string_view>>>,
		           5U>
		  res = line_parser.parse(str).UNWRAP().value;

		ASSERT_EQUAL(static_cast<uint64_t>(res[0].first), 0x00001234'0000'0'0'04U);
		ASSERT(!res[0].second.has_value());

		ASSERT_EQUAL(static_cast<uint64_t>(res[1].first), 0x00000000'1234'0'0'05U);
		ASSERT(!res[1].second.has_value());

		ASSERT_EQUAL(static_cast<uint64_t>(res[2].first), 0x00001234'0000'0'0'07U);
		ASSERT(!res[2].second.has_value());

		ASSERT_EQUAL(static_cast<uint64_t>(res[3].first), 0x00000000'0000'2'1'0CU);
		ASSERT(!res[3].second.has_value());

		ASSERT_EQUAL(static_cast<uint64_t>(res[4].first), 0x00001234'0000'0'0'18U);
		ASSERT(res[4].second.has_value());
		ASSERT_EQUAL(static_cast<uint64_t>(*res[4].second->template get<0>()),
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
