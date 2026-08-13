#ifndef CIE_CMF_COMMON_HPP
#define CIE_CMF_COMMON_HPP

#include <cstddef>
#include <cstdint>

namespace cie_cmf
{
	struct entry
	{
		uint16_t wavelength; // nm
		double x;
		double y;
		double z;
	};
}

#endif
