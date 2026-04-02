#include "lak/system/file.hpp"
#include "lak/system/win32/windows.hpp"
#include "lak/system/win32/wrapper.hpp"

#include "lak/array.hpp"

namespace lak
{
	struct mapped_file_impl
	{
		HANDLE file      = INVALID_HANDLE_VALUE;
		HANDLE file_map  = NULL;
		LPVOID file_view = NULL;
		~mapped_file_impl()
		{
			if (file_view != NULL) ::UnmapViewOfFile(file_view);
			if (file_map != NULL) ::CloseHandle(file_map);
			if (file != INVALID_HANDLE_VALUE) ::CloseHandle(file);
		}
	};
}

lak::mapped_file::~mapped_file()
{
	data = {};
	if (_impl)
	{
		delete _impl;
		_impl = nullptr;
	}
}

lak::error_code_result<lak::mapped_file> lak::map_file(const fs::path &path)
{
	auto impl = new lak::mapped_file_impl;
	DEFER({
		if (impl) delete impl;
	});

	RES_TRY_ASSIGN(
	  impl->file =,
	  lak::winapi::invoke_invalid_handle_err(::CreateFileW,
	                                         path.native().c_str(),
	                                         GENERIC_READ,
	                                         FILE_SHARE_READ,
	                                         (LPSECURITY_ATTRIBUTES)NULL,
	                                         OPEN_EXISTING,
	                                         FILE_ATTRIBUTE_NORMAL,
	                                         (HANDLE)NULL));

	LARGE_INTEGER size;
	RES_TRY(lak::winapi::invoke_false_err(::GetFileSizeEx, impl->file, &size));

	RES_TRY_ASSIGN(
	  impl->file_map =,
	  lak::winapi::invoke_null_handle_err(::CreateFileMappingW,
	                                      impl->file,
	                                      (LPSECURITY_ATTRIBUTES)NULL,
	                                      PAGE_READONLY,
	                                      0,
	                                      0,
	                                      (LPCWSTR)NULL));

	RES_TRY_ASSIGN(impl->file_view =,
	               lak::winapi::invoke_null_err(
	                 ::MapViewOfFile, impl->file_map, FILE_MAP_READ, 0, 0, 0));

	lak::mapped_file result;

	result._impl = lak::exchange(impl, nullptr);
	result.data  = lak::span<const byte_t>(lak::span<const void>(
    result._impl->file_view, static_cast<size_t>(size.QuadPart)));

	return lak::move_ok(result);
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
