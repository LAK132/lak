#include "lak/json.hpp"
#include "lak/char_utils.hpp"
#include "lak/string_utils.hpp"
#include "lak/string_view.hpp"

#include <cmath>

/* --- JSON::value_proxy --- */

lak::result<lak::JSON::object_proxy> lak::JSON::value_proxy::get_object() const
{
	return _index(lak::span(_parser._data))
	  .and_then(
	    [](const value_type &v) -> lak::result<const object &> {
		    return lak::result_from_pointer(v.get<value_type::index_of<object>>());
	    })
	  .map([&](const object &o) -> object_proxy
	       { return object_proxy(_parser, o); });
}

lak::result<lak::JSON::array_proxy> lak::JSON::value_proxy::get_array() const
{
	return _index(lak::span(_parser._data))
	  .and_then(
	    [](const value_type &v) -> lak::result<const array &> {
		    return lak::result_from_pointer(v.get<value_type::index_of<array>>());
	    })
	  .map([&](const array &a) -> array_proxy
	       { return array_proxy(_parser, a); });
}

lak::result<lak::JSON::number_proxy> lak::JSON::value_proxy::get_number() const
{
	return _index(lak::span(_parser._data))
	  .and_then(
	    [](const value_type &v) -> lak::result<number>
	    {
		    return lak::visit(
		      v,
		      lak::overloaded{
		        [](const uintmax_t &v) -> lak::result<number> {
			        return lak::ok_t{number::make<number::index_of<uintmax_t>>(v)};
		        },
		        [](const intmax_t &v) -> lak::result<number>
		        { return lak::ok_t{number::make<number::index_of<intmax_t>>(v)}; },
		        [](const double &v) -> lak::result<number>
		        { return lak::ok_t{number::make<number::index_of<double>>(v)}; },
		        [](auto, ...) -> lak::result<number> { return lak::err_t{}; }});
	    })
	  .map([&](number n) -> number_proxy { return number_proxy(_parser, n); });
}

lak::result<lak::u8string_view> lak::JSON::value_proxy::get_string() const
{
	return _index(lak::span(_parser._data))
	  .and_then(
	    [](const value_type &v) -> lak::result<const subspan &> {
		    return lak::result_from_pointer(
		      v.get<value_type::index_of<subspan>>());
	    })
	  .and_then(
	    [&](const subspan &s) -> lak::result<lak::u8string_view>
	    { return s(lak::u8string_view(lak::span(_parser._string_data))); });
}

lak::result<bool> lak::JSON::value_proxy::get_bool() const
{
	return _index(lak::span(_parser._data))
	  .and_then(
	    [](const value_type &v) -> lak::result<bool>
	    {
		    return lak::copy_result_from_pointer(
		      v.get<value_type::index_of<bool>>());
	    });
}

lak::result<nullptr_t> lak::JSON::value_proxy::get_null() const
{
	return _index(lak::span(_parser._data))
	  .and_then(
	    [](const value_type &v) -> lak::result<nullptr_t>
	    {
		    return lak::copy_result_from_pointer(
		      v.get<value_type::index_of<nullptr_t>>());
	    });
}

lak::result<lak::JSON::value_proxy::value> lak::JSON::value_proxy::get_value()
  const
{
	return _index(lak::span(_parser._data))
	  .map(
	    [&](const value_type &v) -> value
	    {
		    return lak::visit(
		      v,
		      lak::overloaded{
		        [&](const lak::JSON::subspan &str) -> value
		        {
			        return value::make<value::index_of<lak::u8string_view>>(
			          str(lak::string_view(lak::span(_parser._string_data)))
			            .unwrap());
		        },
		        [](const nullptr_t &) -> value
		        { return value::make<value::index_of<nullptr_t>>(nullptr); },
		        [](const bool &v) -> value
		        { return value::make<value::index_of<bool>>(v); },
		        [](const uintmax_t &v) -> value
		        { return value::make<value::index_of<uintmax_t>>(v); },
		        [](const intmax_t &v) -> value
		        { return value::make<value::index_of<intmax_t>>(v); },
		        [](const double &v) -> value
		        { return value::make<value::index_of<double>>(v); },
		        [&](const object &o) -> value
		        {
			        return value::make<value::index_of<object_proxy>>(
			          object_proxy(_parser, o));
		        },
		        [&](const array &a) -> value {
			        return value::make<value::index_of<array_proxy>>(
			          array_proxy(_parser, a));
		        }});
	    });
}

/* --- JSON --- */

bool lak::JSON::skip_ascii_whitespace()
{
	while (!empty() && lak::is_ascii_whitespace(_input[_position])) ++_position;
	return !empty();
};

