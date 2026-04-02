#include "lak/system/file.hpp"
#include "lak/span.hpp"
#include "lak/system/os.hpp"
#include "lak/system/posix/wrapper.hpp"

namespace lak
{
	struct mapped_file_impl
	{
		lak::span<void> map;
		int fd;
		~mapped_file_impl()
		{
			::munmap(map.data(), map.size());
			::close(fd);
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

	RES_TRY_ASSIGN(impl->fd =,
	               lak::posix::open(path.native().c_str(), O_RDONLY));
	RES_TRY_ASSIGN(stat s =, lak::posix::fstat(impl->fd));
	size_t size = s.st_size;
	RES_TRY_ASSIGN(
	  auto ptr =,
	  lak::posix::mmap(
	    nullptr, 0, PROT_READ, MAP_PRIVATE | MAP_NORESERVE, impl->fd, size));
	impl->map = lak::span<void>(ptr, size);

	lak::mapped_file result;

	result._impl = lak::exchange(impl, nullptr);
	result.data  = lak::span<const byte_t>(lak::span<const void>(ptr, size));

	return lak::move_ok(result);
}
