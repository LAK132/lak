#ifndef LAK_ERROR_CODE_RESULT_HPP
#define LAK_ERROR_CODE_RESULT_HPP

#include "lak/result.hpp"
#include "lak/strconv.hpp"
#include "lak/streamify.hpp"

#include <cstring>
#include <ostream>
#include <system_error>

namespace lak
{
	template<typename T = lak::monostate>
	using error_code_result = lak::result<T, std::error_code>;
}

#endif
