#include "lak/strconv.hpp"

namespace lak
{
  template struct string_conv_iterator<char>;
  template struct string_conv_iterator<wchar_t>;
  template struct string_conv_iterator<char8_t>;
  template struct string_conv_iterator<char16_t>;
  template struct string_conv_iterator<char32_t>;

  template<>
  ascii_code_iterator_t &ascii_code_iterator_t::operator++()
  {
    _current = 0;
    if (_begin != _end)
    {
      _current = *_begin;
      ++_begin;
    }
    return *this;
  }

  template<>
  utf8_code_iterator_t &utf8_code_iterator_t::operator++()
  {
    _current = 0;
    if (_begin != _end)
    {
      const char8_t c = *_begin;
      ++_begin;
      if (c >= 0x00 && c <= 0x7F)
      {
        _current = c;
      }
      else if (c >= 0xC0 && c <= 0xDF && (_end - _begin) > 0)
      {
        _current = ((c - 0xC0) << 6) | (*_begin - 0x80);
        ++_begin;
      }
      else if (c >= 0xE0 && c <= 0xEF && (_end - _begin) > 1)
      {
        _current = (c - 0xE0) << 12;
        _current |= (*_begin - 0x80) << 6;
        ++_begin;
        _current |= *_begin - 0x80;
        ++_begin;
      }
      else if (c >= 0xF0 && c <= 0xF7 && (_end - _begin) > 2)
      {
        _current = (c - 0xF0) << 18;
        _current |= (*_begin - 0x80) << 12;
        ++_begin;
        _current |= (*_begin - 0x80) << 6;
        ++_begin;
        _current |= *_begin - 0x80;
        ++_begin;
      }
    }
    return *this;
  }

  template<>
  utf16_code_iterator_t &utf16_code_iterator_t::operator++()
  {
    _current = 0;
    if (_begin != _end)
    {
      const char16_t c = *_begin;
      ++_begin;
      if ((c >= 0x0000 && c <= 0xD7FF) || (c >= 0xE000 && c <= 0xFFFF))
      {
        _current = c;
      }
      else if (_begin != _end)
      {
        _current = 0x010000 | (0xD800 + (c << 10));
        _current |= 0xDC00 + *_begin;
        ++_begin;
      }
    }
    return *this;
  }

  template<>
  utf32_code_iterator_t &utf32_code_iterator_t::operator++()
  {
    _current = 0;
    if (_begin != _end)
    {
      _current = *_begin;
      ++_begin;
    }
    return *this;
  }

  template<>
  wide_code_iterator_t &wide_code_iterator_t::operator++()
  {
    _current = 0;
    if (_begin != _end)
    {
      const wchar_t c = *_begin;
      ++_begin;
      if constexpr (sizeof(wchar_t) == sizeof(char8_t))
      {
        if (c >= 0x00 && c <= 0x7F)
        {
          _current = c;
        }
        else if (c >= 0xC0 && c <= 0xDF && (_end - _begin) > 0)
        {
          _current = ((c - 0xC0) << 6) | (*_begin - 0x80);
          ++_begin;
        }
        else if (c >= 0xE0 && c <= 0xEF && (_end - _begin) > 1)
        {
          _current = (c - 0xE0) << 12;
          _current |= (*_begin - 0x80) << 6;
          ++_begin;
          _current |= *_begin - 0x80;
          ++_begin;
        }
        else if (c >= 0xF0 && c <= 0xF7 && (_end - _begin) > 2)
        {
          _current = (c - 0xF0) << 18;
          _current |= (*_begin - 0x80) << 12;
          ++_begin;
          _current |= (*_begin - 0x80) << 6;
          ++_begin;
          _current |= *_begin - 0x80;
          ++_begin;
        }
      }
      else if constexpr (sizeof(wchar_t) == sizeof(char16_t))
      {
        if ((c >= 0x0000 && c <= 0xD7FF) || (c >= 0xE000 && c <= 0xFFFF))
        {
          _current = c;
        }
        else if (_begin != _end)
        {
          _current = 0x010000 | (0xD800 + (c << 10));
          _current |= 0xDC00 + *_begin;
          ++_begin;
        }
      }
      else
      {
        _current = c;
      }
    }
    return *this;
  }

  void append_code(std::string &str, const char32_t code)
  {
    if (code <= 0x7F) str += static_cast<char>(code);
  }

  void append_code(std::wstring &str, const char32_t code)
  {
    if constexpr (sizeof(wchar_t) == sizeof(char8_t))
    {
      if (code <= 0x7F)
      {
        str += static_cast<wchar_t>(code);
      }
      else if (code >= 0x80 && code <= 0x7FF)
      {
        str += 0xC0 + static_cast<wchar_t>(code >> 6);
        str += 0x80 + static_cast<wchar_t>(code & 0x3F);
      }
      else if (code >= 0x800 && code <= 0xFFFF)
      {
        str += 0xE0 + static_cast<wchar_t>(code >> 12);
        str += 0x80 + static_cast<wchar_t>((code >> 6) & 0x3F);
        str += 0x80 + static_cast<wchar_t>(code & 0x3F);
      }
      else if (code >= 0x10000 && code <= 0x10FFFF)
      {
        str += 0xF0 + static_cast<wchar_t>(code >> 18);
        str += 0x80 + static_cast<wchar_t>((code >> 12) & 0x3F);
        str += 0x80 + static_cast<wchar_t>((code >> 6) & 0x3F);
        str += 0x80 + static_cast<wchar_t>(code & 0x3F);
      }
    }
    else if constexpr (sizeof(wchar_t) == sizeof(char16_t))
    {
      if (code <= 0xFFFF)
      {
        str += static_cast<wchar_t>(code);
      }
      else
      {
        const auto _code = code - 0x01000;
        str += static_cast<wchar_t>(0xD800 + ((_code >> 10) & 0x3FF));
        str += static_cast<wchar_t>(0xDC00 + (_code & 0x3FF));
      }
    }
    else
    {
      str += static_cast<wchar_t>(code);
    }
  }

  void append_code(std::u8string &str, const char32_t code)
  {
    if (code <= 0x7F)
    {
      str += static_cast<char8_t>(code);
    }
    else if (code >= 0x80 && code <= 0x7FF)
    {
      str += 0xC0 + static_cast<char8_t>(code >> 6);
      str += 0x80 + static_cast<char8_t>(code & 0x3F);
    }
    else if (code >= 0x800 && code <= 0xFFFF)
    {
      str += 0xE0 + static_cast<char8_t>(code >> 12);
      str += 0x80 + static_cast<char8_t>((code >> 6) & 0x3F);
      str += 0x80 + static_cast<char8_t>(code & 0x3F);
    }
    else if (code >= 0x10000 && code <= 0x10FFFF)
    {
      str += 0xF0 + static_cast<char8_t>(code >> 18);
      str += 0x80 + static_cast<char8_t>((code >> 12) & 0x3F);
      str += 0x80 + static_cast<char8_t>((code >> 6) & 0x3F);
      str += 0x80 + static_cast<char8_t>(code & 0x3F);
    }
  }

  void append_code(std::u16string &str, const char32_t code)
  {
    if (code <= 0xFFFF)
    {
      str += static_cast<char16_t>(code);
    }
    else
    {
      const auto _code = code - 0x01000;
      str += static_cast<char16_t>(0xD800 + ((_code >> 10) & 0x3FF));
      str += static_cast<char16_t>(0xDC00 + (_code & 0x3FF));
    }
  }

  void append_code(std::u32string &str, const char32_t code) { str += code; }
}
