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
	do                                                                          \
	{                                                                           \
		std::cerr << "Press enter to continue...\n";                              \
		getchar();                                                                \
	} while (false)
#define PAUSEF(str)                                                           \
	do                                                                          \
	{                                                                           \
		std::cerr << str << "\nPress enter to continue...\n";                     \
		getchar();                                                                \
	} while (false)

#undef DEBUG_STRINGIFY_EXPR
#if defined(LAK_NO_DEBUG_COLOURS)
#	define DEBUG_STRINGIFY_EXPR(...) STRINGIFY(__VA_ARGS__)
#else
#	define DEBUG_STRINGIFY_EXPR(...)                                           \
		LAK_BLUE STRINGIFY(__VA_ARGS__) LAK_SGR_RESET
#endif

#undef DEBUG_EVAL_EXPR
#if defined(LAK_NO_DEBUG_COLOURS)
#	define DEBUG_EVAL_EXPR(...) STRINGIFY(__VA_ARGS__) " (", __VA_ARGS__, ")"
#else
#	define DEBUG_EVAL_EXPR(...)                                                \
		LAK_BLUE STRINGIFY(__VA_ARGS__) " (", __VA_ARGS__, ")" LAK_SGR_RESET
#endif

#undef DEBUG_EXPR_EX
#define DEBUG_EXPR_EX(...)                                                    \
	DEBUG_STRINGIFY_EXPR(__VA_ARGS__)                                           \
	": ", lak::spaced_streamify(u8", ", __VA_ARGS__)

#undef LAK_TRACE_MODIFIER
#if defined(LAK_NO_DEBUG_COLOURS)
#	define LAK_TRACE_MODIFIER(STR) "(" STR ")"
#elif defined(LAK_OS_WINDOWS)
#	define LAK_TRACE_MODIFIER(STR) LAK_BRIGHT_BLACK "(" STR ")" LAK_SGR_RESET
#else
#	define LAK_TRACE_MODIFIER(STR) LAK_FAINT "(" STR ")" LAK_SGR_RESET
#endif

#undef LAK_DEBUG_MESSAGE
#undef LAK_WARNING_MESSAGE
#undef LAK_ERROR_MESSAGE
#undef LAK_FATAL_MESSAGE
#define LAK_DEBUG_MESSAGE(LF) "DEBUG " LAK_TRACE_MODIFIER(LF) ": "
#if defined(LAK_NO_DEBUG_COLOURS)
#	define LAK_WARNING_MESSAGE(LF) "WARNING " LAK_TRACE_MODIFIER(LF) ": "
#	define LAK_ERROR_MESSAGE(LF)   "ERROR " LAK_TRACE_MODIFIER(LF) ": "
#	define LAK_FATAL_MESSAGE(LF)   "FATAL " LAK_TRACE_MODIFIER(LF) ": "
#else
#	define LAK_WARNING_MESSAGE(LF)                                             \
		LAK_YELLOW LAK_BOLD "WARNING " LAK_SGR_RESET LAK_TRACE_MODIFIER(LF) ": "
#	define LAK_ERROR_MESSAGE(LF)                                               \
		LAK_BRIGHT_RED LAK_BOLD "ERROR " LAK_SGR_RESET LAK_TRACE_MODIFIER(LF) ": "
#	define LAK_FATAL_MESSAGE(LF)                                               \
		LAK_BRIGHT_RED LAK_BOLD "FATAL " LAK_SGR_RESET LAK_TRACE_MODIFIER(LF) ": "
#endif

#undef DEBUG_DEBUG_LINE_FILE
#undef DEBUG_WARNING_LINE_FILE
#undef DEBUG_ERROR_LINE_FILE
#undef DEBUG_FATAL_LINE_FILE
#undef DEBUG_CHECKPOINT_LINE_FILE
#define DEBUG_DEBUG_LINE_FILE   LAK_DEBUG_MESSAGE(LINE_TRACE_STR)
#define DEBUG_WARNING_LINE_FILE LAK_WARNING_MESSAGE(LINE_TRACE_STR)
#define DEBUG_ERROR_LINE_FILE   LAK_ERROR_MESSAGE(LINE_TRACE_STR)
#define DEBUG_FATAL_LINE_FILE   LAK_FATAL_MESSAGE(LINE_TRACE_STR)
#define DEBUG_CHECKPOINT_LINE_FILE                                            \
	"CHECKPOINT " LAK_TRACE_MODIFIER(LINE_TRACE_STR)

