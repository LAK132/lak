#ifndef LAK_CONST_STRING
#define LAK_CONST_STRING

#include "lak/array.hpp"
#include "lak/string.hpp"
#include "lak/string_view.hpp"

namespace lak
{
#define LAK_BASIC_CONST_STRING(CHAR, PREFIX)                                  \
  template<size_t N>                                                          \
  struct PREFIX##const_string                                                 \
  {                                                                           \
    CHAR _value[N];                                                           \
    inline constexpr PREFIX##const_string(const CHAR (&str)[N + 1])           \
    {                                                                         \
      for (size_t i = 0; i < N; ++i) _value[i] = str[i];                      \
    }                                                                         \
    inline constexpr CHAR &operator[](size_t index) { return _value[index]; } \
    inline constexpr const CHAR &operator[](size_t index) const               \
    {                                                                         \
      return _value[index];                                                   \
    }                                                                         \
    inline constexpr CHAR *begin() { return _value; }                         \
    inline constexpr CHAR *end() { return _value + N; }                       \
                                                                              \
    inline constexpr const CHAR *begin() const { return _value; }             \
    inline constexpr const CHAR *end() const { return _value + N; }           \
                                                                              \
    inline constexpr const CHAR *cbegin() const { return _value; }            \
    inline constexpr const CHAR *cend() const { return _value + N; }          \
                                                                              \
    inline constexpr CHAR *data() { return _value; }                          \
    inline constexpr const CHAR *data() const { return _value; }              \
                                                                              \
    inline constexpr size_t size() const { return N; }                        \
                                                                              \
    operator std::basic_string<CHAR>() const                                  \
    {                                                                         \
      return std::basic_string<CHAR>(begin(), end());                         \
    }                                                                         \
  };                                                                          \
                                                                              \
  template<size_t N>                                                          \
  PREFIX##const_string(const CHAR(&)[N])->PREFIX##const_string<N - 1>;

  LAK_BASIC_CONST_STRING(char, a)
  LAK_BASIC_CONST_STRING(wchar_t, w)
  LAK_BASIC_CONST_STRING(char8_t, u8)
  LAK_BASIC_CONST_STRING(char16_t, u16)
  LAK_BASIC_CONST_STRING(char32_t, u32)
}

#endif
