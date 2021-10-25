#ifndef LAK_ENDIAN_HPP
#define LAK_ENDIAN_HPP

#include "lak/compiler.hpp"

#if defined(LAK_COMPILER_CLANG) || defined(LAK_COMPILER_GNUC)
#	if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#		define LAK_BIG_ENDIAN
#	elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#		define LAK_LITTLE_ENDIAN
#	else
#		define LAK_MIDDLE_ENDIAN
#		error "Endianness not supported"
#	endif
#elif defined(LAK_COMPILER_MSVC)
#	include "lak/windows.hpp"
#	if REG_DWORD == REG_DWORD_BIG_ENDIAN
#		define LAK_BIG_ENDIAN
#	elif REG_DWORD == REG_DWORD_LITTLE_ENDIAN
#		define LAK_LITTLE_ENDIAN
#	else
#		define LAK_MIDDLE_ENDIAN
#		error "Endianness not supported"
#	endif
#else
#	error "Compiler not supported"
#endif

namespace lak
{
	enum struct endian
	{
		little = 0,
		big    = 1,
#if defined(LAK_LITTLE_ENDIAN)
		native         = little,
		reverse_native = big
#elif defined(LAK_BIG_ENDIAN)
		native         = big,
		reverse_native = little
#elif defined(LAK_MIDDLE_ENDIAN)
		native,
		reverse_native
#endif
	};

	inline constexpr lak::endian operator~(lak::endian e)
	{
		switch (e)
		{
			case lak::endian::little: return lak::endian::big;
			case lak::endian::big: return lak::endian::little;
#if defined(LAK_MIDDLE_ENDIAN)
			case lak::endian::native: return lak::endian::reverse_native;
			case lak::endian::reverse_native: return lak::endian::native;
#endif
		}
	}
}

#endif