#undef DEBUG
#undef DEBUG_EXPR
#if defined(NOLOG)
#	define DEBUG(...)
#	define DEBUG_EXPR(...)
#else
#	define DEBUG(...)                                                          \
		lak::debugger.std_out(u8"" DEBUG_DEBUG_LINE_FILE,                         \
		                      lak::streamify(__VA_ARGS__, "\n"));
#	define DEBUG_EXPR(...)                                                     \
		lak::debugger.std_out(u8"" DEBUG_DEBUG_LINE_FILE,                         \
		                      lak::streamify(DEBUG_EXPR_EX(__VA_ARGS__), "\n"));
#endif

#undef CHECKPOINT
#undef SCOPED_CHECKPOINT_S
#undef FUNCTION_CHECKPOINT_S
#undef MEMBER_FUNCTION_CHECKPOINT_S
#undef SCOPED_CHECKPOINT
#undef FUNCTION_CHECKPOINT
#undef MEMBER_FUNCTION_CHECKPOINT
#if defined(NOLOG)
#	define CHECKPOINT()
#	define SCOPED_CHECKPOINT_S(...)
#	define FUNCTION_CHECKPOINT_S(...)
#	define MEMBER_FUNCTION_CHECKPOINT_S(...)
#	define SCOPED_CHECKPOINT(...)
#	define FUNCTION_CHECKPOINT(...)
#	define MEMBER_FUNCTION_CHECKPOINT(...)
#else
#	define CHECKPOINT()                                                        \
		lak::debugger.std_out(u8"" DEBUG_CHECKPOINT_LINE_FILE, u8"\n");

#	define SCOPED_CHECKPOINT_S(...)                                            \
		lak::debugger_t::scoped_indenter UNIQUIFY(SCOPED_INDENTOR_)(              \
		  __VA_OPT__(lak::u8string(__VA_ARGS__) +                                 \
		             u8" " +) u8"" LAK_TRACE_MODIFIER(LINE_TRACE_STR));
#	define FUNCTION_CHECKPOINT_S(...)                                          \
		SCOPED_CHECKPOINT_S(                                                      \
		  lak::to_u8string(__func__) +                                            \
		  u8"(" __VA_OPT__(+lak::u8string(__VA_ARGS__) +) u8")")
#	define MEMBER_FUNCTION_CHECKPOINT_S(...)                                   \
		SCOPED_CHECKPOINT_S(                                                      \
		  lak::to_u8string(TYPE_NAME(*this)) + u8"::" +                           \
		  lak::to_u8string(__func__) +                                            \
		  u8"(" __VA_OPT__(+lak::u8string(__VA_ARGS__) +) u8")")

#	define SCOPED_CHECKPOINT(...)                                              \
		SCOPED_CHECKPOINT_S(__VA_OPT__(lak::streamify(__VA_ARGS__)));
#	define FUNCTION_CHECKPOINT(...)                                            \
		FUNCTION_CHECKPOINT_S(__VA_OPT__(lak::streamify(__VA_ARGS__)));
#	define MEMBER_FUNCTION_CHECKPOINT(...)                                     \
		MEMBER_FUNCTION_CHECKPOINT_S(__VA_OPT__(lak::streamify(__VA_ARGS__)));
#endif

#undef ABORT
#undef ABORTF
#undef ABORTF_S
#undef NOISY_ABORT
#define ABORT()                                                               \
	do                                                                          \
	{                                                                           \
		if (std::is_constant_evaluated())                                         \
			std::abort();                                                           \
		else                                                                      \
			lak::debugger.abort();                                                  \
	} while (false)
#define ABORTF(...)                                                           \
	do                                                                          \
	{                                                                           \
		if (!std::is_constant_evaluated())                                        \
			lak::debugger.std_err(u8"", lak::streamify(__VA_ARGS__, "\n"));         \
		ABORT();                                                                  \
	} while (false)
