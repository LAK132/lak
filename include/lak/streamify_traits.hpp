#ifndef LAK_STREAMIFY_TRAITS_HPP
#define LAK_STREAMIFY_TRAITS_HPP

#include "lak/concepts.hpp"

#include <ostream>

namespace lak
{
	namespace concepts
	{
		template<typename T>
		concept streamable = requires(T thing) {
			{
				std::declval<std::ostream &>() << thing
			} -> lak::concepts::same_as<std::ostream &>;
		};
	}

	template<typename T>
	constexpr bool is_streamable_v = lak::concepts::streamable<T>;
}

#endif
