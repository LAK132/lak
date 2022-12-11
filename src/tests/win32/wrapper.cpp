#include "lak/test.hpp"

#include "../../win32/wrapper.hpp"

#include <io.h>
#include <ostream>
#include <stdio.h>

BEGIN_TEST(alloc_attach_console)
{
	if_let_ok (HANDLE handle, lak::winapi::get_std_handle(STD_OUTPUT_HANDLE))
	{
		if (handle != INVALID_HANDLE_VALUE)
		{
			lak::winapi::get_console_mode(handle)
			  .if_err(
			    [](auto err)
			    {
				    (void)err;
				    WARNING("failed to get stdout console mode: ", err.to_string());
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
					        WARNING("failed to set stdout console mode: ",
					                err.to_string());
				        });
			    });
		}
	}

	fflush(stdin);
	fflush(stdout);
	fflush(stderr);
	std::flush(std::cout);
	std::flush(std::wcout);
	std::flush(std::cerr);
	std::flush(std::wcerr);
	if_let_ok (HANDLE handle, lak::winapi::get_std_handle(STD_OUTPUT_HANDLE))
		lak::winapi::invoke_false_err(::FlushFileBuffers, handle)
		  // .IF_ERR("bad handle ", handle)
		  ;
	// else
	// 	WARNING("stdout no flush :(");
	if_let_ok (HANDLE handle, lak::winapi::get_std_handle(STD_ERROR_HANDLE))
		lak::winapi::invoke_false_err(::FlushFileBuffers, handle)
		  // .IF_ERR("bad handle ", handle)
		  ;
	// else
	// 	WARNING("stderr no flush :(");

	// // std::ios::sync_with_stdio();
	// // std::cout.clear();
	// // std::wcout.clear();
	// // std::cerr.clear();
	// // std::wcerr.clear();
	// fputc('\n', stdout);
	// fflush(stdin);
	// fflush(stdout);
	// fflush(stderr);
	// std::flush(std::cout);
	// std::flush(std::wcout);
	// std::flush(std::cerr);
	// std::flush(std::wcerr);

	if (lak::winapi::attach_or_alloc_console().UNWRAP() ==
	    lak::winapi::attach_or_alloc_console_result::already_attached)
	{
		// DEBUG("already alloced");
		// std::wcout << std::flush;
		lak::winapi::free_console().UNWRAP();
		// DEBUG("freed console");
		ASSERT(lak::winapi::attach_or_alloc_console().UNWRAP() !=
		       lak::winapi::attach_or_alloc_console_result::already_attached);
	}

	if_let_ok (HANDLE handle, lak::winapi::get_std_handle(STD_OUTPUT_HANDLE))
		lak::winapi::invoke_false_err(::FlushFileBuffers, handle)
		  // .IF_ERR("bad handle ", handle)
		  ;
	// else
	// 	WARNING("stdout no flush :(");
	if_let_ok (HANDLE handle, lak::winapi::get_std_handle(STD_ERROR_HANDLE))
		lak::winapi::invoke_false_err(::FlushFileBuffers, handle)
		  // .IF_ERR("bad handle ", handle)
		  ;
	// else
	// 	WARNING("stderr no flush :(");

	auto [in, out, err] =
	  lak::winapi::redirect_stdio_to_console(false, true, false);
	if_let_err (auto err, in) FATAL(err);
	if_let_err (auto err, out) FATAL(err);
	if_let_err (auto err, err) FATAL(err);

	DEBUG("hello, world!");
	// std::wcout << "hello, world!\n";

	// PAUSE();

	return 0;
}
END_TEST()
