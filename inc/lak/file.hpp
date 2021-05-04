#ifndef LAK_FILE_HPP
#define LAK_FILE_HPP

#ifndef LAK_NO_FILESYSTEM

#  include "lak/errno_result.hpp"
#  include "lak/string.hpp"

#  include <filesystem>
#  include <system_error>
#  include <tuple>
#  include <vector>

namespace lak
{
  namespace fs = std::filesystem;

  lak::errno_result<std::vector<uint8_t>> read_file(const fs::path &path);

  bool save_file(const fs::path &path, lak::span<const uint8_t> data);

  bool save_file(const fs::path &path, const std::vector<uint8_t> &data);

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

#endif
#endif
