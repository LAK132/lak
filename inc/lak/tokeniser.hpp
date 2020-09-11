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
  };

  struct token_position
  {
    codepoint_position begin = {};
    codepoint_position end   = {};
  };

  template<typename CHAR>
  struct tokeniser
  {
  private:
    span<const CHAR> _data;
    std::vector<std::u32string> _operators;
    size_t _longest_operator;

    span<const CHAR> _current;
    token_position _current_position;

    mutable span<const CHAR> _next;
    mutable token_position _next_position;

    void internal_start_next() const noexcept;

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

    inline tokeniser(const std::basic_string<CHAR> &str,
                     const std::vector<std::u32string> &operators) noexcept
    : tokeniser(lak::string_view(str), operators)
    {
    }

    inline tokeniser &begin() noexcept { return *this; }

    inline nullptr_t end() const noexcept { return nullptr; }

    inline token_position position() const noexcept
    {
      return _current_position;
    }

    inline token_position peek_position() const noexcept
    {
      peek();
      return _next_position;
    }

    inline const span<const CHAR> &operator*() const noexcept
    {
      return _current;
    }

    tokeniser &operator++() noexcept;

    // Peek at the next token.
    span<const CHAR> peek() const noexcept;

    // Scan until codepoint is found (exclusive).
    span<const CHAR> until(char32_t codepoint) noexcept;
    // If you're tokenising something like:
    //  R"delimiter(this is a string)delimiter";
    // You can mark `R"` as an operator, then do `until(U'(')` to get
    // "delimiter".
    // This may also be useful for skipping comments:
    //  # this is a comment [newline]
    // Marking `#` as an operator will allow you do to `until(U'\n')` to scan
    // until it reaches the end of the line (or end of the file).

    // Scan until str is found (exclusive).
    span<const CHAR> until(span<const char32_t> str) noexcept;
    // Useful for tokenising something like multiline comments:
    //  /*
    //  this is a multiline comment
    //  */
    // Once the operator `/*` is found, you can do `until(U"*/")` to scan until
    // the end of the comment (or end of the file) is found.

    // Scan until codepoint is found (inclusive).
    span<const CHAR> skip(char32_t codepoint) noexcept;

    // Scan until str is found (inclusive).
    span<const CHAR> skip(span<const char32_t> str) noexcept;

    void reset() noexcept;

    inline bool operator==(nullptr_t) const noexcept
    {
      return _current.begin() >= _data.end();
    }

    inline bool operator!=(nullptr_t) const noexcept
    {
      return _current.begin() < _data.end();
    }

    operator bool() const noexcept { return _current.size() != 0; }
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

#include "tokeniser.inl"

#endif