#ifndef LAK_TRACE_HPP
#define LAK_TRACE_HPP

#include "lak/debug.hpp"
#include "lak/result.hpp"
#include "lak/string_literals.hpp"
#include "lak/unicode.hpp"

#include <ostream>
#include <version>
#ifdef __cpp_lib_source_location
#	include <source_location>
#endif

namespace lak
{
#ifdef __cpp_lib_source_location
	using source_location = std::source_location
#else
	struct source_location
	{
		static source_location current() { return {}; }
		constexpr const char *file_name() const { return {}; }
		constexpr const char *function_name() const { return {}; }
		constexpr uint_least32_t line() const { return 0U; }
		constexpr uint_least32_t column() const { return 0U; }
	};
#endif

	  struct trace
	{
		lak::source_location location;
		lak::u8string message;

		inline trace(lak::u8string m        = {},
		             lak::source_location l = lak::source_location::current())
		: location(l), message(lak::move(m))
		{
		}

		trace(trace &&)      = default;
		trace(const trace &) = default;

		trace &operator=(trace &&)      = default;
		trace &operator=(const trace &) = default;

		inline lak::u8string line_string() const
		{
			return TO_U8STRING(location.file_name()
			                   << ":" << std::dec << location.line() << ":"
			                   << location.column());
		}

		inline lak::u8string to_string() const { return TO_U8STRING(*this); }

		friend inline std::ostream &operator<<(std::ostream &strm,
		                                       const lak::trace &tr)
		{
			strm << tr.location.file_name() << ":" << std::dec << tr.location.line()
			     << ":" << tr.location.column() << " in "
			     << tr.location.function_name();
			if (!tr.message.empty()) strm << ":\n\t" << tr.message;
			return strm;
		}
	};

	struct stack_trace
	{
		lak::u8string message;
		std::vector<lak::trace> traces;

		inline stack_trace &add(lak::trace trace) &
		{
			traces.emplace_back(lak::move(trace));
			return *this;
		}

		inline stack_trace &&add(lak::trace trace) &&
		{
			traces.emplace_back(lak::move(trace));
			return lak::move(*this);
		}

		inline stack_trace &set_message(lak::u8string msg) &
		{
			message = msg;
			return *this;
		}

		inline stack_trace &&set_message(lak::u8string msg) &&
		{
			message = msg;
			return lak::move(*this);
		}

		inline auto begin() const { return traces.rbegin(); }
		inline auto end() const { return traces.rend(); }

		inline lak::u8string to_string() const { return TO_U8STRING(*this); }

		friend inline std::ostream &operator<<(std::ostream &strm,
		                                       const lak::stack_trace &tr)
		{
			strm << tr.message;
			for (const auto &trace : tr) strm << "\nat " << trace;
			return strm;
		}
	};

	inline auto to_stack_trace(
	  lak::u8string message  = {},
	  lak::source_location l = lak::source_location::current())
	{
		return [message = lak::move(message),
		        l       = lak::move(l)]<typename T>(T &&err) -> lak::stack_trace
		{
			static_assert(!lak::is_same_v<lak::remove_cvref_t<T>, lak::stack_trace>,
			              "Use lak::add_trace to append traces to the stack trace");
			if constexpr (lak::is_same_v<lak::remove_cvref_t<T>, lak::monostate>)
				return lak::stack_trace{.message = message}.add(lak::trace({}, l));
			else
				return lak::stack_trace{.message = message}.add(
				  lak::trace(lak::streamify(err), l));
		};
	}

	template<typename T = lak::monostate>
	using trace_result = lak::result<T, lak::stack_trace>;

	template<typename T, typename E>
	inline lak::trace_result<T> to_stack_trace(
	  const lak::result<T, E> &result,
	  lak::u8string message  = {},
	  lak::source_location l = lak::source_location::current())
	{
		return result.map_err(lak::to_stack_trace(lak::move(message), l));
	}

	template<typename T, typename E>
	inline lak::trace_result<T> to_stack_trace(
	  lak::result<T, E> &&result,
	  lak::u8string message  = {},
	  lak::source_location l = lak::source_location::current())
	{
		return lak::move(result).map_err(
		  lak::to_stack_trace(lak::move(message), l));
	}

	inline auto add_trace(
	  lak::u8string message  = {},
	  lak::source_location l = lak::source_location::current())
	{
		return [message = lak::move(message),
		        l       = lak::move(l)](lak::stack_trace trace) -> lak::stack_trace
		{ return trace.add(lak::trace(message, l)); };
	}
}

#define RES_MAP_TO_TRACE(...)                                                 \
	map_err(lak::to_stack_trace(lak::streamify(__VA_ARGS__)))

#define RES_ADD_TRACE(...) map_err(lak::add_trace(lak::streamify(__VA_ARGS__)))

