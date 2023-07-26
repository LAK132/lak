#ifndef LAK_STRING_VIEW_HPP
#	define LAK_STRING_VIEW_HPP

#	include "lak/string.hpp"
#	include "lak/compare.hpp"

#	define LAK_SPAN_FORWARD_ONLY
#	include "lak/span.hpp"

namespace lak
{
	/* --- string_length --- */

	// Length of the null terminated string not including the null terminator.
	size_t string_length(const char *str);
	size_t string_length(const wchar_t *str);
	size_t string_length(const char8_t *str);
	size_t string_length(const char16_t *str);
	size_t string_length(const char32_t *str);

	/* --- string_view --- */

	template<typename CHAR>
	struct string_view
	{
	private:
		lak::span<const CHAR> _value;

	public:
		constexpr string_view()                               = default;
		constexpr string_view(const string_view &)            = default;
		constexpr string_view &operator=(const string_view &) = default;

		template<size_t N>
		constexpr string_view(const CHAR (&str)[N]);

		explicit constexpr string_view(const CHAR *str, size_t sz);

		explicit constexpr string_view(const CHAR *begin, const CHAR *end);

		explicit constexpr string_view(lak::span<const CHAR> str);

		string_view(const lak::string<CHAR> &str);

		constexpr static string_view from_c_str(const CHAR *str);

		constexpr const CHAR &operator[](size_t index) const
		{
			return _value[index];
		}

		constexpr const CHAR *data() const { return _value.data(); }
		constexpr size_t size() const { return _value.size(); }
		constexpr bool empty() const { return _value.empty(); }

		constexpr const CHAR *begin() const { return _value.begin(); }
		constexpr const CHAR *end() const { return _value.end(); }

		constexpr const CHAR *cbegin() const { return _value.cbegin(); }
		constexpr const CHAR *cend() const { return _value.cend(); }

		lak::string<CHAR> to_string() const;
		operator lak::string<CHAR>() const;

		constexpr lak::span<const CHAR> to_span() const;
		constexpr operator lak::span<const CHAR>() const;

		constexpr string_view substr(size_t offset,
		                             size_t count = lak::dynamic_extent) const;
		constexpr string_view first(size_t count) const;
		constexpr string_view last(size_t count) const;

		constexpr bool operator==(const string_view &rhs) const;
	};

	template<typename CHAR, size_t N>
	string_view(const CHAR (&)[N]) -> string_view<CHAR>;
	template<typename CHAR, size_t N>
	string_view(lak::span<CHAR, N>) -> string_view<lak::remove_const_t<CHAR>>;
	template<typename CHAR>
	string_view(const lak::string<CHAR> &) -> string_view<CHAR>;
	template<typename CHAR>
	string_view(const CHAR *, size_t) -> string_view<CHAR>;

	template<typename CHAR>
	span(lak::string_view<CHAR>) -> span<const CHAR>;

	template<typename CHAR>
	struct is_string<lak::string_view<CHAR>>
	{
		static constexpr bool value = true;
	};

	using astring_view   = string_view<char>;
	using wstring_view   = string_view<wchar_t>;
	using u8string_view  = string_view<char8_t>;
	using u16string_view = string_view<char16_t>;
	using u32string_view = string_view<char32_t>;

	template<typename CHAR>
	size_t compare(lak::string_view<CHAR> a, lak::string_view<CHAR> b);

	template<typename CHAR>
	lak::strong_ordering lexical_compare(lak::string_view<CHAR> a,
	                                     lak::string_view<CHAR> b);
}

#endif

#ifdef LAK_STRING_VIEW_FORWARD_ONLY
#	undef LAK_STRING_VIEW_FORWARD_ONLY
#else
#	ifndef LAK_STRING_VIEW_HPP_IMPL
#		define LAK_STRING_VIEW_HPP_IMPL

#		include "lak/string_view.inl"

#	endif
#endif
