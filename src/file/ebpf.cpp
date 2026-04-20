#include "lak/file/ebpf.hpp"

#include "ebpf.ebnf.hpp"

lak::dsl::result<lak::ebpf::instruction_parser_t::value_type>
lak::ebpf::instruction_parser_t::parse(lak::u8string_view str) const
{
	return lak::ebpf_parse::ebpf_parser.parse(str);
}
