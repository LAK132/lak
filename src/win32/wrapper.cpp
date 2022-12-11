#include "wrapper.hpp"

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

lak::u8string lak::winapi::error_code_to_u8string(DWORD error_code)
{
	return lak::to_u8string(lak::winapi::error_code_to_wstring(error_code));
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
		return lak::err_t{::GetLastError()};
}

lak::winapi::result<MEMORY_BASIC_INFORMATION> lak::winapi::virtual_query(
  LPVOID address)
{
	MEMORY_BASIC_INFORMATION info;
	return lak::winapi::invoke_null_err(
	         ::VirtualQuery, address, &info, sizeof(info))
	  .map([&](SIZE_T) -> MEMORY_BASIC_INFORMATION { return info; });
}
