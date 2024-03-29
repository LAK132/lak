#include "lak/span.hpp"
#include "lak/strcast.hpp"
#include "lak/strconv.hpp"
#include "lak/string_ostream.hpp"
#include "lak/string_view.hpp"
#include "lak/type_traits.hpp"

#include "lak/unicode.hpp"

#include <ios>
#include <ostream>
#include <sstream>
#include <system_error>

template<typename ARG, typename... ARGS>
lak::u8string lak::spaced_streamify(const lak::u8string &space,
                                    const ARG &arg,
                                    const ARGS &...args)
{
	return (lak::streamify(arg) + ... + (lak::streamify(space, args)));
}

template<typename T, size_t S>
lak::u8string lak::spaced_streamify(const lak::u8string &space,
                                    lak::span<T, S> args)
{
	if (args.empty()) return {};
	lak::u8string result = lak::streamify(args[0]);
	for (const auto &e : args.subspan(1)) result += lak::streamify(space, e);
	return result;
}

namespace
{
	template<lak::concepts::streamable T>
	void streamable_streamer(std::ostream &strm, const T &arg)
	{
		strm << arg;
	}

	template<typename T>
	void streamer(std::ostream &strm, const T &arg)
	{
		strm << std::hex << std::noshowbase << std::uppercase << std::boolalpha;

		if constexpr (lak::is_same_v<T, bool>)
		{
			strm << (arg ? "true" : "false");
		}
		else if constexpr (std::is_enum_v<T> && lak::is_streamable_v<T>)
		{
			::streamable_streamer(strm, arg);
		}
		else if constexpr ((std::is_integral_v<T> && !std::is_same_v<T, char>) ||
		                   std::is_enum_v<T>)
		{
			if constexpr (std::is_unsigned_v<T>)
				strm << "0x" << static_cast<uintmax_t>(arg);
			else if (static_cast<intmax_t>(arg) < intmax_t(0))
				strm << "-0x" << static_cast<uintmax_t>(-static_cast<intmax_t>(arg));
			else
				strm << "0x" << static_cast<uintmax_t>(arg);
		}
		else if constexpr (std::is_null_pointer_v<T>)
		{
			strm << "nullptr";
		}
		else if constexpr (lak::is_string_v<lak::remove_cvref_t<T>>)
		{
			using char_type = lak::remove_cvref_t<decltype(arg[0])>;
			if constexpr (lak::is_pointer_v<T>)
				strm << lak::string_view<char_type>::from_c_str(arg);
			else
				strm << lak::string_view<char_type>(arg);
		}
		else if constexpr (lak::is_same_v<std::error_code, lak::remove_cvref_t<T>>)
		{
			strm << arg << " (" << arg.message() << ")";
		}
		else
		{
			::streamable_streamer(strm, arg);
		}
	};
}

template<typename... ARGS>
lak::u8string lak::streamify(const ARGS &...args)
{
	std::stringstream _strm;
	auto &strm = static_cast<std::ostream &>(_strm);
	(::streamer(strm, args), ...);
	return lak::as_u8string(_strm.str()).to_string();
}
