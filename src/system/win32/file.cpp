#include "lak/system/file.hpp"
#include "lak/system/win32/windows.hpp"
#include "lak/system/win32/wrapper.hpp"

#include "lak/array.hpp"

namespace
{
	namespace local
	{
		lak::error_code_result<lak::span<byte_t>> mmap_file(
		  const lak::fs::path &path, bool write_access, bool copy_on_write)
		{
			RES_TRY_ASSIGN(HANDLE file =,
			               lak::winapi::invoke_invalid_handle_err(
			                 ::CreateFileW,
			                 path.wstring().c_str(),
			                 GENERIC_READ | (write_access ? GENERIC_WRITE : 0L),
			                 (write_access ? 0L : FILE_SHARE_READ),
			                 (LPSECURITY_ATTRIBUTES)NULL,
			                 OPEN_EXISTING,
			                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
			                 (HANDLE)NULL));

			DEFER(lak::winapi::invoke_false_err(::CloseHandle, file)
			        .IF_ERR_WARN("CloseHandle(file) failed"));

			LARGE_INTEGER size;
			RES_TRY(lak::winapi::invoke_false_err(::GetFileSizeEx, file, &size));

			RES_TRY_ASSIGN(
			  HANDLE map =,
			  lak::winapi::invoke_null_handle_err(
			    ::CreateFileMappingW,
			    file,
			    (LPSECURITY_ATTRIBUTES)NULL,
			    (write_access ? (copy_on_write ? PAGE_WRITECOPY : PAGE_READWRITE)
			                  : PAGE_READONLY),
			    0,
			    0,
			    (LPCWSTR)NULL));

			DEFER(lak::winapi::invoke_false_err(::CloseHandle, map)
			        .IF_ERR_WARN("CloseHandle(map) failed"));

			RES_TRY_ASSIGN(
			  LPVOID view =,
			  lak::winapi::invoke_null_err(
			    ::MapViewOfFile,
			    map,
			    FILE_MAP_READ |
			      (write_access ? (copy_on_write ? FILE_MAP_COPY : FILE_MAP_WRITE)
			                    : 0),
			    0,
			    0,
			    0));

			return lak::ok_t{lak::span<byte_t>(
			  lak::span<void>(view, static_cast<size_t>(size.QuadPart)))};
		}
	}
}

lak::error_code_result<lak::unique_ptr<const byte_t[]>> lak::ro_mmap_file(
  const fs::path &path)
{
	RES_TRY_ASSIGN(auto data =, local::mmap_file(path, false, false));
	return lak::ok_t{lak::unique_ptr<const byte_t[]>(
	  data,
	  +[](lak::span<const byte_t> f)
	  {
		  lak::winapi::invoke_false_err(::UnmapViewOfFile, f.data())
		    .IF_ERR_WARN("UnmapViewOfFile failed");
	  })};
}

lak::error_code_result<lak::unique_ptr<byte_t[]>> lak::rw_mmap_file(
  const fs::path &path)
{
	RES_TRY_ASSIGN(auto data =, local::mmap_file(path, true, false));
	return lak::ok_t{lak::unique_ptr<byte_t[]>(
	  data,
	  +[](lak::span<byte_t> f)
	  {
		  lak::winapi::invoke_false_err(::UnmapViewOfFile, f.data())
		    .IF_ERR_WARN("UnmapViewOfFile failed");
	  })};
}

lak::error_code_result<lak::unique_ptr<byte_t[]>> lak::cw_mmap_file(
  const fs::path &path)
{
	RES_TRY_ASSIGN(auto data =, local::mmap_file(path, true, true));
	return lak::ok_t{lak::unique_ptr<byte_t[]>(
	  data,
	  +[](lak::span<byte_t> f)
	  {
		  lak::winapi::invoke_false_err(::UnmapViewOfFile, f.data())
		    .IF_ERR_WARN("UnmapViewOfFile failed");
	  })};
}

lak::fs::path lak::exe_path()
{
	lak::array<wchar_t> path;
	path.resize(MAX_PATH);

	DWORD used = ::GetModuleFileNameW(NULL, path.data(), DWORD(path.size()));

	while (used >= path.size() - 1)
	{
		path.resize(path.size() * 2);
		used = ::GetModuleFileNameW(NULL, path.data(), DWORD(path.size()));
	}

	path[used + 1] = 0;

	return lak::fs::path(path.data());
}