lak::result<> lak::JSON::parse_string()
{
	if (peek() != u8'"') return lak::err_t{};
	++_position;

	subspan result{.offset = _string_data.size(), .count = 0};
	bool in_escape_char = false;

	while (!empty())
	{
		char8_t c = read();

		if (in_escape_char)
		{
			switch (c)
			{
				case u8'"': _string_data.push_back(u8'"'); break;
				case u8'\\': _string_data.push_back(u8'\\'); break;
				case u8'/': _string_data.push_back(u8'/'); break;
				case u8'b': _string_data.push_back(u8'\x08'); break;
				case u8'f': _string_data.push_back(u8'\x0C'); break;
				case u8'n': _string_data.push_back(u8'\x0A'); break;
				case u8'r': _string_data.push_back(u8'\x0D'); break;
				case u8't': _string_data.push_back(u8'\x09'); break;
				case u8'u':
				{
					char16_t hex_char[2] = {0, 0};

					for (size_t i = 0; i < 4; ++i)
					{
						if (empty()) return lak::err_t{};
						if_let_ok (uint8_t digit, lak::from_hex_alphanumeric(read()))
						{
							hex_char[0] <<= 4;
							hex_char[0] |= char16_t(digit);
						}
						else return lak::err_t{};
					}

					if (hex_char[0] >= 0xD800 && hex_char[0] < 0xE000)
					{
						if (empty()) return lak::err_t{};
						if (read() != u8'\\') return lak::err_t{};

						if (empty()) return lak::err_t{};
						if (read() != u8'u') return lak::err_t{};

						for (size_t i = 0; i < 4; ++i)
						{
							if (empty()) return lak::err_t{};
							if_let_ok (uint8_t digit, lak::from_hex_alphanumeric(read()))
							{
								hex_char[1] <<= 4;
								hex_char[1] |= char16_t(digit);
							}
							else return lak::err_t{};
						}
					}

					char32_t codepoint =
					  lak::codepoint(lak::string_view(lak::span(hex_char)));

					if (codepoint == 0 && hex_char[0] != 0) return lak::err_t{};

					char8_t buffer[lak::chars_per_codepoint_v<char8_t>];
					for (const auto hex_c : lak::from_codepoint(
					       lak::codepoint_buffer(lak::span(buffer)), codepoint))
					{
						_string_data.push_back(hex_c);
					}
				}
				break;
				default: return lak::err_t{};
			}
			in_escape_char = false;
		}
		else if (c == u8'\\')
		{
			in_escape_char = true;
		}
		else if (c == u8'"')
		{
			result.count = _string_data.size() - result.offset;
			_data.push_back(value_type::make<value_type::index_of<subspan>>(result));
			return lak::ok_t{};
		}
		else
		{
			_string_data.push_back(c);
		}
	}

	return lak::err_t{};
}

lak::result<> lak::JSON::parse_null()
{
	// "null"
	if (lak::compare(remaining(), u8"null"_view) == 4)
	{
		_position += 4;
		_data.push_back(
		  value_type::make<value_type::index_of<nullptr_t>>(nullptr));
		return lak::ok_t{};
	}

	return lak::err_t{};
}

lak::result<> lak::JSON::parse_bool()
{
	// "true"
	if (lak::compare(remaining(), u8"true"_view) == 4)
	{
		_position += 4;
		_data.push_back(value_type::make<value_type::index_of<bool>>(true));
		return lak::ok_t{};
	}

	// "false"
	if (lak::compare(remaining(), u8"false"_view) == 5)
	{
		_position += 5;
		_data.push_back(value_type::make<value_type::index_of<bool>>(false));
		return lak::ok_t{};
	}

	return lak::err_t{};
}

