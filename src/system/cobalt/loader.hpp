#include "lak/system/compiler.hpp"

#ifdef LAK_COMPILER_MSVC
#	include "lak/system/win32/windows.hpp"
#	include <delayimp.h>
#	pragma comment(lib, "delayimp.lib")

#	define TRY_RENDERER_FUNCTION(FUNC)                                         \
		auto load_fail_exception = [](int exc)                                    \
		{                                                                         \
			if (exc == VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND) ||  \
			    exc == VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND))   \
				return EXCEPTION_EXECUTE_HANDLER;                                     \
			else                                                                    \
				return EXCEPTION_CONTINUE_SEARCH;                                     \
		};                                                                        \
		__try                                                                     \
		{                                                                         \
			return FUNC;                                                            \
		}                                                                         \
		__except (load_fail_exception(GetExceptionCode()))                        \
		{                                                                         \
			return false;                                                           \
		}
#endif
