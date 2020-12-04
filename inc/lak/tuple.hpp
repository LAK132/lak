#ifndef LAK_TUPLE_HPP
#define LAK_TUPLE_HPP

namespace lak
{
  template<size_t I, typename T>
  struct tuple_element;
  template<size_t I, typename T>
  using tuple_element_t = typename lak::tuple_element<I, T>::type;
}

#endif