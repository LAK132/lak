#include "lak/strcast.hpp"
#include "lak/strconv.hpp"
#include "lak/string_view.hpp"

#include "lak/functional.hpp"

#ifndef LAK_DEBUG_FORWARD_ONLY
#	define LAK_DEBUG_FORWARD_ONLY
#endif
#include "lak/debug.hpp"

template<typename TO, typename FROM>
size_t lak::converted_string_length(lak::string_view<FROM> str)
{
	size_t result = 0;
	while (str.size() > 0)
	{
		auto char_len = lak::character_length(str);
		ASSERT(char_len != 0);
		result += lak::codepoint_length<TO>(lak::codepoint(str));
		str = str.substr(char_len);
	}
	return result;
}

template<typename CHAR>
uint8_t lak::character_length(const lak::string<CHAR> &str, size_t offset)
{
	return lak::character_length(lak::string_view(str).substr(offset));
}

template<typename CHAR>
char32_t lak::codepoint(const lak::string<CHAR> &str, size_t offset)
{
	return lak::codepoint(lak::string_view(str).substr(offset));
}

template<typename CHAR>
uint8_t lak::codepoint_length(char32_t code)
{
	if constexpr (std::is_same_v<CHAR, char>)
	{
		return code <= 0x7FU ? 1 : 0;
	}
	else if constexpr (std::is_same_v<CHAR, wchar_t>)
	{
		return lak::codepoint_length<wchar_unicode_t>(code);
	}
	else if constexpr (std::is_same_v<CHAR, char8_t>)
	{
		if (code <= 0x7FU)
			return 1;
		else if (code >= 0x80U && code <= 0x07FFU)
			return 2;
		else if (code >= 0x0800U && code <= 0xFFFFU)
			return 3;
		else if (code >= 0x00010000U && code <= 0x0010FFFFU)
			return 4;
		else
			return 0;
	}
	else if constexpr (std::is_same_v<CHAR, char16_t>)
	{
		if (code <= 0xD7FFU || (code >= 0xE000U && code <= 0xFFFFU))
			return 1;
		else if (code <= 0x0010FFFFU)
			return 2;
		else
			return 0;
	}
	else if constexpr (std::is_same_v<CHAR, char32_t>)
	{
		return (code <= 0xD7FFU || (code >= 0xE000U && code <= 0x0010FFFFU)) ? 1
		                                                                     : 0;
	}
}

template<typename CHAR>
void lak::append_codepoint(lak::string<CHAR> &str, char32_t code)
{
	lak::codepoint_buffer_t<CHAR> buffer;
	auto span = lak::from_codepoint(buffer, code);
	str.insert(str.end(), span.begin(), span.end());
}

template<typename CHAR>
inline constexpr const lak::pair<char32_t, uint8_t> &
lak::codepoint_iterator<CHAR>::operator*() const noexcept
{
	return _current;
}

template<typename CHAR>
inline constexpr lak::codepoint_iterator<CHAR> &
lak::codepoint_iterator<CHAR>::operator++() noexcept
{
	_current.second = lak::character_length(_data);
	_current.first  = _current.second ? codepoint(_data) : 0;
	_data           = _data.substr(_current.second);
	return *this;
}

template<typename CHAR>
inline constexpr bool lak::codepoint_iterator<CHAR>::operator==(
  sentinel) const noexcept
{
	return _current.second == 0U;
}

template<typename CHAR>
inline constexpr bool lak::codepoint_iterator<CHAR>::operator!=(
  sentinel) const noexcept
{
	return _current.second != 0U;
}

template<typename CHAR>
inline constexpr lak::codepoint_iterator<CHAR>
lak::codepoint_range<CHAR>::begin() const noexcept
{
	return {_data};
}

template<typename CHAR>
inline constexpr lak::codepoint_iterator<CHAR>::sentinel
lak::codepoint_range<CHAR>::end() const noexcept
{
	return {};
}

template<typename TO, typename FROM>
inline void lak::foreach_char(lak::string_view<FROM> str, auto &&func)
{
	if constexpr (lak::is_same_v<TO, FROM>)
		for (const FROM &c : str) func(c);
	else
		for (lak::codepoint_buffer_t<TO> buffer;
		     const auto &[c, len] : lak::codepoint_range(str))
			for (const TO &ac : lak::from_codepoint(buffer, c)) func(ac);
}

template<typename FROM>
inline void lak::foreach_achar(lak::string_view<FROM> str, auto &&func)
{
	return lak::foreach_char<char>(str, func);
}

template<typename FROM>
inline void lak::foreach_wchar(lak::string_view<FROM> str, auto &&func)
{
	return lak::foreach_char<wchar_t>(str, func);
}

template<typename FROM>
inline void lak::foreach_u8char(lak::string_view<FROM> str, auto &&func)
{
	return lak::foreach_char<char8_t>(str, func);
}

template<typename FROM>
inline void lak::foreach_u16char(lak::string_view<FROM> str, auto &&func)
{
	return lak::foreach_char<char16_t>(str, func);
}

template<typename FROM>
inline void lak::foreach_u32char(lak::string_view<FROM> str, auto &&func)
{
	return lak::foreach_char<char32_t>(str, func);
}
