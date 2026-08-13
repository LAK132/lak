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

					const auto [r_X, r_Y, r_Z] = to_XYZ(r);
					const auto [g_X, g_Y, g_Z] = to_XYZ(g);
					const auto [b_X, b_Y, b_Z] = to_XYZ(b);
					const auto [w_X, w_Y, w_Z] = to_XYZ(w);

					const auto rgb_Y =
					  inverse(lak::mat3f_t{
					    {r_X, g_X, b_X}, {r_Y, g_Y, b_Y}, {r_Z, g_Z, b_Z}}) *
					  lak::vec3f_t(w_X, w_Y, w_Z);

					r.Y = rgb_Y.x;
					g.Y = rgb_Y.y;
					b.Y = rgb_Y.z;
				}

				constexpr lak::vec3f_t r_basis() const
				{
					return {r.x - w.x, r.y - w.y, r.Y};
				}
				constexpr lak::vec3f_t g_basis() const
				{
					return {g.x - w.x, g.y - w.y, g.Y};
				}
				constexpr lak::vec3f_t b_basis() const
				{
					return {b.x - w.x, b.y - w.y, b.Y};
				}
				constexpr lak::mat<float, 4U, 3U> bases() const
				{
					return {
					  r_basis(), g_basis(), b_basis(), lak::vec3f_t(w.x, w.y, 0.f)};
				}

				constexpr lak::mat4f_t linear_to_xyY() const
				{
					return {
					  lak::vec4f_t(r.x - w.x, g.x - w.x, b.x - w.x, w.x),
					  lak::vec4f_t(r.y - w.y, g.y - w.y, b.y - w.y, w.y),
					  lak::vec4f_t(r.Y, g.Y, b.Y, 0.f),
					  lak::vec4f_t(0.f, 0.f, 0.f, 1.f),
					};
				}

				constexpr lak::col::cie::xyY linear_to_xyY(lak::vec3f_t colour) const
				{
					const auto [x, y, Y] = homogenise(colour) * bases();
					return {
					  .x = x,
					  .y = y,
					  .Y = Y,
					};
				}

				constexpr void linear_to_xyY(lak::span<lak::col::cie::xyY> dst,
				                             lak::span<const lak::vec3f_t> src) const
				{
					ASSERT_EQUAL(src.size(), dst.size());
					const auto _bases = bases();
					for (size_t i = 0U; i < src.size(); ++i)
					{
						const auto [x, y, Y] = homogenise(src[i]) * _bases;
						dst[i].x             = x;
						dst[i].y             = y;
						dst[i].Y             = Y;
					}
				}

				constexpr lak::mat4f_t xyY_to_linear() const
				{
					return inverse(linear_to_xyY());
				}

				constexpr lak::vec3f_t xyY_to_linear(lak::col::cie::xyY colour) const
				{
					return homogeneous_mult(xyY_to_linear(),
					                        lak::vec3f_t(colour.x, colour.y, colour.Y));
				}

				constexpr void xyY_to_linear(lak::span<lak::vec3f_t> dst,
				                             lak::span<const lak::col::cie::xyY> src)
				{
					ASSERT_EQUAL(src.size(), dst.size());
					const auto transform = xyY_to_linear();
					for (size_t i = 0U; i < src.size(); ++i)
						dst[i] = homogeneous_mult(
						  transform, lak::vec3f_t(src[i].x, src[i].y, src[i].Y));
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
