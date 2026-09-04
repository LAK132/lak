#include "lak/string_utils.hpp"

template<typename CHAR>
lak::string<CHAR> lak::join_strings(lak::string_view<CHAR> separator,
                                    lak::span<const lak::string<CHAR>> strings)
{
	if (strings.empty()) return {};

	size_t len = separator.size() * (strings.size() - 1U);
	for (const auto &str : strings) len += str.size();

	lak::string<CHAR> result;
	result.reserve(len);

	result += strings[0U];

	for (const auto &str : strings.subspan(1U))
	{
		result += separator;
		result += str;
	}

	return result;
}

template<typename CHAR>
lak::string<CHAR> lak::join_strings(
  lak::string_view<CHAR> separator,
  lak::span<const lak::string_view<CHAR>> strings)
{
	if (strings.empty()) return {};

	size_t len = separator.size() * (strings.size() - 1U);
	for (const auto &str : strings) len += str.size();

	lak::string<CHAR> result;
	result.reserve(len);

	result += strings[0U];

	for (const auto &str : strings.subspan(1U))
	{
		result += separator;
		result += str;
	}

	return result;
}
