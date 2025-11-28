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
		};

		struct value_out_of_range
		{
			inline lak::astring to_string() const { return "value out of range"; }
		};
	}
}

#endif
