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

		lak::winapi::result<> attach_console(DWORD process_id);

		lak::winapi::result<> alloc_console();

		lak::winapi::result<> free_console();

		// std_handle: STD_INPUT_HANDLE, STD_OUTPUT_HANDLE or STD_ERROR_HANDLE.
		// result handle may be NULL, this is not an error.
		// result handle should not be closed.
		lak::winapi::result<HANDLE> get_std_handle(DWORD std_handle);

		lak::winapi::result<DWORD> get_console_mode(HANDLE handle);

		lak::winapi::result<> set_console_mode(HANDLE handle, DWORD mode);

		enum struct attach_or_alloc_console_result
		{
			already_attached,
			allocated_and_attached,
			attached,
		};
		lak::winapi::result<attach_or_alloc_console_result>
		attach_or_alloc_console();

		using redirect_result =
		  lak::result<HANDLE,
		              lak::variant<lak::winapi::dword_error, lak::errno_error>>;

		template<typename CHAR>
		std::basic_ostream<CHAR> &operator<<(
		  std::basic_ostream<CHAR> &strm,
		  const lak::variant<lak::winapi::dword_error, lak::errno_error> &err)
		{
			return strm << lak::string_view{
			         lak::visit(err, [](auto err) { return err.to_string(); })};
		}

		// example:
		//  attach_or_alloc_console().expect("failed to attach to a console");
		//  redirect_stdin("CONIN$", "r").expect("failed to redirect stdin");
		//  std::ios::sync_with_stdio(true);
		//  std::wcin.clear();
		//  std::cin.clear();
		redirect_result redirect_stdin(const char *file, const char *mode = "r");
		redirect_result redirect_stdout(const char *file, const char *mode = "w");
		redirect_result redirect_stderr(const char *file, const char *mode = "w");

		struct redirect_stdio_to_console_result
		{
			redirect_result redirect_stdin;
			redirect_result redirect_stdout;
			redirect_result redirect_stderr;
		};
		// requires a console to already be attached to the currect process.
		// example:
		//  attach_or_alloc_console().unwrap()
		//  auto [in_res, out_res, err_res] =
		//    redirect_stdio_to_console(true, true, false); // redirect in and out
		//  in_res.expect("failed to redirect stdin");
		//  out_res.expect("failed to redirect stderr");
		redirect_stdio_to_console_result redirect_stdio_to_console(
		  bool redirect_stdin_to_console,
		  bool redirect_stdout_to_console,
		  bool redirect_stderr_to_console);
	}
}

#endif
