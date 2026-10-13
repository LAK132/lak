#ifndef LAK_COL_CIE_1964_HPP
#define LAK_COL_CIE_1964_HPP

#include "lak/col/cie_1931.hpp"
#include "lak/col/cie_1976.hpp"

#include <cie-cmf/CIE_cc_1964_10deg.csv.hpp>
#include <cie-cmf/CIE_xyz_1964_10deg.csv.hpp>

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

				static constexpr UVW from_vec(lak::vec3f_t v)
				{
					return {.U = v.x, .V = v.y, .W = v.z};
				}
				constexpr lak::vec3f_t to_vec() const { return {U, V, W}; }
			};

			LAK_CMATH_CONSTEXPR lak::col::cie::UVW to_UVW(lak::col::cie::uvY colour,
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

			// see cie_1964.inl for constexpr table
			inline lak::col::cie::XYZ planckian_locus_1964_10deg_at(
			  double temperature)
			{
				lak::col::cie::XYZ result{.X = 0.f, .Y = 0.f, .Z = 0.f};
				for (const auto &entry :
				     lak::span(cie_cmf::cie_1964_xyz_cmf_10deg_observer))
				{
					double weight =
					  lak::blackbody_radiance(double(entry.wavelength), temperature);
					result.X += entry.x * weight;
					result.Y += entry.y * weight;
					result.Z += entry.z * weight;
				}
				return result;
			}
		}
	}
}

#include "lak/col/cie_1964.inl"

#endif
