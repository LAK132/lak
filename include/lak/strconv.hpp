// This library assumes that char only holds 7bit ASCII characters, and that
// wchar_t will hold Unicode characters. wchar_t encoding is determined based
// on its size.

#ifndef LAK_STRCONV_HPP
#define LAK_STRCONV_HPP

#include "lak/string.hpp"

#define LAK_STRING_VIEW_FORWARD_ONLY
#include "lak/string_view.hpp"

#include <tuple>

namespace lak
{
	template<typename TO, typename FROM>
	inline lak::string<TO> strconv(const lak::string<FROM> &str);
	template<typename FROM>
	inline lak::astring to_astring(const lak::string<FROM> &str);
	template<typename FROM>
	inline lak::wstring to_wstring(const lak::string<FROM> &str);
	template<typename FROM>
	inline lak::u8string to_u8string(const lak::string<FROM> &str);
	template<typename FROM>
	inline lak::u16string to_u16string(const lak::string<FROM> &str);
	template<typename FROM>
	inline lak::u32string to_u32string(const lak::string<FROM> &str);

	template<typename TO, typename FROM>
	inline lak::string<TO> strconv(lak::string_view<FROM> str);
	template<typename FROM>
	inline lak::astring to_astring(lak::string_view<FROM> str);
	template<typename FROM>
	inline lak::wstring to_wstring(lak::string_view<FROM> str);
	template<typename FROM>
	inline lak::u8string to_u8string(lak::string_view<FROM> str);
	template<typename FROM>
	inline lak::u16string to_u16string(lak::string_view<FROM> str);
	template<typename FROM>
	inline lak::u32string to_u32string(lak::string_view<FROM> str);

	template<typename FROM>
	inline lak::astring to_astring(const FROM *str);
	template<typename FROM>
	inline lak::wstring to_wstring(const FROM *str);
	template<typename FROM>
	inline lak::u8string to_u8string(const FROM *str);
	template<typename FROM>
	inline lak::u16string to_u16string(const FROM *str);
	template<typename FROM>
	inline lak::u32string to_u32string(const FROM *str);
}

#include "lak/strconv.inl"

#endif
