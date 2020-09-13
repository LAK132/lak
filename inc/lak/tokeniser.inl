#include "lak/defer.hpp"

#include <tuple>

template<typename CHAR>
bool lak::token<CHAR>::operator==(const token &other) const
{
  return source == other.source && position == other.position;
}

template<typename CHAR>
bool lak::token<CHAR>::operator!=(const token &other) const
{
  return !operator==(other);
}

template<typename CHAR>
std::ostream &operator<<(std::ostream &strm, const lak::token<CHAR> &token)
{
  return strm << std::dec << "{source: " << token.source
              << ", position: " << token.position << "}";
}

template<typename CHAR>
std::optional<lak::token<CHAR>> lak::token_buffer<CHAR>::match(
  lak::span<const lak::u32string> operators, const lak::token<CHAR> &token)
{
  ASSERT(_buffer.size());

  for (const auto &op : operators)
  {
    ASSERT(!op.empty());

    if (_buffer.size() < op.size()) continue;

    if (lak::span<const char32_t>(_buffer).first(op.size()) ==
        lak::string_view(op))
    {
      // :TRICKY: token.position.end may not be corrent here!

      // token.source is tracking the entire length of the current token,
      // whereas buffer_source_begin is only tracking where the next
      // operator might be starting from. This means that
      // buffer_source_begin can be between token.source.begin() and
      // token.source.end() if _longest_operator is less than
      // token.source.size(). This also means that buffer_source_begin can
      // be before token.source.begin() if _longest_operator is greater
      // than token.source.size() (this should only happen while skipping
      // whitespace at the start of the token).
      if (token.source.begin() < _buffer_token.source.begin())
      {
        // We hit the start of an operator part way through a token.
        // Return the token up to the operator.

        return lak::token<CHAR>{
          lak::span(token.source.begin(), _buffer_token.source.begin()),
          lak::token_position{token.position.begin,
                              _buffer_token.position.begin}};
      }
      else
      {
        // _buffer_token.source.begin() <= token.source.begin()

        // We matched an operator.

        lak::codepoint_position end_position = _buffer_token.position.begin;

        size_t converted_length = 0;
        for (const auto &c : op)
        {
          converted_length += lak::codepoint_length<CHAR>(c);
          end_position += c;
        }

        if (_buffer_token.source.begin() == token.source.begin())
          ASSERT_EQUAL(_buffer_token.position.begin, token.position.begin);

        return lak::token<CHAR>{
          lak::span(_buffer_token.source.begin(), converted_length),
          lak::token_position{_buffer_token.position.begin, end_position}};
      }
    }
  }

  // No matches.
  return std::nullopt;
}

template<typename CHAR>
void lak::token_buffer<CHAR>::operator++()
{
  if (_max_size == 0) return;

  ASSERT_EQUAL(_buffer_token.source.begin(),
               _buffer_token.position.begin.find(_source).begin());

  auto pre_buffer       = _buffer; // this is a full buffer copy.
  auto pre_buffer_token = _buffer_token;

  auto remaining = lak::span(_buffer_token.source.end(), _source.end());

  auto next_len            = lak::character_length(remaining);
  const char32_t next_code = lak::codepoint(remaining);

  ASSERT_GREATER_OR_EQUAL(_max_size, _buffer.size());
  if (_buffer.size() == _max_size) --*this;

  if (remaining.size() == 0) return;
  ASSERT_GREATER(next_len, 0);

  _buffer.push_back(next_code);
  _buffer_token.source = lak::span(_buffer_token.source.begin(),
                                   _buffer_token.source.end() + next_len);
  _buffer_token.position.end += next_code;

  if (pre_buffer.size() > 0)
  {
    auto u8buffer = lak::to_u8string(lak::span(_buffer));
    auto u8source = lak::to_u8string(_buffer_token.source);
    ASSERT_EQUAL(u8buffer, u8source);

    auto max_buffer = std::max(pre_buffer.size(), _buffer.size());
    auto buffcmp    = lak::compare<char32_t>(pre_buffer, _buffer);
    ASSERTF_GREATER(max_buffer,
                    buffcmp,
                    "'" << lak::to_astring(lak::span(pre_buffer)) << "' '"
                        << lak::to_astring(lak::span(_buffer)) << "'");
    ASSERT_NOT_EQUAL(pre_buffer_token.source, _buffer_token.source);
    ASSERT_NOT_EQUAL(pre_buffer_token.position, _buffer_token.position);
  }
}

template<typename CHAR>
void lak::token_buffer<CHAR>::operator--()
{
  ASSERT(_buffer.size());
  ASSERT(_buffer_token.source.size());

  ASSERT_EQUAL(_buffer_token.source.begin(),
               _buffer_token.position.begin.find(_source).begin());

  const char32_t front = _buffer.front();

  ASSERT_EQUAL(lak::codepoint_length<CHAR>(front),
               lak::character_length(_buffer_token.source));

  _buffer.erase(_buffer.begin());

  _buffer_token.source =
    _buffer_token.source.subspan(lak::codepoint_length<CHAR>(front));

  _buffer_token.position.begin += front;

  // ASSERT_EQUAL(_buffer_token.source.begin(),
  //              _buffer_token.position.begin.find(_source).begin());
}

template<typename CHAR>
void lak::tokeniser<CHAR>::internal_pump() noexcept
{
  _current             = _next;
  _next.source         = lak::span(_next.source.end(), size_t(0));
  _next.position.begin = _next.position.end;

  ASSERT_EQUAL(_next.source.begin(), _current.source.end());
  ASSERT_EQUAL(_next.position.begin, _current.position.end);
}

