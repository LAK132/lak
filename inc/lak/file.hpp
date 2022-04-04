#ifndef LAK_FILE_HPP
#define LAK_FILE_HPP

#ifndef LAK_NO_FILESYSTEM

#	include "lak/array.hpp"
#	include "lak/errno_result.hpp"
#	include "lak/error_code_result.hpp"
#	include "lak/string.hpp"

#	include <filesystem>
#	include <system_error>

namespace lak
{
	namespace fs = std::filesystem;

	template<typename TO>
	inline lak::string<TO> strconv(const fs::path &path);
	lak::astring to_astring(const fs::path &path);
	lak::wstring to_wstring(const fs::path &path);
	lak::u8string to_u8string(const fs::path &path);
	lak::u16string to_u16string(const fs::path &path);
	lak::u32string to_u32string(const fs::path &path);

	// true if path exists
	// false if path does not exist
	// error if an OS error occurs
	lak::error_code_result<bool> path_exists(const fs::path &path);

	// ok if copy was successful
	// error if an OS error occurs
	lak::error_code_result<> copy_file(const fs::path &from, const fs::path &to);

	// true if directory was created
	// false if directory already exists
	// error if an OS error occurred
	lak::error_code_result<bool> create_directory(const fs::path &path);

	// ok if copy was successful
	// error if an OS error occurs
	lak::error_code_result<> create_hard_link(const fs::path &file,
	                                          const fs::path &link);

	lak::error_code_result<bool> remove_path(const fs::path &path);

	lak::errno_result<lak::array<byte_t>> read_file(const fs::path &path);

	bool save_file(const fs::path &path, lak::span<const byte_t> data);

	bool save_file(const fs::path &path, const lak::astring &string);
	bool save_file(const fs::path &path, const lak::wstring &string);
	bool save_file(const fs::path &path, const lak::u8string &string);
	bool save_file(const fs::path &path, const lak::u16string &string);
	bool save_file(const fs::path &path, const lak::u32string &string);

	fs::path exe_path();

	fs::path normalised(const fs::path &path);

	fs::path relative(const fs::path &from, const fs::path &to);

	bool has_parent(const fs::path &path);

	// The the ACTUAL parent path.
	// fs::path::parent_path() incorrectly gets "a/b" from "a/b/", this function
	// would instead return "a/" by normalising to "a/b/." first.
	fs::path parent(const fs::path &path);

	struct deepest_folder_result
	{
		fs::path folder;
		fs::path file;
	};
	lak::result<lak::deepest_folder_result, std::error_code> deepest_folder(
	  const fs::path &path);
}

#	include "lak/file.inl"

#endif
#endif
