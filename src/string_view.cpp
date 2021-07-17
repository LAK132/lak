#include "lak/string_view.hpp"

/* --- string_length --- */

size_t lak::string_length(const char *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}

size_t lak::string_length(const wchar_t *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}

size_t lak::string_length(const char8_t *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}

size_t lak::string_length(const char16_t *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}

size_t lak::string_length(const char32_t *str)
{
  size_t length = 0;
  while (*(str++) != 0) ++length;
  return length;
}
