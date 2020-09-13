#ifndef LAK_FILE_HPP
#define LAK_FILE_HPP

#include "lak/string.hpp"

#include <filesystem>
#include <tuple>
#include <vector>

namespace lak
{
  namespace fs = std::filesystem;

  std::vector<uint8_t> read_file(const fs::path &path);

  bool save_file(const fs::path &path, const std::vector<uint8_t> &data);

  bool save_file(const fs::path &path, const lak::astring &string);
  bool save_file(const fs::path &path, const lak::wstring &string);
  bool save_file(const fs::path &path, const lak::u8string &string);
  bool save_file(const fs::path &path, const lak::u16string &string);
  bool save_file(const fs::path &path, const lak::u32string &string);

  fs::path exe_path();

  fs::path normalised(const fs::path &path);

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
  deepest_folder_result deepest_folder(const fs::path &path,
                                       std::error_code &ec);
}

#endif
