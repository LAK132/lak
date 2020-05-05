#ifndef LAK_FILE_HPP
#define LAK_FILE_HPP

#include "string.hpp"

#include <filesystem>
#include <vector>

namespace lak
{
  namespace fs = std::filesystem;

  std::vector<uint8_t> read_file(const fs::path &path);

  bool save_file(const fs::path &path, const std::vector<uint8_t> &data);

  bool save_file(const fs::path &path, const std::string &string);
  bool save_file(const fs::path &path, const std::wstring &string);
  bool save_file(const fs::path &path, const std::u8string &string);
  bool save_file(const fs::path &path, const std::u16string &string);
  bool save_file(const fs::path &path, const std::u32string &string);
}

#endif
