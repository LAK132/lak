#ifndef LAK_TOKENISER_HPP
#define LAK_TOKENISER_HPP

#include "span.hpp"
#include "string.hpp"

#include <vector>

namespace lak
{
  template<typename CHAR>
  struct tokeniser
  {
  private:
    span<const CHAR> _data;
    std::vector<std::u32string> _operators;
    size_t _longest_operator;
    span<const CHAR> _current;
    mutable span<const CHAR> _next;

  public:
    inline tokeniser(span<const CHAR> str,
                     const std::vector<std::u32string> &operators) noexcept
    : _data(str), _operators(operators), _longest_operator(0)
    {
      for (const auto &op : _operators)
        if (op.size() > _longest_operator) _longest_operator = op.size();

      reset();
    }

    inline tokeniser(const std::basic_string<CHAR> &str,
                     const std::vector<std::u32string> &operators) noexcept
    : tokeniser(span(str.c_str(), str.size()), operators)
    {
    }

    inline tokeniser &begin() noexcept { return *this; }

    inline nullptr_t end() const noexcept { return nullptr; }

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

    inline void reset() noexcept
    {
      _current = {_data.begin(), 0};
      operator++();
    }

    inline bool operator==(nullptr_t) const noexcept
    {
      return _current.size() == 0;
    }

    inline bool operator!=(nullptr_t) const noexcept
    {
      return _current.size() != 0;
    }

    operator bool() const noexcept { return _current.size() != 0; }
  };

  extern template struct tokeniser<char>;
  extern template struct tokeniser<wchar_t>;
  extern template struct tokeniser<char8_t>;
  extern template struct tokeniser<char16_t>;
  extern template struct tokeniser<char32_t>;
}

#include "tokeniser.inl"

#endif