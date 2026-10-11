#ifndef LAK_COL_PROPHOTO_HPP
#define LAK_COL_PROPHOTO_HPP

#include "lak/col/cie.hpp"

namespace lak
{
	namespace col
	{
		constexpr lak::col::cie::xyY_primaries ProPhotoRGB_primaries{
		  .r = {.x = 0.734699f, .y = 0.265301f, .Y = 0.28804f},
		  .g = {.x = 0.159597f, .y = 0.840403f, .Y = 0.71188f},
		  .b = {.x = 0.036598f, .y = 0.000105f, .Y = 0.00009f},
		  .w = {.x = 0.345704f, .y = 0.358540f},
		};
	}
}

#endif
