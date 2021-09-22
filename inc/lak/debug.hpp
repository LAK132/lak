#ifndef LAK_DEBUG_HPP
#	define LAK_DEBUG_HPP
#endif

#undef TO_STRING
#define TO_STRING(x)                                                          \
	[&]                                                                         \
	{                                                                           \
		std::stringstream _debug_stream;                                          \
		_debug_stream << x;                                                       \
		return _debug_stream.str();                                               \
	}()
#undef TO_U8STRING
#define TO_U8STRING(x)                                                        \
	[&]                                                                         \
	{                                                                           \
		std::stringstream _debug_stream;                                          \
		_debug_stream << x;                                                       \
		return lak::as_u8string(_debug_stream.str()).to_string();                 \
	}()
#undef TO_WSTRING
#define TO_WSTRING(x)                                                         \
	[&]                                                                         \
	{                                                                           \
		std::wstringstream _debug_stream;                                         \
		_debug_stream << x;                                                       \
		return _debug_stream.str();                                               \
	}()
#undef WTO_U8STRING
#define WTO_U8STRING(x) [&] { return lak::to_u8string(TO_WSTRING(x)); }()

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

#undef DEBUG_STRINGINFY_EXPR
#if defined(LAK_NO_DEBUG_COLOURS)
#	define DEBUG_STRINGIFY_EXPR(...) STRINGIFY(__VA_ARGS__)
#else
#	define DEBUG_STRINGIFY_EXPR(...)                                           \
		LAK_BLUE STRINGIFY(__VA_ARGS__) LAK_SGR_RESET
#endif

#undef DEBUG_LINE_FILE
#undef DEBUG_DEBUG_LINE_FILE
#undef DEBUG_CHECKPOINT_LINE_FILE
#undef CHECKPOINT
#undef SCOPED_CHECKPOINT
#undef FUNCTION_CHECKPOINT
#undef DEBUG
#if defined(NOLOG)
#	define CHECKPOINT()
#	define SCOPED_CHECKPOINT(...)
#	define FUNCTION_CHECKPOINT(...)
#	define DEBUG(...)
#else
#	if defined(LAK_NO_DEBUG_COLOURS)
#		define DEBUG_LINE_FILE "(" LINE_TRACE_STR ")"
#	elif defined(LAK_OS_WINDOWS)
#		define DEBUG_LINE_FILE                                                   \
			LAK_BRIGHT_BLACK "(" LINE_TRACE_STR ")" LAK_SGR_RESET
#	else
#		define DEBUG_LINE_FILE LAK_FAINT "(" LINE_TRACE_STR ")" LAK_SGR_RESET
#	endif
#	define DEBUG_DEBUG_LINE_FILE      "DEBUG " DEBUG_LINE_FILE ": "
#	define DEBUG_CHECKPOINT_LINE_FILE "CHECKPOINT " DEBUG_LINE_FILE
#	define CHECKPOINT()                                                        \
		lak::debugger.std_out(TO_U8STRING(DEBUG_CHECKPOINT_LINE_FILE),            \
		                      lak::to_u8string("\n"));
#	define SCOPED_CHECKPOINT(...)                                              \
		lak::scoped_indenter UNIQUIFY(SCOPED_INDENTOR_)(                          \
		  lak::streamify(__VA_ARGS__, " " DEBUG_LINE_FILE));
#	define FUNCTION_CHECKPOINT(...) SCOPED_CHECKPOINT("" __VA_ARGS__, __func__)
#	define DEBUG(...)                                                          \
		lak::debugger.std_out(TO_U8STRING(DEBUG_DEBUG_LINE_FILE),                 \
		                      lak::streamify(__VA_ARGS__, "\n"));
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
		                      lak::streamify(__VA_ARGS__, "\n"));                 \
		ABORT();                                                                  \
	}
#define NOISY_ABORT()                                                         \
	{                                                                           \
		DEBUG_BREAK();                                                            \
		std::cerr << lak::as_astring(lak::debugger.stream.str()) << "\n";         \
		ABORT();                                                                  \
	}

