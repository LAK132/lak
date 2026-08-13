#ifndef LAK_COL_ITU_HPP
#define LAK_COL_ITU_HPP

#include "lak/col/cie.hpp"

namespace lak
{
	namespace col
	{
		namespace itu
		{
			constexpr lak::col::cie::xyY_primaries rec709_1_primaries{
			  .r = {.x = 0.640f, .y = 0.330f, .Y = 0.2125f},
			  .g = {.x = 0.300f, .y = 0.600f, .Y = 0.7154f},
			  .b = {.x = 0.150f, .y = 0.060f, .Y = 0.0721f},
			  .w = {.x = 0.3127f, .y = 0.3290f},
			};

			constexpr lak::col::cie::xyY_primaries rec709_primaries{
			  .r = {.x = 0.640f, .y = 0.330f, .Y = 0.2126f},
			  .g = {.x = 0.300f, .y = 0.600f, .Y = 0.7152f},
			  .b = {.x = 0.150f, .y = 0.060f, .Y = 0.0722f},
			  .w = {.x = 0.3127f, .y = 0.3290f},
			};

			struct rec709
			{
				float R; // E'_R
				float G; // E'_G
				float B; // E'_B

				static constexpr rec709 from_vec(lak::vec3f_t v)
				{
					return {.R = v.x, .G = v.y, .B = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {R, G, B}; }
			};

			struct rec709_linear
			{
				float R; // E_R
				float G; // E_G
				float B; // E_B

				static constexpr rec709_linear from_vec(lak::vec3f_t v)
				{
					return {.R = v.x, .G = v.y, .B = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {R, G, B}; }
			};

			constexpr lak::col::itu::rec709 to_rec709(
			  lak::col::itu::rec709_linear colour)
			{
				auto from_linear = [](float value)
				{
					if (value < 0.0018f)
						return 4.5f * value;
					else
						return (1.099f * std::pow(value, 0.45f)) - 0.099f;
				};
				return {
				  .R = from_linear(colour.R),
				  .G = from_linear(colour.G),
				  .B = from_linear(colour.B),
				};
			}

			constexpr lak::col::itu::rec709_linear to_rec709_linear(
			  lak::col::itu::rec709 colour)
			{
				auto to_linear = [](float value)
				{
					if (value < 0.081f)
						return value / 4.5f;
					else
						return std::pow((value + 0.099f) / 1.099f, 1.f / 0.45f);
				};
				return {
				  .R = to_linear(colour.R),
				  .G = to_linear(colour.G),
				  .B = to_linear(colour.B),
				};
			}

			constexpr lak::col::cie::xyY_primaries rec2020_primaries =
			  to_xyY(lak::col::cie::xy_primaries{
			    .r = {.x = 0.708f, .y = 0.292f},
			    .g = {.x = 0.170f, .y = 0.797f},
			    .b = {.x = 0.131f, .y = 0.046f},
			    .w = {.x = 0.3127f, .y = 0.3290f},
			  });

			constexpr lak::col::cie::xyY_primaries rec2100_primaries =
			  to_xyY(lak::col::cie::xy_primaries{
			    .r = {.x = 0.708f, .y = 0.292f},
			    .g = {.x = 0.170f, .y = 0.797f},
			    .b = {.x = 0.131f, .y = 0.046f},
			    .w = {.x = 0.3127f, .y = 0.3290f},
			  });
		}
	}
}

#endif