#define ABORTF_S(...)                                                         \
	do                                                                          \
	{                                                                           \
		if (!std::is_constant_evaluated())                                        \
			lak::debugger.std_err(u8"",                                             \
			                      __VA_OPT__(lak::u8string(__VA_ARGS__) +) u8"\n"); \
		ABORT();                                                                  \
	} while (false)
#define NOISY_ABORT()                                                         \
	do                                                                          \
	{                                                                           \
		if (!std::is_constant_evaluated())                                        \
		{                                                                         \
			DEBUG_BREAK();                                                          \
			std::cerr << lak::as_astring(lak::debugger.stream.str()) << "\n";       \
		}                                                                         \
		ABORT();                                                                  \
	} while (false)

#undef WARNING_S
#undef ERROR_S
#undef FATAL_S
#undef WARNING
#undef ERROR
#undef FATAL
#if defined(NOLOG)
#	define WARNING_S(...)
#	define ERROR_S(...)
#	define FATAL_S(...) ABORT()
#	define WARNING(...)
#	define ERROR(...)
#	define FATAL(...) ABORT()
#else
#	define WARNING_S(...)                                                      \
		do                                                                        \
		{                                                                         \
			if (!std::is_constant_evaluated())                                      \
				lak::debugger.std_err(                                                \
				  u8"" DEBUG_WARNING_LINE_FILE,                                       \
				  __VA_OPT__(lak::u8string(__VA_ARGS__) +) u8"\n");                   \
		} while (false)
#	define ERROR_S(...)                                                        \
		do                                                                        \
		{                                                                         \
			if (!std::is_constant_evaluated())                                      \
				lak::debugger.std_err(                                                \
				  u8"" DEBUG_ERROR_LINE_FILE,                                         \
				  __VA_OPT__(lak::u8string(__VA_ARGS__) +) u8"\n");                   \
		} while (false)
#	define FATAL_S(...)                                                        \
		ABORTF_S(                                                                 \
		  u8"" DEBUG_FATAL_LINE_FILE __VA_OPT__(+lak::u8string(__VA_ARGS__)))

#	define WARNING(...) WARNING_S(__VA_OPT__(lak::streamify(__VA_ARGS__)))
#	define ERROR(...)   ERROR_S(__VA_OPT__(lak::streamify(__VA_ARGS__)))
#	define FATAL(...)   FATAL_S(__VA_OPT__(lak::streamify(__VA_ARGS__)))
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
#define ASSERT(...)                                                           \
	do                                                                          \
	{                                                                           \
		if (!(__VA_ARGS__)) [[unlikely]]                                          \
		{                                                                         \
			FATAL_S(u8"Assertion '" DEBUG_STRINGIFY_EXPR(__VA_ARGS__) "' failed");  \
		}                                                                         \
	} while (false)
#define ASSERT_NYI()                                                          \
	do                                                                          \
	{                                                                           \
		FATAL_S(u8"Behaviour not yet implemented");                               \
	} while (false)
#define ASSERT_UNREACHABLE()                                                  \
	do                                                                          \
	{                                                                           \
		FATAL_S(u8"Unreachable code reached");                                    \
	} while (false)
#define ASSERT_EQUAL(X, Y)                                                    \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::equal_to<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]            \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X == Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' != '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	} while (false)
#define ASSERT_CLOSE(X, Y, ...)                                               \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::close_to(UNIQUIFY(x), UNIQUIFY(y) __VA_OPT__(, ) __VA_ARGS__))  \
		  [[unlikely]]                                                            \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X == Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' not close to '",                                               \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	} while (false)
#define ASSERT_ARRAY_EQUAL(X, Y)                                              \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x)  = (X);                                           \
		const auto &UNIQUIFY(y)  = (Y);                                           \
		const size_t UNIQUIFY(c) = UNIQUIFY(x).size();                            \
		if (UNIQUIFY(c) != UNIQUIFY(y).size()) [[unlikely]]                       \
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
			if (!lak::equal_to<>{}(UNIQUIFY(x_i), UNIQUIFY(y_i))) [[unlikely]]      \
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
	} while (false)
