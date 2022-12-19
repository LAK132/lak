#include "lak/process.hpp"
#include "lak/memmanip.hpp"
#include "lak/span_manip.hpp"

#include "wrapper.hpp"

#include <fstream>
#include <io.h>

struct lak::process_impl
{
	PROCESS_INFORMATION process_info;

	std::ofstream std_in_strm;
	std::ifstream std_out_strm;
	std::ifstream std_err_strm;

	~process_impl()
	{
		if (process_info.hProcess != NULL) ::CloseHandle(process_info.hProcess);
		if (process_info.hThread != NULL) ::CloseHandle(process_info.hThread);
	}
};

template struct lak::unique_ptr<lak::process_impl>;

lak::process::process(lak::unique_ptr<lak::process_impl> &&impl)
: _impl(lak::move(impl))
{
}

lak::result<lak::process> lak::process::create(
  const lak::fs::path &app, lak::string_view<char8_t> arguments)
{
	auto impl{lak::unique_ptr<lak::process_impl>::make()};
	if (!impl) return lak::err_t{};

	const auto app_name{app.lexically_normal().native()};
	auto command_line{L"\"" + app_name + L"\" " + lak::to_wstring(arguments)};

	struct proc_pipe
	{
		HANDLE read  = NULL;
		HANDLE write = NULL;

		~proc_pipe()
		{
			if (read != NULL) ::CloseHandle(read);
			if (write != NULL) ::CloseHandle(write);
		}
	};

	proc_pipe std_in;
	proc_pipe std_out;
	proc_pipe std_err;

	auto make_pipe = [](proc_pipe &pipe) -> lak::result<>
	{
		SECURITY_ATTRIBUTES security;
		security.nLength              = sizeof(security);
		security.bInheritHandle       = TRUE;
		security.lpSecurityDescriptor = nullptr;

		const DWORD pipe_size = 0;

		if (!::CreatePipe(&pipe.read, &pipe.write, &security, pipe_size))
		{
			ERROR(lak::winapi::error_code_to_wstring(::GetLastError()));
			return lak::err_t{};
		}
		return lak::ok_t{};
	};

	RES_TRY(make_pipe(std_in));
	RES_TRY(make_pipe(std_out));
	RES_TRY(make_pipe(std_err));

	auto open_pipe = [&](HANDLE &handle, const char *mode) -> lak::result<FILE *>
	{
		if (!::SetHandleInformation(handle, HANDLE_FLAG_INHERIT, 0))
			return lak::err_t{};
		int fd = _open_osfhandle(reinterpret_cast<intptr_t>(handle), 0);
		if (fd == -1) return lak::err_t{};
		handle     = NULL;
		FILE *file = _fdopen(fd, mode);
		if (file == nullptr) return lak::err_t{};
		return lak::ok_t{file};
	};

	RES_TRY_ASSIGN(FILE *f_std_in =, open_pipe(std_in.write, "w"));
	RES_TRY_ASSIGN(FILE *f_std_out =, open_pipe(std_out.read, "r"));
	RES_TRY_ASSIGN(FILE *f_std_err =, open_pipe(std_err.read, "r"));

	impl->std_in_strm  = std::ofstream(f_std_in);
	impl->std_out_strm = std::ifstream(f_std_out);
	impl->std_err_strm = std::ifstream(f_std_err);

	STARTUPINFO startup_info;
	lak::bzero(&startup_info);
	startup_info.cb         = sizeof(startup_info);
	startup_info.hStdInput  = std_in.read;
	startup_info.hStdOutput = std_out.write;
	startup_info.hStdError  = std_err.write;
	startup_info.dwFlags |= STARTF_USESTDHANDLES;

	lak::bzero(&impl->process_info);

	if (!::CreateProcessW(nullptr,
	                      command_line.data(),
	                      nullptr,
	                      nullptr,
	                      true,
	                      0,
	                      nullptr,
	                      nullptr,
	                      &startup_info,
	                      &impl->process_info))
	{
		// ERROR(lak::winapi::error_code_to_u8string(::GetLastError()));
		return lak::err_t{};
	}

	return lak::ok_t{lak::process(lak::move(impl))};
}

lak::process::~process()
{
	release();
}

lak::process::operator bool() const
{
	return bool(_impl);
}

lak::result<int> lak::process::join()
{
	if (_impl)
	{
		if (::WaitForSingleObject(_impl->process_info.hProcess, INFINITE) !=
		    WAIT_OBJECT_0)
			return lak::err_t{};

		DWORD result;
		if (!::GetExitCodeProcess(_impl->process_info.hProcess, &result))
		{
			// ERROR(lak::winapi::error_code_to_u8string(::GetLastError()));
			return lak::err_t{};
		}

		if (result == STILL_ACTIVE) return lak::err_t{};

		release();

		ASSERT_LESS_OR_EQUAL(result, DWORD(INT_MAX));

		return lak::ok_t{static_cast<int>(result)};
	}

	return lak::err_t{};
}

void lak::process::release()
{
	_impl.reset();
}

std::ostream *lak::process::std_in() const
{
	if (!_impl) return nullptr;
	return &_impl->std_in_strm;
}

std::istream *lak::process::std_out() const
{
	if (!_impl) return nullptr;
	return &_impl->std_out_strm;
}

std::istream *lak::process::std_err() const
{
	if (!_impl) return nullptr;
	return &_impl->std_err_strm;
}

void lak::process::close_std_in()
{
	if (!_impl) return;
	_impl->std_in_strm.close();
}

void lak::process::close_std_out()
{
	if (!_impl) return;
	_impl->std_out_strm.close();
}

void lak::process::close_std_err()
{
	if (!_impl) return;
	_impl->std_err_strm.close();
}
