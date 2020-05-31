#ifndef LAK_TYPE_UTILS_HPP
#define LAK_TYPE_UTILS_HPP

#include <optional>

namespace lak
{
  /* --- remove_refs_ptrs --- */

  template<typename T>
  struct remove_refs_ptrs
  {
    using type = T;
  };
  template<typename T>
  struct remove_refs_ptrs<T *>
  {
    using type = typename remove_refs_ptrs<T>::type;
  };
  template<typename T>
  struct remove_refs_ptrs<T &>
  {
    using type = typename remove_refs_ptrs<T>::type;
  };
  template<typename T>
  struct remove_refs_ptrs<T &&>
  {
    using type = typename remove_refs_ptrs<T>::type;
  };
  template<typename T>
  using remove_refs_ptrs_t = typename remove_refs_ptrs<T>::type;

  /* --- is_function --- */

  template<typename T>
  struct is_function : std::is_function<lak::remove_refs_ptrs_t<T>>
  {
  };
  template<typename T>
  static constexpr bool is_function_v = is_function<T>::value;
}

#endif
