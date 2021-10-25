#ifdef LAK_NO_FILESYSTEM
#	error This file should not be compiled with filesystems disabled
#endif

#include "lak/os.hpp"

#if defined(LAK_OS_WINDOWS)
#	include "lak/windows.hpp"
#endif

#if defined(LAK_OS_LINUX)
#	include <sys/stat.h>
#	include <unistd.h>
#endif

#include "lak/debug.hpp"
#include "lak/file.hpp"

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

lak::error_code_result<bool> lak::path_exists(const lak::fs::path &path)
{
	std::error_code ec;
	if (bool result = fs::exists(path, ec); ec)
		return lak::err_t{lak::error_code_error{ec}};
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
		return lak::err_t{lak::error_code_error{ec}};
	}
	else
		return lak::ok_t{};
}

lak::error_code_result<bool> lak::create_directory(const fs::path &path)
{
	std::error_code ec;
	if (bool result = fs::create_directory(path, ec); ec)
		return lak::err_t{lak::error_code_error{ec}};
	else
		return lak::ok_t{result};
}

lak::error_code_result<> lak::create_hard_link(const fs::path &file,
                                               const fs::path &link)
{
	std::error_code ec;
	if (fs::create_hard_link(file, link, ec); ec)
		return lak::err_t{lak::error_code_error{ec}};
	else
		return lak::ok_t{};
}

lak::errno_result<lak::array<byte_t>> lak::read_file(const lak::fs::path &path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open()) return lak::err_t{lak::errno_error::last_error()};

	std::streampos file_size = file.tellg();
	if (file_size == std::streampos(-1))
		return lak::err_t{lak::errno_error::last_error()};

	file.seekg(0);
	if (file.fail()) return lak::err_t{lak::errno_error::last_error()};

	auto result = lak::array<byte_t>(static_cast<size_t>(file_size));

	file.read(reinterpret_cast<char *>(result.data()), result.size());
	if (file.fail()) return lak::err_t{lak::errno_error::last_error()};

	return lak::ok_t{lak::move(result)};
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

bool lak::save_file(const lak::fs::path &path, const lak::astring &string)
{
	return lak::save_file(
	  path, lak::span<const byte_t>(lak::span(lak::string_view(string))));
}

bool lak::save_file(const lak::fs::path &path, const lak::wstring &string)
{
	return lak::save_file(
	  path, lak::span<const byte_t>(lak::span(lak::string_view(string))));
}

bool lak::save_file(const lak::fs::path &path, const lak::u8string &string)
{
	return lak::save_file(
	  path, lak::span<const byte_t>(lak::span(lak::string_view(string))));
}

bool lak::save_file(const lak::fs::path &path, const lak::u16string &string)
{
	return lak::save_file(
	  path, lak::span<const byte_t>(lak::span(lak::string_view(string))));
}

bool lak::save_file(const lak::fs::path &path, const lak::u32string &string)
{
	return lak::save_file(
	  path, lak::span<const byte_t>(lak::span(lak::string_view(string))));
}

lak::fs::path lak::exe_path()
{
#if defined(LAK_OS_WINDOWS)
	std::vector<wchar_t> path;
	path.resize(MAX_PATH);

	DWORD used = ::GetModuleFileNameW(NULL, path.data(), DWORD(path.size()));

	while (used >= path.size() - 1)
	{
		path.resize(path.size() * 2);
		used = ::GetModuleFileNameW(NULL, path.data(), DWORD(path.size()));
	}

	path[used + 1] = 0;

	return lak::fs::path(path.data());

#elif defined(LAK_OS_LINUX)
	const auto *proc_str = "/proc/self/exe";
	struct stat sb;
	std::vector<char> path;

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

#else
	ASSERT_NYI();
	return {};
#endif
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

lak::result<lak::deepest_folder_result, std::error_code> lak::deepest_folder(
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
