#ifndef LAK_STREAMIFY_HPP
#define LAK_STREAMIFY_HPP

#include "lak/string.hpp"

namespace lak
{
	namespace concepts
	{
		template<typename T>
		concept streamable = requires(T thing)
		{
			{
				std::declval<std::ostream &>() << thing
				} -> lak::concepts::same_as<std::ostream &>;
		};
	}

	template<typename T>
	constexpr bool is_streamable_v = lak::concepts::streamable<T>;

	template<typename ARG, typename... ARGS>
	lak::u8string spaced_streamify(const lak::u8string &space,
	                               const ARG &arg,
	                               const ARGS &...args);

	template<typename... ARGS>
	lak::u8string streamify(const ARGS &...args);
}

#include "lak/streamify.inl"

#endif
