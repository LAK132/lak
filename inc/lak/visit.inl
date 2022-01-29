#include "lak/visit.hpp"

#include "lak/concepts.hpp"
#include "lak/optional.hpp"

namespace lak
{
	namespace
	{
		template<typename FUNCTOR, typename T, T I, T... J>
		requires(lak::is_void_v<
		         lak::invoke_result_t<FUNCTOR, lak::integral_constant<T, I>>>) //
		  force_inline auto _visit_switch(lak::integer_sequence<T, I, J...>,
		                                  T i,
		                                  FUNCTOR &&functor) -> lak::optional<>
		{
			if (I == i)
			{
				functor(lak::integral_constant<T, I>{});
				return lak::optional<>(lak::in_place);
			}
			else if constexpr (sizeof...(J) > 0)
			{
				lak::optional<> result{_visit_switch(lak::integer_sequence<T, J...>{},
				                                     i,
				                                     lak::forward<FUNCTOR>(functor))};

				return result;
			}
			else
			{
				return lak::optional<>(lak::nullopt);
			}
		}

		template<typename FUNCTOR, typename T, T I, T... J>
		requires(!lak::is_void_v<
		         lak::invoke_result_t<FUNCTOR, lak::integral_constant<T, I>>>) //
		  force_inline auto _visit_switch(lak::integer_sequence<T, I, J...>,
		                                  T i,
		                                  FUNCTOR &&functor)
		    -> lak::optional<
		      lak::invoke_result_t<FUNCTOR, lak::integral_constant<T, I>>>
		{
			using result_type = lak::optional<
			  lak::invoke_result_t<FUNCTOR, lak::integral_constant<T, I>>>;
			if (I == i)
			{
				return result_type(lak::in_place,
				                   functor(lak::integral_constant<T, I>{}));
			}
			else if constexpr (sizeof...(J) > 0)
			{
				result_type result{_visit_switch(lak::integer_sequence<T, J...>{},
				                                 i,
				                                 lak::forward<FUNCTOR>(functor))};

				return result;
			}
			else
			{
				return result_type(lak::nullopt);
			}
		}
	}
}

template<typename FUNCTOR, typename T, T I, T... J>
auto lak::visit_switch(lak::integer_sequence<T, I, J...>,
                       T i,
                       FUNCTOR &&functor)
{
	return lak::_visit_switch(
	  lak::integer_sequence<T, I, J...>{}, i, lak::forward<FUNCTOR>(functor));
}

template<typename VAR, typename FUNCTOR>
auto lak::visit(VAR &&variant, FUNCTOR &&functor)
{
	static_assert(lak::is_variant_v<lak::remove_cvref_t<VAR>>);

	return *lak::visit_switch(
	  typename lak::remove_cvref_t<VAR>::indices{},
	  variant.index(),
	  [&]<lak::concepts::integral_constant I>(const I &)
	  { return functor(*variant.template get<I::value>()); });
}
