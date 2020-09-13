#include "lak/tokeniser.hpp"

void lak::codepoint_position::operator+=(char32_t c)
{
  if (c == U'\n')
  {
    ++line;
    column = 1;
  }
  else
  {
    ++column;
  }
}

bool lak::codepoint_position::operator==(
  const lak::codepoint_position &other) const
{
  return line == other.line && column == other.column;
}

bool lak::codepoint_position::operator!=(
  const lak::codepoint_position &other) const
{
  return !operator==(other);
}

std::ostream &operator<<(std::ostream &strm,
                         const lak::codepoint_position &pos)
{
  return strm << std::dec << "{line: " << pos.line
              << ", column: " << pos.column << "}";
}

bool lak::token_position::operator==(const lak::token_position &other) const
{
  return begin == other.begin && end == other.end;
}

bool lak::token_position::operator!=(const lak::token_position &other) const
{
  return !operator==(other);
}

std::ostream &operator<<(std::ostream &strm, const lak::token_position &pos)
{
  return strm << std::dec << "{begin: " << pos.begin << ", end: " << pos.end
              << "}";
}

template struct lak::tokeniser<char>;
template struct lak::tokeniser<wchar_t>;
template struct lak::tokeniser<char8_t>;
template struct lak::tokeniser<char16_t>;
template struct lak::tokeniser<char32_t>;
