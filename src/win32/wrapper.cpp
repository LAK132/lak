#include "wrapper.hpp"

#include "lak/memmanip.hpp"

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

struct _win32_error_category : public std::error_category
{
	const char *name() const noexcept override { return "Win32"; }
	std::string message(int condition) const override
	{
		static_assert(sizeof(DWORD) == sizeof(int));
		return lak::winapi::error_code_to_string<char>(
		  lak::bit_cast<DWORD>(condition));
	}
};

const std::error_category &lak::winapi::win32_error_category()
{
	static _win32_error_category cat;
	return cat;
}

std::error_code lak::winapi::make_win32_error(DWORD condition)
{
	return std::error_code(lak::bit_cast<int>(condition),
	                       lak::winapi::win32_error_category());
}

struct _com_error_category : public std::error_category
{
	const char *name() const noexcept override { return "com"; }
	std::string message(int condition) const override
	{
		static_assert(sizeof(HRESULT) == sizeof(int));
		const TCHAR *str =
		  _com_error(lak::bit_cast<HRESULT>(condition)).ErrorMessage();
		return lak::to_astring(lak::string_view<TCHAR>::from_c_str(str));
	}
};

const std::error_category &lak::winapi::com_error_category()
{
	static _com_error_category cat;
	return cat;
}

std::error_code lak::winapi::make_com_error(HRESULT condition)
{
	return std::error_code(lak::bit_cast<int>(condition),
	                       lak::winapi::com_error_category());
}

lak::error_code_result<LPVOID> lak::winapi::virtual_alloc(
  LPVOID address, SIZE_T size, DWORD allocation_type, DWORD protect)
{
	return lak::winapi::invoke_null_err(
	  ::VirtualAlloc, address, size, allocation_type, protect);
}

lak::error_code_result<> lak::winapi::virtual_free(LPVOID address,
                                                   SIZE_T size,
                                                   DWORD free_type)
{
	if (BOOL result = ::VirtualFree(address, size, free_type); result != 0)
		return lak::ok_t{};
	else
		return lak::err_t{lak::winapi::last_win32_error()};
}

lak::error_code_result<MEMORY_BASIC_INFORMATION> lak::winapi::virtual_query(
  LPVOID address)
{
	MEMORY_BASIC_INFORMATION info;
	return lak::winapi::invoke_null_err(
	         ::VirtualQuery, address, &info, sizeof(info))
	  .map([&](SIZE_T) -> MEMORY_BASIC_INFORMATION { return info; });
}
