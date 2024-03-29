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
			if constexpr (lak::is_reference_v<TUPLE>)
				return func(lak::forward<TUPLE>(tuple).template get<I>()...);
			else
				return func(lak::forward<lak::tuple_element_t<I, TUPLE>>(
				  lak::forward<TUPLE>(tuple).template get<I>())...);
		}
	}
}

template<typename FUNC, typename TUPLE>
auto lak::apply(FUNC &&func, TUPLE &&tuple)
{
	static_assert(lak::is_tuple_v<lak::remove_cvref_t<TUPLE>>);
	return lak::_apply(lak::make_index_sequence<lak::tuple_size_v<TUPLE>>{},
	                   lak::forward<FUNC>(func),
	                   lak::forward<TUPLE>(tuple));
}