template<typename CHAR>
lak::tokeniser<CHAR> &lak::tokeniser<CHAR>::operator++() noexcept
{
  peek();
  internal_pump();
  return *this;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::peek() const noexcept
{
  if (_next.source.size() > 0 || _next.source.begin() == _data.end())
    return _next;

  // _next is currently empty, move its end to the end of _data.
  _next.source = lak::span(_next.source.begin(), _data.end());

  ASSERT_EQUAL(_current.source.end(), _next.source.begin());

  lak::token_buffer<CHAR> buffer(_data, _longest_operator, _next);

  // Scan until we find the first non-whitespace character.
  for (const auto &[c, len] : lak::codepoint_range(_next.source))
  {
    if (!lak::is_whitespace(c)) break;

    ASSERT_EQUAL(
      _next.source.size(),
      lak::span(buffer._buffer_token.source.begin(), buffer._source.end())
        .size());

    if (auto match = buffer.match(_operators, _next); match)
    {
      if (_next.source.begin() == match->source.begin())
      {
        ASSERT_EQUAL(_next.position.begin, match->position.begin);
      }
      else
      {
        ASSERT_NOT_EQUAL(_next.position.begin, match->position.begin);
      }
      _next = *match;
      return _next;
    }

    auto before = buffer.source();
    ++buffer;
    ASSERT_NOT_EQUAL(before, buffer.source());

    _next.position.begin += c;

    _next.source = _next.source.subspan(len);
  }

  auto whitespace_begin = _next;
  DEFER(ASSERT_EQUAL(whitespace_begin.source.begin(), _next.source.begin()));
  DEFER(ASSERT_EQUAL(whitespace_begin.position.begin, _next.position.begin));

  _next.position.end = _next.position.begin;

  size_t count = 0;

  // Continue scanning until we run out of non-whitespace characters.
  for (const auto &[c, len] : lak::codepoint_range(_next.source))
  {
    if (lak::is_whitespace(c)) break;

    ASSERT_EQUAL(_next.source.begin() + count,
                 buffer._buffer_token.source.begin());

    if (auto match = buffer.match(_operators, _next); match)
    {
      if (_next.source.begin() == match->source.begin())
      {
        ASSERT_EQUAL(_next.position.begin, match->position.begin);
      }
      else
      {
        ASSERT_NOT_EQUAL(_next.position.begin, match->position.begin);
      }
      _next = *match;
      return _next;
    }

    auto before = buffer.source();
    ++buffer;
    ASSERT_NOT_EQUAL(before, buffer.source());

    _next.position.end += c;

    count += len;
  }

  _next.source = _next.source.first(count);

  // Flush out the buffer.
  while (buffer.buffer().size() > 0)
  {
    if (auto match = buffer.match(_operators, _next); match)
    {
      ASSERT_EQUAL(match->source.begin(), _next.source.begin());
      ASSERT_GREATER(match->source.end(), _next.source.begin());
      ASSERT_GREATER(_next.source.end(), match->source.end());
      _next = *match;
      return _next;
    }

    --buffer;
  }

  return _next;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::until(char32_t codepoint) noexcept
{
  _next.source = lak::span(_next.source.begin(), _data.end());

  size_t count = 0;

  for (const auto &[c, len] : codepoint_range(_next.source))
  {
    if (c == codepoint) break;

    count += len;

    if (c == U'\n')
    {
      ++_next.position.end.line;
      _next.position.end.column = 1;
    }
    else
    {
      ++_next.position.end.column;
    }
  }

  _next.source = _next.source.first(count);

  internal_pump();

  return _current;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::until(
  lak::span<const char32_t> str) noexcept
{
  ASSERT(str.size() > 0);

  _next.source = lak::span(_next.source.begin(), _data.end());

  size_t count                = 0;
  size_t pre_match_count      = 0;
  size_t match_len            = 0;
  codepoint_position end_line = _next.position.end;

  for (const auto &[c, len] : codepoint_range(_next.source))
  {
    if (c == str[match_len])
    {
      ++match_len;
    }
    else
    {
      pre_match_count    = count;
      match_len          = 0;
      _next.position.end = end_line;
    }

    ASSERT(match_len <= str.size());

    if (match_len == str.size()) break;

    count += len;

    if (c == U'\n')
    {
      ++end_line.line;
      end_line.column = 1;
    }
    else
    {
      ++end_line.column;
    }
  }

  _next.source = _next.source.first(pre_match_count);

  internal_pump();

  return _current;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::skip(char32_t codepoint) noexcept
{
  _next.source = lak::span(_next.source.begin(), _data.end());

  size_t count = 0;

  for (const auto &[c, len] : codepoint_range(_next.source))
  {
    count += len;

    if (c == U'\n')
    {
      ++_next.position.end.line;
      _next.position.end.column = 1;
    }
    else
    {
      ++_next.position.end.column;
    }

    if (c == codepoint) break;
  }

  _next.source = _next.source.first(count);

  internal_pump();

  return _current;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::skip(
  lak::span<const char32_t> str) noexcept
{
  ASSERT(str.size() > 0);

  _next.source = lak::span(_next.source.begin(), _data.end());

  size_t count     = 0;
  size_t match_len = 0;

  for (const auto &[c, len] : codepoint_range(_next.source))
  {
    count += len;

    if (c == U'\n')
    {
      ++_next.position.end.line;
      _next.position.end.column = 1;
    }
    else
    {
      ++_next.position.end.column;
    }

    if (c == str[match_len])
      ++match_len;
    else
      match_len = 0;

    ASSERT(match_len <= str.size());

    if (match_len == str.size()) break;
  }

  _next.source = _next.source.first(count);

  internal_pump();

  return _current;
}

template<typename CHAR>
inline void lak::tokeniser<CHAR>::reset() noexcept
{
  _current.source   = _data.first(0);
  _current.position = token_position{};
  _next             = _current;
  operator++();
}
