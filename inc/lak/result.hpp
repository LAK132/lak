#ifndef LAK_RESULT_HPP
#define LAK_RESULT_HPP

#include "lak/macro_utils.hpp"
#include "lak/optional.hpp"
#include "lak/type_utils.hpp"
#include "lak/variant.hpp"

#include <csetjmp>

namespace lak
{
  template<typename... T>
  struct is_result : public lak::false_type
  {
  };
  template<typename... T>
  constexpr bool is_result_v = is_result<T...>::value;

  // the OK type of the result without reference removed
  template<typename RESULT>
  struct result_ok_type : public lak::type_identity<void>
  {
  };
  template<typename RESULT>
  using result_ok_type_t = typename result_ok_type<RESULT>::type;

  // the ERR type of the result without references removed;
  template<typename RESULT>
  struct result_err_type : public lak::type_identity<void>
  {
  };
  template<typename RESULT>
  using result_err_type_t = typename result_err_type<RESULT>::type;

  /* --- ok_t --- */

  template<typename T = lak::monostate>
  struct ok_t
  {
    T value;
  };

  template<typename T>
  ok_t(T &&) -> ok_t<T &&>;

  template<typename T>
  ok_t(T &) -> ok_t<T &>;

  /* --- err_t --- */

  template<typename T = lak::monostate>
  struct err_t
  {
    T value;
  };

  template<typename T>
  err_t(T &&) -> err_t<T &&>;

  template<typename T>
  err_t(T &) -> err_t<T &>;

  /* --- result --- */

  template<typename OK, typename ERR>
  struct result
  {
    using ok_type  = lak::remove_reference_t<OK>;
    using err_type = lak::remove_reference_t<ERR>;

  private:
    lak::variant<OK, ERR> _value;

    force_inline ok_type &get_ok() { return *_value.get<0>(); }
    force_inline const ok_type &get_ok() const { return *_value.get<0>(); }
    force_inline OK &&forward_ok()
    {
      return lak::forward<OK>(*_value.get<0>());
    }

    force_inline err_type &get_err() { return *_value.get<1>(); }
    force_inline const err_type &get_err() const { return *_value.get<1>(); }
    force_inline ERR &&forward_err()
    {
      return lak::forward<ERR>(*_value.get<1>());
    }

    result(const lak::variant<OK, ERR> &value) : _value(value) {}
    result(lak::variant<OK, ERR> &&value) : _value(lak::move(value)) {}

  public:
    result(const result &) = default;
    result(result &&)      = default;

    result &operator=(const result &) = default;
    result &operator=(result &&) = default;

    static inline result make_ok(const ok_type &value)
    {
      return result(lak::variant<OK, ERR>(lak::in_place_index<0>, value));
    }
    static inline result make_ok(OK &&value)
    {
      return result(lak::variant<OK, ERR>(lak::in_place_index<0>,
                                          lak::forward<OK>(value)));
    }
    static inline result make_err(const err_type &value)
    {
      return result(lak::variant<OK, ERR>(lak::in_place_index<1>, value));
    }
    static inline result make_err(ERR &&value)
    {
      return result(lak::variant<OK, ERR>(lak::in_place_index<1>,
                                          lak::forward<ERR>(value)));
    }

    template<typename T>
    result(ok_t<T> value) : result(make_ok(lak::forward<T>(value.value)))
    {
    }
    template<typename T>
    result(err_t<T> value) : result(make_err(lak::forward<T>(value.value)))
    {
    }

    bool is_ok() const { return _value.index() == 0; }
    bool is_err() const { return _value.index() == 1; }

    /* --- ok --- */

    [[nodiscard]] ok_type *ok() & { return _value.get<0>(); }
    [[nodiscard]] const ok_type *ok() const & { return _value.get<0>(); }
    [[nodiscard]] lak::optional<OK> ok() &&
    {
      return is_ok() ? lak::optional<OK>(forward_ok()) : lak::nullopt;
    }

    /* --- err --- */

    [[nodiscard]] err_type *err() & { return _value.get<1>(); }
    [[nodiscard]] const err_type *err() const & { return _value.get<1>(); }
    [[nodiscard]] lak::optional<ERR> err() &&
    {
      return is_err() ? lak::optional<ERR>(forward_err()) : lak::nullopt;
    }

    /* --- map --- */

    template<typename FUNCTOR>
    result<lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, ok_type &>>, ERR>
    map(FUNCTOR &&functor) &
    {
      if (is_ok())
        return lak::ok_t{functor(get_ok())};
      else
        return lak::err_t{get_err()};
    }

