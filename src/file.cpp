#include "lak/file.hpp"

#include <fstream>
#include <iostream>

namespace lak
{
  std::vector<uint8_t> read_file(const fs::path &path)
  {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::vector<uint8_t> result;
    if (!file.is_open()) return result;
    std::streampos fileSize = file.tellg();
    if (fileSize == std::streampos(-1)) return result;
    file.seekg(0);
    result.resize(fileSize);
    file.read(reinterpret_cast<char *>(result.data()), result.size());
    return result;
  }

  bool save_file(const fs::path &path, const std::vector<uint8_t> &data)
  {
    std::ofstream file(path,
                       std::ios::binary | std::ios::out | std::ios::trunc);
    if (!file.is_open()) return false;
    file.write(reinterpret_cast<const char *>(data.data()), data.size());
    return true;
  }

  bool save_file(const fs::path &path, const std::string &string)
  {
    std::ofstream file(path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) return false;
    file << string;
    return true;
  }

  bool save_file(const fs::path &path, const std::wstring &string)
  {
    std::basic_ofstream<wchar_t> file(path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) return false;
    file << string;
    return true;
  }

  bool save_file(const fs::path &path, const std::u8string &string)
  {
    std::basic_ofstream<char8_t> file(path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) return false;
    file << string;
    return true;
  }

  bool save_file(const fs::path &path, const std::u16string &string)
  {
    std::basic_ofstream<char16_t> file(path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) return false;
    file << string;
    return true;
  }

  bool save_file(const fs::path &path, const std::u32string &string)
  {
    std::basic_ofstream<char32_t> file(path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) return false;
    file << string;
    return true;
  }

}
