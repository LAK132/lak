#include "lak/system/posix/wrapper.hpp"

lak::posix::result<int> lak::posix::close(int fd)
{
	if (int r = ::close(fd); r != -1)
		return lak::ok_t{r};
	else
		return lak::err_t{lak::errno_error::last_error()};
}

lak::posix::result<struct stat> lak::posix::fstat(int fd)
{
	struct stat s;
	if (int r = ::fstat(fd, &s); r != -1)
		return lak::ok_t{s};
	else
		return lak::err_t{lak::errno_error::last_error()};
}

lak::posix::result<void *> lak::posix::mmap(void *address,
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
		return lak::err_t{lak::errno_error::last_error()};
}

lak::posix::result<int> lak::posix::munmap(void *address, size_t length)
{
	if (int result = ::munmap(address, length); result != -1)
		return lak::ok_t{result};
	else
		return lak::err_t{lak::errno_error::last_error()};
}

lak::posix::result<int> lak::posix::mprotect(void *address,
                                             size_t length,
                                             int protect)
{
	if (int result = ::mprotect(address, length, protect); result != -1)
		return lak::ok_t{result};
	else
		return lak::err_t{lak::errno_error::last_error()};
}

lak::posix::result<int> lak::posix::madvise(void *address,
                                            size_t length,
                                            int advise)
{
	if (int result = ::madvise(address, length, advise); result != -1)
		return lak::ok_t{result};
	else
		return lak::err_t{lak::errno_error::last_error()};
}