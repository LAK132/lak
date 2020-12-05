#ifndef LAK_DEFER_HPP
#define LAK_DEFER_HPP

#include "lak/compiler.hpp"
#include "lak/macro_utils.hpp"

#include <functional>

namespace lak
{
  template<typename LAMBDA>
  struct defer_t
  {
  public:
    static_assert(std::is_invocable_v<LAMBDA>);
    LAMBDA _lambda;
    defer_t(LAMBDA lambda) : _lambda(lambda) {}
    ~defer_t() { _lambda(); }
  };
  template<typename LAMBDA>
  force_inline defer_t<LAMBDA> defer(LAMBDA lambda)
  {
    return defer_t<LAMBDA>(lambda);
  }
}

#define DEFER(...)                                                            \
  auto UNIQUIFY(DEFER_OBJECT_) = lak::defer([&]() { __VA_ARGS__; })
#define DEFER_RESET(OBJ)                                                      \
  auto UNIQUIFY(DEFER_RESET_OBJECT_) = OBJ;                                   \
  DEFER(OBJ = lak::move(UNIQUIFY(DEFER_RESET_OBJECT_)));
#define DEFER_CALL(FUNC, ...)                                                 \
  auto UNIQUIFY(DEFER_OBJECT_) = lak::defer(std::bind(FUNC, __VA_ARGS__))

#endif
