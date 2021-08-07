#ifndef LAK_ALGORITHM_HPP
#define LAK_ALGORITHM_HPP

#include "lak/utility.hpp"

namespace lak
{
	template<typename IN_ITER, typename OUT_ITER>
	OUT_ITER move(IN_ITER begin, IN_ITER end, OUT_ITER output)
	{
		for (; begin != end; ++begin, ++output) *output = lak::move(*begin);
		return output;
	}

	template<typename IN_ITER, typename OUT_ITER>
	OUT_ITER copy(IN_ITER begin, IN_ITER end, OUT_ITER output)
	{
		for (; begin != end; ++begin, ++output) *output = *begin;
		return output;
	}
}

#endif