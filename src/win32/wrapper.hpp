#ifndef LAK_WIN32_WRAPPER_HPP
#define LAK_WIN32_WRAPPER_HPP

#include "lak/windows.hpp"

#include "lak/defer.hpp"
#include "lak/os.hpp"
#include "lak/result.hpp"
#include "lak/string.hpp"

namespace lak
{
	namespace winapi
	{
		template<typename T = lak::monostate>
		using result = lak::result<T, DWORD>;

		lak::wstring error_code_to_wstring(DWORD error_code);
		lak::u8string error_code_to_u8string(DWORD error_code);

		template<typename T>
		lak::result<T, lak::u8string> to_string(lak::winapi::result<T> result)
		{
			return result.map_err(lak::winapi::error_code_to_u8string);
		}

		template<typename R, typename... T, typename... ARGS>
		lak::winapi::result<R> invoke_null_err(R(__stdcall *f)(T...),
		                                       ARGS &&...args)
		{
			if (R result = f(lak::forward<ARGS>(args)...); result != NULL)
				return lak::ok_t{result};
			else
				return lak::err_t{::GetLastError()};
		}

		lak::winapi::result<LPVOID> virtual_alloc(LPVOID address,
		                                          SIZE_T size,
		                                          DWORD allocation_type,
		                                          DWORD protect);

		lak::winapi::result<> virtual_free(LPVOID address,
		                                   SIZE_T size,
		                                   DWORD free_type);

		lak::winapi::result<MEMORY_BASIC_INFORMATION> virtual_query(
		  LPVOID address);
	}
}

#endif
