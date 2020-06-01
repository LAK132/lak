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

    inline const tokeniser &begin() const noexcept { return *this; }

    inline nullptr_t end() const noexcept { return nullptr; }

    inline const span<const CHAR> &operator*() const { return _current; }

    tokeniser &operator++();

    inline span<const CHAR> peek() const { return _current; }

    inline span<const CHAR> pop()
    {
      auto rtn = _current;
      operator++();
      return rtn;
    }

    inline void reset()
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
  };

  extern template struct tokeniser<char>;
  extern template struct tokeniser<wchar_t>;
  extern template struct tokeniser<char8_t>;
  extern template struct tokeniser<char16_t>;
  extern template struct tokeniser<char32_t>;
}

#include "tokeniser.inl"

#endif