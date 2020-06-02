#include <tuple>

namespace lak
{
  template<typename CHAR>
  tokeniser<CHAR> &tokeniser<CHAR>::operator++() noexcept
  {
    _current = peek();
    _next    = {};
    return *this;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::peek() const noexcept
  {
    if (_next.size() > 0) return _next;

    _next = _data.subspan(_current.end() - _data.begin());

    std::vector<char32_t> buffer;
    buffer.reserve(_longest_operator);
    const CHAR *buffer_begin = _next.begin();

    auto match = [&, this]() -> lak::span<const CHAR> {
      ASSERT(buffer.size());

      for (const auto &op : _operators)
      {
        if (buffer.size() < op.size()) continue;

        // Get the first subspan of buffer that is equal in length to op.
        if (auto buf = lak::span(buffer).first(op.size());
            buf.size() > 0 && buf.size() == op.size())
        {
          // Check if this subspan is lexically equal to op.
          bool match = true;
          for (size_t i = 0; match && i < buf.size(); ++i)
            if (buf[i] != op[i]) match = false;

          if (match)
          {
            if (buffer_begin <= _next.begin())
            {
              // We matched an operator.
              return {buffer_begin, buf.size()};
            }
            else
            {
              // We hit an operator part way through a token. Return the token
              // up to the operator.
              return {_next.begin(),
                      static_cast<size_t>(buffer_begin - _next.begin())};
            }
          }
        }
      }

      // No matches.
      return {};
    };

    auto match_operator = [&, this](char32_t c) -> lak::span<const CHAR> {
      if (_longest_operator < 1) return {};

      if (buffer.size() < _longest_operator)
      {
        buffer.push_back(c);

        if (buffer.size() == _longest_operator)
          if (auto m = match(); m.size() > 0) return m;
      }
      else
      {
        ASSERT(buffer.size() == _longest_operator);

        for (size_t i = 1; i < buffer.size(); ++i) buffer[i - 1] = buffer[i];

        buffer.back() = c;

        ++buffer_begin;

        if (auto m = match(); m.size() > 0) return m;
      }

      // No matches.
      return {};
    };

    // Scan until we find the first non-whitespace character.
    for (const auto &[c, len] : codepoint_range(_next))
    {
      if (!is_whitespace(c)) break;

      if (auto m = match_operator(c); m.size() > 0)
      {
        _next = m;
        return _next;
      }

      _next = _next.subspan(len);
    }

    size_t count = 0;

    // Continue scanning until we run out of non-whitespace character.
    for (const auto &[c, len] : codepoint_range(_n_currentext))
    {
      if (is_whitespace(c)) break;

      if (auto m = match_operator(c); m.size() > 0)
      {
        _next = m;
        return _next;
      }

      count += len;
    }

    _next = _next.first(count);

    // Flush out the buffer.
    while (buffer.size() > 0)
    {
      if (auto m = match(); m.size() > 0)
      {
        _next = m;
        return _next;
      }

      buffer.erase(buffer.begin());
    }

    return _next;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::until(char32_t codepoint) noexcept
  {
    _current = _data.subspan(_current.end() - _data.begin());
    _next    = {};

    size_t count = 0;

    for (const auto &[c, len] : codepoint_range(_current))
    {
      if (c == codepoint) break;
      count += len;
    }

    _current = _current.first(count);

    return _current;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::until(span<const char32_t> str) noexcept
  {
    ASSERT(str.size() > 0);

    _current = _data.subspan(_current.end() - _data.begin());
    _next    = {};

    size_t count           = 0;
    size_t pre_match_count = 0;
    size_t match_len       = 0;

    for (const auto &[c, len] : codepoint_range(_current))
    {
      if (c == str[match_len])
      {
        ++match_len;
      }
      else
      {
        pre_match_count = count;
        match_len       = 0;
      }

      ASSERT(match_len <= str.size());

      if (match_len == str.size()) break;

      count += len;
    }

    _current = _current.first(pre_match_count);

    return _current;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::skip(char32_t codepoint) noexcept
  {
    _current = _data.subspan(_current.end() - _data.begin());
    _next    = {};

    size_t count = 0;

    for (const auto &[c, len] : codepoint_range(_current))
    {
      count += len;
      if (c == codepoint) break;
    }

    _current = _current.first(count);

    return _current;
  }

  template<typename CHAR>
  span<const CHAR> tokeniser<CHAR>::skip(span<const char32_t> str) noexcept
  {
    ASSERT(str.size() > 0);

    _current = _data.subspan(_current.end() - _data.begin());
    _next    = {};

    size_t count     = 0;
    size_t match_len = 0;

    for (const auto &[c, len] : codepoint_range(_current))
    {
      count += len;

      if (c == str[match_len])
        ++match_len;
      else
        match_len = 0;

      ASSERT(match_len <= str.size());

      if (match_len == str.size()) break;
    }

    _current = _current.first(count);

    return _current;
  }
}