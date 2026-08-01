#include "lak/system/file.hpp"

#include "lak/array.hpp"
#include "lak/debug.hpp"
#include "lak/errno_result.hpp"

#include <sys/stat.h>
#include <unistd.h>

lak::fs::path lak::exe_path()
{
	const char *proc_str = "/proc/self/exe";
	struct stat sb;
	lak::array<char> path;

	if (lstat(proc_str, &sb) == -1)
	{
		WARNING(
		  "Failed to lstat ", proc_str, " (", lak::errno_error::last_error(), ")");
		path.resize(PATH_MAX);
	}
	else if (sb.st_size < 0)
	{
		WARNING("Bad stat size ", sb.st_size);
		path.resize(PATH_MAX);
	}
	else if (sb.st_size == 0)
	{
		path.resize(PATH_MAX);
	}
	else
	{
		path.resize(static_cast<size_t>(sb.st_size) + 1U);
	}

	size_t read = lak::dynamic_extent;

	for (; read >= path.size(); path.resize((path.size() * 2U) + 1U))
	{
		ssize_t _read = readlink(proc_str, path.data(), path.size());

		if (_read < 0)
		{
			ERROR("Failed to read link (", lak::errno_error::last_error(), ")");
			return {};
		}

		read = static_cast<size_t>(_read);
	}

	ASSERT_GREATER(path.size(), read);
	path[read] = 0;

	return lak::fs::path(path.data());
}
