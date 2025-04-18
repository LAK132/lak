#ifndef LAK_OVERLOADED_VISITOR_HPP
#define LAK_OVERLOADED_VISITOR_HPP

#include "lak/overloaded.hpp"
#include "lak/visit.hpp"

namespace lak
{
	/*

	Example:

	void variant_callback_function(auto fn)
	{
	  lak::variant<char, int> var;
	  fn(var);
	}

	variant_callback_function(lak::overloaded_visitor{
	  [](char value) { },
	  [](int value) { },
	});

	*/

	template<class... Ts>
	struct overloaded_visitor : lak::overloaded<Ts...>
	{
		template<typename VARIANT>
		auto operator()(VARIANT &&variant)
		{
			return lak::visit(*static_cast<lak::overloaded<Ts...> *>(this),
			                  lak::forward<VARIANT>(variant));
		}
	};

	template<class... Ts>
	overloaded_visitor(Ts...) -> overloaded_visitor<Ts...>;
}

#endif
