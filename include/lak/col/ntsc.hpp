#ifndef LAK_COL_NTSC_HPP
#define LAK_COL_NTSC_HPP

#include "lak/col/cie.hpp"
#include "lak/col/cie_1931.hpp"

namespace lak
{
	namespace col
	{
		constexpr auto NTSC_primaries =
		  lak::col::cie::to_xyY(lak::col::cie::xy_primaries{
		    .r = {.630f, .340f},
		    .g = {.310f, .595f},
		    .b = {.155f, .070f},
		    .w = lak::col::cie::D65_xy,
		  });

		constexpr auto NTSC_J_primaries =
		  lak::col::cie::to_xyY(lak::col::cie::xy_primaries{
		    .r = {NTSC_primaries.r.x, NTSC_primaries.r.y},
		    .g = {NTSC_primaries.g.x, NTSC_primaries.g.y},
		    .b = {NTSC_primaries.b.x, NTSC_primaries.b.y},
		    .w = lak::col::cie::D93_xy,
		  });

		constexpr auto NTSC_FCC_primaries =
		  lak::col::cie::to_xyY(lak::col::cie::xy_primaries{
		    .r = {.670f, .330f},
		    .g = {.210f, .710f},
		    .b = {.140f, .080f},
		    .w = lak::col::cie::C_xy,
		  });
	}
}

#endif
