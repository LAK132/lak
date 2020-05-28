#include "lak/debug.hpp"

#include <fstream>

namespace lak
{
  void terminate_handler() { ABORT(); }

  void debugger_t::std_out(const std::string &line_info,
                           const std::string &str)
  {
    stream << scoped_indenter::str() << line_info << str;
    if (live_output_enabled && !live_errors_only)
    {
      std::cout << scoped_indenter::str();
      if (line_info_enabled)
      {
        std::cout << line_info;
      }
      std::cout << str << std::flush;
    }
  }

  void debugger_t::std_err(const std::string &line_info,
                           const std::string &str)
  {
    stream << scoped_indenter::str() << line_info << str;
    if (live_output_enabled)
    {
      std::cerr << scoped_indenter::str();
      if (line_info_enabled)
      {
        std::cerr << line_info;
      }
      std::cerr << str << std::flush;
    }
  }

  void debugger_t::clear() { stream.clear(); }

  std::string debugger_t::str() { return stream.str(); }

  void debugger_t::abort()
  {
    std::cerr << "Something went wrong! Aborting...\n";
    if (!crash_path.empty())
      std::cerr
        << "Saving crash log to '" << lak::debugger.save()
        << "'.\nPlease forward the crash log onto the developer so they can "
           "attempt to fix the issues that caused this crash.\n";
    PAUSE();
    std::abort();
  }

  std::filesystem::path debugger_t::save() { return save(crash_path); }

  std::filesystem::path debugger_t::save(const std::filesystem::path &path)
  {
    if (!path.string().empty())
    {
      std::ofstream file(path, std::ios::out | std::ios::trunc);
      if (file.is_open()) file << str();
    }

    std::error_code ec;
    if (auto absolute = std::filesystem::absolute(path, ec); ec)
      return path;
    else
      return absolute;
  }

  scoped_indenter::scoped_indenter(const std::string &name)
  {
    debugger.std_out("", name + " {\n");
    ++debug_indent;
  }

  scoped_indenter::~scoped_indenter()
  {
    --debug_indent;
    debugger.std_out("", "}\n");
  }

  std::string scoped_indenter::str()
  {
    std::string s;
    for (size_t i = debug_indent; i-- > 0;)
      s += ((debug_indent - i) & 1) ? "| " : ": ";
    return s;
  }

  std::wstring scoped_indenter::wstr()
  {
    std::wstring s;
    for (size_t i = debug_indent; i-- > 0;)
      s += ((debug_indent - i) & 1) ? L"| " : L": ";
    return s;
  }

  size_t debug_indent;

  debugger_t debugger;
}
