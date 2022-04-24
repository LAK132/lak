#include "lak/process.hpp"
#include "lak/memmanip.hpp"

#include "wrapper.hpp"

struct lak::process_impl
{
	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
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

	const auto app_name{app.wstring()};
	auto command_line{L"\"" + app_name + L"\" " + lak::to_wstring(arguments)};

	if (!::CreateProcessW(app_name.c_str() /* lpApplicationName */,
	                      command_line.data() /* lpCommandLine */,
	                      nullptr /* lpProcessAttributes */,
	                      nullptr /* lpThreadAttributes */,
	                      FALSE /* bInheritHandles */,
	                      0 /* dwCreationFlags */,
	                      nullptr /* lpEnvironment */,
	                      nullptr /* lpCurrentDirectory */,
	                      &impl->startup_info /* lpStartupInfo */,
	                      &impl->process_info /* lpProcessInformation */
	                      ))
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
	if (_impl)
	{
		::CloseHandle(_impl->process_info.hProcess);
		::CloseHandle(_impl->process_info.hThread);
	}
}
