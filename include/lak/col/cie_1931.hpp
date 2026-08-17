#ifndef LAK_COL_CIE_1931_HPP
#define LAK_COL_CIE_1931_HPP

#include "lak/debug.hpp"
#include "lak/span.hpp"
#include "lak/vec.hpp"

// https://www.cie.co.at/datatable/cie-1931-chromaticity-coordinates-spectrum-loci-2-degree-observer

namespace lak
{
	namespace col
	{
		namespace cie
		{
			struct xy
			{
				float x;
				float y;

				static constexpr xy from_vec(lak::vec2f_t v)
				{
					return {.x = v.x, .y = v.y};
				}
				constexpr lak::vec2f_t to_vec() const { return {x, y}; }
			};

			struct xyY
			{
				float x;
				float y;
				float Y;

				static constexpr xyY from_vec(lak::vec3f_t v)
				{
					return {.x = v.x, .y = v.y, .Y = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {x, y, Y}; }
			};

			struct XYZ
			{
				float X;
				float Y;
				float Z;

				static constexpr XYZ from_vec(lak::vec3f_t v)
				{
					return {.X = v.x, .Y = v.y, .Z = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {X, Y, Z}; }
			};

			struct RGB
			{
				float R;
				float G;
				float B;

				static constexpr RGB from_vec(lak::vec3f_t v)
				{
					return {.R = v.x, .G = v.y, .B = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {R, G, B}; }
			};

			constexpr lak::col::cie::xyY to_xyY(lak::col::cie::xy chroma, float luma)
			{
				return {
				  .x = chroma.x,
				  .y = chroma.y,
				  .Y = luma,
				};
			}

			constexpr lak::col::cie::xyY to_xyY(lak::col::cie::XYZ colour)
			{
				const float sum = colour.X + colour.Y + colour.Z;
				return {
				  .x = colour.X / sum,
				  .y = colour.Y / sum,
				  .Y = colour.Y,
				};
			}

			// Assuming Y=1
			constexpr lak::col::cie::XYZ to_XYZ(lak::col::cie::xy colour)
			{
				return {
				  .X = colour.x / colour.y,
				  .Y = 1.f,
				  .Z = (1.f - (colour.x + colour.y)) / colour.y,
				};
			}

			constexpr lak::col::cie::XYZ to_XYZ(lak::col::cie::xyY colour)
			{
				const float Yy = colour.Y / colour.y;
				return {
				  .X = Yy * colour.x,
				  .Y = colour.Y,
				  .Z = Yy * (1.f - (colour.x + colour.y)),
				};
			}

			constexpr lak::col::cie::XYZ to_XYZ(lak::col::cie::RGB colour)
			{
				// https://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_rg_chromaticity_space
				const auto from_vec = lak::vec3f64_t(colour.R, colour.G, colour.B);
				const auto mat =
				  lak::mat3f64_t(lak::vec3f64_t(0.49000, 0.31000, 0.20000),
				                 lak::vec3f64_t(0.17697, 0.81240, 0.01063),
				                 lak::vec3f64_t(0.00000, 0.01000, 0.99000));
				const auto to_vec = mat * from_vec;
				return {
				  .X = float(to_vec.x),
				  .Y = float(to_vec.y),
				  .Z = float(to_vec.z),
				};
			}

			constexpr lak::col::cie::RGB to_RGB(lak::col::cie::XYZ colour)
			{
				// https://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_rg_chromaticity_space
				const auto from_vec = lak::vec3f64_t(colour.X, colour.Y, colour.Z);
				const auto mat =
				  lak::mat3f64_t(lak::vec3f64_t(2.36461385, -0.89654057, -0.46807328),
				                 lak::vec3f64_t(-0.51516621, 1.4264081, 0.0887581),
				                 lak::vec3f64_t(0.0052037, -0.01440816, 1.00920446));
				const auto to_vec = mat * from_vec;
				return {
				  .R = float(to_vec.x),
				  .G = float(to_vec.y),
				  .B = float(to_vec.z),
				};
			}

			// ---

			constexpr lak::col::cie::xyY to_xyY(lak::col::cie::RGB colour)
			{
				return to_xyY(to_XYZ(colour));
			}

