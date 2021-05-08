#include "lak/unicode.hpp"

inline lak::span<const char> lak::as_astring(const lak::astring &str)
{
  return lak::span<const char>(lak::string_view(str));
}

inline lak::span<const char> lak::as_astring(const lak::u8string &str)
{
  return lak::span<const char>(lak::string_view(str));
}

inline lak::span<const char> lak::as_astring(lak::span<const char8_t> str)
{
  return lak::span<const char>(str);
}

inline lak::span<const char> lak::as_astring(const char8_t *str)
{
  return lak::span<const char>(lak::string_view(str));
}

inline lak::span<const char8_t> lak::as_u8string(const lak::astring &str)
{
  return lak::span<const char8_t>(lak::string_view(str));
}

inline lak::span<const char8_t> lak::as_u8string(const lak::u8string &str)
{
  return lak::span<const char8_t>(lak::string_view(str));
}

inline lak::span<const char8_t> lak::as_u8string(lak::span<const char> str)
{
  return lak::span<const char8_t>(str);
}

inline lak::span<const char8_t> lak::as_u8string(const char *str)
{
  return lak::span<const char8_t>(lak::string_view(str));
}
