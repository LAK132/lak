#include "lak/span.hpp"
#include "lak/string_view.hpp"

/* --- string_view --- */

template<typename CHAR>
lak::span<CHAR> lak::string_view(CHAR *str)
{
  return {str, lak::string_length(str)};
}

template<typename CHAR>
lak::span<const CHAR> lak::string_view(const lak::string<CHAR> &str)
{
  return {str.c_str(), str.length()};
}

/* --- operator"" _view --- */

static lak::span<const char> operator"" _aview(const char *str, size_t size)
{
  return lak::span<const char>(str, size);
}

static lak::string<char> operator"" _astr(const char *str, size_t size)
{
  return lak::string<char>(str, str + size);
}

static lak::span<const char8_t> operator"" _u8view(const char *str,
                                                   size_t size)
{
  return lak::span<const char8_t>(reinterpret_cast<const char8_t *>(str),
                                  size);
}

static lak::string<char8_t> operator"" _u8str(const char *str, size_t size)
{
  return lak::string<char8_t>(str, str + size);
}

#ifdef LAK_COMPILER_CPP20
static lak::span<const char8_t> operator"" _u8view(const char8_t *str,
                                                   size_t size)
{
  return lak::span<const char8_t>(str, size);
}

static lak::string<char8_t> operator"" _u8str(const char8_t *str, size_t size)
{
  return lak::string<char8_t>(str, str + size);
}
#endif

static lak::span<const char16_t> operator"" _u16view(const char16_t *str,
                                                     size_t size)
{
  return lak::span<const char16_t>(str, size);
}

static lak::string<char16_t> operator"" _u16str(const char16_t *str,
                                                size_t size)
{
  return lak::string<char16_t>(str, str + size);
}

static lak::span<const char32_t> operator"" _u32view(const char32_t *str,
                                                     size_t size)
{
  return lak::span<const char32_t>(str, size);
}

static lak::string<char32_t> operator"" _u32str(const char32_t *str,
                                                size_t size)
{
  return lak::string<char32_t>(str, str + size);
}
