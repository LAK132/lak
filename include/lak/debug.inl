#ifndef LAK_DEBUG_INL
#	define LAK_DEBUG_INL

#	include "lak/string.hpp"

#	include <cstdlib>
#	ifndef LAK_NO_FILESYSTEM
#		include <filesystem>
#	endif
#	include <iostream>
#	include <mutex>
#	include <sstream>

namespace lak
{
	// Use std::set_terminate(lak::terminate_handler); to enable
	void terminate_handler();

	struct debugger_t
	{
#	ifndef LAK_DEBUG_STREAM_CHAR
#		define LAK_DEBUG_STREAM_CHAR char8_t
#	endif
		std::basic_stringstream<LAK_DEBUG_STREAM_CHAR> stream;

		void std_out(const lak::u8string &line_info, const lak::astring &str);
		void std_out(const lak::u8string &line_info, const lak::wstring &str);
		void std_out(const lak::u8string &line_info, const lak::u8string &str);

		void std_err(const lak::u8string &line_info, const lak::astring &str);
		void std_err(const lak::u8string &line_info, const lak::wstring &str);
		void std_err(const lak::u8string &line_info, const lak::u8string &str);

		void std_out_cont(const lak::astring &str);
		void std_out_cont(const lak::wstring &str);
		void std_out_cont(const lak::u8string &str);

		void std_err_cont(const lak::astring &str);
		void std_err_cont(const lak::wstring &str);
		void std_err_cont(const lak::u8string &str);

		void clear();

		lak::string<LAK_DEBUG_STREAM_CHAR> str();

		[[noreturn]] void abort();

#	ifndef LAK_NO_FILESYSTEM
		std::filesystem::path save();

		std::filesystem::path save(const std::filesystem::path &path);

		std::filesystem::path crash_path;
#	endif

		std::mutex mutex;
		bool live_output_enabled = true;
		bool live_errors_only    = false;
		bool line_info_enabled   = true;
	};

	struct scoped_indenter
	{
		scoped_indenter(const lak::u8string &name);

		~scoped_indenter();

		static lak::astring str();
		static lak::wstring wstr();
		static lak::u8string u8str();
	};

	[[maybe_unused]] static thread_local size_t debug_indent = 0U;

	extern debugger_t debugger;
}

#	include "lak/strcast.hpp"

#	include "lak/streamify.hpp"

#endif

#ifdef LAK_DEBUG_FORWARD_ONLY
#	undef LAK_DEBUG_FORWARD_ONLY
#else
#	ifndef LAK_DEBUG_INL_IMPL
#		define LAK_DEBUG_INL_IMPL

#		include "lak/functional.hpp"

#	endif
#endif
