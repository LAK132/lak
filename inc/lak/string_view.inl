#include "lak/span.hpp"
#include "lak/string_view.hpp"

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
lak::span<const CHAR> lak::string_view<CHAR>::to_span() const
{
  return _value;
}

template<typename CHAR>
lak::string_view<CHAR>::operator lak::span<const CHAR>() const
{
  return _value;
}

template<typename CHAR>
lak::string_view<CHAR> lak::string_view<CHAR>::substr(size_t offset,
                                                      size_t count) const
{
  return lak::string_view(_value.subspan(offset, count));
}

template<typename CHAR>
lak::string_view<CHAR> lak::string_view<CHAR>::first(size_t count) const
{
  return lak::string_view(_value.first(count));
}

template<typename CHAR>
lak::string_view<CHAR> lak::string_view<CHAR>::last(size_t count) const
{
  return lak::string_view(_value.last(count));
}

template<typename CHAR>
bool lak::string_view<CHAR>::operator==(const string_view &rhs) const
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

/* --- operator"" _view --- */

inline lak::astring_view operator"" _view(const char *str, size_t size)
{
  return lak::astring_view(str, size);
}

inline lak::wstring_view operator"" _view(const wchar_t *str, size_t size)
{
  return lak::wstring_view(str, size);
}

inline lak::u8string_view operator"" _view(const char8_t *str, size_t size)
{
  return lak::u8string_view(str, size);
}

inline lak::u16string_view operator"" _view(const char16_t *str, size_t size)
{
  return lak::u16string_view(str, size);
}

inline lak::u32string_view operator"" _view(const char32_t *str, size_t size)
{
  return lak::u32string_view(str, size);
}
