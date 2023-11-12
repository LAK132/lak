#include "lak/span.hpp"
#include "lak/string_view.hpp"

/* --- null_string_span --- */

template<typename CHAR>
lak::span<CHAR> lak::null_string_span(CHAR *str)
{
	return lak::span<CHAR>(str, lak::string_length(str) + 1U);
}

/* --- string_view --- */

template<typename CHAR>
template<size_t N>
constexpr lak::string_view<CHAR>::string_view(const CHAR (&str)[N])
: _value(str, str + N - 1)
{
}

template<typename CHAR>
constexpr lak::string_view<CHAR>::string_view(const CHAR *str, size_t sz)
: _value(str, sz)
{
}

template<typename CHAR>
constexpr lak::string_view<CHAR>::string_view(const CHAR *begin,
                                              const CHAR *end)
: _value(begin, end)
{
}

template<typename CHAR>
constexpr lak::string_view<CHAR>::string_view(lak::span<const CHAR> str)
: _value(str)
{
}

template<typename CHAR>
lak::string_view<CHAR>::string_view(const lak::string<CHAR> &str)
: _value(str.data(), str.size())
{
}

template<typename CHAR>
constexpr lak::string_view<CHAR> lak::string_view<CHAR>::from_c_str(
  const CHAR *str)
{
	return lak::string_view<CHAR>(str, lak::string_length(str));
}

template<typename CHAR>
lak::string<CHAR> lak::string_view<CHAR>::to_string() const
{
	return lak::string<CHAR>(begin(), end());
}

template<typename CHAR>
lak::string_view<CHAR>::operator lak::string<CHAR>() const
{
	return lak::string<CHAR>(begin(), end());
}

template<typename CHAR>
constexpr lak::span<const CHAR> lak::string_view<CHAR>::to_span() const
{
	return _value;
}

template<typename CHAR>
constexpr lak::string_view<CHAR>::operator lak::span<const CHAR>() const
{
	return _value;
}

template<typename CHAR>
constexpr lak::string_view<CHAR> lak::string_view<CHAR>::substr(
  size_t offset, size_t count) const
{
	return lak::string_view(_value.subspan(offset, count));
}

template<typename CHAR>
constexpr lak::string_view<CHAR> lak::string_view<CHAR>::first(
  size_t count) const
{
	return lak::string_view(_value.first(count));
}

template<typename CHAR>
constexpr lak::string_view<CHAR> lak::string_view<CHAR>::last(
  size_t count) const
{
	return lak::string_view(_value.last(count));
}

template<typename CHAR>
constexpr bool lak::string_view<CHAR>::operator==(const string_view &rhs) const
{
	return _value == rhs._value;
}

template<typename CHAR>
size_t lak::compare(lak::string_view<CHAR> a, lak::string_view<CHAR> b)
{
	if (a.data() == b.data() && a.size() == b.size()) return a.size();
	size_t result = 0;
	while (result < a.size() && result < b.size() && a[result] == b[result])
		++result;
	return result;
}

template<typename CHAR>
lak::strong_ordering lak::lexical_compare(lak::string_view<CHAR> a,
                                          lak::string_view<CHAR> b)
{
	if (a.size() > b.size())
		return lak::strong_ordering::greater;
	else if (a.size() < b.size())
		return lak::strong_ordering::less;
	else
		for (size_t i = a.size(); i-- > 0;)
			if (a[i] > b[i])
				return lak::strong_ordering::greater;
			else if (a[i] < b[i])
				return lak::strong_ordering::less;

	return lak::strong_ordering::equal;
}
