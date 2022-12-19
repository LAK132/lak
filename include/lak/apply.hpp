#ifndef LAK_APPLY_HPP
#define LAK_APPLY_HPP

namespace lak
{
	template<typename FUNC, typename TUPLE>
	auto apply(FUNC &&func, TUPLE &&tuple);
}

#include "lak/apply.inl"

#endif
