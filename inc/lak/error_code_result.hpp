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
	struct error_code_error
	{
		std::error_code value;

		inline lak::astring to_string() const { return value.message(); }

		friend inline std::ostream &operator<<(std::ostream &strm,
		                                       const lak::error_code_error &err)
		{
			return strm << err.to_string();
		}
	};

	template<typename T = lak::monostate>
	using error_code_result = lak::result<T, error_code_error>;
}

#endif