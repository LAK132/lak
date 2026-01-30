#ifndef LAK_SYSTEM_COBALT_RESULT_HPP
#define LAK_SYSTEM_COBALT_RESULT_HPP

#include "lak/result.hpp"

#include <RendererInterface/SuccessToken.h>

namespace lak
{
	namespace cobalt
	{
		inline lak::result<lak::monostate, lak::monostate> as_result(
		  ::cobalt::graphics::SuccessToken st)
		{
			if (st.Failed())
				return lak::err_t{};
			else
				return lak::ok_t{};
		}
	}
}

#endif
