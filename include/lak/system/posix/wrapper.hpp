#ifndef LAK_SYSTEM_POSIX_WRAPPER_HPP
#define LAK_SYSTEM_POSIX_WRAPPER_HPP

#include "lak/system/os.hpp"

#include "lak/defer.hpp"
#include "lak/errno_result.hpp"
#include "lak/result.hpp"
#include "lak/string.hpp"

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace lak
{
	namespace posix
	{
		template<typename T = lak::monostate>
		using result = lak::errno_result<T>;

		template<typename... ARGS>
		lak::posix::result<int> open(const char *path, int oflag, ARGS &&...args)
		{
			if (int result = ::open(path, oflag, lak::forward<ARGS>(args)...);
			    result != -1)
				return lak::ok_t{result};
			else
				return lak::err_t{lak::errno_error::last_error()};
		}

		lak::posix::result<struct stat> fstat(int fd);

		lak::posix::result<void *> mmap(void *address,
		                                size_t length,
		                                int protect,
		                                int flags,
		                                int file_descriptor,
		                                off_t offset);

		lak::posix::result<int> munmap(void *address, size_t length);

		lak::posix::result<int> mprotect(void *address,
		                                 size_t length,
		                                 int protect);

		lak::posix::result<int> madvise(void *address, size_t length, int advise);
	}
}

#endif