			constexpr lak::col::cie::XYZ to_XYZ(lak::col::cie::xy chroma, float luma)
			{
				return to_XYZ(to_xyY(chroma, luma));
			}

			constexpr lak::col::cie::RGB to_RGB(lak::col::cie::xy chroma, float luma)
			{
				return to_RGB(to_XYZ(chroma, luma));
			}

			constexpr lak::col::cie::RGB to_RGB(lak::col::cie::xyY colour)
			{
				return to_RGB(to_XYZ(colour));
			}

			// ---

			struct xyY_primaries
			{
				lak::col::cie::xyY r;
				lak::col::cie::xyY g;
				lak::col::cie::xyY b;
				lak::col::cie::xy w;

				constexpr lak::vec3f_t w_xyz() const
				{
					return {w.x, w.y, 1.f - (w.x + w.y)};
				}

				constexpr lak::col::cie::xyY w_xyY() const { return {w.x, w.y, 1.f}; }
				constexpr lak::col::cie::XYZ w_XYZ() const { return to_XYZ(w); }

				constexpr void regenerate_Y()
				{
					r.Y = g.Y = b.Y = 1.f;

					const auto rgb_Y = lak::inverse(lak::transpose(lak::mat3f_t{
					                     to_XYZ(r).to_vec(),
					                     to_XYZ(g).to_vec(),
					                     to_XYZ(b).to_vec(),
					                   })) *
					                   to_XYZ(w).to_vec();

					r.Y = rgb_Y.x;
					g.Y = rgb_Y.y;
					b.Y = rgb_Y.z;
				}

				constexpr lak::mat3f_t linear_to_XYZ() const
				{
					return lak::transpose(lak::mat3f_t{
					  to_XYZ(r).to_vec(),
					  to_XYZ(g).to_vec(),
					  to_XYZ(b).to_vec(),
					});
				}

				constexpr auto linear_to_xyY() const
				{
					return
					  [mat = linear_to_XYZ()](lak::vec3f_t colour) -> lak::col::cie::xyY
					{
						return lak::col::cie::to_xyY(
						  lak::col::cie::XYZ::from_vec(mat * colour));
					};
				}

				constexpr lak::col::cie::xyY linear_to_xyY(lak::vec3f_t colour) const
				{
					return linear_to_xyY()(colour);
				}

				constexpr void linear_to_xyY(lak::span<lak::col::cie::xyY> dst,
				                             lak::span<const lak::vec3f_t> src) const
				{
					ASSERT_EQUAL(src.size(), dst.size());
					const auto transform = linear_to_xyY();
					for (size_t i = 0U; i < src.size(); ++i) dst[i] = transform(src[i]);
				}

				constexpr lak::mat3f_t XYZ_to_linear() const
				{
					return lak::inverse(linear_to_XYZ());
				}

				constexpr auto xyY_to_linear() const
				{
					return
					  [mat = XYZ_to_linear()](lak::col::cie::xyY colour) -> lak::vec3f_t
					{ return mat * lak::col::cie::to_XYZ(colour).to_vec(); };
				}

				constexpr lak::vec3f_t xyY_to_linear(lak::col::cie::xyY colour) const
				{
					return xyY_to_linear()(colour);
				}

				constexpr void xyY_to_linear(
				  lak::span<lak::vec3f_t> dst,
				  lak::span<const lak::col::cie::xyY> src) const
				{
					ASSERT_EQUAL(src.size(), dst.size());
					const auto transform = xyY_to_linear();
					for (size_t i = 0U; i < src.size(); ++i) dst[i] = transform(src[i]);
				}
			};

			struct xy_primaries
			{
				lak::col::cie::xy r;
				lak::col::cie::xy g;
				lak::col::cie::xy b;
				lak::col::cie::xy w;
			};

			constexpr lak::col::cie::xyY_primaries to_xyY(
			  lak::col::cie::xy_primaries primaries)
			{
				lak::col::cie::xyY_primaries result{
				  .r = {primaries.r.x, primaries.r.y, 1.f},
				  .g = {primaries.g.x, primaries.g.y, 1.f},
				  .b = {primaries.b.x, primaries.b.y, 1.f},
				  .w = primaries.w,
				};

				result.regenerate_Y();

				return result;
			}
		}
	}
}

#endif
