// This library assumes that char only holds 7bit ASCII characters, and that
// wchar_t will hold Unicode characters. wchar_t encoding is determined based
// on its size.

#ifndef LAK_STRCAST_HPP
#define LAK_STRCAST_HPP

#include "lak/compiler.hpp"
#include "lak/span_forward.hpp"
#include "lak/string.hpp"

namespace lak
{
  // Reinterpret UTF-8 string as an ASCII string.
  force_inline lak::span<const char> as_astring(const lak::u8string &str);
  force_inline lak::span<const char> as_astring(lak::span<const char8_t> str);
  force_inline lak::span<const char> as_astring(const char8_t *str);

  // Reinterpret ASCII string as a UTF-8 string.
  force_inline lak::span<const char8_t> as_u8string(const lak::astring &str);
  force_inline lak::span<const char8_t> as_u8string(lak::span<const char> str);
  force_inline lak::span<const char8_t> as_u8string(const char *str);
}

#include "lak/span.hpp"

#include "lak/strcast.inl"

#endif
