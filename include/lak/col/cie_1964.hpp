#ifndef LAK_COL_CIE_1964_HPP
#define LAK_COL_CIE_1964_HPP

#include "lak/col/cie_1976.hpp"

namespace lak
{
	namespace col
	{
		namespace cie
		{
			// CIEUVW U* V* W*
			struct UVW
			{
				float U; // U*
				float V; // V*
				float W; // W*

				static constexpr UVW from_linear(lak::vec3f_t v)
				{
					return {.U = v.x, .V = v.y, .W = v.z};
				}
				constexpr lak::vec3f_t to_linear() const { return {U, V, W}; }
			};

			constexpr lak::col::cie::UVW to_UVW(lak::col::cie::uvY colour,
			                                    lak::col::cie::uv white)
			{
				const float W   = (25.f * std::cbrt(colour.Y)) - 17.f;
				const float W13 = 13.f * W;
				return {
				  .U = W13 * (colour.u - white.u),
				  .V = W13 * (colour.v - white.v),
				  .W = W,
				};
			}
		}
	}
}

#endif
