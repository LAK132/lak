#include "lak/system/compiler.hpp"

#ifdef LAK_COMPILER_MSVC
#	include "lak/system/win32/delayload.hpp"
#	define TRY_RENDERER_FUNCTION(FUNC, ...)                                    \
		__try                                                                     \
		{                                                                         \
			return FUNC(__VA_ARGS__);                                               \
		}                                                                         \
		__except (lak::dll_load_fail_exception(GetExceptionCode()))               \
		{                                                                         \
			return false;                                                           \
		}
#else
#	define TRY_RENDERER_FUNCTION(FUNC, ...)                                    \
		if (FUNC != nullptr)                                                      \
			return FUNC(__VA_ARGS__);                                               \
		else                                                                      \
			return false;
#endif
