#ifndef LAK_DEBUG_HPP
#  include "lak/compiler.hpp"
#  include "lak/macro_utils.hpp"
#  include "lak/os.hpp"
#  include "lak/strconv.hpp"
#  include "lak/string.hpp"

#  include <cstdlib>
#  include <filesystem>
#  include <iostream>
#  include <sstream>
#endif

#undef TO_STRING
#define TO_STRING(x)                                                          \
  [&] {                                                                       \
    std::stringstream _debug_stream;                                          \
    _debug_stream << x;                                                       \
    return _debug_stream.str();                                               \
  }()
#undef TO_U8STRING
#define TO_U8STRING(x)                                                        \
  [&] {                                                                       \
    std::stringstream _debug_stream;                                          \
    _debug_stream << x;                                                       \
    return std::u8string(                                                     \
      reinterpret_cast<const char8_t *>(_debug_stream.str().c_str()));        \
  }()
#undef TO_WSTRING
#define TO_WSTRING(x)                                                         \
  [&] {                                                                       \
    std::wstringstream _debug_stream;                                         \
    _debug_stream << x;                                                       \
    return _debug_stream.str();                                               \
  }()
#undef WTO_U8STRING
#define WTO_U8STRING(x) [&] { return lak::to_u8string(TO_WSTRING(x)); }()

#ifndef LAK_DEBUG_HPP
namespace lak
{
  // Use std::set_terminate(lak::terminate_handler); to enable
  void terminate_handler();

  struct debugger_t
  {
    std::basic_stringstream<char8_t> stream;

    void std_out(const std::u8string &line_info, const std::string &str);
    void std_out(const std::u8string &line_info, const std::wstring &str);
    void std_out(const std::u8string &line_info, const std::u8string &str);

    void std_err(const std::u8string &line_info, const std::string &str);
    void std_err(const std::u8string &line_info, const std::wstring &str);
    void std_err(const std::u8string &line_info, const std::u8string &str);

    void clear();

    std::u8string str();

    [[noreturn]] void abort();

    std::filesystem::path save();

    std::filesystem::path save(const std::filesystem::path &path);

    std::filesystem::path crash_path;

    bool live_output_enabled = true;
    bool live_errors_only    = false;
    bool line_info_enabled   = true;
  };

  struct scoped_indenter
  {
    scoped_indenter(const std::string &name);

    ~scoped_indenter();

    static std::string str();
    static std::wstring wstr();
    static std::u8string u8str();
  };

  extern size_t debug_indent;

  extern debugger_t debugger;
}
#endif

#undef TRY
#undef CATCH
#if defined(NDEBUG)
#  define TRY      try
#  define CATCH(X) catch (X)
#else
#  define TRY
#  define CATCH(X)                                                            \
    try                                                                       \
    {                                                                         \
    }                                                                         \
    catch (X)
#endif

#ifndef LAK_DEBUG_HPP
#  define LAK_ESC        "\x1B"
#  define LAK_CSI        LAK_ESC "["
#  define LAK_SGR(x)     LAK_CSI STRINGIFY(x) "m"
#  define LAK_SGR_RESET  LAK_SGR(0)
#  define LAK_BOLD       LAK_SGR(1)
#  define LAK_FAINT      LAK_SGR(2)
#  define LAK_ITALIC     LAK_SGR(3)
#  define LAK_YELLOW     LAK_SGR(33)
#  define LAK_BRIGHT_RED LAK_SGR(91)
#endif

#undef PAUSE
#undef PAUSEF
#define PAUSE()                                                               \
  {                                                                           \
    std::cerr << "Press enter to continue...\n";                              \
    getchar();                                                                \
  }
#define PAUSEF(str)                                                           \
  {                                                                           \
    std::cerr << str << "\nPress enter to continue...\n";                     \
    getchar();                                                                \
  }

#undef CHECKPOINT
#undef DEBUG_LINE_FILE
#undef DEBUG
#undef WDEBUG_LINE_FILE
#undef WDEBUG
#if defined(NOLOG)
#  define CHECKPOINT()
#  define DEBUG(x)
#  define WDEBUG(x)
#else
#  if defined(LAK_OS_WINDOWS)
#    define DEBUG_LINE_FILE  "(" LINE_TRACE_STR ")"
#    define WDEBUG_LINE_FILE L"(" LINE_TRACE_STR ")"
#  else
#    define DEBUG_LINE_FILE  LAK_FAINT "(" LINE_TRACE_STR ")" LAK_SGR_RESET
#    define WDEBUG_LINE_FILE L"" LAK_FAINT "(" LINE_TRACE_STR ")" LAK_SGR_RESET
#  endif
#  define CHECKPOINT()                                                        \
    lak::debugger.std_out(TO_U8STRING("CHECKPOINT" << DEBUG_LINE_FILE),       \
                          TO_U8STRING("\n"));
#  define DEBUG(x)                                                            \
    lak::debugger.std_out(TO_U8STRING("DEBUG" << DEBUG_LINE_FILE << ": "),    \
                          TO_U8STRING(std::hex << x << "\n"));
#  define WDEBUG(x)                                                           \
    lak::debugger.std_out(TO_U8STRING("DEBUG" << DEBUG_LINE_FILE << ": "),    \
                          TO_WSTRING(std::hex << x << L"\n"));
