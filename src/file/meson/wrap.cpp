#include "lak/file/meson/wrap.hpp"

#include "wrap.ebnf.hpp"

lak::dsl::result<typename lak::dsl::meson_wrap_t::value_type>
lak::dsl::meson_wrap_t::parse(lak::u8string_view str) const
{
	return lak::wrap_parse::wrap.parse(str);
}
