#ifndef LAK_RESULT_HPP
#define LAK_RESULT_HPP

#include "lak/macro_utils.hpp"
#include "lak/type_utils.hpp"

#include <csetjmp>
#include <optional>
#include <variant>

namespace lak
{
  template<typename... T>
  struct is_result : public lak::false_type
  {
  };
  template<typename... T>
  constexpr bool is_result_v = is_result<T...>::value;

  template<typename T>
  struct ok_t
  {
    T value;
  };

  template<typename T>
  ok_t(T &&) -> ok_t<T &&>;

  template<typename T>
  ok_t(T &) -> ok_t<T &>;

  template<typename T>
  ok_t(const T &) -> ok_t<const T &>;

  template<typename T>
  struct err_t
  {
    T value;
  };

  template<typename T>
  err_t(T &&) -> err_t<T &&>;

  template<typename T>
  err_t(T &) -> err_t<T &>;

  template<typename T>
  err_t(const T &) -> err_t<const T &>;

  template<typename OK, typename ERR>
  struct result
  {
    using ok_type  = OK;
    using err_type = ERR;

  private:
    std::variant<OK, ERR> _value;

    force_inline OK &get_ok() { return std::get<0>(_value); }
    force_inline const OK &get_ok() const { return std::get<0>(_value); }

    force_inline ERR &get_err() { return std::get<1>(_value); }
    force_inline const ERR &get_err() const { return std::get<1>(_value); }

    result(const std::variant<OK, ERR> &value) : _value(value) {}
    result(std::variant<OK, ERR> &&value) : _value(value) {}

  public:
    result(const result &) = default;
    result(result &&)      = default;

    result &operator=(const result &) = default;
    result &operator=(result &&) = default;

    static inline result make_ok(const OK &value)
    {
      return result(std::variant<OK, ERR>(std::in_place_index_t<0>{}, value));
    }
    static inline result make_ok(OK &&value)
    {
      return result(std::variant<OK, ERR>(std::in_place_index_t<0>{}, value));
    }

    static inline result make_err(const ERR &value)
    {
      return result(std::variant<OK, ERR>(std::in_place_index_t<1>{}, value));
    }
    static inline result make_err(ERR &&value)
    {
      return result(std::variant<OK, ERR>(std::in_place_index_t<1>{}, value));
    }

    template<typename T>
    result(ok_t<T> value)
    : _value(std::in_place_index_t<0>{}, std::forward<T>(value.value))
    {
    }
    template<typename T>
    result(err_t<T> value)
    : _value(std::in_place_index_t<1>{}, std::forward<T>(value.value))
    {
    }

    bool is_ok() const { return _value.index() == 0; }
    bool is_err() const { return _value.index() == 1; }

    [[nodiscard]] OK *ok() & { return std::get_if<0>(&_value); }
    [[nodiscard]] const OK *ok() const & { return std::get_if<0>(&_value); }
    [[nodiscard]] std::optional<OK> ok() &&
    {
      return is_ok() ? std::optional<OK>(lak::move(get_ok())) : std::nullopt;
    }

    [[nodiscard]] ERR *err() & { return std::get_if<1>(&_value); }
    [[nodiscard]] const ERR *err() const & { return std::get_if<1>(&_value); }
    [[nodiscard]] std::optional<ERR> err() &&
    {
      return is_err() ? std::optional<ERR>(lak::move(get_err()))
                      : std::nullopt;
    }

    template<typename FUNCTOR>
    auto map(FUNCTOR &&functor)
    {
      using result_t =
        result<lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, OK &>>, ERR>;
      return is_ok() ? result_t::make_ok(functor(get_ok()))
                     : result_t::make_err(get_err());
    }

    template<typename FUNCTOR>
    auto map(FUNCTOR &&functor) const
    {
      using result_t =
        result<lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, const OK &>>,
               ERR>;
      return is_ok() ? result_t::make_ok(functor(get_ok()))
                     : result_t::make_err(get_err());
    }

    template<typename FUNCTOR>
    auto map_err(FUNCTOR &&functor)
    {
      using result_t =
        result<OK, lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, ERR &>>>;
      return is_ok() ? result_t::make_ok(get_ok())
                     : result_t::make_err(functor(get_err()));
    }

    template<typename FUNCTOR>
    auto map_err(FUNCTOR &&functor) const
    {
      using result_t = result<
        OK,
        lak::remove_cvref_t<std::invoke_result_t<FUNCTOR, const ERR &>>>;
      return is_ok() ? result_t::make_ok(get_ok())
                     : result_t::make_err(functor(get_err()));
    }

    template<typename STR>
    OK &expect(STR &&error_str)
    {
      if (is_err()) ABORTF(error_str, ": ", get_err());
      return get_ok();
    }

    template<typename STR>
    const OK &expect(STR &&error_str) const
    {
      if (is_err()) ABORTF(error_str, ": ", get_err());
      return get_ok();
    }

    template<typename STR>
    ERR &expect_err(STR &&error_str)
    {
      if (is_ok()) ABORTF(error_str, ": ", get_ok());
      return get_err();
    }

    template<typename STR>
    const ERR &expect_err(STR &&error_str) const
    {
      if (is_ok()) ABORTF(error_str, ": ", get_ok());
      return get_err();
    }

    OK &unwrap() { return expect("unwrap failed"); }

    const OK &unwrap() const { return expect("unwrap failed"); }

    ERR &unwrap_err() { return expect_err("unwrap_err failed"); }

    const ERR &unwrap_err() const { return expect_err("unwrap_err failed"); }
  };

  template<typename... T>
  struct is_result<lak::result<T...>> : public lak::true_type
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
  std::jmp_buf _error_result_jump_buffer;                                     \
  std::optional<__VA_ARGS__> _error_result;                                   \
  if (setjmp(_error_result_jump_buffer))                                      \
  {                                                                           \
    return lak::err_t{lak::move(*_error_result)};                             \
  }

#define RES_TRY(...)                                                          \
  [&]() {                                                                     \
    if (auto result = __VA_ARGS__; result.is_ok())                            \
      return (result.unwrap());                                               \
    else                                                                      \
      _error_result = lak::move(result.unwrap_err());                         \
    std::longjmp(_error_result_jump_buffer, 1);                               \
  }()
}

#endif