#endif

#undef ABORT
#undef NOISY_ABORT
#define ABORT()                                                               \
  {                                                                           \
    lak::debugger.abort();                                                    \
  }
#define NOISY_ABORT()                                                         \
  {                                                                           \
    DEBUG_BREAK();                                                            \
    std::cerr << reinterpret_cast<const char *>(                              \
                   lak::debugger.stream.str().c_str())                        \
              << "\n";                                                        \
    ABORT();                                                                  \
  }

#undef WARNING
#undef WWARNING
#undef ERROR
#undef WERROR
#undef FATAL
#if defined(NOLOG)
#  define WARNING(x)
#  define WWARNING(x)
#  define ERROR(x)
#  define WERROR(x)
#  define FATAL(x) ABORT()
#elif defined(LAK_OS_WINDOWS)
#  define WARNING(x)                                                          \
    lak::debugger.std_err(TO_U8STRING("WARNING" << DEBUG_LINE_FILE << ": "),  \
                          TO_U8STRING(std::hex << x << "\n"));
#  define WWARNING(x)                                                         \
    lak::debugger.std_err(TO_U8STRING("WARNING" << WDEBUG_LINE_FILE << ": "), \
                          TO_WSTRING(std::hex << x << L"\n"));
#  define ERROR(x)                                                            \
    lak::debugger.std_err(TO_U8STRING("ERROR" << DEBUG_LINE_FILE << ": "),    \
                          TO_U8STRING(std::hex << x << "\n"));
#  define WERROR(x)                                                           \
    lak::debugger.std_err(TO_U8STRING("ERROR" << DEBUG_LINE_FILE << ": "),    \
                          TO_WSTRING(std::hex << x << L"\n"));
#  define FATAL(x)                                                            \
    {                                                                         \
      lak::debugger.std_err(TO_U8STRING("FATAL" << DEBUG_LINE_FILE << ": "),  \
                            TO_U8STRING(std::hex << x << "\n"));              \
      ABORT();                                                                \
    }
#  define WFATAL(x)                                                           \
    {                                                                         \
      lak::debugger.std_err(TO_U8STRING("FATAL" << DEBUG_LINE_FILE << ": "),  \
                            TO_WSTRING(std::hex << x << L"\n"));              \
      ABORT();                                                                \
    }
#else
#  define WARNING(x)                                                          \
    lak::debugger.std_err(                                                    \
      TO_U8STRING(LAK_YELLOW LAK_BOLD "WARNING" LAK_SGR_RESET LAK_YELLOW      \
                  << DEBUG_LINE_FILE << ": "),                                \
      TO_U8STRING(std::hex << x << "\n"));
#  define WWARNING(x)                                                         \
    lak::debugger.std_err(                                                    \
      TO_U8STRING("" LAK_YELLOW LAK_BOLD "WARNING" LAK_SGR_RESET LAK_YELLOW   \
                  << WDEBUG_LINE_FILE << ": "),                               \
      TO_WSTRING(std::hex << x << L"\n"));
#  define ERROR(x)                                                            \
    lak::debugger.std_err(                                                    \
      TO_U8STRING(                                                            \
        LAK_BRIGHT_RED LAK_BOLD "ERROR" LAK_SGR_RESET LAK_BRIGHT_RED          \
        << DEBUG_LINE_FILE << ": "),                                          \
      TO_U8STRING(std::hex << x << "\n"));
#  define WERROR(x)                                                           \
    lak::debugger.std_err(                                                    \
      TO_U8STRING(                                                            \
        "" LAK_BRIGHT_RED LAK_BOLD "ERROR" LAK_SGR_RESET LAK_BRIGHT_RED       \
        << WDEBUG_LINE_FILE << ": "),                                         \
      TO_WSTRING(std::hex << x << L"\n"));
#  define FATAL(x)                                                            \
    {                                                                         \
      lak::debugger.std_err(                                                  \
        TO_U8STRING(                                                          \
          LAK_BRIGHT_RED LAK_BOLD "FATAL" LAK_SGR_RESET LAK_BRIGHT_RED        \
          << DEBUG_LINE_FILE << ": "),                                        \
        TO_U8STRING(std::hex << x << "\n"));                                  \
      ABORT();                                                                \
    }
#  define WFATAL(x)                                                           \
    {                                                                         \
      lak::debugger.std_err(                                                  \
        TO_U8STRING(                                                          \
          LAK_BRIGHT_RED LAK_BOLD "FATAL" LAK_SGR_RESET LAK_BRIGHT_RED        \
          << DEBUG_LINE_FILE << ": "),                                        \
        TO_WSTRING(std::hex << x << L"\n"));                                  \
      ABORT();                                                                \
    }
#endif

#undef ASSERT
#undef ASSERTF
#define ASSERT(x)                                                             \
  {                                                                           \
    if (!(x))                                                                 \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(x) "' failed");                           \
    }                                                                         \
  }
#define ASSERTF(x, str)                                                       \
  {                                                                           \
    if (!(x))                                                                 \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(x) "' failed: " << str);                  \
    }                                                                         \
  }
#ifndef LAK_DEBUG_HPP
#  define LAK_DEBUG_HPP
#endif