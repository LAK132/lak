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

std::vector<uint8_t> lak::read_file(const fs::path &path)
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

bool lak::save_file(const fs::path &path, const std::vector<uint8_t> &data)
{
  std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
  if (!file.is_open()) return false;
  file.write(reinterpret_cast<const char *>(data.data()), data.size());
  return true;
}

bool lak::save_file(const fs::path &path, const lak::astring &string)
{
  std::ofstream file(path, std::ios::out | std::ios::trunc);
  if (!file.is_open()) return false;
  file << string;
  return true;
}

bool lak::save_file(const fs::path &path, const lak::wstring &string)
{
  std::basic_ofstream<wchar_t> file(path, std::ios::out | std::ios::trunc);
  if (!file.is_open()) return false;
  file << string;
  return true;
}

bool lak::save_file(const fs::path &path, const lak::u8string &string)
{
  std::basic_ofstream<char8_t> file(path, std::ios::out | std::ios::trunc);
  if (!file.is_open()) return false;
  file << string;
  return true;
}

bool lak::save_file(const fs::path &path, const lak::u16string &string)
{
  std::basic_ofstream<char16_t> file(path, std::ios::out | std::ios::trunc);
  if (!file.is_open()) return false;
  file << string;
  return true;
}

bool lak::save_file(const fs::path &path, const lak::u32string &string)
{
  std::basic_ofstream<char32_t> file(path, std::ios::out | std::ios::trunc);
  if (!file.is_open()) return false;
  file << string;
  return true;
}

lak::fs::path lak::exe_path()
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

  return fs::path(path.data());

#else
  FATAL("Not implemented");
  return {};
#endif
}

lak::fs::path lak::normalised(const fs::path &path)
{
  // ("a/b" | "a/b/") -> "a/b/." -> "a/b"
  return (path / ".").parent_path();
}

bool lak::has_parent(const fs::path &path)
{
  return lak::normalised(path).has_parent_path();
}

lak::fs::path lak::parent(const fs::path &path)
{
  return lak::normalised(path).parent_path();
}

lak::deepest_folder_result lak::deepest_folder(const fs::path &path,
                                               std::error_code &ec)
{
  fs::path folder = lak::normalised(path);
  fs::path file;
  auto entry = fs::directory_entry(path, ec);
  while (!entry.is_directory() && lak::has_parent(folder))
  {
    folder = lak::parent(folder);
    file   = path.lexically_relative(folder);
    // we're intentionally ignoring errors if there's still parent
    // directories, but we don't want to ignore the error of the last call
    // to fs::directory_entry.
    ec.clear();
    entry = fs::directory_entry(folder, ec);
  }
  if (ec) WARNING(path, ": ", ec.message());
  return {lak::normalised(folder), lak::normalised(file)};
}
