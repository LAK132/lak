#include "lak/os.hpp"

#if defined(LAK_OS_WINDOWS)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <Windows.h>
#endif

#if defined(LAK_OS_LINUX)
#  include <sys/stat.h>
#  include <unistd.h>
#endif

#include "lak/debug.hpp"
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

  fs::path exe_path()
  {
#if defined(LAK_OS_WINDOWS)
    std::vector<wchar_t> path;
    path.resize(MAX_PATH);

    DWORD used = ::GetModuleFileNameW(NULL, path.data(), path.size());

    while (used >= path.size() - 1)
    {
      path.resize(path.size() * 2);
      used = ::GetModuleFileNameW(NULL, path.data(), path.size());
    }

    path[used + 1] = 0;

    return fs::path(path.data());

#elif defined(LAK_OS_LINUX)
    const auto *proc_str = "/proc/self/exe";
    struct stat sb;
    std::vector<char> path;

    do
    {
      if (lstat(proc_str, &sb) == -1)
      {
        ERROR("Failed to lstat " << proc_str);
        return {};
      }

      path.resize(sb.st_size + 1);

      auto read = readlink(proc_str, path.data(), path.size());

      if (read < 0)
      {
        ERROR("Failed to read link");
        return {};
      }
    } while (read > sb.st_size);

    path[read + 1] = 0;

    return fs::path(path.data());

#else
    FATAL("Not implemented");
    return {};
#endif
  }

  fs::path normalised(const fs::path &path)
  {
    // ("a/b" | "a/b/") -> "a/b/." -> "a/b"
    return (path / ".").parent_path();
  }

  bool has_parent(const fs::path &path)
  {
    return normalised(path).has_parent_path();
  }

  fs::path parent(const fs::path &path)
  {
    return normalised(path).parent_path();
  }

  deepest_folder_result deepest_folder(const fs::path &path,
                                       std::error_code &ec)
  {
    fs::path folder = normalised(path);
    fs::path file;
    auto entry = fs::directory_entry(path, ec);
    while (!entry.is_directory() && has_parent(folder))
    {
      folder = parent(folder);
      file   = path.lexically_relative(folder);
      // we're intentionally ignoring errors if there's still parent
      // directories, but we don't want to ignore the error of the last call
      // to fs::directory_entry.
      ec.clear();
      entry = fs::directory_entry(folder, ec);
    }
    if (ec) WARNING(path << ": " << ec.message());
    return {normalised(folder), normalised(file)};
  }

}
