#ifndef LAK_STRING_OSTREAM
#define LAK_STRING_OSTREAM

#include "lak/string.hpp"
#include "lak/string_view.hpp"
#include "lak/unicode.hpp"

#include <ostream>

namespace lak
{
	template<typename OUT_CHAR, typename IN_CHAR>
	std::basic_ostream<OUT_CHAR> &operator<<(
	  std::basic_ostream<OUT_CHAR> &strm, const lak::string_view<IN_CHAR> &str)
	{
		if constexpr (lak::is_same_v<OUT_CHAR, IN_CHAR>)
		{
			for (const auto &c : str) strm << c;
		}
		else if constexpr (lak::is_same_v<OUT_CHAR, char>)
		{
			for (const auto &[code, len] : lak::codepoint_range<IN_CHAR>(str))
			{
				if (len > 0U)
				{
					lak::codepoint_buffer_t<char8_t> buff{};
					for (const char8_t &c : lak::from_codepoint(
					       lak::codepoint_buffer_span<char8_t>(buff), code))
						strm << char(c);
				}
			}
		}
		else
		{
			for (const auto &[code, len] : lak::codepoint_range<IN_CHAR>(str))
			{
				if (len > 0U)
				{
					typename lak::codepoint_buffer_t<OUT_CHAR> buff{};
					for (const OUT_CHAR &c : lak::from_codepoint(
					       typename lak::codepoint_buffer_span<OUT_CHAR>(buff), code))
						strm << c;
				}
			}
		}
		return strm;
	}

	// :TODO: lak::astring is just std::string
	template<typename OUT_CHAR, typename IN_CHAR>
	requires(!lak::is_same_v<OUT_CHAR, IN_CHAR>)
	inline std::basic_ostream<OUT_CHAR> &operator<<(
	  std::basic_ostream<OUT_CHAR> &strm, const lak::string<IN_CHAR> &str)
	{
		return strm << lak::string_view<IN_CHAR>(str);
	}
}

#endif
