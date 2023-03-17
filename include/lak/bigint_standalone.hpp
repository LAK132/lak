#ifndef LAK_BIGINT_STANDALONE_HPP
#define LAK_BIGINT_STANDALONE_HPP

#include <span>
#include <tuple>
#include <vector>

#include "lak/bigint.hpp"

#endif

#ifdef LAK_BIGINT_STANDALONE_IMPL
#undef LAK_BIGINT_STANDALONE_IMPL

#include "../../src/bigint.cpp"
#include "../../src/debug.cpp"
#include "../../src/memmanip.cpp"
#include "../../src/strconv.cpp"
#include "../../src/string_view.cpp"
#include "../../src/unicode.cpp"

#endif
