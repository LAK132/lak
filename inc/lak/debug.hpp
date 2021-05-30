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
    return lak::as_u8string(_debug_stream.str()).to_string();                 \
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
#undef SCOPED_CHECKPOINT
#undef DEBUG_LINE_FILE
#undef DEBUG
#if defined(NOLOG)
#  define CHECKPOINT()
#  define SCOPED_CHECKPOINT(...)
#  define DEBUG(...)
#else
#  if defined(LAK_NO_DEBUG_COLOURS)
#    define DEBUG_LINE_FILE "(" LINE_TRACE_STR ")"
#  elif defined(LAK_OS_WINDOWS)
#    define DEBUG_LINE_FILE                                                   \
      LAK_BRIGHT_BLACK "(" LINE_TRACE_STR ")" LAK_SGR_RESET
#  else
#    define DEBUG_LINE_FILE LAK_FAINT "(" LINE_TRACE_STR ")" LAK_SGR_RESET
#  endif
#  define CHECKPOINT()                                                        \
    lak::debugger.std_out(TO_U8STRING("CHECKPOINT" << DEBUG_LINE_FILE),       \
                          lak::to_u8string("\n"));
#  define SCOPED_CHECKPOINT(...)                                              \
    lak::scoped_indenter UNIQUIFY(SCOPED_INDENTOR_)(                          \
      lak::streamify<char>(__VA_ARGS__, " " DEBUG_LINE_FILE));
#  define DEBUG(...)                                                          \
    lak::debugger.std_out(TO_U8STRING("DEBUG" << DEBUG_LINE_FILE << ": "),    \
                          lak::streamify<char8_t>(__VA_ARGS__, "\n"));
#endif

#undef ABORT
#undef ABORTF
#undef NOISY_ABORT
#define ABORT()                                                               \
  {                                                                           \
    lak::debugger.abort();                                                    \
  }
#define ABORTF(...)                                                           \
  {                                                                           \
    lak::debugger.std_err(reinterpret_cast<const char8_t *>(""),              \
                          lak::streamify<char8_t>(__VA_ARGS__, "\n"));        \
    ABORT();                                                                  \
  }
#define NOISY_ABORT()                                                         \
  {                                                                           \
    DEBUG_BREAK();                                                            \
    std::cerr << lak::as_astring(lak::debugger.stream.str()) << "\n";         \
    ABORT();                                                                  \
  }

#undef WARNING
#undef ERROR
#undef FATAL
#if defined(NOLOG)
#  define WARNING(...)
#  define ERROR(...)
#  define FATAL(...) ABORT()
#elif defined(LAK_NO_DEBUG_COLOURS)
#  define WARNING(...)                                                        \
    lak::debugger.std_err(TO_U8STRING("WARNING" << DEBUG_LINE_FILE << ": "),  \
                          lak::streamify<char8_t>(__VA_ARGS__, "\n"));
#  define ERROR(...)                                                          \
    lak::debugger.std_err(TO_U8STRING("ERROR" << DEBUG_LINE_FILE << ": "),    \
                          lak::streamify<char8_t>(__VA_ARGS__, "\n"));
#  define FATAL(...)                                                          \
    ABORTF(TO_U8STRING("FATAL" << DEBUG_LINE_FILE << ": "), __VA_ARGS__)
#else
#  define WARNING(...)                                                        \
    lak::debugger.std_err(                                                    \
      TO_U8STRING(LAK_YELLOW LAK_BOLD "WARNING" LAK_SGR_RESET LAK_YELLOW      \
                  << DEBUG_LINE_FILE << ": "),                                \
      lak::streamify<char8_t>(__VA_ARGS__, "\n"));
#  define ERROR(...)                                                          \
    lak::debugger.std_err(                                                    \
      TO_U8STRING(                                                            \
        LAK_BRIGHT_RED LAK_BOLD "ERROR" LAK_SGR_RESET LAK_BRIGHT_RED          \
        << DEBUG_LINE_FILE << ": "),                                          \
      lak::streamify<char8_t>(__VA_ARGS__, "\n"));
#  define FATAL(...)                                                          \
    ABORTF(TO_U8STRING(                                                       \
             LAK_BRIGHT_RED LAK_BOLD "FATAL" LAK_SGR_RESET LAK_BRIGHT_RED     \
             << DEBUG_LINE_FILE << ": "),                                     \
           __VA_ARGS__)
#endif