    template<typename FUNCTOR>
    result<lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, const ok_type &>>,
           err_type>
    map(FUNCTOR &&functor) const &
    {
      if (is_ok())
        return lak::ok_t{functor(get_ok())};
      else
        return lak::err_t{get_err()};
    }

    template<typename FUNCTOR>
    result<lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, OK &&>>, ERR> map(
      FUNCTOR &&functor) &&
    {
      if (is_ok())
        return lak::ok_t{functor(forward_ok())};
      else
        return lak::err_t{forward_err()};
    }

    /* --- map_err --- */

    template<typename FUNCTOR>
    result<OK, lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, err_type &>>>
    map_err(FUNCTOR &&functor) &
    {
      if (is_ok())
        return lak::ok_t{get_ok()};
      else
        return lak::err_t{functor(get_err())};
    }

    template<typename FUNCTOR>
    result<
      ok_type,
      lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, const err_type &>>>
    map_err(FUNCTOR &&functor) const &
    {
      if (is_ok())
        return lak::ok_t{get_ok()};
      else
        return lak::err_t{functor(get_err())};
    }

    template<typename FUNCTOR>
    result<OK, lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, ERR &&>>>
    map_err(FUNCTOR &&functor) &&
    {
      if (is_ok())
        return lak::ok_t{forward_ok()};
      else
        return lak::err_t{functor(forward_err())};
    }

    /* --- and_then --- */

    template<
      typename FUNCTOR,
      lak::enable_if_i<lak::is_same_v<
        ERR,
        lak::result_err_type_t<std::invoke_result_t<FUNCTOR, ok_type &>>>> = 0>
    lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, ok_type &>> and_then(
      FUNCTOR &&functor) &
    {
      if (is_ok())
        return functor(get_ok());
      else
        return lak::err_t{get_err()};
    }

    template<typename FUNCTOR,
             lak::enable_if_i<lak::is_same_v<
               ERR,
               lak::result_err_type_t<
                 std::invoke_result_t<FUNCTOR, const ok_type &>>>> = 0>
    lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, const ok_type &>>
    and_then(FUNCTOR &&functor) const &
    {
      if (is_ok())
        return functor(get_ok());
      else
        return lak::err_t{get_err()};
    }

    template<
      typename FUNCTOR,
      lak::enable_if_i<lak::is_same_v<
        ERR,
        lak::result_err_type_t<std::invoke_result_t<FUNCTOR, OK &&>>>> = 0>
    lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, OK &&>> and_then(
      FUNCTOR &&functor) &&
    {
      if (is_ok())
        return functor(forward_ok());
      else
        return lak::err_t{forward_err()};
    }

    /* --- or_else --- */

    template<
      typename FUNCTOR,
      lak::enable_if_i<lak::is_same_v<
        ERR,
        lak::result_err_type_t<std::invoke_result_t<FUNCTOR, err_type &>>>> =
        0>
    lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, err_type &>> or_else(
      FUNCTOR &&functor) &
    {
      if (is_ok())
        return lak::ok_t{get_ok()};
      else
        return functor(get_err());
    }

    template<typename FUNCTOR,
             lak::enable_if_i<lak::is_same_v<
               ERR,
               lak::result_err_type_t<
                 std::invoke_result_t<FUNCTOR, const err_type &>>>> = 0>
    lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, const err_type &>>
    or_else(FUNCTOR &&functor) const &
    {
      if (is_ok())
        return lak::ok_t{get_ok()};
      else
        return functor(get_err());
    }

    template<
      typename FUNCTOR,
      lak::enable_if_i<lak::is_same_v<
        ERR,
        lak::result_err_type_t<std::invoke_result_t<FUNCTOR, ERR &&>>>> = 0>
    lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, ERR &&>> or_else(
      FUNCTOR &&functor) &&
    {
      if (is_ok())
        return lak::ok_t{forward_ok()};
      else
        return functor(forward_err());
    }

    /* --- expect --- */

    template<typename STR>
    ok_type &expect(STR &&error_str) &
    {
      if (is_err()) ABORTF(error_str, ": ", get_err());
      return get_ok();
    }

    template<typename STR>
    const ok_type &expect(STR &&error_str) const &
    {
      if (is_err()) ABORTF(error_str, ": ", get_err());
      return get_ok();
    }

    template<typename STR>
    OK &&expect(STR &&error_str) &&
    {
      if (is_err()) ABORTF(error_str, ": ", get_err());
      return forward_ok();
    }

