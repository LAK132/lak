#include "lak/visit.hpp"

#include "lak/optional.hpp"

namespace lak
{
  namespace
  {
    template<
      typename FUNCTOR,
      typename T,
      T I,
      T... J,
      lak::enable_if_i<lak::is_void_v<
        std::invoke_result_t<FUNCTOR, lak::integral_constant<T, I>>>> = 0>
    force_inline auto _visit_switch(lak::integer_sequence<T, I, J...>,
                                    T i,
                                    FUNCTOR &&functor) -> lak::optional<>
    {
      if (I == i)
      {
        functor(lak::integral_constant<T, I>{});
        return lak::true_type{};
      }
      else if constexpr (sizeof...(J) > 0)
      {
        return visit_switch(
          lak::integer_sequence<T, J...>{}, i, lak::forward<FUNCTOR>(functor));
      }
      else
      {
        return lak::nullopt;
      }
    }

    template<
      typename FUNCTOR,
      typename T,
      T I,
      T... J,
      lak::enable_if_i<!lak::is_void_v<
        std::invoke_result_t<FUNCTOR, lak::integral_constant<T, I>>>> = 0>
    force_inline auto _visit_switch(lak::integer_sequence<T, I, J...>,
                                    T i,
                                    FUNCTOR &&functor)
      -> lak::optional<
        std::invoke_result_t<FUNCTOR, lak::integral_constant<T, I>>>
    {
      if (I == i)
      {
        return lak::optional<
          std::invoke_result_t<FUNCTOR, lak::integral_constant<T, I>>>(
          lak::in_place, functor(lak::integral_constant<T, I>{}));
      }
      else if constexpr (sizeof...(J) > 0)
      {
        return visit_switch(
          lak::integer_sequence<T, J...>{}, i, lak::forward<FUNCTOR>(functor));
      }
      else
      {
        return lak::nullopt;
      }
    }
  }
}

template<typename FUNCTOR, typename T, T I, T... J>
auto lak::visit_switch(lak::integer_sequence<T, I, J...>,
                       T i,
                       FUNCTOR &&functor)
{
  return lak::_visit_switch(
    lak::integer_sequence<T, I, J...>{}, i, lak::forward<FUNCTOR>(functor));
}

template<typename VAR, typename FUNCTOR>
auto lak::visit(VAR &&variant, FUNCTOR &&functor)
{
  static_assert(lak::is_variant_v<lak::remove_cvref_t<VAR>>);

  return *lak::visit_switch(typename lak::remove_cvref_t<VAR>::indices{},
                            variant.index(),
                            [&](auto index) {
                              using I = lak::remove_cvref_t<decltype(index)>;
                              static_assert(lak::is_integral_constant_v<I>);
                              return functor(*variant.get<I::value>());
                            });
}