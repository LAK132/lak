#include <tuple>

namespace lak
{
  template<typename CHAR>
  tokeniser<CHAR> &tokeniser<CHAR>::operator++()
  {
    _current = _data.subspan(_current.end() - _data.begin());

    std::vector<char32_t> buffer;
    buffer.reserve(_longest_operator);
    const CHAR *buffer_begin = _current.begin();

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
            if (buffer_begin <= _current.begin())
            {
              // We matched an operator.
              return {buffer_begin, buf.size()};
            }
            else
            {
              // We hit an operator part way through a token. Return the token
              // up to the operator.
              return {_current.begin(),
                      static_cast<size_t>(buffer_begin - _current.begin())};
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
    for (const auto &[c, len] : codepoint_range(_current))
    {
      if (!is_whitespace(c)) break;

      if (auto m = match_operator(c); m.size() > 0)
      {
        _current = m;
        return *this;
      }

      _current = _current.subspan(len);
    }

    size_t count = 0;

    // Continue scanning until we run out of non-whitespace character.
    for (const auto &[c, len] : codepoint_range(_current))
    {
      if (is_whitespace(c)) break;

      if (auto m = match_operator(c); m.size() > 0)
      {
        _current = m;
        return *this;
      }

      count += len;
    }

    _current = _current.first(count);

    // Flush out the buffer.
    while (buffer.size() > 0)
    {
      if (auto m = match(); m.size() > 0)
      {
        _current = m;
        return *this;
      }

      buffer.erase(buffer.begin());
    }

    return *this;
  }
}