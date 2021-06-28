#ifndef LAK_WIN32_WRAPPER_HPP
#define LAK_WIN32_WRAPPER_HPP

#undef ERROR
#ifndef WIN32_MEAN_AND_LEAN
#  define WIN32_MEAN_AND_LEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#undef ERROR

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

    lak::u8string error_code_to_string(DWORD error_code)
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

      return lak::to_u8string(lpMsgBuf ? lpMsgBuf
                                       : L"failed to read error code");
    }

    template<typename T>
    lak::result<T, lak::u8string> to_string(lak::winapi::result<T> result)
    {
      return result.map_err(lak::winapi::error_code_to_string);
    }

    lak::winapi::result<LPVOID> virtual_alloc(LPVOID address,
                                              SIZE_T size,
                                              DWORD allocation_type,
                                              DWORD protect)
    {
      if (LPVOID result =
            ::VirtualAlloc(address, size, allocation_type, protect);
          result != NULL)
        return lak::ok_t{result};
      else
        return lak::err_t{::GetLastError()};
    }

    lak::winapi::result<> virtual_free(LPVOID address,
                                       SIZE_T size,
                                       DWORD free_type)
    {
      if (BOOL result = ::VirtualFree(address, size, free_type); result != 0)
        return lak::ok_t{};
      else
        return lak::err_t{::GetLastError()};
    }

    lak::winapi::result<MEMORY_BASIC_INFORMATION> virtual_query(LPVOID address)
    {
      MEMORY_BASIC_INFORMATION info;
      if (SIZE_T result = ::VirtualQuery(address, &info, sizeof(info));
          result != 0)
        return lak::ok_t{info};
      else
        return lak::err_t{::GetLastError()};
    }
  }
}

#endif
