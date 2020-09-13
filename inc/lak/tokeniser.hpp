#ifndef LAK_TOKENISER_HPP
#define LAK_TOKENISER_HPP

#include "lak/span.hpp"
#include "lak/string.hpp"
#include "lak/vec.hpp"

#include <vector>

namespace lak
{
  struct codepoint_position
  {
    // Measured in codepoints not source characters.
    size_t line   = 1;
    size_t column = 1;

    void operator+=(char32_t c);

    template<typename CHAR>
    lak::span<CHAR> find(lak::span<CHAR> str) const
    {
      ASSERT_GREATER(line, 0);
      ASSERT_GREATER(column, 0);

      for (size_t l = line; --l > 0;)
      {
        for (const auto &[c, len] : lak::codepoint_range(str))
        {
          str = str.subspan(len);
          if (c == U'\n') break;
        }
      }

      size_t col = 0;
      for (const auto &[c, len] : lak::codepoint_range(str))
      {
        if (++col == column) break;
        ASSERT(c != U'\n');
        str = str.subspan(len);
      }

      return str;
    }

    bool operator==(const codepoint_position &other) const;
    bool operator!=(const codepoint_position &other) const;
  };

  struct token_position
  {
    codepoint_position begin = {};
    codepoint_position end   = {};

    bool operator==(const token_position &other) const;
    bool operator!=(const token_position &other) const;
  };

  template<typename CHAR>
  struct token
  {
    lak::span<const CHAR> source;
    lak::token_position position;

    bool operator==(const token &other) const;
    bool operator!=(const token &other) const;

    template<size_t I>
    inline auto &get()
    {
      if constexpr (I == 0)
        return source;
      else if constexpr (I == 1)
        return position;
    }

    template<size_t I>
    inline const auto &get() const
    {
      if constexpr (I == 0)
        return source;
      else if constexpr (I == 1)
        return position;
    }
  };

  template<typename CHAR>
  struct token_buffer
  {
    lak::span<const CHAR> _source;
    size_t _max_size;

    lak::token<CHAR> _buffer_token;
    std::vector<char32_t> _buffer;

    inline token_buffer(lak::span<const CHAR> data,
                        size_t max_size,
                        const token<CHAR> &start)
    : _source(data),
      _max_size(
        std::min(max_size, lak::span(start.source.begin(), data.end()).size()))
    {
      ASSERT_GREATER(_max_size, 0);
      _buffer.reserve(_max_size);
      _buffer_token.source   = start.source.first(0);
      _buffer_token.position = {start.position.begin, start.position.begin};

      // Prefill up to _max_size characters into the buffer.
      while (_buffer.size() < _max_size) ++*this;
    }

    inline lak::span<const char32_t> buffer() const { return _buffer; }

    inline lak::span<const CHAR> source() const
    {
      return _buffer_token.source;
    }

    inline const lak::token_position &position() const
    {
      return _buffer_token.position;
    }

    std::optional<lak::token<CHAR>> match(
      lak::span<const lak::u32string> operators,
      const lak::token<CHAR> &token);

    void operator++();

    void operator--();
  };

  template<typename CHAR>
  struct tokeniser
  {
  private:
    span<const CHAR> _data;
    std::vector<std::u32string> _operators;
    size_t _longest_operator;

    lak::token<CHAR> _current;
    mutable lak::token<CHAR> _next;

    void internal_pump() noexcept;

  public:
    inline tokeniser(span<const CHAR> str,
                     const std::vector<std::u32string> &operators) noexcept
    : _data(str), _operators(operators), _longest_operator(0)
    {
      for (auto it = _operators.begin(); it != _operators.end();)
      {
        if (it->empty())
        {
          WARNING("Empty operator (", it - _operators.begin(), ")");
          it = _operators.erase(it);
        }
        else
          ++it;
      }

      for (const auto &op : _operators)
        if (op.size() > _longest_operator) _longest_operator = op.size();

      reset();
    }

    inline tokeniser &begin() noexcept { return *this; }

    inline nullptr_t end() const noexcept { return nullptr; }

    inline token_position position() const noexcept
    {
      return _current.position;
    }

    inline token_position peek_position() const noexcept
    {
      peek();
      return _next.position;
    }

    inline const lak::token<CHAR> &operator*() const noexcept
    {
      return _current;
    }

    tokeniser &operator++() noexcept;

    // Peek at the next token.
    lak::token<CHAR> peek() const noexcept;

    // Scan until codepoint is found (exclusive).
    lak::token<CHAR> until(char32_t codepoint) noexcept;
    // If you're tokenising something like:
    //  R"delimiter(this is a string)delimiter";
    // You can mark `R"` as an operator, then do `until(U'(')` to get
    // "delimiter".
    // This may also be useful for skipping comments:
    //  # this is a comment [newline]
    // Marking `#` as an operator will allow you do to `until(U'\n')` to scan
    // until it reaches the end of the line (or end of the file).

    // Scan until str is found (exclusive).
    lak::token<CHAR> until(span<const char32_t> str) noexcept;
    // Useful for tokenising something like multiline comments:
    //  /*
    //  this is a multiline comment
    //  */
    // Once the operator `/*` is found, you can do `until(U"*/")` to scan until
    // the end of the comment (or end of the file) is found.

    // Scan until codepoint is found (inclusive).
    lak::token<CHAR> skip(char32_t codepoint) noexcept;

    // Scan until str is found (inclusive).
    lak::token<CHAR> skip(span<const char32_t> str) noexcept;

    void reset() noexcept;

    inline bool operator==(nullptr_t) const noexcept
    {
      return _current.source.begin() >= _data.end();
    }

    inline bool operator!=(nullptr_t) const noexcept
    {
      return _current.source.begin() < _data.end();
    }

    operator bool() const noexcept { return _current.source.size() != 0; }
  };

  extern template struct tokeniser<char>;
  extern template struct tokeniser<wchar_t>;
  extern template struct tokeniser<char8_t>;
  extern template struct tokeniser<char16_t>;
  extern template struct tokeniser<char32_t>;

  using atokeniser   = tokeniser<char>;
  using wtokeniser   = tokeniser<wchar_t>;
  using u8tokeniser  = tokeniser<char8_t>;
  using u16tokeniser = tokeniser<char16_t>;
  using u32tokeniser = tokeniser<char32_t>;
}

std::ostream &operator<<(std::ostream &strm,
                         const lak::codepoint_position &pos);

std::ostream &operator<<(std::ostream &strm, const lak::token_position &pos);

template<typename CHAR>
std::ostream &operator<<(std::ostream &strm, const lak::token<CHAR> &token);

template<typename CHAR>
struct std::tuple_size<lak::token<CHAR>>
{
  static constexpr size_t value = 2;
};

template<size_t I, typename CHAR>
struct std::tuple_element<I, lak::token<CHAR>>
{
  static_assert(I < 2);
  using type = lak::remove_cvref_t<decltype(
    std::declval<lak::token<CHAR> &>().template get<I>())>;
};

#include "tokeniser.inl"

#endif