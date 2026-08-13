#ifndef LAK_COL_CIE_1976_HPP
#define LAK_COL_CIE_1976_HPP

#include "lak/col/cie_1931.hpp"

#include <cmath>

namespace lak
{
	namespace col
	{
		namespace cie
		{
			// CIELUV u' v'
			struct uv
			{
				float u; // u'
				float v; // v'

				static constexpr uv from_vec(lak::vec2f_t v)
				{
					return {.u = v.x, .v = v.y};
				}
				constexpr lak::vec2f_t to_vec() const { return {u, v}; }
			};

			// CIELUV u' v' Y
			struct uvY
			{
				float u; // u'
				float v; // v'
				float Y; // Y

				static constexpr uvY from_vec(lak::vec3f_t v)
				{
					return {.u = v.x, .v = v.y, .Y = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {u, v, Y}; }
			};

			// CIELUV L* u* v*
			struct Luv
			{
				float L; // L*
				float u; // u*
				float v; // v*

				static constexpr Luv from_vec(lak::vec3f_t v)
				{
					return {.L = v.x, .u = v.y, .v = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {L, u, v}; }
			};

			// CIELCh L* C*uv huv
			struct LCh
			{
				float L; // L*
				float C; // C*uv
				float h; // huv

				constexpr float s() const { return C / L; }

				static constexpr LCh from_vec(lak::vec3f_t v)
				{
					return {.L = v.x, .C = v.y, .h = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {L, C, h}; }
			};

			// CIELAB L* a* b*
			struct Lab
			{
				float L; // L*
				float a; // a*
				float b; // b*

				static constexpr Lab from_vec(lak::vec3f_t v)
				{
					return {.L = v.x, .a = v.y, .b = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {L, a, b}; }
			};

			constexpr lak::col::cie::xy to_xy(lak::col::cie::uv colour)
			{
				const float divisor = (6.f * colour.u) + (-16.f * colour.v) + 12.f;
				return {
				  .x = (9.f * colour.u) / divisor,
				  .y = (4.f * colour.v) / divisor,
				};
			}

			constexpr lak::col::cie::xyY to_xyY(lak::col::cie::uvY colour)
			{
				const auto [x, y] =
				  to_xy(lak::col::cie::uv{.u = colour.u, .v = colour.v});
				return {
				  .x = x,
				  .y = y,
				  .Y = colour.Y,
				};
			}

			constexpr lak::col::cie::XYZ to_XYZ(lak::col::cie::Luv colour,
			                                    lak::col::cie::uvY white)
			{
				const float u = ((colour.u / colour.L) / 13.f) + white.u;
				const float v = ((colour.v / colour.L) / 13.f) + white.v;
				const float Y =
				  (colour.L <= 8.f)
				    ? (std::pow(3.f / 29.f, 3.f) * colour.L * white.Y)
				    : (std::pow((colour.L + 16.f) / 116.f, 3.f) * white.Y);
				return {
				  .X = ((9.f * u) / (4.f * v)) * Y,
				  .Y = Y,
				  .Z = (((-3.f * u) + (-20.f * v) + 12.f) / (4.f * v)) * Y,
				};
			}

			constexpr lak::col::cie::uv to_uv(lak::col::cie::xy colour)
			{
				const float divisor = (-2.f * colour.x) + (12.f * colour.y) + 3.f;
				return {
				  .u = (4.f * colour.x) / divisor,
				  .v = (9.f * colour.y) / divisor,
				};
			}

			constexpr lak::col::cie::uvY to_uvY(lak::col::cie::uv chroma, float luma)
			{
				return {
				  .u = chroma.u,
				  .v = chroma.v,
				  .Y = luma,
				};
			}

			constexpr lak::col::cie::uvY to_uvY(lak::col::cie::xyY colour)
			{
				const auto c_uv =
				  to_uv(lak::col::cie::xy{.x = colour.x, .y = colour.y});
				return {
				  .u = c_uv.u,
				  .v = c_uv.v,
				  .Y = colour.Y,
				};
			}

