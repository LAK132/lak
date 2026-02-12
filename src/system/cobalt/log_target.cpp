#include "lak/system/cobalt/log_target.hpp"

#include "lak/span.hpp"
#include "lak/string_view.hpp"

#include <version>
#ifdef __cpp_lib_stacktrace
#	if __cpp_lib_stacktrace
#		include <stacktrace>
#	endif
#endif

void lak::cobalt::log_target::LogMessage(
  const char *scope,
  size_t scope_length,
  ::cobalt::logging::ILogger::Severity severity,
  const char *message,
  size_t message_length)
{
	switch (severity)
	{
		case ::cobalt::logging::ILogger::Severity::Critical:
			debugger->std_err(
			  u8"" LAK_FATAL_MESSAGE(
			    +lak::as_u8string(lak::astring_view(scope, scope_length))
			       .to_string() +
			    u8""),
			  lak::as_u8string(lak::astring_view(message, message_length))
			      .to_string() +
			    u8"\n");
			debugger->abort();
			break;

		case ::cobalt::logging::ILogger::Severity::Error:
			debugger->std_err(
			  u8"" LAK_ERROR_MESSAGE(
			    +lak::as_u8string(lak::astring_view(scope, scope_length))
			       .to_string() +
			    u8""),
			  lak::as_u8string(lak::astring_view(message, message_length))
			      .to_string() +
			    u8"\n");
#ifdef __cpp_lib_stacktrace
#	if __cpp_lib_stacktrace
			debugger->std_err_cont(std::to_string(std::stacktrace::current()) +
			                       "\n");
#	endif
#endif
			break;

		case ::cobalt::logging::ILogger::Severity::Warning:
			debugger->std_err(
			  u8"" LAK_WARNING_MESSAGE(
			    +lak::as_u8string(lak::astring_view(scope, scope_length))
			       .to_string() +
			    u8""),
			  lak::as_u8string(lak::astring_view(message, message_length))
			      .to_string() +
			    u8"\n");
			break;

		case ::cobalt::logging::ILogger::Severity::Debug:
			debugger->std_out(
			  u8"" LAK_DEBUG_MESSAGE(
			    +lak::as_u8string(lak::astring_view(scope, scope_length))
			       .to_string() +
			    u8""),
			  lak::as_u8string(lak::astring_view(message, message_length))
			      .to_string() +
			    u8"\n");
			break;

		case ::cobalt::logging::ILogger::Severity::Info:
			debugger->std_out(
			  u8"INFO " LAK_TRACE_MODIFIER(
			    +lak::as_u8string(lak::astring_view(scope, scope_length))
			       .to_string() +) u8": ",
			  lak::as_u8string(lak::astring_view(message, message_length))
			      .to_string() +
			    u8"\n");
			break;

		case ::cobalt::logging::ILogger::Severity::Trace:
			debugger->std_out(
			  u8"TRACE " LAK_TRACE_MODIFIER(
			    +lak::as_u8string(lak::astring_view(scope, scope_length))
			       .to_string() +) u8": ",
			  lak::as_u8string(lak::astring_view(message, message_length))
			      .to_string() +
			    u8"\n");
			break;

		default: ASSERT_UNREACHABLE();
	}
}

cobalt::logging::LogManager lak::cobalt::log_manager =
  []() -> ::cobalt::logging::LogManager
{
	::cobalt::logging::LogManager result;
	auto log_target = lak::cobalt::log_target::create();
	log_target->set_external(&lak::debugger);
	result.AddLogTarget(lak::move(log_target));
	return result;
}();
