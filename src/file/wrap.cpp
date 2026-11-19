#include "lak/file/wrap.hpp"

#include "wrap.ebnf.hpp"

lak::dsl::result<typename lak::dsl::wrap_t::value_type>
lak::dsl::wrap_t::parse(lak::u8string_view str) const
{
	return lak::wrap_parse::wrap.parse(str);
}