			constexpr lak::col::cie::Luv to_Luv(lak::col::cie::uvY colour,
			                                    lak::col::cie::uvY white)
			{
				constexpr float thresh   = float(std::pow(6.0 / 29.0, 3.0));
				constexpr float less_pre = float(std::pow(29.0 / 3.0, 3.0));

				const float YYn = colour.Y / white.Y;
				const float L   = (YYn <= thresh) ? (less_pre * YYn)
				                                  : ((116.f * std::cbrt(YYn)) - 16.f);
				const float L13 = 13.f * L;
				return {
				  .L = L,
				  .u = L13 * (colour.u - white.u),
				  .v = L13 * (colour.v - white.v),
				};
			}

			constexpr lak::col::cie::Luv to_Luv(lak::col::cie::LCh colour)
			{
				return {
				  .L = colour.L,
				  .u = colour.C * std::cos(colour.h),
				  .v = colour.C * std::sin(colour.h),
				};
			}

			constexpr lak::col::cie::LCh to_LCh(lak::col::cie::Luv colour)
			{
				return {
				  .L = colour.L,
				  .C = std::sqrt((colour.u * colour.u) + (colour.v * colour.v)),
				  .h = std::atan2(colour.v, colour.u),
				};
			}

			// ---

			constexpr lak::col::cie::xyY to_xyY(lak::col::cie::Luv colour,
			                                    lak::col::cie::uvY white)
			{
				return to_xyY(to_XYZ(colour, white));
			}

			constexpr lak::col::cie::xyY to_xyY(lak::col::cie::Luv colour,
			                                    lak::col::cie::uv white)
			{
				return to_xyY(colour, {white.u, white.v, 1.f});
			}

			constexpr lak::col::cie::xyY to_xyY(lak::col::cie::LCh colour,
			                                    lak::col::cie::uvY white)
			{
				return to_xyY(to_Luv(colour), white);
			}

			constexpr lak::col::cie::xyY to_xyY(lak::col::cie::LCh colour,
			                                    lak::col::cie::uv white)
			{
				return to_xyY(colour, {white.u, white.v, 1.f});
			}

			constexpr lak::col::cie::XYZ to_XYZ(lak::col::cie::Luv colour,
			                                    lak::col::cie::uv white)
			{
				return to_XYZ(colour, {white.u, white.v, 1.f});
			}

			constexpr lak::col::cie::uvY to_uvY(lak::col::cie::XYZ colour)
			{
				return to_uvY(to_xyY(colour));
			}

			constexpr lak::col::cie::uvY to_uvY(lak::col::cie::Luv colour,
			                                    lak::col::cie::uv white)
			{
				return to_uvY(to_XYZ(colour, white));
			}

			constexpr lak::col::cie::uvY to_uvY(lak::col::cie::Luv colour,
			                                    lak::col::cie::uvY white)
			{
				return to_uvY(to_XYZ(colour, white));
			}

			constexpr lak::col::cie::Luv to_Luv(lak::col::cie::uvY colour,
			                                    lak::col::cie::uv white)
			{
				return to_Luv(colour, {white.u, white.v, 1.f});
			}

			constexpr lak::col::cie::Luv to_Luv(lak::col::cie::xyY colour,
			                                    lak::col::cie::uvY white)
			{
				return to_Luv(to_uvY(colour), white);
			}

			constexpr lak::col::cie::Luv to_Luv(lak::col::cie::xyY colour,
			                                    lak::col::cie::uv white)
			{
				return to_Luv(to_uvY(colour), white);
			}

			constexpr lak::col::cie::Luv to_Luv(lak::col::cie::XYZ colour,
			                                    lak::col::cie::uvY white)
			{
				return to_Luv(to_uvY(colour), white);
			}

			constexpr lak::col::cie::Luv to_Luv(lak::col::cie::XYZ colour,
			                                    lak::col::cie::uv white)
			{
				return to_Luv(to_uvY(colour), white);
			}

			constexpr lak::col::cie::LCh to_LCh(lak::col::cie::xyY colour,
			                                    lak::col::cie::uvY white)
			{
				return to_LCh(to_Luv(colour, white));
			}

			constexpr lak::col::cie::LCh to_LCh(lak::col::cie::xyY colour,
			                                    lak::col::cie::uv white)
			{
				return to_LCh(to_Luv(colour, white));
			}
		}
	}
}

#endif