#undef DEBUG_WARNING_LINE_FILE
#undef DEBUG_ERROR_LINE_FILE
#undef DEBUG_FATAL_LINE_FILE
#undef WARNING
#undef ERROR
#undef FATAL
#if defined(NOLOG)
#	define WARNING(...)
#	define ERROR(...)
#	define FATAL(...) ABORT()
#else
#	if defined(LAK_NO_DEBUG_COLOURS)
#		define DEBUG_WARNING_LINE_FILE "WARNING " DEBUG_LINE_FILE ": "
#		define DEBUG_ERROR_LINE_FILE   "ERROR " DEBUG_LINE_FILE ": "
#		define DEBUG_FATAL_LINE_FILE   "FATAL " DEBUG_LINE_FILE ": "
#	else
#		define DEBUG_WARNING_LINE_FILE                                           \
			LAK_YELLOW LAK_BOLD "WARNING " LAK_SGR_RESET DEBUG_LINE_FILE ": "
#		define DEBUG_ERROR_LINE_FILE                                             \
			LAK_BRIGHT_RED LAK_BOLD "ERROR " LAK_SGR_RESET DEBUG_LINE_FILE ": "
#		define DEBUG_FATAL_LINE_FILE                                             \
			LAK_BRIGHT_RED LAK_BOLD "FATAL " LAK_SGR_RESET DEBUG_LINE_FILE ": "
#	endif
#	define WARNING(...)                                                        \
		lak::debugger.std_err(TO_U8STRING(DEBUG_WARNING_LINE_FILE),               \
		                      lak::streamify(__VA_ARGS__, "\n"));
#	define ERROR(...)                                                          \
		lak::debugger.std_err(TO_U8STRING(DEBUG_ERROR_LINE_FILE),                 \
		                      lak::streamify(__VA_ARGS__, "\n"));
#	define FATAL(...) ABORTF(TO_U8STRING(DEBUG_FATAL_LINE_FILE), __VA_ARGS__)
#endif

#undef ASSERT
#undef ASSERT_NYI
#undef ASSERT_UNREACHABLE
#undef ASSERT_EQUAL
#undef ASSERT_ARRAY_EQUAL
#undef ASSERT_NOT_EQUAL
#undef ASSERT_ARRAY_NOT_EQUAL
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
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(x) "' failed");                \
		}                                                                         \
	}
#define ASSERT_NYI()                                                          \
	{                                                                           \
		FATAL("Behaviour not yet implemented");                                   \
	}
#define ASSERT_UNREACHABLE()                                                  \
	{                                                                           \
		FATAL("Unreachable code reached");                                        \
	}
#define ASSERT_EQUAL(X, Y)                                                    \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) == UNIQUIFY(y)))                                        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X == Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' != '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	}
#define ASSERT_ARRAY_EQUAL(X, Y)                                              \
	{                                                                           \
		const auto &UNIQUIFY(x)  = (X);                                           \
		const auto &UNIQUIFY(y)  = (Y);                                           \
		const size_t UNIQUIFY(c) = UNIQUIFY(x).size();                            \
		if (UNIQUIFY(c) != UNIQUIFY(y).size())                                    \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X.size() ==                    \
			                                         Y.size()) "' failed: '",       \
			      UNIQUIFY(c),                                                      \
			      "' != '",                                                         \
			      UNIQUIFY(y).size(),                                               \
			      "'");                                                             \
		}                                                                         \
		for (size_t UNIQUIFY(i) = 0; UNIQUIFY(i) < UNIQUIFY(c); ++UNIQUIFY(i))    \
		{                                                                         \
			const auto &UNIQUIFY(x_i) = UNIQUIFY(x)[UNIQUIFY(i)];                   \
			const auto &UNIQUIFY(y_i) = UNIQUIFY(y)[UNIQUIFY(i)];                   \
			if (!(UNIQUIFY(x_i) == UNIQUIFY(y_i)))                                  \
			{                                                                       \
				FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X) "[",                      \
				      UNIQUIFY(i),                                                    \
				      "] == " DEBUG_STRINGIFY_EXPR(Y) "[",                            \
				      UNIQUIFY(i),                                                    \
				      "]' failed: '",                                                 \
				      UNIQUIFY(x_i),                                                  \
				      "' != '",                                                       \
				      UNIQUIFY(y_i),                                                  \
				      "'");                                                           \
			}                                                                       \
		}                                                                         \
	}
