#ifndef LAK_VISIT_HPP
#define LAK_VISIT_HPP

#include "lak/compiler.hpp"
#include "lak/stdint.hpp"
#include "lak/type_traits.hpp"

namespace lak
{
	/*

	Example:

	template<size_t S>
	void some_function();

	ASSERT(lak::visit_switch(lak::index_sequence<0, 1, 2>{}, 1, [](auto index) {
	  using I = lak::remove_cvref_t<decltype(index)>;
	  static_assert(lak::is_integral_constant_v<I>);
	  some_function<I::value>();
	}));

	*/
	template<typename FUNCTOR, typename T, T I, T... J>
	auto visit_switch(lak::integer_sequence<T, I, J...>, T i, FUNCTOR &&functor);

	/*

	Example:

	lak::variant<char, int> my_variant;

	lak::visit(my_variant, [](auto &value) {
	  if constexpr (lak::is_same_v<decltype(value), char>)
	  {
	    //
	  }
	  else if constexpr (lak::is_same_v<decltype(value), int>)
	  {
	    //
	  }
	});

	*/
	template<typename VAR, typename FUNCTOR>
	auto visit(VAR &&variant, FUNCTOR &&functor);

	/*

	Example:

	lak::variant<char, int> my_variant;

	lak::visit(my_variant, overloaded {
	  [](char value) { },
	  [](int value) { },
	});

	*/
	template<class... Ts>
	struct overloaded : Ts...
	{
		using Ts::operator()...;
	};
	template<class... Ts>
	overloaded(Ts...) -> overloaded<Ts...>;
}

#include "lak/visit.inl"

#endif