#ifndef LAK_DEBUG_INL
#define LAK_DEBUG_INL

#include "lak/string.hpp"

#include <cstdlib>
#ifndef LAK_NO_FILESYSTEM
#  include <filesystem>
#endif
#include <iostream>
#include <sstream>

namespace lak
{
  // Use std::set_terminate(lak::terminate_handler); to enable
  void terminate_handler();

  struct debugger_t
  {
#ifndef LAK_DEBUG_STREAM_CHAR
#  define LAK_DEBUG_STREAM_CHAR char8_t
#endif
    std::basic_stringstream<LAK_DEBUG_STREAM_CHAR> stream;

    void std_out(const lak::u8string &line_info, const lak::astring &str);
    void std_out(const lak::u8string &line_info, const lak::wstring &str);
    void std_out(const lak::u8string &line_info, const lak::u8string &str);

    void std_err(const lak::u8string &line_info, const lak::astring &str);
    void std_err(const lak::u8string &line_info, const lak::wstring &str);
    void std_err(const lak::u8string &line_info, const lak::u8string &str);

    void clear();

    lak::u8string str();

    [[noreturn]] void abort();

#ifndef LAK_NO_FILESYSTEM
    std::filesystem::path save();

    std::filesystem::path save(const std::filesystem::path &path);

    std::filesystem::path crash_path;
#endif

    bool live_output_enabled = true;
    bool live_errors_only    = false;
    bool line_info_enabled   = true;
  };

  struct scoped_indenter
  {
    scoped_indenter(const lak::astring &name);

    ~scoped_indenter();

    static lak::astring str();
    static lak::wstring wstr();
    static lak::u8string u8str();
  };

  extern size_t debug_indent;

  extern debugger_t debugger;
}

#include "lak/strcast.hpp"

#include "lak/streamify.hpp"

#endif