lak::result<> lak::JSON::parse_number()
{
	struct json_number
	{
		bool negative_integer  = false;
		bool negative_exponent = false;
		uintmax_t integer      = 0;
		uintmax_t exponent     = 0;

		double to_double(double pos_fraction = 0.0)
		{
			return (negative_integer ? -1.0 : 1.0) *
			       ((double(integer) + pos_fraction) *
			        std::pow(10.0,
			                 (negative_exponent ? -1.0 : 1.0) * double(exponent)));
		}
	};

	lak::optional<double> fraction;
	json_number result;

	auto finish = [&]() -> lak::result<>
	{
		if (fraction)
		{
			// double
			_data.push_back(
			  lak::JSON::value_type::make<lak::JSON::value_type::index_of<double>>(
			    result.to_double(*fraction)));
			return lak::ok_t{};
		}
		else if (result.exponent == 0)
		{
			if (!result.negative_integer)
			{
				// uintmax_t
				_data.push_back(
				  lak::JSON::value_type::make<
				    lak::JSON::value_type::index_of<uintmax_t>>(result.integer));
				return lak::ok_t{};
			}
			else
			{
				if (result.integer <= INTMAX_MAX)
				{
					// intmax_t
					_data.push_back(lak::JSON::value_type::make<
					                lak::JSON::value_type::index_of<intmax_t>>(
					  -intmax_t(result.integer)));
					return lak::ok_t{};
				}
				else
				{
					// double
					_data.push_back(lak::JSON::value_type::make<
					                lak::JSON::value_type::index_of<double>>(
					  -double(result.integer)));
					return lak::ok_t{};
				}
			}
		}
		else
		{
			// double
			_data.push_back(
			  lak::JSON::value_type::make<lak::JSON::value_type::index_of<double>>(
			    result.to_double()));
			return lak::ok_t{};
		}
	};

	auto parse_simple_integer = [&]() -> lak::result<uintmax_t>
	{
		// ([0-9]+)

		uintmax_t result = 0;

		auto v = lak::from_alphanumeric(read());

		if (v.is_err())
			return lak::err_t{}; // not alphanumeric
		else
			result = uintmax_t(v.unsafe_unwrap());

		while (!empty())
		{
			if_let_ok (const uintmax_t new_digit, lak::from_alphanumeric(peek()))
			{
				++_position;

				const uintmax_t new_result = result * 10;
				if (uintmax_t(new_result / 10) != result)
					return lak::err_t{}; // integer too large

				if (new_digit > (UINTMAX_MAX - new_result))
					return lak::err_t{}; // integer too large

				result = new_result + new_digit;
			}
			else break; // not alphanumeric
		}

		return lak::ok_t<uintmax_t>{result};
	};

	auto parse_fraction = [&]() -> lak::result<double>
	{
		size_t pos = _position;

		// :TODO: this should probably handle "integer too large" a little more
		// gracefully when we're dealing with floats anyway.
		if_let_ok (uintmax_t integer, parse_simple_integer())
		{
			uintmax_t chars_read = _position - pos;
			return lak::ok_t<double>{double(integer) *
			                         std::pow(10.0, -double(chars_read))};
		}
		else return lak::err_t{};
	};

	// -?([1-9][0-9]*|0)(\.[0-9]+)?([eE][\+\-]?[0-9]+)?

	if (peek() == u8'-')
	{
		// -([1-9][0-9]*|0)(\.[0-9]+)?([eE][\+\-]?[0-9]+)?

		++_position;
		if (empty()) return lak::err_t{};

		result.negative_integer = true;
	}

	// ([1-9][0-9]*|0)(\.[0-9]+)?([eE][\+\-]?[0-9]+)?

	if (peek() == u8'0')
	{
		// (0)(\.[0-9]+)?([eE][\+\-]?[0-9]+)?

		++_position;

		// should be no integer values after the leading 0
		if (!empty() && lak::from_alphanumeric(peek()).is_ok())
			return lak::err_t{};
	}
	else
	{
		// ([1-9][0-9]*)(\.[0-9]+)?([eE][\+\-]?[0-9]+)?
		// ~= ([0-9]+)(\.[0-9]+)?([eE][\+\-]?[0-9]+)?

		if_let_ok (uintmax_t integer_part, parse_simple_integer())
			result.integer = integer_part;
		else return lak::err_t{};
	}

	if (empty()) return finish();

	// (\.[0-9]+)?([eE][\+\-]?[0-9]+)?

	if (peek() == u8'.')
	{
		// (\.[0-9]+)([eE][\+\-]?[0-9]+)?

		++_position;
		if (empty()) return lak::err_t{};

		if_let_ok (double fraction_part, parse_fraction())
			fraction.emplace(fraction_part);
		else return lak::err_t{};

		if (empty()) return finish();
	}

	// ([eE][\+\-]?[0-9]+)?

	if (auto c = peek(); c == u8'e' || c == u8'E')
	{
		// ([eE][\+\-]?[0-9]+)

		++_position;
		if (empty()) return lak::err_t{};

		if (c = peek(); c == u8'+' || c == u8'-')
		{
			// ([\+\-][0-9]+)
			++_position;
			if (empty()) return lak::err_t{};

			result.negative_exponent = c == u8'-';
		}

		// ([0-9]+)

		if_let_ok (uintmax_t exponent_part, parse_simple_integer())
			result.exponent = exponent_part;
		else return lak::err_t{};
	}

	return finish();
}

lak::result<> lak::JSON::parse_literal_value()
{
	switch (peek())
	{
		case u8'"': return parse_string();
		case u8'n': return parse_null();
		case u8't': [[fallthrough]];
		case u8'f': return parse_bool();
		default: return parse_number();
	}
}

[[maybe_unused]] auto bool_result = [](bool value) -> lak::result<>
{
	if (value)
		return lak::ok_t{};
	else
		return lak::err_t{};
};

