#include "lak/debug.hpp"

#include <fstream>

namespace lak
{
  void terminate_handler() { ABORT(); }

  void debugger_t::std_out(const std::u8string &line_info,
                           const std::string &str)
  {
    const auto indent = scoped_indenter::u8str();
    stream << indent << line_info << lak::to_u8string(str);
    if (live_output_enabled && !live_errors_only)
    {
      std::cout << reinterpret_cast<const char *>(indent.c_str());
      if (line_info_enabled)
      {
        std::cout << reinterpret_cast<const char *>(line_info.c_str());
      }
      std::cout << str << std::flush;
    }
  }

  void debugger_t::std_out(const std::u8string &line_info,
                           const std::wstring &str)
  {
    const auto indent = scoped_indenter::u8str();
    stream << indent << line_info << lak::to_u8string(str);
    if (live_output_enabled && !live_errors_only)
    {
      std::wcout << lak::to_wstring(indent);
      if (line_info_enabled)
      {
        std::wcout << lak::to_wstring(line_info);
      }
      std::wcout << str << std::flush;
    }
  }

  void debugger_t::std_out(const std::u8string &line_info,
                           const std::u8string &str)
  {
    const auto indent = scoped_indenter::u8str();
    stream << indent << line_info << str;
    if (live_output_enabled && !live_errors_only)
    {
      std::wcout << lak::to_wstring(indent);
      if (line_info_enabled)
      {
        std::wcout << lak::to_wstring(line_info);
      }
      std::wcout << lak::to_wstring(str) << std::flush;
    }
  }

  void debugger_t::std_err(const std::u8string &line_info,
                           const std::string &str)
  {
    const auto indent = scoped_indenter::u8str();
    stream << indent << line_info << lak::to_u8string(str);
    if (live_output_enabled)
    {
      std::cout << reinterpret_cast<const char *>(indent.c_str());
      if (line_info_enabled)
      {
        std::cout << reinterpret_cast<const char *>(line_info.c_str());
      }
      std::cout << str << std::flush;
    }
  }

  void debugger_t::std_err(const std::u8string &line_info,
                           const std::wstring &str)
  {
    const auto indent = scoped_indenter::u8str();
    stream << indent << line_info << lak::to_u8string(str);
    if (live_output_enabled)
    {
      std::wcout << lak::to_wstring(indent);
      if (line_info_enabled)
      {
        std::wcout << lak::to_wstring(line_info);
      }
      std::wcout << str << std::flush;
    }
  }

  void debugger_t::std_err(const std::u8string &line_info,
                           const std::u8string &str)
  {
    const auto indent = scoped_indenter::u8str();
    stream << indent << line_info << str;
    if (live_output_enabled)
    {
      std::wcout << lak::to_wstring(indent);
      if (line_info_enabled)
      {
        std::wcout << lak::to_wstring(line_info);
      }
      std::wcout << lak::to_wstring(str) << std::flush;
    }
  }

  void debugger_t::clear() { stream.clear(); }

  std::u8string debugger_t::str() { return stream.str(); }

  void debugger_t::abort()
  {
    DEBUG_BREAK();
    std::cerr << "Something went wrong!\n";
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
      if (file.is_open())
        file << reinterpret_cast<const char *>(str().c_str());
    }

    std::error_code ec;
    if (auto absolute = std::filesystem::absolute(path, ec); ec)
      return path;
    else
      return absolute;
  }

  scoped_indenter::scoped_indenter(const std::string &name)
  {
    debugger.std_out(lak::to_u8string(""), name + " {\n");
    ++debug_indent;
  }

  scoped_indenter::~scoped_indenter()
  {
    --debug_indent;
    debugger.std_out(lak::to_u8string(""), "}\n");
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

  std::u8string scoped_indenter::u8str()
  {
    static const std::u8string bar = lak::to_u8string("| ");
    static const std::u8string dot = lak::to_u8string(": ");
    std::u8string s;
    for (size_t i = debug_indent; i-- > 0;)
      s += ((debug_indent - i) & 1) ? bar : dot;
    return s;
  }

  size_t debug_indent;

  debugger_t debugger;
}
