#ifndef LAK_COL_CIE_1960_HPP
#define LAK_COL_CIE_1960_HPP

#include "lak/col/cie_1976.hpp"

namespace lak
{
	namespace col
	{
		namespace cie
		{
			struct uv_1960
			{
				float u;
				float v;

				static constexpr uv_1960 from_vec(lak::vec2f_t v)
				{
					return {.u = v.x, .v = v.y};
				}
				constexpr lak::vec2f_t to_vec() const { return {u, v}; }
			};

			struct UVW_1960
			{
				float U;
				float V;
				float W;

				static constexpr UVW_1960 from_vec(lak::vec3f_t v)
				{
					return {.U = v.x, .V = v.y, .W = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {U, V, W}; }
			};

			constexpr lak::col::cie::xy to_xy(lak::col::cie::uv_1960 colour)
			{
				const float divisor = (2.f * colour.u) + (-8.f * colour.v) + 4.f;
				return {
				  .x = (3.f * colour.u) / divisor,
				  .y = (2.f * colour.v) / divisor,
				};
			}

			constexpr lak::col::cie::XYZ to_XYZ(lak::col::cie::UVW_1960 colour)
			{
				const float X = (3.f / 2.f) * colour.U;
				return {
				  .X = X,
				  .Y = colour.V,
				  .Z = X + (-3.f * colour.V) + (2.f * colour.W),
				};
			}

			constexpr lak::col::cie::uv_1960 to_uv_1960(lak::col::cie::xy colour)
			{
				const float divisor = (-2.f * colour.x) + (12.f * colour.y) + 3.f;
				return {
				  .u = (4.f * colour.x) / divisor,
				  .v = (6.f * colour.y) / divisor,
				};
			}

			constexpr lak::col::cie::uv_1960 to_uv_1960(lak::col::cie::uv colour)
			{
				return {
				  .u = colour.u,
				  .v = (3.f / 2.f) * colour.v,
				};
			}

			constexpr lak::col::cie::UVW_1960 to_UVW_1960(lak::col::cie::XYZ colour)
			{
				return {
				  .U = (2.f / 3.f) * colour.X,
				  .V = colour.Y,
				  .W = (1.f / 2.f) * ((3.f * colour.Y) + colour.Z - colour.X),
				};
			}

			constexpr lak::col::cie::uv to_uv(lak::col::cie::uv_1960 colour)
			{
				return {
				  .u = colour.u,
				  .v = (2.f / 3.f) * colour.v,
				};
			}
		}
	}
}

#endif
