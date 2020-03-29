#ifndef LAK_DEFER_HPP
#define LAK_DEFER_HPP

namespace lak
{
  template<typename LAMBDA>
  struct defer_t
  {
  public:
    LAMBDA _lambda;
    defer_t(LAMBDA lambda) : _lambda(lambda) {}
    ~defer_t() { _lambda(); }
  };
  template<typename LAMBDA>
  inline defer_t<LAMBDA> defer(LAMBDA lambda)
  {
    return defer_t<LAMBDA>(lambda); // hello
  }
}

#define LAK_DEFER_CONCAT_EX(s1, s2) s1##s2
#define LAK_DEFER_CONCAT(s1, s2)    LAK_DEFER_CONCAT_EX(s1, s2)
#define LAK_DEFER_OBJECT            LAK_DEFER_CONCAT(_DEFER_OBJECT_, __LINE__)
#define DEFER(...)                  auto LAK_DEFER_OBJECT = lak::defer([&]() { __VA_ARGS__; })

#endif // LAK_DEFER_HPP
