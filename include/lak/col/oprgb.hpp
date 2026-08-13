#ifndef LAK_COL_OPRGB_HPP
#define LAK_COL_OPRGB_HPP

#include "lak/col/cie.hpp"

namespace lak
{
	namespace col
	{
		constexpr lak::col::cie::xyY_primaries opRGB_primaries =
		  lak::col::to_xyY(lak::col::cie::xyY_primaries{
		    .r = {.x = 0.64f, .y = 0.33f},
		    .g = {.x = 0.21f, .y = 0.71f},
		    .b = {.x = 0.15f, .y = 0.06f},
		    .w = {.x = 0.3127f, .y = 0.3290f},
		  });

		constexpr lak::col::cie::XYZ opRGB_white = {
		  .X = 152.07f,
		  .Y = 160.00f,
		  .Z = 174.25f,
		};

		constexpr lak::col::cie::XYZ opRGB_black = {
		  .X = 0.5282f,
		  .Y = 0.5557f,
		  .Z = 0.6052f,
		};

		struct opRGB
		{
			float R; // R'
			float G; // G'
			float B; // B'

			static constexpr opRGB from_vec(lak::vec3f_t v)
			{
				return {.R = v.x, .G = v.y, .B = v.z};
			}
			constexpr lak::vec3f_t to_vec() const { return {R, G, B}; }
		};

		struct opRGB_linear
		{
			float R;
			float G;
			float B;

			static constexpr opRGB_linear from_vec(lak::vec3f_t v)
			{
				return {.R = v.x, .G = v.y, .B = v.z};
			}
			constexpr lak::vec3f_t to_vec() const { return {R, G, B}; }
		};
	}
}

#endif
