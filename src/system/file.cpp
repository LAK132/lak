#include "lak/system/file.hpp"
#include "lak/system/os.hpp"

#include "lak/functional.hpp"

#include "lak/debug.hpp"

#include "lak/strconv.hpp"
#include "lak/string_view.hpp"

#include <fstream>
#include <iostream>

lak::astring lak::to_astring(const fs::path &path)
{
	return lak::to_astring(path.native());
}

lak::wstring lak::to_wstring(const fs::path &path)
{
	return lak::to_wstring(path.native());
}

lak::u8string lak::to_u8string(const fs::path &path)
{
	return lak::to_u8string(path.native());
}

lak::u16string lak::to_u16string(const fs::path &path)
{
	return lak::to_u16string(path.native());
}

lak::u32string lak::to_u32string(const fs::path &path)
{
	return lak::to_u32string(path.native());
}

lak::error_code_result<lak::fs::directory_iterator> lak::directory_iterator(
  const fs::path &path)
{
	std::error_code ec;
	if (fs::directory_iterator di{path, ec}; ec)
		return lak::err_t{ec};
	else
		return lak::ok_t{lak::move(di)};
}

lak::error_code_result<bool> lak::path_exists(const lak::fs::path &path)
{
	std::error_code ec;
	if (bool result = fs::exists(path, ec); ec)
		return lak::err_t{ec};
	else
		return lak::ok_t{result};
}

lak::error_code_result<> lak::copy_file(const fs::path &from,
                                        const fs::path &to)
{
	std::error_code ec;
	// this overload of copy_file only returns false if an error occurs
	if (bool result = fs::copy_file(from, to, ec); ec)
	{
		ASSERT(!result);
		return lak::err_t{ec};
	}
	else
		return lak::ok_t{};
}

lak::error_code_result<bool> lak::create_directory(const fs::path &path)
{
	std::error_code ec;
	if (bool result = fs::create_directory(path, ec); ec)
		return lak::err_t{ec};
	else
		return lak::ok_t{result};
}

lak::error_code_result<bool> lak::create_directories(const fs::path &path)
{
	std::error_code ec;
	if (bool result = fs::create_directories(path, ec); ec)
		return lak::err_t{ec};
	else
		return lak::ok_t{result};
}

lak::error_code_result<> lak::create_hard_link(const fs::path &file,
                                               const fs::path &link)
{
	std::error_code ec;
	if (fs::create_hard_link(file, link, ec); ec)
		return lak::err_t{ec};
	else
		return lak::ok_t{};
}

lak::error_code_result<bool> lak::remove_path(const fs::path &path)
{
	std::error_code ec;
	bool result = fs::remove_all(path, ec) > 0U;
	if (ec)
		return lak::err_t{ec};
	else
		return lak::ok_t{result};
}

lak::error_code_result<lak::array<byte_t>> lak::read_file(
  const lak::fs::path &path)
{
	RES_TRY_ASSIGN(lak::unique_ptr<const byte_t[]> mapped =,
	               lak::ro_mmap_file(path));
	return lak::ok_t<lak::array<byte_t>>{{mapped->begin(), mapped->end()}};
}

bool lak::save_file(const lak::fs::path &path, lak::span<const byte_t> data)
{
	std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
	if (!file.is_open()) return false;
	file.write(reinterpret_cast<const char *>(data.data()), data.size());
	if (!file.good()) return false;
	file.close();
	return !file.fail();
}

bool lak::save_file(const lak::fs::path &path, lak::astring_view string)
{
	return lak::save_file(path, lak::span<const byte_t>(lak::span(string)));
}

bool lak::save_file(const lak::fs::path &path, lak::wstring_view string)
{
	return lak::save_file(path, lak::span<const byte_t>(lak::span(string)));
}

bool lak::save_file(const lak::fs::path &path, lak::u8string_view string)
{
	return lak::save_file(path, lak::span<const byte_t>(lak::span(string)));
}

bool lak::save_file(const lak::fs::path &path, lak::u16string_view string)
{
	return lak::save_file(path, lak::span<const byte_t>(lak::span(string)));
}

bool lak::save_file(const lak::fs::path &path, lak::u32string_view string)
{
	return lak::save_file(path, lak::span<const byte_t>(lak::span(string)));
}

lak::fs::path lak::normalised(const lak::fs::path &path)
{
	// ("a/b" | "a/b/" | "a/b/.") -> "a/b/" -> "a/b/." -> "a/b"
	auto norm = (path.lexically_normal() / ".").parent_path();
	if (norm == ".")
		return "";
	else
		return norm;
}

lak::fs::path lak::relative(const lak::fs::path &from, const lak::fs::path &to)
{
	return lak::normalised(from.lexically_relative(to));
}

bool lak::has_parent(const lak::fs::path &path)
{
	return lak::normalised(path).has_parent_path();
}

lak::fs::path lak::parent(const lak::fs::path &path)
{
	return lak::normalised(path).parent_path();
}

lak::error_code_result<lak::deepest_folder_result> lak::deepest_folder(
  const lak::fs::path &path)
{
	lak::fs::path folder = lak::normalised(path);
	std::error_code ec;
	auto entry = lak::fs::directory_entry(folder, ec);
	while (!entry.is_directory() && lak::has_parent(folder))
	{
		folder = lak::parent(folder);
		// we're intentionally ignoring errors if there's still parent
		// directories, but we don't want to ignore the error of the last call
		// to fs::directory_entry.
		ec.clear();
		entry = lak::fs::directory_entry(folder, ec);
	}
	if (ec) return lak::err_t{ec};
	return lak::ok_t{lak::deepest_folder_result{lak::normalised(folder),
	                                            lak::relative(path, folder)}};
}
