#ifndef LAK_TRACE_HPP
#define LAK_TRACE_HPP

#include "lak/debug.hpp"
#include "lak/unicode.hpp"

namespace lak
{
	struct trace
	{
		lak::astring _file;
		size_t _line;

		trace(const lak::astring &file, size_t line)
		: _file(lak::move(file)), _line(line)
		{
		}

		trace(trace &&)      = default;
		trace(const trace &) = default;

		trace &operator=(trace &&) = default;
		trace &operator=(const trace &) = default;

		inline lak::astring to_string() const
		{
			return _file + ":" + TO_STRING(_line);
		}
	};

#define LINE_TRACE lak::trace(__FILE__, __LINE__)
}

#ifndef LAK_NO_STD
#	include <ostream>

static std::ostream &operator<<(std::ostream &strm, const lak::trace &tr)
{
	return strm << tr.to_string();
}
#endif

#endif