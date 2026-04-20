#include "lak/system/file.hpp"
#include "lak/span.hpp"
#include "lak/system/os.hpp"
#include "lak/system/posix/wrapper.hpp"

#include <system_error>

namespace
{
	namespace local
	{
		std::error_code errno_to_errc(const lak::errno_error &err)
		{
			return std::error_code{err.value, std::generic_category()};
		}

		lak::error_code_result<lak::span<byte_t>> mmap_file(
		  const lak::fs::path &path, bool write_access, bool copy_on_write)
		{
			RES_TRY_ASSIGN(int fd =,
			               lak::posix::open(path.native().c_str(),
			                                write_access ? O_RDWR : O_RDONLY)
			                 .map_err(errno_to_errc));

			DEFER(lak::posix::close(fd).IF_ERR_WARN("close(fd) failed"));

			RES_TRY_ASSIGN(struct stat s =,
			               lak::posix::fstat(fd).map_err(errno_to_errc));
			size_t size = s.st_size;

			RES_TRY_ASSIGN(
			  auto ptr =,
			  lak::posix::mmap(nullptr,
			                   size,
			                   PROT_READ | (write_access ? PROT_WRITE : 0),
			                   write_access
			                     ? (copy_on_write ? MAP_PRIVATE : MAP_SHARED)
			                     : MAP_PRIVATE | MAP_NORESERVE,
			                   fd,
			                   0)
			    .map_err(errno_to_errc));

			return lak::ok_t{lak::span<byte_t>(lak::span<void>(ptr, size))};
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
		  lak::posix::munmap(
		    const_cast<void *>(reinterpret_cast<const void *>(f.data())), f.size())
		    .IF_ERR_WARN("munmap failed");
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
		  lak::posix::munmap(reinterpret_cast<void *>(f.data()), f.size())
		    .IF_ERR_WARN("munmap failed");
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
		  lak::posix::munmap(reinterpret_cast<void *>(f.data()), f.size())
		    .IF_ERR_WARN("munmap failed");
	  })};
}