template<size_t I, typename V>
auto get_variant(V &variant)
  -> lak::result<decltype(*variant.template get<I>())>
{
	if (auto *v = variant.template get<I>(); v) return lak::ok_t{*v};
	return lak::err_t{};
}

lak::result<> lak::JSON::parse_object()
{
	auto parse_object_pair = [&]() -> lak::result<>
	{
		auto &this_object =
		  *(_data.push_back(value_type::make<value_type::index_of<object>>())
		      .get<value_type::index_of<object>>());
		this_object.next.value = 0;

		if (!skip_ascii_whitespace()) return lak::err_t{};

		if (peek() == u8'}')
		{
			this_object.key.value   = 0;
			this_object.value.value = 0;
			return lak::ok_t{};
		}

		this_object.key.value = _data.size();

		if (parse_string().is_err()) return lak::err_t{};

		if (!skip_ascii_whitespace()) return lak::err_t{};

		if (read() != u8':') return lak::err_t{};

		this_object.value.value = _data.size();

		return lak::ok_t{};
	};

	if (peek() == u8'{')
	{
		// new object

		++_position;

		_state.push_back(index{.value = _data.size()});

		return parse_object_pair();
	}

	if (peek() == u8',')
	{
		// continuataion

		++_position;

		if (_state.empty() ||
		    _state.back()(lak::span(_data))
		      .and_then([](value_type &var) -> lak::result<object &>
		                { return get_variant<value_type::index_of<object>>(var); })
		      .if_ok([&](object &o) { o.next.value = _data.size(); })
		      .is_err())
			return lak::err_t{};

		_state.back().value = _data.size();

		return parse_object_pair();
	}

	if (peek() == u8'}')
	{
		// end of object

		++_position;

		_state.pop_back();

		return lak::ok_t{};
	}

	return lak::err_t{};
}

lak::result<> lak::JSON::parse_array()
{
	auto parse_array_value = [&]() -> lak::result<>
	{
		auto &this_array =
		  *(_data.push_back(value_type::make<value_type::index_of<array>>())
		      .get<value_type::index_of<array>>());
		this_array.next.value = 0;

		if (!skip_ascii_whitespace()) return lak::err_t{};

		if (peek() == u8']')
			this_array.value.value = 0;
		else
			this_array.value.value = _data.size();

		return lak::ok_t{};
	};

	if (peek() == u8'[')
	{
		// new object

		++_position;

		_state.push_back(index{.value = _data.size()});

		return parse_array_value();
	}

	if (peek() == u8',')
	{
		// continuataion

		++_position;

		if (_state.empty() ||
		    _state.back()(lak::span(_data))
		      .and_then([](value_type &var) -> lak::result<array &>
		                { return get_variant<value_type::index_of<array>>(var); })
		      .if_ok([&](array &a) { a.next.value = _data.size(); })
		      .is_err())
			return lak::err_t{};

		_state.back().value = _data.size();

		return parse_array_value();
	}

	if (peek() == u8']')
	{
		// end of array

		_state.pop_back();

		return lak::ok_t{};
	}

	return lak::err_t{};
}

lak::result<> lak::JSON::parse_value()
{
	do
	{
		if (!skip_ascii_whitespace()) return lak::err_t{};

		switch (peek())
		{
			case u8'{': [[fallthrough]];
			case u8'}':
				if (auto err = parse_object(); err.is_err()) return err;
				break;

			case u8'[': [[fallthrough]];
			case u8']':
				if (auto err = parse_array(); err.is_err()) return err;
				break;

			case u8',':
				if_let_ok (auto &v, _state.back()(lak::span(_data)))
				{
					switch (v.index())
					{
						case value_type::index_of<object>:
							if (auto err = parse_object(); err.is_err()) return err;
							break;

						case value_type::index_of<array>:
							if (auto err = parse_array(); err.is_err()) return err;
							break;

						default: return lak::err_t{};
					}
				}
				else return lak::err_t{};
				break;

			default:
				if (auto err = parse_literal_value(); err.is_err()) return err;
				break;
		}
	} while (!_state.empty());

	return lak::ok_t{};
}

lak::result<> lak::JSON::parse_value(lak::u8string_view str)
{
	_position = 0;
	_input    = str;
	_string_data.clear();
	_data.clear();
	_state.clear();

	return parse_value();
}

lak::result<lak::JSON> lak::JSON::parse(lak::u8string_view str)
{
	JSON parser;
	if (parser.parse_value(str).is_ok())
		return lak::ok_t{lak::move(parser)};
	else
		return lak::err_t{};
}

lak::result<lak::JSON::value_proxy> lak::JSON::value() const
{
	if (_data.empty()) return lak::err_t{};
	return lak::ok_t{value_proxy(*this, index{.value = 0})};
}
