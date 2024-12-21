#ifndef LAK_VISIT_HPP
#	define LAK_VISIT_HPP

#	define LAK_INDEX_SET_FORWARD_ONLY
#	include "lak/index_set.hpp"

namespace lak
{
	/*

	Example:

	template<size_t S>
	void some_function();

	lak::visit_switch(
	  lak::index_set<0, 1, 2>::make(1).unwrap(),
	  []<size_t I>(lak::size_type<I>) { some_function<I>(); });

	*/
	template<typename FUNC, size_t... I>
	auto visit_switch(lak::index_set<I...> index, FUNC &&func);

	/*

	Example:

	lak::variant<char, int> my_variant;

	lak::visit([]<typename T>(T &value) {
	  if constexpr (lak::is_same_v<T, char>)
	  {
	    //
	  }
	  else if constexpr (lak::is_same_v<T, int>)
	  {
	    //
	  }
	}, my_variant);

	*/
	template<typename FUNC, typename... VAR>
	auto visit(FUNC &&func, VAR &&...variants);
}

#endif

#ifdef LAK_VISIT_FORWARD_ONLY
#	undef LAK_VISIT_FORWARD_ONLY
#else
#	ifndef LAK_VISIT_HPP_IMPL
#		define LAK_VISIT_HPP_IMPL

#		include "lak/visit.inl"

#	endif
#endif
