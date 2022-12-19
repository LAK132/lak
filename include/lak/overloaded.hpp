#ifndef LAK_OVERLOADED_HPP
#define LAK_OVERLOADED_HPP

namespace lak
{
	/*

	Example:

	lak::variant<char, int> my_variant;

	lak::visit(lak::overloaded{
	  [](char value) { },
	  [](int value) { },
	}, my_variant);

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
