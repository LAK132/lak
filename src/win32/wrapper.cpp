#include "wrapper.hpp"

#include <io.h>

lak::wstring lak::winapi::error_code_to_wstring(DWORD error_code)
{
	LPWSTR lpMsgBuf = nullptr;
	::FormatMessageW(
	  FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
	    FORMAT_MESSAGE_IGNORE_INSERTS,           /* dwFlags */
	  NULL,                                      /* lpSource */
	  error_code,                                /* dwMessageId */
	  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* dwLanguageId */
	  (LPWSTR)&lpMsgBuf,                         /* lpBuffer */
	  0,                                         /* nSize */
	  NULL                                       /* Arguments */
	);
	DEFER(::LocalFree(lpMsgBuf));

	return lak::wstring(lpMsgBuf ? lpMsgBuf : L"failed to read error code");
}

lak::winapi::result<LPVOID> lak::winapi::virtual_alloc(LPVOID address,
                                                       SIZE_T size,
                                                       DWORD allocation_type,
                                                       DWORD protect)
{
	return lak::winapi::invoke_null_err(
	  ::VirtualAlloc, address, size, allocation_type, protect);
}

lak::winapi::result<> lak::winapi::virtual_free(LPVOID address,
                                                SIZE_T size,
                                                DWORD free_type)
{
	if (BOOL result = ::VirtualFree(address, size, free_type); result != 0)
		return lak::ok_t{};
	else
		return lak::err_t{lak::winapi::dword_error::last_error()};
}

lak::winapi::result<MEMORY_BASIC_INFORMATION> lak::winapi::virtual_query(
  LPVOID address)
{
	MEMORY_BASIC_INFORMATION info;
	return lak::winapi::invoke_null_err(
	         ::VirtualQuery, address, &info, sizeof(info))
	  .map([&](SIZE_T) -> MEMORY_BASIC_INFORMATION { return info; });
}

lak::winapi::result<> lak::winapi::attach_console(DWORD process_id)
{
	return lak::winapi::invoke_false_err(::AttachConsole, process_id);
}

lak::winapi::result<> lak::winapi::alloc_console()
{
	return lak::winapi::invoke_false_err(::AllocConsole);
}

lak::winapi::result<> lak::winapi::free_console()
{
	return lak::winapi::invoke_false_err(::FreeConsole);
}

lak::winapi::result<HANDLE> lak::winapi::get_std_handle(DWORD std_handle)
{
	if (HANDLE result = ::GetStdHandle(std_handle);
	    result != INVALID_HANDLE_VALUE)
		return lak::ok_t{result};
	else
		return lak::err_t{lak::winapi::dword_error::last_error()};
}

lak::winapi::result<DWORD> lak::winapi::get_console_mode(HANDLE handle)
{
	DWORD mode;
	return lak::winapi::invoke_false_err(::GetConsoleMode, handle, &mode)
	  .map([&](auto) { return mode; });
}

lak::winapi::result<> lak::winapi::set_console_mode(HANDLE handle, DWORD mode)
{
	return lak::winapi::invoke_false_err(::SetConsoleMode, handle, mode);
}

lak::winapi::result<lak::winapi::attach_or_alloc_console_result>
lak::winapi::attach_or_alloc_console()
{
	using enum attach_or_alloc_console_result;

	return lak::winapi::attach_console(ATTACH_PARENT_PROCESS)
	  .map([](auto) { return attached; })
	  .or_else(
	    [](lak::winapi::dword_error err)
	      -> lak::winapi::result<lak::winapi::attach_or_alloc_console_result>
	    {
		    switch (err.value)
		    {
			    case ERROR_ACCESS_DENIED:
				    return lak::ok_t{already_attached};
			    case ERROR_INVALID_HANDLE:
				    return lak::winapi::alloc_console().map(
				      [](auto) { return allocated_and_attached; });
			    default:
				    return lak::err_t{err};
		    }
	    });
}

lak::winapi::redirect_result redirect_stdio(DWORD handle,
                                            FILE *io,
                                            const char *file,
                                            const char *mode)
{
	match_result(lak::winapi::get_std_handle(handle))
	{
		match_let_err(lak::winapi::dword_error err, { return lak::err_t{err}; });

		match_let_ok(HANDLE ok, {
			FILE *fp = NULL;
			if (errno_t err = freopen_s(&fp, file, mode, io); err != 0)
				return lak::err_t{lak::errno_error{err}};
			else
				return lak::ok_t{ok};
		});
	}

	UNREACHABLE();
}

lak::winapi::redirect_result lak::winapi::redirect_stdin(const char *file,
                                                         const char *mode)
{
	return redirect_stdio(STD_INPUT_HANDLE, stdin, file, mode);
}

