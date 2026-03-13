#include "lak/streamify_traits.hpp"

#ifndef LAK_STREAMIFY_HPP
#	define LAK_STREAMIFY_HPP

#	include "lak/string.hpp"

#	ifndef LAK_SPAN_FORWARD_ONLY
#		define LAK_SPAN_FORWARD_ONLY
#	endif
#	include "lak/span.hpp"

namespace lak
{
	template<typename ARG, typename... ARGS>
	lak::u8string spaced_streamify(const lak::u8string &space,
	                               const ARG &arg,
	                               const ARGS &...args);

	template<typename T, size_t S>
	lak::u8string spaced_streamify(const lak::u8string &space,
	                               lak::span<T, S> args);

	template<typename... ARGS>
	lak::u8string streamify(const ARGS &...args);
}

#endif

#ifdef LAK_STREAMIFY_FORWARD_ONLY
#	undef LAK_STREAMIFY_FORWARD_ONLY
#else
#	ifndef LAK_STREAMIFY_HPP_IMPL
#		define LAK_STREAMIFY_HPP_IMPL
#		include "lak/streamify.inl"
#	endif
#endif
