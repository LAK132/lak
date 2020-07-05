#include <tuple>

namespace lak
{
  template<typename CHAR>
  tokeniser<CHAR> &tokeniser<CHAR>::operator++() noexcept
  {
    peek();
    internal_pump();
    return *this;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::peek() const noexcept
  {
    if (_next.size() > 0 || _next.begin() == _data.end()) return _next;

    // _next is currently empty, move its end to the end of _data.
    internal_start_next();

    std::vector<char32_t> buffer;
    buffer.reserve(_longest_operator);
    const CHAR *buffer_begin = _next.begin();
    // Set by match_operator.
    token_position buffer_position = _next_position;

    auto match = [&, this]() -> bool {
      ASSERT(buffer.size());

      for (const auto &op : _operators)
      {
        if (buffer.size() < op.size()) continue;

        // Get the first subspan of buffer that is equal in length to op.
        if (auto buf = lak::span(buffer).first(op.size());
            buf.size() > 0 && buf.size() == op.size())
        {
          // Check if this subspan is lexically equal to op.
          bool match          = true;
          buffer_position.end = buffer_position.begin;
          for (size_t i = 0; match && i < buf.size(); ++i)
          {
            if (buf[i] != op[i]) match = false;

            if (buf[i] == U'\n')
            {
              ++buffer_position.end.line;
              buffer_position.end.column = 1;
            }
            else
            {
              ++buffer_position.end.column;
            }
          }

          if (match)
          {
            if (buffer_begin <= _next.begin())
            {
              // We matched an operator.
              _next          = {buffer_begin, buf.size()};
              _next_position = buffer_position;
              return true;
            }
            else
            {
              // We hit an operator part way through a token. Return the token
              // up to the operator.
              _next = {_next.begin(),
                       static_cast<size_t>(buffer_begin - _next.begin())};

              _next_position = buffer_position;
              return true;
            }
          }
        }
      }

      // No matches.
      return false;
    };

    auto match_operator = [&, this](char32_t c, uint8_t len) -> bool {
      if (_longest_operator < 1) return false;

      if (buffer.size() < _longest_operator)
      {
        buffer.push_back(c);

        if (buffer.size() == _longest_operator)
          if (match()) return true;
      }
      else
      {
        ASSERT(buffer.size() == _longest_operator);

        for (size_t i = 1; i < buffer.size(); ++i) buffer[i - 1] = buffer[i];

        buffer.back() = c;

        ++buffer_begin;

        if (c == U'\n')
        {
          ++buffer_position.begin.line;
          buffer_position.begin.column = 1;
        }
        else
        {
          ++buffer_position.begin.column;
        }

        if (match()) return true;
      }

      // No matches.
      return false;
    };

    // Scan until we find the first non-whitespace character.
    for (const auto &[c, len] : codepoint_range(_next))
    {
      if (!is_whitespace(c)) break;

      if (match_operator(c, len)) return _next;

      if (c == U'\n')
      {
        ++_next_position.begin.line;
        _next_position.begin.column = 1;
      }
      else
      {
        ++_next_position.begin.column;
      }

      _next = _next.subspan(len);
    }

    _next_position.end = _next_position.begin;

    size_t count = 0;

    // Continue scanning until we run out of non-whitespace character.
    for (const auto &[c, len] : codepoint_range(_next))
    {
      if (is_whitespace(c)) break;

      if (match_operator(c, len)) return _next;

      if (c == U'\n')
      {
        ++_next_position.end.line;
        _next_position.end.column = 1;
      }
      else
      {
        ++_next_position.end.column;
      }

      count += len;
    }

    _next = _next.first(count);

    // Flush out the buffer.
    while (buffer.size() > 0)
    {
      if (match()) return _next;

      buffer.erase(buffer.begin());
    }

    return _next;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::until(char32_t codepoint) noexcept
  {
    internal_start_next();

    size_t count = 0;

    for (const auto &[c, len] : codepoint_range(_next))
    {
      if (c == codepoint) break;

      count += len;

      if (c == U'\n')
      {
        ++_next_position.end.line;
        _next_position.end.column = 1;
      }
      else
      {
        ++_next_position.end.column;
      }
    }

    _next = _next.first(count);

    internal_pump();

    return _current;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::until(span<const char32_t> str) noexcept
  {
    ASSERT(str.size() > 0);

    internal_start_next();

    size_t count                = 0;
    size_t pre_match_count      = 0;
    size_t match_len            = 0;
    codepoint_position end_line = _next_position.end;

    for (const auto &[c, len] : codepoint_range(_next))
    {
      if (c == str[match_len])
      {
        ++match_len;
      }
      else
      {
        pre_match_count    = count;
        match_len          = 0;
        _next_position.end = end_line;
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

    _next = _next.first(pre_match_count);

    internal_pump();

    return _current;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::skip(char32_t codepoint) noexcept
  {
    internal_start_next();

    size_t count = 0;

    for (const auto &[c, len] : codepoint_range(_next))
    {
      count += len;

      if (c == U'\n')
      {
        ++_next_position.end.line;
        _next_position.end.column = 1;
      }
      else
      {
        ++_next_position.end.column;
      }

      if (c == codepoint) break;
    }

    _next = _next.first(count);

    internal_pump();

    return _current;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::skip(span<const char32_t> str) noexcept
  {
    ASSERT(str.size() > 0);

    internal_start_next();

    size_t count     = 0;
    size_t match_len = 0;

    for (const auto &[c, len] : codepoint_range(_next))
    {
      count += len;

      if (c == U'\n')
      {
        ++_next_position.end.line;
        _next_position.end.column = 1;
      }
      else
      {
        ++_next_position.end.column;
      }

      if (c == str[match_len])
        ++match_len;
      else
        match_len = 0;

      ASSERT(match_len <= str.size());

      if (match_len == str.size()) break;
    }

    _next = _next.first(count);

    internal_pump();

    return _current;
  }
}