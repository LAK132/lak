#ifndef LAK_CHAR_HPP
#define LAK_CHAR_HPP

// char8_t typedef for C++ < 20
#if __cplusplus <= 201703L
#  include <cstdint>
using char8_t = uint_least8_t;
#endif

#if WCHAR_MAX > 0xFFFFU
using wchar_unicode_t = char32_t;
#elif WCHAR_MAX > 0xFFU
using wchar_unicode_t = char16_t;
#else
using wchar_unicode_t = char8_t;
#endif

namespace lak
{
  template<typename CHAR>
  struct chars_per_codepoint;

  template<>
  struct chars_per_codepoint<char>
  {
    static constexpr size_t value = 1;
  };

  template<>
  struct chars_per_codepoint<char8_t>
  {
    static constexpr size_t value = 4;
  };

  template<>
  struct chars_per_codepoint<char16_t>
  {
    static constexpr size_t value = 2;
  };

  template<>
  struct chars_per_codepoint<char32_t>
  {
    static constexpr size_t value = 1;
  };

  template<>
  struct chars_per_codepoint<wchar_t>
  {
    static constexpr size_t value =
      chars_per_codepoint<wchar_unicode_t>::value;
  };

  template<typename CHAR>
  inline constexpr size_t chars_per_codepoint_v =
    chars_per_codepoint<CHAR>::value;
}

#endif