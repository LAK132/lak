#include "lak/system/file.hpp"

#include <mach-o/dyld.h>

lak::fs::path lak::exe_path()
{
	lak::array<char> path;
	uint32_t size = 0U;
	_NSGetExecutablePath(path.data(), &size);
	path.resize(size);
	if (_NSGetExecutablePath(path.data(), &size) == 0)
	{
		char resolved_path[PATH_MAX] = {0};
		if (realpath(path.data(), resolved_path) != nullptr)
			return lak::fs::path(resolved_path);
		else
		{
			BOUNDS_ASSERT_UNREACHABLE();
			return lak::fs::path(lak::astring(path.begin(), path.end()));
		}
	}
	BOUNDS_ASSERT_UNREACHABLE();
	return {};
}
