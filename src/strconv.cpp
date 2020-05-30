#include "lak/strconv.hpp"

namespace lak
{
  template struct codepoint_iterator<char>;
  template struct codepoint_iterator<wchar_t>;
  template struct codepoint_iterator<char8_t>;
  template struct codepoint_iterator<char16_t>;
  template struct codepoint_iterator<char32_t>;

  template struct codepoint_range<char>;
  template struct codepoint_range<wchar_t>;
  template struct codepoint_range<char8_t>;
  template struct codepoint_range<char16_t>;
  template struct codepoint_range<char32_t>;
}