#define ASSERT_NOT_EQUAL(X, Y)                                                \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::not_equal_to<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X != Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' == '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	} while (false)
#define ASSERT_NOT_CLOSE(X, Y, ...)                                           \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (lak::close_to(UNIQUIFY(x), UNIQUIFY(y) __VA_OPT__(, ) __VA_ARGS__))   \
		  [[unlikely]]                                                            \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X == Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' close to '",                                                   \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	} while (false)
#define ASSERT_ARRAY_NOT_EQUAL(X, Y)                                          \
	do                                                                          \
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
				if (!lak::not_equal_to<>{}(UNIQUIFY(x_i), UNIQUIFY(y_i)))             \
				  [[unlikely]]                                                        \
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
	} while (false)
#define ASSERT_GREATER(X, Y)                                                  \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::greater<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]             \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X > Y) "' failed: '",          \
			      UNIQUIFY(x),                                                      \
			      "' <= '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	} while (false)
#define ASSERT_GREATER_OR_EQUAL(X, Y)                                         \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::greater_equal<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]       \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X >= Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' < '",                                                          \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	} while (false)
#define ASSERT_LESS(X, Y)                                                     \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::less<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]                \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X < Y) "' failed: '",          \
			      UNIQUIFY(x),                                                      \
			      "' >= '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	} while (false)
#define ASSERT_LESS_OR_EQUAL(X, Y)                                            \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::less_equal<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]          \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X <= Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' > '",                                                          \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	} while (false)
#define ASSERTF(X, str)                                                       \
	do                                                                          \
	{                                                                           \
		if (!(X)) [[unlikely]]                                                    \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X) "' failed: ",               \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	} while (false)
#define ASSERTF_EQUAL(X, Y, str)                                              \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::equal_to<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]            \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X == Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' != '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	} while (false)
#define ASSERTF_NOT_EQUAL(X, Y, str)                                          \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::not_equal_to<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]        \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X != Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' == '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	} while (false)
#define ASSERTF_GREATER(X, Y, str)                                            \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::greater<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]             \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X > Y) "' failed: '",          \
			      UNIQUIFY(x),                                                      \
			      "' <= '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	} while (false)
#define ASSERTF_GREATER_OR_EQUAL(X, Y, str)                                   \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::greater_equal<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]       \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X >= Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' < '",                                                          \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	} while (false)
#define ASSERTF_LESS(X, Y, str)                                               \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::less<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]                \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X < Y) "' failed: '",          \
			      UNIQUIFY(x),                                                      \
			      "' >= '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	} while (false)
#define ASSERTF_LESS_OR_EQUAL(X, Y, str)                                      \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::less_equal<>{}(UNIQUIFY(x), UNIQUIFY(y))) [[unlikely]]          \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(X <= Y) "' failed: '",         \
			      UNIQUIFY(x),                                                      \
			      "' > '",                                                          \
			      UNIQUIFY(y),                                                      \
			      "': ",                                                            \
			      TO_U8STRING(str));                                                \
		}                                                                         \
	} while (false)

#undef BOUNDS_ASSERT
#undef BOUNDS_ASSERT_EQUAL
#undef BOUNDS_ASSERT_NOT_EQUAL
#undef BOUNDS_ASSERT_LESS
#undef BOUNDS_ASSERT_LESS_OR_EQUAL
#undef BOUNDS_ASSERT_GREATER
#undef BOUNDS_ASSERT_GREATER_OR_EQUAL
#undef BOUNDS_ASSERT_UNREACHABLE
#ifndef ASSERT_NO_BOUNDS_CHECKS
#	define BOUNDS_ASSERT(...)                   ASSERT(__VA_ARGS__)
#	define BOUNDS_ASSERT_EQUAL(X, Y)            ASSERT_EQUAL(X, Y)
#	define BOUNDS_ASSERT_NOT_EQUAL(X, Y)        ASSERT_NOT_EQUAL(X, Y)
#	define BOUNDS_ASSERT_LESS(X, Y)             ASSERT_LESS(X, Y)
#	define BOUNDS_ASSERT_LESS_OR_EQUAL(X, Y)    ASSERT_LESS_OR_EQUAL(X, Y)
#	define BOUNDS_ASSERT_GREATER(X, Y)          ASSERT_GREATER(X, Y)
#	define BOUNDS_ASSERT_GREATER_OR_EQUAL(X, Y) ASSERT_GREATER_OR_EQUAL(X, Y)
#	define BOUNDS_ASSERT_UNREACHABLE(...)                                      \
		do                                                                        \
		{                                                                         \
			ASSERT_UNREACHABLE();                                                   \
			__VA_ARGS__;                                                            \
		} while (false)
