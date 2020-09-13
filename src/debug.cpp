#include "lak/debug.hpp"

#include <fstream>

void lak::terminate_handler()
{
  ABORT();
}

void lak::debugger_t::std_out(const lak::u8string &line_info,
                              const lak::astring &str)
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

void lak::debugger_t::std_out(const lak::u8string &line_info,
                              const lak::wstring &str)
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

void lak::debugger_t::std_out(const lak::u8string &line_info,
                              const lak::u8string &str)
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

void lak::debugger_t::std_err(const lak::u8string &line_info,
                              const lak::astring &str)
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

void lak::debugger_t::std_err(const lak::u8string &line_info,
                              const lak::wstring &str)
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

void lak::debugger_t::std_err(const lak::u8string &line_info,
                              const lak::u8string &str)
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

void lak::debugger_t::clear()
{
  stream.clear();
}

lak::u8string lak::debugger_t::str()
{
  return stream.str();
}

void lak::debugger_t::abort()
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

std::filesystem::path lak::debugger_t::save()
{
  return save(crash_path);
}

std::filesystem::path lak::debugger_t::save(const std::filesystem::path &path)
{
  if (!path.string().empty())
  {
    std::ofstream file(path, std::ios::out | std::ios::trunc);
    if (file.is_open()) file << reinterpret_cast<const char *>(str().c_str());
  }

  std::error_code ec;
  if (auto absolute = std::filesystem::absolute(path, ec); ec)
    return path;
  else
    return absolute;
}

lak::scoped_indenter::scoped_indenter(const lak::astring &name)
{
  lak::debugger.std_out(lak::to_u8string(""), name + " {\n");
  ++lak::debug_indent;
}

lak::scoped_indenter::~scoped_indenter()
{
  --lak::debug_indent;
  lak::debugger.std_out(lak::to_u8string(""), "}\n");
}

lak::astring lak::scoped_indenter::str()
{
  lak::astring s;
  for (size_t i = lak::debug_indent; i-- > 0;)
    s += ((lak::debug_indent - i) & 1) ? "| " : ": ";
  return s;
}

lak::wstring lak::scoped_indenter::wstr()
{
  lak::wstring s;
  for (size_t i = lak::debug_indent; i-- > 0;)
    s += ((lak::debug_indent - i) & 1) ? L"| " : L": ";
  return s;
}

lak::u8string lak::scoped_indenter::u8str()
{
  static const lak::u8string bar = lak::to_u8string("| ");
  static const lak::u8string dot = lak::to_u8string(": ");
  lak::u8string s;
  for (size_t i = lak::debug_indent; i-- > 0;)
    s += ((lak::debug_indent - i) & 1) ? bar : dot;
  return s;
}

size_t lak::debug_indent;

lak::debugger_t lak::debugger;
