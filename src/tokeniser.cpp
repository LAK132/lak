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

bool lak::token_position::operator==(const lak::token_position &other) const
{
	return begin == other.begin && end == other.end;
}

bool lak::token_position::operator!=(const lak::token_position &other) const
{
	return !operator==(other);
}

LAK_TEMPLATE_FOREACH_CHAR(lak::token)
LAK_TEMPLATE_FOREACH_CHAR(lak::token_buffer)
LAK_TEMPLATE_FOREACH_CHAR(lak::tokeniser)
