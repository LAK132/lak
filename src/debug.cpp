#include "lak/debug.hpp"

#include "lak/os.hpp"
#include "lak/strconv.hpp"
#include "lak/string_ostream.hpp"

#include <fstream>

void lak::terminate_handler() { ABORT(); }

void lak::debugger_t::std_out(const lak::u8string &line_info,
                              const lak::astring &str)
{
	std::lock_guard lock{mutex};
	const auto indent = scoped_indenter::u8str();
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(indent)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(line_info)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled && !live_errors_only)
	{
		std::cout << indent;
		if (line_info_enabled)
		{
			std::cout << line_info;
		}
		std::cout << str << std::flush;
	}
}

void lak::debugger_t::std_out(const lak::u8string &line_info,
                              const lak::wstring &str)
{
	std::lock_guard lock{mutex};
	const auto indent = scoped_indenter::u8str();
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(indent)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(line_info)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled && !live_errors_only)
	{
		std::wcout << indent;
		if (line_info_enabled)
		{
			std::wcout << line_info;
		}
		std::wcout << str << std::flush;
	}
}

void lak::debugger_t::std_out(const lak::u8string &line_info,
                              const lak::u8string &str)
{
	std::lock_guard lock{mutex};
	const auto indent = scoped_indenter::u8str();
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(indent)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(line_info)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled && !live_errors_only)
	{
		std::wcout << indent;
		if (line_info_enabled)
		{
			std::wcout << line_info;
		}
		std::wcout << str << std::flush;
	}
}

void lak::debugger_t::std_err(const lak::u8string &line_info,
                              const lak::astring &str)
{
	std::lock_guard lock{mutex};
	const auto indent = scoped_indenter::u8str();
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(indent)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(line_info)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled)
	{
		std::cout << indent;
		if (line_info_enabled)
		{
			std::cout << line_info;
		}
		std::cout << str << std::flush;
	}
}

void lak::debugger_t::std_err(const lak::u8string &line_info,
                              const lak::wstring &str)
{
	std::lock_guard lock{mutex};
	const auto indent = scoped_indenter::u8str();
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(indent)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(line_info)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled)
	{
		std::wcout << indent;
		if (line_info_enabled)
		{
			std::wcout << line_info;
		}
		std::wcout << str << std::flush;
	}
}

void lak::debugger_t::std_err(const lak::u8string &line_info,
                              const lak::u8string &str)
{
	std::lock_guard lock{mutex};
	const auto indent = scoped_indenter::u8str();
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(indent)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(line_info)
	       << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled)
	{
		std::wcout << indent;
		if (line_info_enabled)
		{
			std::wcout << line_info;
		}
		std::wcout << str << std::flush;
	}
}

void lak::debugger_t::std_out_cont(const lak::astring &str)
{
	std::lock_guard lock{mutex};
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled && !live_errors_only)
	{
		std::cout << str << std::flush;
	}
}

void lak::debugger_t::std_out_cont(const lak::wstring &str)
{
	std::lock_guard lock{mutex};
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled && !live_errors_only)
	{
		std::wcout << str << std::flush;
	}
}

void lak::debugger_t::std_out_cont(const lak::u8string &str)
{
	std::lock_guard lock{mutex};
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled && !live_errors_only)
	{
		std::wcout << lak::to_wstring(str) << std::flush;
	}
}

void lak::debugger_t::std_err_cont(const lak::astring &str)
{
	std::lock_guard lock{mutex};
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled)
	{
		std::cout << str << std::flush;
	}
}

void lak::debugger_t::std_err_cont(const lak::wstring &str)
{
	std::lock_guard lock{mutex};
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled)
	{
		std::wcout << str << std::flush;
	}
}

void lak::debugger_t::std_err_cont(const lak::u8string &str)
{
	std::lock_guard lock{mutex};
	stream << lak::strconv<LAK_DEBUG_STREAM_CHAR>(str);
	if (live_output_enabled)
	{
		std::wcout << str << std::flush;
	}
}

void lak::debugger_t::clear()
{
	std::lock_guard lock{mutex};
	stream.clear();
}

lak::string<LAK_DEBUG_STREAM_CHAR> lak::debugger_t::str()
{
	std::lock_guard lock{mutex};
	return stream.str();
}

void lak::debugger_t::abort()
{
	std::cerr << "Something went wrong!\n" << std::flush;
#ifndef NDEBUG
	DEBUG_BREAK();
#endif
#ifndef LAK_NO_FILESYSTEM
	if (!crash_path.empty())
		std::cerr
		  << "Saving crash log to '" << lak::debugger.save()
		  << "'.\nPlease forward the crash log onto the developer so they can "
		     "attempt to fix the issues that caused this crash.\n";
#endif
	PAUSE();
	std::abort();
}

#ifndef LAK_NO_FILESYSTEM
std::filesystem::path lak::debugger_t::save()
{
	std::lock_guard lock{mutex};
	return save(crash_path);
}

std::filesystem::path lak::debugger_t::save(const std::filesystem::path &path)
{
	std::lock_guard lock{mutex};
	if (!path.string().empty())
	{
		std::ofstream file(path, std::ios::out | std::ios::trunc);
		if (file.is_open()) file << str();
	}

	std::error_code ec;
	if (auto absolute = std::filesystem::absolute(path, ec); ec)
		return path;
	else
		return absolute;
}
#endif

lak::scoped_indenter::scoped_indenter(const lak::u8string &name)
{
	lak::debugger.std_out(u8"", name + u8"\n");
	lak::debugger.std_out(u8"", u8"{\n");
	++lak::debug_indent;
}

lak::scoped_indenter::~scoped_indenter()
{
	--lak::debug_indent;
	lak::debugger.std_out(u8"", u8"}\n");
}

lak::astring lak::scoped_indenter::str()
{
	lak::astring s;
	for (size_t i = lak::debug_indent; i-- > 0;)
		s += ((lak::debug_indent - i) & 1U) ? "| " : ": ";
	return s;
}

lak::wstring lak::scoped_indenter::wstr()
{
	lak::wstring s;
	for (size_t i = lak::debug_indent; i-- > 0;)
		s += ((lak::debug_indent - i) & 1U) ? L"| " : L": ";
	return s;
}

lak::u8string lak::scoped_indenter::u8str()
{
	static const auto bar = u8"| ";
	static const auto dot = u8": ";
	lak::u8string s;
	for (size_t i = lak::debug_indent; i-- > 0;)
		s += ((lak::debug_indent - i) & 1U) ? bar : dot;
	return s;
}

lak::debugger_t lak::debugger;
