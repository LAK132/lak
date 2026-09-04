#ifndef LAK_STRING_UTILS_HPP
#define LAK_STRING_UTILS_HPP

#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/string.hpp"
#include "lak/tuple.hpp"

namespace lak
{
	template<typename CHAR>
	lak::string<CHAR> join_strings(lak::string_view<CHAR> separator,
	                               lak::span<const lak::string<CHAR>> strings);
	template<typename CHAR>
	lak::string<CHAR> join_strings(
	  lak::string_view<CHAR> separator,
	  lak::span<const lak::string_view<CHAR>> strings);
}

#include "lak/string_utils.inl"

#endif
