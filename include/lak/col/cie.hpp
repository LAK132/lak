#ifndef LAK_COL_CIE_HPP
#define LAK_COL_CIE_HPP

#include "lak/debug.hpp"
#include "lak/vec.hpp"

#include "lak/col/cie_1931.hpp"
#include "lak/col/cie_1960.hpp"
#include "lak/col/cie_1964.hpp"
#include "lak/col/cie_1976.hpp"
#include "lak/col/col.hpp"

#include <cmath>

namespace lak
{
	namespace col
	{
		namespace cie
		{
			constexpr lak::col::LMS to_LMS_bradford(lak::col::cie::XYZ colour)
			{
				return lak::col::LMS::from_vec(lak::col::bradford_matrix *
				                               colour.to_vec());
			}

			constexpr lak::col::cie::XYZ to_XYZ_bradford(lak::col::LMS colour)
			{
				return lak::col::cie::XYZ::from_vec(
				  lak::inverse(lak::col::bradford_matrix) * colour.to_vec());
			}

			constexpr lak::mat3f_t bradford_adaption_matrix(lak::col::cie::XYZ src,
			                                                lak::col::cie::XYZ dst)
			{
				const auto src_lms = lak::col::cie::to_LMS_bradford(src);
				const auto dst_lms = lak::col::cie::to_LMS_bradford(dst);
				const auto scale = lak::diagonal(dst_lms.to_vec() / src_lms.to_vec());
				return lak::inverse(lak::col::bradford_matrix) *
				       (scale * lak::col::bradford_matrix);
			}

			constexpr lak::mat3f_t bradford_adaption_matrix(lak::col::cie::xyY src,
			                                                lak::col::cie::xyY dst)
			{
				return lak::col::cie::bradford_adaption_matrix(
				  lak::col::cie::to_XYZ(src), lak::col::cie::to_XYZ(dst));
			}

			constexpr lak::mat3f_t bradford_adaption_matrix(lak::col::cie::xy src,
			                                                lak::col::cie::xy dst)
			{
				return lak::col::cie::bradford_adaption_matrix(
				  lak::col::cie::to_XYZ(src), lak::col::cie::to_XYZ(dst));
			}

			constexpr lak::col::cie::xyY_primaries bradford_adapt(
			  lak::col::cie::xyY_primaries src, lak::col::cie::xy dst)
			{
				const auto w = lak::col::cie::to_XYZ(dst, 1.f);
				const auto m = lak::col::cie::bradford_adaption_matrix(src.w_XYZ(), w);
				const auto r = lak::col::cie::XYZ::from_vec(
				  m * lak::col::cie::to_XYZ(src.r).to_vec());
				const auto g = lak::col::cie::XYZ::from_vec(
				  m * lak::col::cie::to_XYZ(src.g).to_vec());
				const auto b = lak::col::cie::XYZ::from_vec(
				  m * lak::col::cie::to_XYZ(src.b).to_vec());
				lak::col::cie::xyY_primaries result{
				  .r = lak::col::cie::to_xyY(r),
				  .g = lak::col::cie::to_xyY(g),
				  .b = lak::col::cie::to_xyY(b),
				  .w = dst,
				};
				result.regenerate_Y();
				return result;
			}

			constexpr lak::col::cie::xy_primaries bradford_adapt(
			  lak::col::cie::xy_primaries src, lak::col::cie::xy dst)
			{
				return lak::col::cie::to_xy(
				  lak::col::cie::bradford_adapt(lak::col::cie::to_xyY(src), dst));
			}

			// ---

			constexpr float D_series_CCT(float temp)
			{
				constexpr double chk =
				  1.438776877 /* (2.998E8 *  6.626E-34) / 1.381E-23 * 100 */;
				constexpr double old_chk = 1.4380;
				return float(double(temp) * (chk / old_chk));
			}

