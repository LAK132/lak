#include "lak/ebpf/ebpf.hpp"

lak::ebpf::instruction::operator lak::u8string() const
{
	return lak::format_traits<lak::ebpf::instruction, char8_t>::to_string(*this);
}
