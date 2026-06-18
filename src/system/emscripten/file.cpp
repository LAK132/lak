#include "lak/system/file.hpp"
#include "lak/system/posix/wrapper.hpp"

#include "lak/debug.hpp"

#include <cstdio>
#include <system_error>

namespace
{
	namespace local
	{
		std::error_code errno_to_errc(int err)
		{
			return std::error_code{err, std::generic_category()};
		}
	}
}

lak::error_code_result<lak::unique_ptr<const byte_t[]>> lak::ro_mmap_file(
  const fs::path &path)
{
	RES_TRY_ASSIGN(auto cw =, lak::cw_mmap_file(path));
	auto s   = cw.release();
	auto res = lak::unique_ptr<const byte_t[]>(lak::span<const byte_t>(s),
	                                           [](lak::span<const byte_t> s)
	                                           { delete[] s.data(); });
	return lak::move_ok(res);
}

lak::error_code_result<lak::unique_ptr<byte_t[]>> lak::rw_mmap_file(
  const fs::path &)
{
	ASSERT_NYI();
}

lak::error_code_result<lak::unique_ptr<byte_t[]>> lak::cw_mmap_file(
  const fs::path &path)
{
	FILE *f = std::fopen(path.c_str(), "r");
	if (!f) return lak::err_t{local::errno_to_errc(EIO)};
	DEFER(std::fclose(f));

	if (std::fseek(f, 0, SEEK_END) != 0)
		return lak::err_t{local::errno_to_errc(EIO)};

	const long size = std::ftell(f);
	if (size < 0) return lak::err_t{local::errno_to_errc(EIO)};

	if (std::fseek(f, 0, SEEK_SET) != 0)
		return lak::err_t{local::errno_to_errc(EIO)};

	auto p = new byte_t[size_t(size)];
	DEFER(if (p) delete[] p);

	auto s = lak::span<byte_t>(p, size_t(size));
	if (std::fread(s.data(), 1U, s.size(), f) != s.size())
		return lak::err_t{local::errno_to_errc(EIO)};

	auto res = lak::unique_ptr<byte_t[]>(
	  s, [](lak::span<byte_t> s) { delete[] s.data(); });

	p = nullptr;

	return lak::move_ok(res);
}

lak::fs::path lak::exe_path() { ASSERT_NYI(); }
