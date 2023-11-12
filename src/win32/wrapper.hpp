#ifndef LAK_WIN32_WRAPPER_HPP
#define LAK_WIN32_WRAPPER_HPP

#include "lak/windows.hpp"

#include "lak/defer.hpp"
#include "lak/errno_result.hpp"
#include "lak/error_code_result.hpp"
#include "lak/os.hpp"
#include "lak/result.hpp"
#include "lak/string.hpp"

namespace lak
{
	namespace winapi
	{
		lak::wstring error_code_to_wstring(DWORD error_code);
		template<typename CHAR>
		lak::string<CHAR> error_code_to_string(DWORD error_code)
		{
			return lak::strconv<CHAR>(error_code_to_wstring(error_code));
		}

		const std::error_category &win32_error_category();
		std::error_code make_win32_error(DWORD condition);
		inline std::error_code last_win32_error()
		{
			return lak::winapi::make_win32_error(::GetLastError());
		}
		inline lak::error_code_result<> result_from_win32(DWORD condition)
		{
			if (condition != 0)
				return lak::err_t{lak::winapi::make_win32_error(condition)};
			else
				return lak::ok_t{};
		}

		const std::error_category &com_error_category();
		std::error_code make_com_error(HRESULT condition);
		inline lak::error_code_result<> result_from_com(HRESULT condition)
		{
			if (FAILED(condition))
				return lak::err_t{lak::winapi::make_com_error(condition)};
			else
				return lak::ok_t{};
		}
#define RES_TRY_COM(...) RES_TRY(lak::winapi::result_from_com(__VA_ARGS__))

		template<typename R, typename... T, typename... ARGS>
		lak::error_code_result<R> invoke_null_err(R(__stdcall *f)(T...),
		                                          ARGS &&...args)
		{
			if (R result = f(lak::forward<ARGS>(args)...); result != NULL)
				return lak::ok_t{result};
			else
				return lak::err_t{lak::winapi::last_win32_error()};
		}

		template<typename... T, typename... ARGS>
		lak::error_code_result<> invoke_false_err(BOOL(__stdcall *f)(T...),
		                                          ARGS &&...args)
		{
			if (BOOL result = f(lak::forward<ARGS>(args)...); result != FALSE)
				return lak::ok_t{};
			else
				return lak::err_t{lak::winapi::last_win32_error()};
		}

		lak::error_code_result<LPVOID> virtual_alloc(LPVOID address,
		                                             SIZE_T size,
		                                             DWORD allocation_type,
		                                             DWORD protect);

		lak::error_code_result<> virtual_free(LPVOID address,
		                                      SIZE_T size,
		                                      DWORD free_type);

		lak::error_code_result<MEMORY_BASIC_INFORMATION> virtual_query(
		  LPVOID address);
	}
}

#endif
