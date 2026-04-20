#include "lak/system/file.hpp"

#include "lak/strconv.hpp"

template<typename CHAR>
lak::error_code_result<lak::string<CHAR>> lak::read_file_str(
  const lak::fs::path &path)
{
	RES_TRY_ASSIGN(lak::unique_ptr<const byte_t[]> mapped =,
	               lak::ro_mmap_file(path));
	auto view =
	  lak::string_view<CHAR>(lak::span<const CHAR>(lak::span(*mapped)));
	return lak::ok_t<lak::string<CHAR>>{{view->begin(), view->end()}};
}

template<typename TO>
inline lak::string<TO> lak::strconv(const fs::path &path)
{
	return lak::strconv<TO>(path.native());
}