#define ASSERT_NOT_EQUAL(X, Y)                                                \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) != UNIQUIFY(y)))                                        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X != Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' == '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	}
#define ASSERT_ARRAY_NOT_EQUAL(X, Y)                                          \
	{                                                                           \
		const auto &UNIQUIFY(x)  = (X);                                           \
		const auto &UNIQUIFY(y)  = (Y);                                           \
		const size_t UNIQUIFY(c) = UNIQUIFY(x).size();                            \
		if (UNIQUIFY(c) == UNIQUIFY(y).size())                                    \
		{                                                                         \
			for (size_t UNIQUIFY(i) = 0; UNIQUIFY(i) < UNIQUIFY(c); ++UNIQUIFY(i))  \
			{                                                                       \
				const auto &UNIQUIFY(x_i) = UNIQUIFY(x)[UNIQUIFY(i)];                 \
				const auto &UNIQUIFY(y_i) = UNIQUIFY(y)[UNIQUIFY(i)];                 \
				if (!(UNIQUIFY(x_i) != UNIQUIFY(y_i)))                                \
				{                                                                     \
					FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X) "[",                    \
					      UNIQUIFY(i),                                                  \
					      "] != " DEBUG_STRINGIFY_EXPR(Y) "[",                          \
					      UNIQUIFY(i),                                                  \
					      "]' failed: '",                                               \
					      UNIQUIFY(x_i),                                                \
					      "' == '",                                                     \
					      UNIQUIFY(y_i),                                                \
					      "'");                                                         \
				}                                                                     \
			}                                                                       \
		}                                                                         \
	}
#define ASSERT_GREATER(X, Y)                                                  \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) > UNIQUIFY(y)))                                         \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X > Y) "' failed: '",          \
			      UNIQUIFY(x),                                                      \
			      "' <= '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	}
#define ASSERT_GREATER_OR_EQUAL(X, Y)                                         \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) >= UNIQUIFY(y)))                                        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X >= Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' < '",                                                          \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	}
#define ASSERT_LESS(X, Y)                                                     \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) < UNIQUIFY(y)))                                         \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X < Y) "' failed: '",          \
			      UNIQUIFY(x),                                                      \
			      "' >= '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	}
#define ASSERT_LESS_OR_EQUAL(X, Y)                                            \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) <= UNIQUIFY(y)))                                        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X <= Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' > '",                                                          \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	}
#define ASSERTF(X, str)                                                       \
	{                                                                           \
		if (!(X))                                                                 \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X) "' failed: ",               \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	}
#define ASSERTF_EQUAL(X, Y, str)                                              \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) == UNIQUIFY(y)))                                        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X == Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' != '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	}
#define ASSERTF_NOT_EQUAL(X, Y, str)                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) != UNIQUIFY(y)))                                        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X != Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' == '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	}
#define ASSERTF_GREATER(X, Y, str)                                            \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) > UNIQUIFY(y)))                                         \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X > Y) "' failed: '",          \
			      UNIQUIFY(x),                                                      \
			      "' <= '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	}
#define ASSERTF_GREATER_OR_EQUAL(X, Y, str)                                   \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) >= UNIQUIFY(y)))                                        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X >= Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' < '",                                                          \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	}
#define ASSERTF_LESS(X, Y, str)                                               \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) < UNIQUIFY(y)))                                         \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X < Y) "' failed: '",          \
			      UNIQUIFY(x),                                                      \
			      "' >= '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	}
#define ASSERTF_LESS_OR_EQUAL(X, Y, str)                                      \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!(UNIQUIFY(x) <= UNIQUIFY(y)))                                        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X <= Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' > '",                                                          \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	}

#include "lak/debug.inl"
