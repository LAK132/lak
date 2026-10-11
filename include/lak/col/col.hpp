#ifndef LAK_COL_COL_HPP
#define LAK_COL_COL_HPP

#include "lak/vec.hpp"

namespace lak
{
	namespace col
	{
		struct LMS
		{
			float L; // long
			float M; // medium
			float S; // short

			static constexpr LMS from_vec(lak::vec3f_t v)
			{
				return {.L = v.x, .M = v.y, .S = v.z};
			}
			constexpr lak::vec3f_t to_vec() const { return {L, M, S}; }
		};

		constexpr lak::mat3f_t bradford_matrix{
		  lak::vec3f_t(0.8951f, 0.2664f, -0.1614f),
		  lak::vec3f_t(-0.7502f, 1.7135f, 0.0367f),
		  lak::vec3f_t(0.0389f, -0.0685f, 1.0296f),
		};

		constexpr lak::mat3f_t von_kries_matrix{
		  lak::vec3f_t(0.40024f, 0.70760f, -0.08081f),
		  lak::vec3f_t(-0.22630f, 1.16532f, 0.04570f),
		  lak::vec3f_t(0.00000f, 0.00000f, 0.91822f),
		};
	}
}

#endif