#undef ASSERT
#undef ASSERT_NYI
#undef ASSERT_EQUAL
#undef ASSERT_NOT_EQUAL
#undef ASSERT_GREATER
#undef ASSERT_GREATER_OR_EQUAL
#undef ASSERT_LESS
#undef ASSERT_LESS_OR_EQUAL
#undef ASSERTF
#undef ASSERTF_EQUAL
#undef ASSERTF_NOT_EQUAL
#undef ASSERTF_GREATER
#undef ASSERTF_GREATER_OR_EQUAL
#undef ASSERTF_LESS
#undef ASSERTF_LESS_OR_EQUAL
#define ASSERT(x)                                                             \
  {                                                                           \
    if (!(x))                                                                 \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(x) "' failed");                           \
    }                                                                         \
  }
#define ASSERT_NYI()                                                          \
  {                                                                           \
    FATAL("Behaviour not yet implemented");                                   \
  }
#define ASSERT_EQUAL(X, Y)                                                    \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) == (y)))                                                        \
    {                                                                         \
      FATAL(                                                                  \
        "Assertion '" STRINGIFY(X == Y) "' failed: '", x, "' != '", y, "'");  \
    }                                                                         \
  }
#define ASSERT_NOT_EQUAL(X, Y)                                                \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) != (y)))                                                        \
    {                                                                         \
      FATAL(                                                                  \
        "Assertion '" STRINGIFY(X != Y) "' failed: '", x, "' == '", y, "'");  \
    }                                                                         \
  }
#define ASSERT_GREATER(X, Y)                                                  \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) > (y)))                                                         \
    {                                                                         \
      FATAL(                                                                  \
        "Assertion '" STRINGIFY(X > Y) "' failed: '", x, "' <= '", y, "'");   \
    }                                                                         \
  }
#define ASSERT_GREATER_OR_EQUAL(X, Y)                                         \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) >= (y)))                                                        \
    {                                                                         \
      FATAL(                                                                  \
        "Assertion '" STRINGIFY(X >= Y) "' failed: '", x, "' < '", y, "'");   \
    }                                                                         \
  }
#define ASSERT_LESS(X, Y)                                                     \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) < (y)))                                                         \
    {                                                                         \
      FATAL(                                                                  \
        "Assertion '" STRINGIFY(X < Y) "' failed: '", x, "' >= '", y, "'");   \
    }                                                                         \
  }
#define ASSERT_LESS_OR_EQUAL(X, Y)                                            \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) <= (y)))                                                        \
    {                                                                         \
      FATAL(                                                                  \
        "Assertion '" STRINGIFY(X <= Y) "' failed: '", x, "' > '", y, "'");   \
    }                                                                         \
  }
#define ASSERTF(X, str)                                                       \
  {                                                                           \
    const auto &x = (X);                                                      \
    if (!(x))                                                                 \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(X) "' failed: ", TO_U8STRING(str));       \
    }                                                                         \
  }
#define ASSERTF_EQUAL(X, Y, str)                                              \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) == (y)))                                                        \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(X == Y) "' failed: '",                    \
            x,                                                                \
            "' != '",                                                         \
            y,                                                                \
            "': ",                                                            \
            TO_U8STRING(str));                                                \
    }                                                                         \
  }
#define ASSERTF_NOT_EQUAL(X, Y, str)                                          \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) != (y)))                                                        \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(X != Y) "' failed: '",                    \
            x,                                                                \
            "' == '",                                                         \
            y,                                                                \
            "': ",                                                            \
            TO_U8STRING(str));                                                \
    }                                                                         \
  }
#define ASSERTF_GREATER(X, Y, str)                                            \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) > (y)))                                                         \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(X > Y) "' failed: '",                     \
            x,                                                                \
            "' <= '",                                                         \
            y,                                                                \
            "': ",                                                            \
            TO_U8STRING(str));                                                \
    }                                                                         \
  }
#define ASSERTF_GREATER_OR_EQUAL(X, Y, str)                                   \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) >= (y)))                                                        \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(X >= Y) "' failed: '",                    \
            x,                                                                \
            "' < '",                                                          \
            y,                                                                \
            "': ",                                                            \
            TO_U8STRING(str));                                                \
    }                                                                         \
  }
#define ASSERTF_LESS(X, Y, str)                                               \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) < (y)))                                                         \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(X < Y) "' failed: '",                     \
            x,                                                                \
            "' >= '",                                                         \
            y,                                                                \
            "': ",                                                            \
            TO_U8STRING(str));                                                \
    }                                                                         \
  }
#define ASSERTF_LESS_OR_EQUAL(X, Y, str)                                      \
  {                                                                           \
    const auto &x = (X);                                                      \
    const auto &y = (Y);                                                      \
    if (!((x) <= (y)))                                                        \
    {                                                                         \
      FATAL("Assertion '" STRINGIFY(X <= Y) "' failed: '",                    \
            x,                                                                \
            "' > '",                                                          \
            y,                                                                \
            "': ",                                                            \
            TO_U8STRING(str));                                                \
    }                                                                         \
  }

#include "lak/debug.inl"
