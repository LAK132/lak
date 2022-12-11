#ifndef LAK_WIN32_WRAPPER_HPP
#define LAK_WIN32_WRAPPER_HPP

#include "lak/windows.hpp"

#include "lak/defer.hpp"
#include "lak/errno_result.hpp"
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

		struct dword_error
		{
			DWORD value;

			static dword_error last_error() { return {::GetLastError()}; }

			inline lak::astring to_string() const
			{
				return lak::winapi::error_code_to_string<char>(value);
			}
		};

		template<typename T = lak::monostate>
		using result = lak::result<T, lak::winapi::dword_error>;

		template<typename CHAR>
		std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
		                                     const lak::winapi::dword_error &err)
		{
			return strm << lak::string_view{err.to_string()};
		}

		template<typename R, typename... T, typename... ARGS>
		lak::winapi::result<R> invoke_null_err(R(__stdcall *f)(T...),
		                                       ARGS &&...args)
		{
			if (R result = f(lak::forward<ARGS>(args)...); result != NULL)
				return lak::ok_t{result};
			else
				return lak::err_t{lak::winapi::dword_error::last_error()};
		}

		template<typename... T, typename... ARGS>
		lak::winapi::result<> invoke_false_err(BOOL(__stdcall *f)(T...),
		                                       ARGS &&...args)
		{
			if (BOOL result = f(lak::forward<ARGS>(args)...); result != FALSE)
				return lak::ok_t{};
			else
				return lak::err_t{lak::winapi::dword_error::last_error()};
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
