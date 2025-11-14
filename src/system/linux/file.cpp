#include "lak/system/file.hpp"

#include "lak/array.hpp"
#include "lak/debug.hpp"

#include <sys/stat.h>
#include <unistd.h>

lak::fs::path lak::exe_path()
{
	const auto *proc_str = "/proc/self/exe";
	struct stat sb;
	lak::array<char> path;

	size_t read = 0;

	do
	{
		if (lstat(proc_str, &sb) == -1)
		{
			ERROR("Failed to lstat ", proc_str);
			return {};
		}

		if (sb.st_size < 0)
		{
			ERROR("Bad stat size ", sb.st_size);
			return {};
		}

		size_t st_size = static_cast<size_t>(sb.st_size);

		if (st_size > path.size()) path.resize(st_size + 1);

		ssize_t _read = readlink(proc_str, path.data(), path.size());

		if (_read < 0)
		{
			ERROR("Failed to read link (", _read, ")");
			return {};
		}

		read = static_cast<size_t>(_read);
	} while (read > static_cast<size_t>(sb.st_size));

	if (read + 1 < path.size()) path[read + 1] = 0;

	return lak::fs::path(path.data());
}
