#ifndef LAK_POSIX_WRAPPER_HPP
#define LAK_POSIX_WRAPPER_HPP

#include "lak/defer.hpp"
#include "lak/os.hpp"
#include "lak/result.hpp"
#include "lak/string.hpp"

#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace lak
{
	namespace posix
	{
		template<typename T = lak::monostate>
		using result = lak::result<T, int /* errno */>;

		lak::u8string error_code_to_string(int error_code)
		{
			return lak::to_u8string(::strerrordesc_np(error_code));
		}

		template<typename T>
		lak::result<T, lak::u8string> to_string(lak::posix::result<T> result)
		{
			return result.map_err(lak::posix::error_code_to_string);
		}

		template<typename... ARGS>
		lak::posix::result<int> open(const char *path, int oflag, ARGS &&...args)
		{
			if (int result = ::open(path, oflag, lak::forward<ARGS>(args)...);
			    result != -1)
				return lak::ok_t{result};
			else
				return lak::err_t{errno};
		}

		lak::posix::result<void *> mmap(void *address,
		                                size_t length,
		                                int protect,
		                                int flags,
		                                int file_descriptor,
		                                off_t offset)
		{
			if (void *result =
			      ::mmap(address, length, protect, flags, file_descriptor, offset);
			    result != MAP_FAILED)
				return lak::ok_t{result};
			else
				return lak::err_t{errno};
		}

		lak::posix::result<int> munmap(void *address, size_t length)
		{
			if (int result = ::munmap(address, length); result != -1)
				return lak::ok_t{result};
			else
				return lak::err_t{errno};
		}

		lak::posix::result<int> mprotect(void *address, size_t length, int protect)
		{
			if (int result = ::mprotect(address, length, protect); result != -1)
				return lak::ok_t{result};
			else
				return lak::err_t{errno};
		}

		lak::posix::result<int> madvise(void *address, size_t length, int advise)
		{
			if (int result = ::madvise(address, length, advise); result != -1)
				return lak::ok_t{result};
			else
				return lak::err_t{errno};
		}
	}
}

#endif