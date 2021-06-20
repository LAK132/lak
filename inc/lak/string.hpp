#ifndef LAK_STRING_HPP
#define LAK_STRING_HPP

#include "lak/char.hpp"
#include "lak/type_traits.hpp"

#include <string>

namespace lak
{
  template<typename CHAR>
  using string    = std::basic_string<CHAR>;
  using astring   = lak::string<char>;
  using wstring   = lak::string<wchar_t>;
  using u8string  = lak::string<char8_t>;
  using u16string = lak::string<char16_t>;
  using u32string = lak::string<char32_t>;

  template<typename T>
  struct is_string
  {
    static constexpr bool value = false;
  };
  template<typename CHAR>
  struct is_string<lak::string<CHAR>>
  {
    static constexpr bool value = true;
  };
  template<>
  struct is_string<const char8_t *>
  {
    static constexpr bool value = true;
  };
  template<>
  struct is_string<char8_t *>
  {
    static constexpr bool value = true;
  };
  template<>
  struct is_string<const char16_t *>
  {
    static constexpr bool value = true;
  };
  template<>
  struct is_string<char16_t *>
  {
    static constexpr bool value = true;
  };
  template<>
  struct is_string<const char32_t *>
  {
    static constexpr bool value = true;
  };
  template<>
  struct is_string<char32_t *>
  {
    static constexpr bool value = true;
  };
  template<>
  struct is_string<const wchar_t *>
  {
    static constexpr bool value = true;
  };
  template<>
  struct is_string<wchar_t *>
  {
    static constexpr bool value = true;
  };
  template<typename T>
  inline constexpr bool is_string_v = lak::is_string<T>::value;

  template<typename T>
  struct is_lak_type<lak::is_string<T>> : lak::true_type
  {
  };
}

static inline const char8_t *operator"" _u8(const char *str, size_t)
{
  return reinterpret_cast<const char8_t *>(str);
}

#endif