#else
#	define BOUNDS_ASSERT(...)                                                  \
		do                                                                        \
		{                                                                         \
			if (std::is_constant_evaluated())                                       \
				if (!(__VA_ARGS__)) std::abort();                                     \
		} while (false)
#	define BOUNDS_ASSERT_EQUAL(X, Y)                                           \
		do                                                                        \
		{                                                                         \
			if (std::is_constant_evaluated())                                       \
			{                                                                       \
				const auto &UNIQUIFY(x) = (X);                                        \
				const auto &UNIQUIFY(y) = (Y);                                        \
				if (!lak::equal_to<>{}(UNIQUIFY(x), UNIQUIFY(y))) std::abort();       \
			}                                                                       \
		} while (false)
#	define BOUNDS_ASSERT_NOT_EQUAL(X, Y)                                       \
		do                                                                        \
		{                                                                         \
			if (std::is_constant_evaluated())                                       \
			{                                                                       \
				const auto &UNIQUIFY(x) = (X);                                        \
				const auto &UNIQUIFY(y) = (Y);                                        \
				if (!lak::not_equal_to<>{}(UNIQUIFY(x), UNIQUIFY(y))) std::abort();   \
			}                                                                       \
		} while (false)
#	define BOUNDS_ASSERT_LESS(X, Y)                                            \
		do                                                                        \
		{                                                                         \
			if (std::is_constant_evaluated())                                       \
			{                                                                       \
				const auto &UNIQUIFY(x) = (X);                                        \
				const auto &UNIQUIFY(y) = (Y);                                        \
				if (!lak::less<>{}(UNIQUIFY(x), UNIQUIFY(y))) std::abort();           \
			}                                                                       \
		} while (false)
#	define BOUNDS_ASSERT_LESS_OR_EQUAL(X, Y)                                   \
		do                                                                        \
		{                                                                         \
			if (std::is_constant_evaluated())                                       \
			{                                                                       \
				const auto &UNIQUIFY(x) = (X);                                        \
				const auto &UNIQUIFY(y) = (Y);                                        \
				if (!lak::less_equal<>{}(UNIQUIFY(x), UNIQUIFY(y))) std::abort();     \
			}                                                                       \
		} while (false)
#	define BOUNDS_ASSERT_GREATER(X, Y)                                         \
		do                                                                        \
		{                                                                         \
			if (std::is_constant_evaluated())                                       \
			{                                                                       \
				const auto &UNIQUIFY(x) = (X);                                        \
				const auto &UNIQUIFY(y) = (Y);                                        \
				if (!lak::greater<>{}(UNIQUIFY(x), UNIQUIFY(y))) std::abort();        \
			}                                                                       \
		} while (false)
#	define BOUNDS_ASSERT_GREATER_OR_EQUAL(X, Y)                                \
		do                                                                        \
		{                                                                         \
			if (std::is_constant_evaluated())                                       \
			{                                                                       \
				const auto &UNIQUIFY(x) = (X);                                        \
				const auto &UNIQUIFY(y) = (Y);                                        \
				if (!lak::greater_equal<>{}(UNIQUIFY(x), UNIQUIFY(y))) std::abort();  \
			}                                                                       \
		} while (false)
#	define BOUNDS_ASSERT_UNREACHABLE(...)                                      \
		do                                                                        \
		{                                                                         \
			if (std::is_constant_evaluated()) std::abort();                         \
			__VA_ARGS__;                                                            \
		} while (false)
#endif

#include "lak/debugger.hpp"

#ifdef LAK_DEBUG_FORWARD_ONLY
#	undef LAK_DEBUG_FORWARD_ONLY
#else
#	ifndef LAK_DEBUG_HPP_IMPL
#		define LAK_DEBUG_HPP_IMPL
#		include "lak/strcast.hpp"
#		include "lak/streamify.hpp"
#		include "lak/functional.hpp"
#	endif
#endif
