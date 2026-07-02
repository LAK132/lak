#ifndef LAK_SYSTEM_COBALT_LOG_TARGET_HPP
#define LAK_SYSTEM_COBALT_LOG_TARGET_HPP

#include "lak/debug.hpp"
#include "lak/unique_ptr.hpp"

#include <Cobalt/Logging/Logging.pkg>

namespace lak
{
	namespace cobalt
	{
		struct log_target : ::cobalt::logging::ILogTarget
		{
			using unique_ptr =
			  std::unique_ptr<log_target, ::cobalt::logging::ILogTarget::Deleter>;

			lak::debugger_t internal_debugger;
			lak::debugger_t *debugger;

			inline log_target() { debugger = &internal_debugger; }

			inline void set_external(lak::debugger_t *external_debugger)
			{
				debugger = external_debugger;
			}

			static inline unique_ptr create()
			{
				return unique_ptr(new log_target());
			}

			inline virtual void Delete() override { delete this; }

			virtual void LogMessage(const char *scope,
			                        size_t scope_length,
			                        ::cobalt::logging::ILogger::Severity severity,
			                        const char *message,
			                        size_t message_length) override;
		};

		extern lak::unique_ref<::cobalt::logging::LogManager> log_manager;
	}
}

#endif
