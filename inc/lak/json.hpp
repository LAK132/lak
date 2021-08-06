#ifndef LAK_JSON_HPP
#define LAK_JSON_HPP

#include "lak/array.hpp"
#include "lak/char.hpp"
#include "lak/result.hpp"
#include "lak/string.hpp"
#include "lak/variant.hpp"

namespace lak
{
  struct JSON
  {
  private:
    struct subspan
    {
      size_t offset;
      size_t count;

      template<typename T>
      lak::result<lak::string_view<T>> operator()(
        lak::string_view<T> values) const;
    };

    struct index
    {
      size_t value;

      template<typename T>
      lak::result<T &> operator()(lak::span<T> values) const;
    };

    using number = lak::variant<uintmax_t, intmax_t, double>;

    struct object
    {
      index key;
      index value;
      index next; // linked list
    };

    struct array
    {
      index value;
      index next; // linked list
    };

    using value_type = lak::variant<subspan,   // string
                                    nullptr_t, // null
                                    bool,      // boolean
                                    uintmax_t, // number
                                    intmax_t,  // number
                                    double,    // number
                                    object,    // object
                                    array      // array
                                    >;

  public:
    struct number_proxy
    {
    private:
      friend struct JSON;
      friend struct subspan;
      friend struct index;
      friend struct object;
      friend struct array;

      const JSON &_parser;
      number _number;

      inline number_proxy(const JSON &parser, number num)
      : _parser(parser), _number(num)
      {
      }

    public:
      inline number number() { return _number; }

      inline bool is_unsigned_integer() { return _number.holds<uintmax_t>(); }
      inline bool is_signed_integer() { return _number.holds<intmax_t>(); }
      inline bool is_floating_point() { return _number.holds<double>(); }

      inline lak::result<uintmax_t> unsigned_integer();
      inline lak::result<intmax_t> signed_integer();
      inline lak::result<double> floating_point();

      inline uintmax_t as_unsigned_integer();
      inline intmax_t as_signed_integer();
      inline double as_floating_point();
    };

    struct object_proxy
    {
    private:
      friend struct JSON;
      friend struct subspan;
      friend struct index;
      friend struct object;
      friend struct array;
      friend struct number_proxy;

      const JSON &_parser;
      object _object;

      inline object_proxy(const JSON &parser, object obj)
      : _parser(parser), _object(obj)
      {
      }

    public:
      inline bool empty() const { return _object.key.value == 0; }

      inline auto operator[](lak::u8string_view key) const;
      // -> lak::result<value_proxy>

      template<typename F>
      void for_each(F &&f) const;
    };

    struct array_proxy
    {
    private:
      friend struct JSON;
      friend struct subspan;
      friend struct index;
      friend struct object;
      friend struct array;
      friend struct number_proxy;
      friend struct object_proxy;

      const JSON &_parser;
      array _array;

      inline array_proxy(const JSON &parser, array arr)
      : _parser(parser), _array(arr)
      {
      }

    public:
      inline bool empty() const { return _array.value.value == 0; }

      inline auto operator[](size_t index) const;
      // -> lak::result<value_proxy>

      template<typename F>
      void for_each(F &&f) const;
    };

    struct value_proxy
    {
    private:
      friend struct JSON;
      friend struct subspan;
      friend struct index;
      friend struct object;
      friend struct array;
      friend struct number_proxy;
      friend struct object_proxy;
      friend struct array_proxy;

      const JSON &_parser;
      index _index;

      inline value_proxy(const JSON &parser, index index)
      : _parser(parser), _index(index)
      {
      }

    public:
      using value = lak::variant<lak::u8string_view, // string
                                 nullptr_t,          // null
                                 bool,               // boolean
                                 uintmax_t,          // number
                                 intmax_t,           // number
                                 double,             // number
                                 object_proxy,       // object
                                 array_proxy         // array
                                 >;

      lak::result<object_proxy> get_object() const;
      lak::result<array_proxy> get_array() const;
      lak::result<number_proxy> get_number() const;
      lak::result<lak::u8string_view> get_string() const;
      lak::result<bool> get_bool() const;
      lak::result<nullptr_t> get_null() const;
      lak::result<value> get_value() const;
    };

  private:
    friend struct subspan;
    friend struct index;
    friend struct object;
    friend struct array;
    friend struct number_proxy;
    friend struct object_proxy;
    friend struct array_proxy;
    friend struct value_proxy;

    size_t _position = 0;
    lak::u8string_view _input;

    lak::array<char8_t> _string_data;
    lak::array<value_type> _data;
    lak::array<index> _state;

    inline bool empty() const { return _position >= _input.size(); }

    bool skip_ascii_whitespace();

    inline lak::u8string_view remaining() const
    {
      return _input.substr(_position);
    }

    inline char8_t peek() const { return _input[_position]; }

    inline char8_t read() { return _input[_position++]; }

    lak::result<> parse_string();
    lak::result<> parse_null();
    lak::result<> parse_bool();
    lak::result<> parse_number();
    lak::result<> parse_literal_value();
    lak::result<> parse_object();
    lak::result<> parse_array();
    lak::result<> parse_value();

    lak::result<> parse_value(lak::u8string_view str);

  public:
    JSON()             = default;
    JSON(const JSON &) = default;
    JSON(JSON &&)      = default;
    JSON &operator=(const JSON &) = default;
    JSON &operator=(JSON &&) = default;

    static lak::result<JSON> parse(lak::u8string_view str);

    lak::result<value_proxy> value() const;
  };
}

#include "lak/json.inl"

#endif
