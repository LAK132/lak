#include "lak/tuple.hpp"
#include "lak/type_traits.hpp"
#include "lak/utility.hpp"

namespace lak
{
	namespace
	{
		template<typename FUNC, typename TUPLE, size_t... I>
		auto _apply(lak::index_sequence<I...>, FUNC &&func, TUPLE &&tuple)
		{
			return func(lak::forward<TUPLE>(tuple).template get<I>()...);
		}
	}
}

template<typename FUNC, typename TUPLE>
auto lak::apply(FUNC &&func, TUPLE &&tuple) requires(tuple.template get<0>())
{
	lak::_apply(lak::make_index_sequence<lak::tuple_size_v<TUPLE>>{},
	            lak::forward<FUNC>(func),
	            lak::forward<TUPLE>(tuple));
}
