#include "lak/system/file.hpp"
#include "lak/system/win32/windows.hpp"

#include "lak/array.hpp"

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
