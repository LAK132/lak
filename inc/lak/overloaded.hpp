#ifndef LAK_OVERLOADED_HPP
#define LAK_OVERLOADED_HPP

namespace lak
{
	/*

	Example:

	lak::variant<char, int> my_variant;

	lak::visit(my_variant, lak::overloaded{
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

#endif
