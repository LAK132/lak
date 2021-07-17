#ifndef LAK_STRING_VIEW_HPP
#define LAK_STRING_VIEW_HPP

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
  lak::span<CHAR> string_view(CHAR *str);
  template<typename CHAR>
  lak::span<const CHAR> string_view(const lak::string<CHAR> &str);
}

static lak::span<const char> operator"" _aview(const char *str, size_t size);

static lak::string<char> operator"" _astr(const char *str, size_t size);

static lak::span<const char8_t> operator"" _u8view(const char *str,
                                                   size_t size);

static lak::string<char8_t> operator"" _u8str(const char *str, size_t size);

#ifdef LAK_COMPILER_CPP20
static lak::span<const char8_t> operator"" _u8view(const char8_t *str,
                                                   size_t size);

static lak::string<char8_t> operator"" _u8str(const char8_t *str, size_t size);
#endif

static lak::span<const char16_t> operator"" _u16view(const char16_t *str,
                                                     size_t size);

static lak::string<char16_t> operator"" _u16str(const char16_t *str,
                                                size_t size);

static lak::span<const char32_t> operator"" _u32view(const char32_t *str,
                                                     size_t size);

static lak::string<char32_t> operator"" _u32str(const char32_t *str,
                                                size_t size);

#include "lak/string_view.inl"

#endif