lak::winapi::redirect_result lak::winapi::redirect_stdout(const char *file,
                                                          const char *mode)
{
	return redirect_stdio(STD_OUTPUT_HANDLE, stdout, file, mode);
}

lak::winapi::redirect_result lak::winapi::redirect_stderr(const char *file,
                                                          const char *mode)
{
	return redirect_stdio(STD_ERROR_HANDLE, stderr, file, mode);
}

lak::winapi::redirect_stdio_to_console_result
lak::winapi::redirect_stdio_to_console(bool redirect_stdin_to_console,
                                       bool redirect_stdout_to_console,
                                       bool redirect_stderr_to_console)
{
	redirect_stdio_to_console_result result{lak::ok_t{INVALID_HANDLE_VALUE},
	                                        lak::ok_t{INVALID_HANDLE_VALUE},
	                                        lak::ok_t{INVALID_HANDLE_VALUE}};

	{
		// prevent debug messages from touching the console streams while we're
		// setting them up.
		DEFER_RESET(lak::debugger.live_output_enabled);
		lak::debugger.live_output_enabled = false;

		if (redirect_stdin_to_console)
		{
			fflush(stdin);
			std::cin.clear();
			std::wcin.clear();
			if_let_ok (HANDLE handle, lak::winapi::get_std_handle(STD_INPUT_HANDLE))
				::FlushFileBuffers(handle);
		}

		if (redirect_stdout_to_console)
		{
			fflush(stdout);
			std::flush(std::cout);
			std::flush(std::wcout);
			std::cout.clear();
			std::wcout.clear();
			if_let_ok (HANDLE handle, lak::winapi::get_std_handle(STD_OUTPUT_HANDLE))
				::FlushFileBuffers(handle);
		}

		if (redirect_stderr_to_console)
		{
			fflush(stderr);
			std::flush(std::cerr);
			std::flush(std::wcerr);
			std::cerr.clear();
			std::wcerr.clear();
			if_let_ok (HANDLE handle, lak::winapi::get_std_handle(STD_ERROR_HANDLE))
				::FlushFileBuffers(handle);
		}

		if (redirect_stdin_to_console)
			result.redirect_stdin = redirect_stdin("CONIN$", "r");

		if (redirect_stdout_to_console)
			result.redirect_stdout = redirect_stdout("CONOUT$", "w");

		if_let_ok (HANDLE handle, lak::winapi::get_std_handle(STD_OUTPUT_HANDLE))
		{
			if (handle != INVALID_HANDLE_VALUE)
			{
				lak::winapi::get_console_mode(handle)
				  .if_err(
				    [](auto err)
				    {
					    (void)err;
					    // WARNING("failed to get stdout console mode: ",
					    // err.to_string());
				    })
				  .and_then(
				    [&](DWORD mode)
				    {
					    return lak::winapi::set_console_mode(
					             handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)
					      .if_err(
					        [](auto err)
					        {
						        (void)err;
						        // WARNING("failed to set stdout console mode: ",
						        // err.to_string());
					        });
				    });
			}
		}

		if (redirect_stderr_to_console)
			if (redirect_stdout_to_console && result.redirect_stdout.is_ok())
			{
				if (bool success = _dup2(1, 2) == 0; !success)
					result.redirect_stderr =
					  lak::err_t{lak::var_t<1>(lak::errno_error::last_error())};
			}
			else
				result.redirect_stderr = redirect_stderr("CONOUT$", "w");

		// std::ios::sync_with_stdio(true);

		if (redirect_stdin_to_console && result.redirect_stdin.is_ok())
		{
			std::wcin.clear();
			std::cin.clear();
		}
		if (redirect_stdout_to_console && result.redirect_stdout.is_ok())
		{
			std::wcout.clear();
			std::cout.clear();
		}
		if (redirect_stderr_to_console && result.redirect_stderr.is_ok())
		{
			std::wcerr.clear();
			std::cerr.clear();
		}
	}

	// if_let_ok (HANDLE handle, result.redirect_stderr)
	// {
	// 	if (handle != INVALID_HANDLE_VALUE)
	// 	{
	// 		lak::winapi::get_console_mode(handle)
	// 		  .if_err(
	// 		    [](auto err)
	// 		    { WARNING("failed to get stderr console mode: ", err.to_string());
	// }) 		  .and_then(
	// 		    [&](DWORD mode)
	// 		    {
	// 			    return lak::winapi::set_console_mode(
	// 			             handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)
	// 			      .if_err(
	// 			        [](auto err) {
	// 				        WARNING("failed to set stderr console mode: ",
	// 				                err.to_string());
	// 			        });
	// 		    });
	// 	}
	// }

	return result;
}
