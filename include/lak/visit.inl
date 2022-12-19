#include "lak/visit.hpp"

#include "lak/concepts.hpp"
#include "lak/optional.hpp"

namespace lak
{
	namespace
	{
		template<typename FUNC, size_t I, size_t... J>
		requires(lak::is_void_v<lak::invoke_result_t<FUNC, lak::size_type<I>>>) //
		  force_inline
		  void _visit_switch(lak::index_set<I, J...> index, FUNC &&func)
		{
			auto caller = [&]<size_t K>(lak::size_type<K> k) -> bool
			{
				if (K == index.value())
				{
					func(k);
					return true;
				}
				else
					return false;
			};

			(caller(lak::size_type<I>{}) || ... || caller(lak::size_type<J>{}));
		}

		template<typename FUNC, size_t I, size_t... J>
		requires(!lak::is_void_v<lak::invoke_result_t<FUNC, lak::size_type<I>>>) //
		  force_inline
		  auto _visit_switch(lak::index_set<I, J...> index, FUNC &&func)
		    -> lak::invoke_result_t<FUNC, lak::size_type<I>>
		{
			using result_type = lak::invoke_result_t<FUNC, lak::size_type<I>>;

			auto caller =
			  [&]<size_t K>(lak::size_type<K> k) -> lak::optional<result_type>
			{
				if (K == index.value())
					return lak::make_optional<result_type>(func(k));
				else
					return lak::nullopt;
			};

			return *(caller(lak::size_type<I>{}) | ... |
			         caller(lak::size_type<J>{}));
		}

		template<typename FUNC, typename VAR>
		force_inline auto _visit(FUNC &&func, VAR &&variant)
		{
			static_assert(lak::is_variant_v<lak::remove_cvref_t<VAR>>);

			return _visit_switch(
			  variant.index_set(),
			  [&]<size_t I>(lak::size_type<I>)
			  { return func(*lak::forward<VAR>(variant).template get<I>()); });
		}

		template<typename FUNC, typename VAR, typename VAR2, typename... VARS>
		force_inline auto _visit(FUNC &&func,
		                         VAR &&variant,
		                         VAR2 &&variant2,
		                         VARS &&...variants)
		{
			static_assert(lak::is_variant_v<lak::remove_cvref_t<VAR>> &&
			              (lak::is_variant_v<lak::remove_cvref_t<VAR2>> && ... &&
			               lak::is_variant_v<lak::remove_cvref_t<VARS>>));

			return _visit_switch(
			  variant.index_set(),
			  [&]<size_t I>(lak::size_type<I>)
			  {
				  return _visit(
				    lak::bind_front(lak::forward<FUNC>(func),
				                    *lak::forward<VAR>(variant).template get<I>()),
				    lak::forward<VAR2>(variant2),
				    lak::forward<VARS>(variants)...);
			  });
		}
	}
}

template<typename FUNC, size_t... I>
auto lak::visit_switch(lak::index_set<I...> index, FUNC &&func)
{
	return lak::_visit_switch(index, lak::forward<FUNC>(func));
}

template<typename FUNC, typename... VAR>
auto lak::visit(FUNC &&func, VAR &&...variants)
{
	return lak::_visit(lak::forward<FUNC>(func), lak::forward<VAR>(variants)...);
}
