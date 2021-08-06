#ifndef LAK_STRING_VIEW_FORWARD_HPP
#define LAK_STRING_VIEW_FORWARD_HPP

#include "lak/span_forward.hpp"
#include "lak/string.hpp"

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
    constexpr string_view()                    = default;
    constexpr string_view(const string_view &) = default;
    constexpr string_view &operator=(const string_view &) = default;

    template<size_t N>
    constexpr string_view(const CHAR (&str)[N]);

    explicit constexpr string_view(const CHAR *str, size_t sz);

    explicit constexpr string_view(const CHAR *begin, const CHAR *end);

    explicit constexpr string_view(lak::span<const CHAR> str);

    string_view(const lak::string<CHAR> &str);

    constexpr static string_view from_c_str(const CHAR *str);

    const CHAR &operator[](size_t index) const { return _value[index]; }

    const CHAR *data() const { return _value.data(); }
    size_t size() const { return _value.size(); }
    bool empty() const { return _value.empty(); }

    const CHAR *begin() const { return _value.begin(); }
    const CHAR *end() const { return _value.end(); }

    const CHAR *cbegin() const { return _value.cbegin(); }
    const CHAR *cend() const { return _value.cend(); }

    lak::string<CHAR> to_string() const;
    operator lak::string<CHAR>() const;

    lak::span<const CHAR> to_span() const;
    operator lak::span<const CHAR>() const;

    string_view substr(size_t offset,
                       size_t count = lak::dynamic_extent) const;
    string_view first(size_t count) const;
    string_view last(size_t count) const;

    bool operator==(const string_view &rhs) const;
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

  using astring_view   = string_view<char>;
  using wstring_view   = string_view<wchar_t>;
  using u8string_view  = string_view<char8_t>;
  using u16string_view = string_view<char16_t>;
  using u32string_view = string_view<char32_t>;

  template<typename CHAR>
  size_t compare(lak::string_view<CHAR> a, lak::string_view<CHAR> b);
}

inline lak::astring_view operator"" _view(const char *str, size_t size);
inline lak::wstring_view operator"" _view(const wchar_t *str, size_t size);
inline lak::u8string_view operator"" _view(const char8_t *str, size_t size);
inline lak::u16string_view operator"" _view(const char16_t *str, size_t size);
inline lak::u32string_view operator"" _view(const char32_t *str, size_t size);

inline lak::span<const char> operator"" _span(const char *str, size_t size);
inline lak::span<const wchar_t> operator"" _span(const wchar_t *str,
                                                 size_t size);
inline lak::span<const char8_t> operator"" _span(const char8_t *str,
                                                 size_t size);
inline lak::span<const char16_t> operator"" _span(const char16_t *str,
                                                  size_t size);
inline lak::span<const char32_t> operator"" _span(const char32_t *str,
                                                  size_t size);

#endif