    /* --- expect_err --- */

    template<typename STR>
    err_type &expect_err(STR &&error_str) &
    {
      if (is_ok()) ABORTF(error_str /*, ": ", get_ok()*/);
      return get_err();
    }

    template<typename STR>
    const err_type &expect_err(STR &&error_str) const &
    {
      if (is_ok()) ABORTF(error_str /*, ": ", get_ok()*/);
      return get_err();
    }

    template<typename STR>
    ERR &&expect_err(STR &&error_str) &&
    {
      if (is_ok()) ABORTF(error_str /*, ": ", get_ok()*/);
      return forward_err();
    }

    /* --- unwrap --- */

    ok_type &unwrap() & { return expect("unwrap failed"); }

    const ok_type &unwrap() const & { return expect("unwrap failed"); }

    OK &&unwrap() && { return lak::forward<OK>(expect("unwrap failed")); }

    /* --- unwrap_Err --- */

    err_type &unwrap_err() & { return expect_err("unwrap_err failed"); }

    const err_type &unwrap_err() const &
    {
      return expect_err("unwrap_err failed");
    }

    ERR &&unwrap_err() &&
    {
      return lak::forward<ERR>(expect_err("unwrap_err failed"));
    }

    /* --- flatten --- */

    template<
      typename U                                                       = OK,
      lak::enable_if_i<lak::is_same_v<ERR, lak::result_err_type_t<U>>> = 0>
    ok_type flatten() const &
    {
      if (is_ok())
        return get_ok();
      else
        return lak::err_t{get_err()};
    }

    template<
      typename U                                                       = OK,
      lak::enable_if_t<lak::is_same_v<ERR, lak::result_err_type_t<U>>> = 0>
    ok_type flatten() &&
    {
      if (is_ok())
        return forward_ok();
      else
        return lak::err_t{forward_err()};
    }
  };

  template<typename OK, typename... ERR>
  using results = lak::result<OK, lak::variant<ERR...>>;

  template<typename ERR>
  using error_code = lak::result<lak::monostate, ERR>;

  template<typename... ERR>
  using error_codes = lak::error_code<lak::variant<ERR...>>;

  template<typename... T>
  struct is_result<lak::result<T...>> : public lak::true_type
  {
  };

  template<typename OK, typename ERR>
  struct result_ok_type<lak::result<OK, ERR>> : public lak::type_identity<OK>
  {
  };

  template<typename OK, typename ERR>
  struct result_err_type<lak::result<OK, ERR>> : public lak::type_identity<ERR>
  {
  };

#if !defined(NOLOG) && !defined(NDEBUG)
#  define EXPECT(STR)     expect(LINE_TRACE_STR ": " STR)
#  define EXPECT_ERR(STR) expect_err(LINE_TRACE_STR ": " STR)

#  define UNWRAP()     expect(LINE_TRACE_STR ": unwrap failed")
#  define UNWRAP_ERR() expect_err(LINE_TRACE_STR ": unwrap_err failed")
#else
#  define EXPECT(STR)     expect(STR)
#  define EXPECT_ERR(STR) expect_err(STR)

#  define UNWRAP()     unwrap()
#  define UNWRAP_ERR() unwrap_err()
#endif

#define HANDLE_RESULT_ERR(...)                                                \
  using _error_result_t = __VA_ARGS__;                                        \
  lak::optional<_error_result_t> _error_result;

  // This should hopefully respect object lifetimes.

#define RES_TRY(ASSIGN, ...)                                                  \
  std::jmp_buf UNIQUIFY(ERROR_RESULT_JUMP_BUFFER_);                           \
  if (setjmp(UNIQUIFY(ERROR_RESULT_JUMP_BUFFER_)))                            \
    return lak::err_t{lak::forward<_error_result_t>(*_error_result)};         \
  ASSIGN[&]()                                                                 \
  {                                                                           \
    if (auto result = __VA_ARGS__; result.is_ok())                            \
      return lak::move(result).unwrap();                                      \
    else                                                                      \
      _error_result = lak::forward<lak::result_err_type_t<decltype(result)>>( \
        result.unwrap_err());                                                 \
    std::longjmp(UNIQUIFY(ERROR_RESULT_JUMP_BUFFER_), 1);                     \
  }                                                                           \
  ()

#define RES_TRY_ERR(...)                                                      \
  if (auto result = __VA_ARGS__; result.is_err())                             \
    return lak::err_t{lak::move(result).unwrap_err()};
}

#endif