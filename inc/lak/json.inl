#include "lak/json.hpp"

/* --- JSON::subspan --- */

template<typename T>
lak::result<lak::string_view<T>> lak::JSON::subspan::operator()(
  lak::string_view<T> values) const
{
  if (offset > (offset + count)) return lak::err_t{};
  if (values.size() < offset + count) return lak::err_t{};
  return lak::ok_t{values.substr(offset, count)};
}

template<typename T>
lak::result<T &> lak::JSON::index::operator()(lak::span<T> values) const
{
  if (values.size() < value) return lak::err_t{};
  return lak::ok_t{values[value]};
}

/* --- JSON::number_proxy --- */

inline lak::result<uintmax_t> lak::JSON::number_proxy::unsigned_integer()
{
  return lak::copy_result_from_pointer(
    _number.get<number::index_of<uintmax_t>>());
}

inline lak::result<intmax_t> lak::JSON::number_proxy::signed_integer()
{
  return lak::copy_result_from_pointer(
    _number.get<number::index_of<intmax_t>>());
}

inline lak::result<double> lak::JSON::number_proxy::floating_point()
{
  return lak::copy_result_from_pointer(
    _number.get<number::index_of<double>>());
}

inline uintmax_t lak::JSON::number_proxy::as_unsigned_integer()
{
  return lak::visit(_number, [](auto v) { return uintmax_t(v); });
}

inline intmax_t lak::JSON::number_proxy::as_signed_integer()
{
  return lak::visit(_number, [](auto v) { return intmax_t(v); });
}

inline double lak::JSON::number_proxy::as_floating_point()
{
  return lak::visit(_number, [](auto v) { return double(v); });
}

/* --- JSON::object_proxy --- */

auto lak::JSON::object_proxy::operator[](lak::string_view<char8_t> key) const
{
  for (object o = _object;;)
  {
    if (o.key.value != 0 &&
        lak::JSON::value_proxy(_parser, o.key).get_string().unwrap() == key)
      return lak::result<lak::JSON::value_proxy>::make_ok(
        lak::JSON::value_proxy(_parser, o.value));
    else if (o.next.value == 0)
      return lak::result<lak::JSON::value_proxy>::make_err({});
    else
      o =
        lak::JSON::value_proxy(_parser, o.next).get_object().unwrap()._object;
  }
}

template<typename F>
void lak::JSON::object_proxy::for_each(F &&f) const
{
  if (_object.key.value == 0) return;

  f(JSON::value_proxy(_parser, _object.key).get_string().unwrap(),
    JSON::value_proxy(_parser, _object.value));

  for (object o = _object; o.next.value != 0;)
  {
    o = JSON::value_proxy(_parser, o.next).get_object().unwrap()._object;

    f(JSON::value_proxy(_parser, o.key).get_string().unwrap(),
      JSON::value_proxy(_parser, o.value));
  }
}

/* --- JSON::array_proxy --- */

auto lak::JSON::array_proxy::operator[](size_t index) const
{
  array a = _array;
  for (; index > 0; --index)
  {
    if (a.next.value == 0)
      return lak::result<lak::JSON::value_proxy>::make_err({});
    else
      a = lak::JSON::value_proxy(_parser, a.next).get_array().unwrap()._array;
  }
  return lak::result<lak::JSON::value_proxy>::make_ok(
    lak::JSON::value_proxy(_parser, a.value));
}

template<typename F>
void lak::JSON::array_proxy::for_each(F &&f) const
{
  if (_array.value.value == 0) return;

  f(JSON::value_proxy(_parser, _array.value));

  for (array a = _array; a.next.value != 0;)
  {
    a = JSON::value_proxy(_parser, a.next).get_array().unwrap()._array;

    f(JSON::value_proxy(_parser, a.value));
  }
}