#define RES_TRY_TRACE(...)                                                    \
	do                                                                          \
	{                                                                           \
		if_let_err (lak::stack_trace err, __VA_ARGS__)                            \
			return lak::err_t<lak::stack_trace>{lak::move(err).add(                 \
			  lak::trace(__VA_OPT__(u8"" STRINGIFY(__VA_ARGS__))))};                \
	} while (false)

#define RES_TRY_TRACE_FLUENT(...)                                             \
	auto UNIQUIFY(RESULT_){__VA_ARGS__};                                        \
	if (UNIQUIFY(RESULT_).is_err())                                             \
		return lak::err_t<lak::stack_trace>{                                      \
		  lak::move(UNIQUIFY(RESULT_))                                            \
		    .unsafe_unwrap_err()                                                  \
		    .add(lak::trace(__VA_OPT__(u8"" STRINGIFY(__VA_ARGS__))))};           \
	lak::move(UNIQUIFY(RESULT_)).unsafe_unwrap()

#define RES_TRY_TRACE_ASSIGN(ASSIGN, ...)                                     \
	auto UNIQUIFY(RESULT_){__VA_ARGS__};                                        \
	if (UNIQUIFY(RESULT_).is_err())                                             \
		return lak::err_t<lak::stack_trace>{                                      \
		  lak::move(UNIQUIFY(RESULT_))                                            \
		    .unsafe_unwrap_err()                                                  \
		    .add(lak::trace(__VA_OPT__(u8"" STRINGIFY(__VA_ARGS__))))};           \
	ASSIGN lak::move(UNIQUIFY(RESULT_)).unsafe_unwrap()

#define RES_TRY_TO_TRACE(...)                                                 \
	do                                                                          \
	{                                                                           \
		if_let_err (const auto &err, __VA_ARGS__)                                 \
			return lak::err_t<lak::stack_trace>{lak::to_stack_trace(                \
			  u8"" __VA_OPT__(STRINGIFY(__VA_ARGS__) ": ") " unwrap failed")(err)}; \
	} while (false)

#define RES_TRY_TO_TRACE_FLUENT(...)                                          \
	auto UNIQUIFY(RESULT_){__VA_ARGS__};                                        \
	if (UNIQUIFY(RESULT_).is_err())                                             \
		return lak::err_t<lak::stack_trace>{lak::to_stack_trace(                  \
		  u8"" __VA_OPT__(STRINGIFY(__VA_ARGS__) ": ") " unwrap failed")(         \
		  UNIQUIFY(RESULT_).unsafe_unwrap_err())};                                \
	lak::move(UNIQUIFY(RESULT_)).unsafe_unwrap()

#define RES_TRY_TO_TRACE_ASSIGN(ASSIGN, ...)                                  \
	auto UNIQUIFY(RESULT_){__VA_ARGS__};                                        \
	if (UNIQUIFY(RESULT_).is_err())                                             \
		return lak::err_t<lak::stack_trace>{lak::to_stack_trace(                  \
		  u8"" __VA_OPT__(STRINGIFY(__VA_ARGS__) ": ") "unwrap failed")(          \
		  UNIQUIFY(RESULT_).unsafe_unwrap_err())};                                \
	ASSIGN lak::move(UNIQUIFY(RESULT_)).unsafe_unwrap()

#ifndef NOLOG
#	define TRACE_EXPECT(trace, ...)                                            \
		expect(lak::streamify(LAK_BRIGHT_RED LAK_BOLD                             \
		                      "FATAL " LAK_SGR_RESET LAK_BRIGHT_BLACK "(",        \
		                      trace.line_string(),                                \
		                      ")" LAK_SGR_RESET ": " __VA_OPT__(, ) __VA_ARGS__))
#	define TRACE_EXPECT_ERR(trace, ...)                                        \
		expect_err(lak::streamify(                                                \
		  LAK_BRIGHT_RED LAK_BOLD "FATAL " LAK_SGR_RESET LAK_BRIGHT_BLACK "(",    \
		  trace.line_string(),                                                    \
		  ")" LAK_SGR_RESET ": " __VA_OPT__(, ) __VA_ARGS__))
#	define TRACE_UNWRAP(trace)     TRACE_EXPECT(trace, "unwrap failed")
#	define TRACE_UNWRAP_ERR(trace) TRACE_EXPECT_ERR(trace, "unwrap_err failed")
#else
#	define TRACE_EXPECT(trace, ...)     expect(lak::streamify(__VA_ARGS__))
#	define TRACE_EXPECT_ERR(trace, ...) expect_err(lak::streamify(__VA_ARGS__))
#	define TRACE_UNWRAP(trace)          unwrap()
#	define TRACE_UNWRAP_ERR(trace)      unwrap_err()
#endif

#endif
