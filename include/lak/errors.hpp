#ifndef LAK_ERRORS_HPP
#define LAK_ERRORS_HPP

#include "lak/string.hpp"

namespace lak
{
	namespace err
	{
		struct out_of_data
		{
			inline lak::astring to_string() const { return "out of data"; }

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const out_of_data &err)
			{
				return strm << err.to_string();
			}
		};

		struct value_out_of_range
		{
			inline lak::astring to_string() const { return "value out of range"; }

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const value_out_of_range &err)
			{
				return strm << err.to_string();
			}
		};
	}
}

#endif
