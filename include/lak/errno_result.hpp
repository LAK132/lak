#ifndef LAK_ERRNO_RESULT_HPP
#define LAK_ERRNO_RESULT_HPP

#include "lak/format.hpp"
#include "lak/result.hpp"
#include "lak/strconv.hpp"

#include <cerrno>
#include <system_error>

namespace lak
{
	struct errno_error
	{
		int value;

		static errno_error last_error() { return {errno}; }

		template<typename CHAR = char>
		inline lak::string<CHAR> to_string() const
		{
			auto msg = std::error_code(value, std::generic_category()).message();
			return lak::fmt<CHAR, "{} ({})">(lak::string_view(msg), value);
		}

		inline lak::u8string to_u8string() const { return to_string<char8_t>(); }
	};

	template<typename CHAR>
	struct format_traits<lak::errno_error, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(const lak::errno_error &err)
		{
			return err.template to_string<CHAR>();
		}
	};

	template<typename T = lak::monostate>
	using errno_result = lak::result<T, lak::errno_error>;
}

#endif
