#include "lak/tokeniser.hpp"

namespace lak
{
  template struct tokeniser<char>;
  template struct tokeniser<wchar_t>;
  template struct tokeniser<char8_t>;
  template struct tokeniser<char16_t>;
  template struct tokeniser<char32_t>;
}