			// CCT (K): 4000.0f <= 25000.0f
			constexpr lak::col::cie::xy D_series_illuminant(float CCT)
			{
				// https://en.wikipedia.org/wiki/Standard_illuminant#Computation
				ASSERT_GREATER_OR_EQUAL(CCT, 4000.0f);
				ASSERT_LESS_OR_EQUAL(CCT, 25000.0f);
				const double temp1 = CCT;
				const double temp2 = temp1 * temp1;
				const double temp3 = temp2 * temp1;
				const double t1    = 1.0E3 / temp1;
				const double t2    = 1.0E6 / temp2;
				const double t3    = 1.0E9 / temp3;
				double x           = 0.0;
				if (temp1 <= 7000.0)
					x = 0.244063 + (0.09911 * t1) + (2.9678 * t2) - (4.6070 * t3);
				else
					x = 0.237040 + (0.24748 * t1) + (1.9018 * t2) - (2.0064 * t3);
				return {
				  .x = float(x),
				  .y = float(-0.275 + (2.870 * x) - (3.0 * x * x)),
				};
			}

			constexpr lak::col::cie::xy A_xy{0.44758f, 0.40745f};
			constexpr auto A_xyY = to_xyY(A_xy, 1.f);
			constexpr auto A_XYZ = to_XYZ(A_xyY);
			constexpr auto A_RGB = to_RGB(A_XYZ);

			constexpr float B_CCT            = D_series_CCT(4874.f);
			constexpr lak::col::cie::xy B_xy = D_series_illuminant(B_CCT);
			constexpr auto B_xyY             = to_xyY(B_xy, 1.f);
			constexpr auto B_XYZ             = to_XYZ(B_xyY);
			constexpr auto B_RGB             = to_RGB(B_XYZ);

			constexpr float C_CCT            = D_series_CCT(6774.f);
			constexpr lak::col::cie::xy C_xy = D_series_illuminant(C_CCT);
			constexpr auto C_xyY             = to_xyY(C_xy, 1.f);
			constexpr auto C_XYZ             = to_XYZ(C_xyY);
			constexpr auto C_RGB             = to_RGB(C_XYZ);

			constexpr float D50_CCT            = D_series_CCT(5000.f);
			constexpr lak::col::cie::xy D50_xy = D_series_illuminant(D50_CCT);
			constexpr auto D50_xyY             = to_xyY(D50_xy, 1.f);
			constexpr auto D50_XYZ             = to_XYZ(D50_xyY);
			constexpr auto D50_RGB             = to_RGB(D50_XYZ);

			constexpr float D55_CCT            = D_series_CCT(5500.f);
			constexpr lak::col::cie::xy D55_xy = D_series_illuminant(D55_CCT);
			constexpr auto D55_xyY             = to_xyY(D55_xy, 1.f);
			constexpr auto D55_XYZ             = to_XYZ(D55_xyY);
			constexpr auto D55_RGB             = to_RGB(D55_XYZ);

			constexpr float D65_CCT            = D_series_CCT(6500.f);
			constexpr lak::col::cie::xy D65_xy = D_series_illuminant(D65_CCT);
			constexpr auto D65_xyY             = to_xyY(D65_xy, 1.f);
			constexpr auto D65_XYZ             = to_XYZ(D65_xyY);
			constexpr auto D65_RGB             = to_RGB(D65_XYZ);

			constexpr float D75_CCT            = D_series_CCT(7500.f);
			constexpr lak::col::cie::xy D75_xy = D_series_illuminant(D75_CCT);
			constexpr auto D75_xyY             = to_xyY(D75_xy, 1.f);
			constexpr auto D75_XYZ             = to_XYZ(D75_xyY);
			constexpr auto D75_RGB             = to_RGB(D75_XYZ);

			constexpr lak::col::cie::xy E_xy{.x = 1.f / 3.f, .y = 1.f / 3.f};
			constexpr auto E_xyY = to_xyY(E_xy, 1.f);
			constexpr lak::col::cie::XYZ E_XYZ{.X = 1.f, .Y = 1.f, .Z = 1.f};
			constexpr auto E_RGB = to_RGB(E_XYZ);
		}

		// ---

	}
}

#endif
