#ifndef LAK_COL_PAL_HPP
#define LAK_COL_PAL_HPP

#include "lak/col/cie.hpp"
#include "lak/col/cie_1931.hpp"

namespace lak
{
	namespace col
	{
		constexpr auto PAL_primaries =
		  lak::col::cie::to_xyY(lak::col::cie::xy_primaries{
		    .r = {.640f, .330f},
		    .g = {.290f, .600},
		    .b = {.150f, .060f},
		    .w = lak::col::cie::D65_xy,
		  });

		constexpr auto PAL_M_primaries =
		  lak::col::cie::to_xyY(lak::col::cie::xy_primaries{
		    .r = {.670f, .330f},
		    .g = {.210f, .710f},
		    .b = {.140f, .080f},
		    .w = lak::col::cie::C_xy,
		  });
	}
}

#endif
