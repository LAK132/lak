#include "lak/file/json.hpp"

void lak::json::block::intern()
{
	// make repeated calls to intern safe
	auto old_internal = lak::exchange(_internal, {});

	size_t alloc_size = 0U;
	for (const auto &t : tokens()) alloc_size += t.value.size();
	for (const auto &s : strings()) alloc_size += s.value.size();
	for (const auto &n : numbers()) alloc_size += n.value.size();
	_internal.reserve(alloc_size);
	auto internalise = [&](lak::u8string_view &view)
	{
		view = lak::u8string_view(_internal.push_back(view.begin(), view.end()));
	};
	for (auto &t : tokens()) internalise(t.value);
	for (auto &s : strings()) internalise(s.value);
	for (auto &n : numbers()) internalise(n.value);
}

lak::json::value_proxy lak::json::block::root() const
{
	ASSERT(!values().empty());
	return lak::json::value_proxy{.block = _value, .value = values().back()};
}

lak::json::value_proxy lak::json::array_proxy::operator[](size_t index) const
{
	BOUNDS_ASSERT_LESS(index, size());
	return lak::json::value_proxy{.block = block,
	                              .value = block[array.values][index]};
}

lak::pair<lak::u8string_view, lak::json::value_proxy>
lak::json::object_proxy::operator[](size_t index) const
{
	BOUNDS_ASSERT_LESS(object.values.begin.index + object.values.size,
	                   block.get<lak::json::object>().size());
	lak::span<const value> subspan = block[object.values];
	BOUNDS_ASSERT_LESS(index, subspan.size());
	const value &k_v = subspan[(index * 2U) + 0U];
	const value &v_v = subspan[(index * 2U) + 1U];
	lak::u8string_view k;
	block[k_v.value].visit(lak::overloaded{
	  [&k](const token &kv) { k = kv.value; },
	  [&k](const string &kv) { k = kv.value; },
	  [&k](const number &kv) { k = kv.value; },
	  [](auto &&) { ASSERT_UNREACHABLE(); },
	});
	return {k, lak::json::value_proxy{.block = block, .value = v_v}};
}

lak::optional<lak::json::value_proxy> lak::json::object_proxy::operator[](
  lak::u8string_view key) const
{
	BOUNDS_ASSERT_LESS(object.values.begin.index + object.values.size,
	                   block.get<lak::json::value>().size());
	lak::span<const value> subspan = block[object.values];
	for (size_t i = 0U; i < size(); ++i)
	{
		const value &k_v = subspan[(i * 2U) + 0U];
		const value &v_v = subspan[(i * 2U) + 1U];
		lak::u8string_view k;
		block[k_v.value].visit(lak::overloaded{
		  [&k](const token &kv) { k = kv.value; },
		  [&k](const string &kv) { k = kv.value; },
		  [&k](const number &kv) { k = kv.value; },
		  [](auto &&) { ASSERT_UNREACHABLE(); },
		});
		if (k == key) return lak::json::value_proxy{.block = block, .value = v_v};
	}
	return lak::nullopt;
}

bool lak::json::value_proxy::is_token() const
{
	return value.value.type_index.value() ==
	       lak::json::_block::index_of<lak::json::token>;
}

bool lak::json::value_proxy::is_string() const
{
	return value.value.type_index.value() ==
	       lak::json::_block::index_of<lak::json::string>;
}

bool lak::json::value_proxy::is_number() const
{
	return value.value.type_index.value() ==
	       lak::json::_block::index_of<lak::json::number>;
}

bool lak::json::value_proxy::is_array() const
{
	return value.value.type_index.value() ==
	       lak::json::_block::index_of<lak::json::array>;
}

bool lak::json::value_proxy::is_object() const
{
	return value.value.type_index.value() ==
	       lak::json::_block::index_of<lak::json::object>;
}

lak::json::value_proxy::result_type<lak::u8string_view>
lak::json::value_proxy::token() const
{
	if (is_token())
		return lak::ok_t{block.get<lak::json::token>()[value.value.index].value};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected =
		    lak::size_type<lak::json::_block::index_of<lak::json::token>>{},
		  .got = value.value.type_index,
		}};
}

lak::json::value_proxy::result_type<lak::u8string_view>
lak::json::value_proxy::string() const
{
	if (is_string())
		return lak::ok_t{block.get<lak::json::string>()[value.value.index].value};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected =
		    lak::size_type<lak::json::_block::index_of<lak::json::string>>{},
		  .got = value.value.type_index,
		}};
}

lak::json::value_proxy::result_type<lak::u8string_view>
lak::json::value_proxy::number_str() const
{
	if (is_number())
		return lak::ok_t{block.get<lak::json::number>()[value.value.index].value};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected =
		    lak::size_type<lak::json::_block::index_of<lak::json::number>>{},
		  .got = value.value.type_index,
		}};
}

lak::json::value_proxy::result_type<lak::json::array_proxy>
lak::json::value_proxy::array() const
{
	if (is_array())
		return lak::ok_t{lak::json::array_proxy{
		  .block = block,
		  .array = *block[value.value].get<const lak::json::array &>()}};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected =
		    lak::size_type<lak::json::_block::index_of<lak::json::array>>{},
		  .got = value.value.type_index,
		}};
}

lak::json::value_proxy::result_type<lak::json::object_proxy>
lak::json::value_proxy::object() const
{
	if (is_object())
		return lak::ok_t{lak::json::object_proxy{
		  .block  = block,
		  .object = *block[value.value].get<const lak::json::object &>()}};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected =
		    lak::size_type<lak::json::_block::index_of<lak::json::object>>{},
		  .got = value.value.type_index,
		}};
}

lak::dsl::result<lak::dsl::json_t::value_type> lak::dsl::json_t::parse(
  lak::u8string_view str) const
{
	lak::u8string_view rem = str;

	lak::json::block result;

	auto move_str = [&](const lak::dsl::parse_result<lak::u8string_view> &res)
	{ rem = res.remaining; };

	constexpr auto lbrace     = lak::dsl::char_literal<U'{'>;
	constexpr auto rbrace     = lak::dsl::char_literal<U'}'>;
	constexpr auto lbracket   = lak::dsl::char_literal<U'['>;
	constexpr auto rbracket   = lak::dsl::char_literal<U']'>;
	constexpr auto colon      = lak::dsl::char_literal<U':'>;
	constexpr auto comma      = lak::dsl::char_literal<U','>;
	constexpr auto whitespace = lak::dsl::one_of_chars_str<U" \r\n\t">;
	constexpr auto number_parser =
	  lak::dsl::as_pure<lak::dsl::dec_float<lak::dsl::char_literal<U'.'>,
	                                        lak::dsl::one_of_chars_str<U"eE">>>;
	constexpr auto escaped_string_character =
	  lak::dsl::str_literal<u8"\\\""> | lak::dsl::str_literal<u8"\\\\"> |
	  lak::dsl::str_literal<u8"\\/"> | lak::dsl::str_literal<u8"\\b"> |
	  lak::dsl::str_literal<u8"\\f"> | lak::dsl::str_literal<u8"\\n"> |
	  lak::dsl::str_literal<u8"\\r"> | lak::dsl::str_literal<u8"\\t"> |
	  (lak::dsl::str_literal<u8"\\u"> +
	   (lak::dsl::repeat_exact<lak::dsl::hex_digit, 4>));
	constexpr auto string_parser =
	  lak::dsl::capture_2nd<lak::dsl::char_literal<U'"'>,
	                        *(escaped_string_character |
	                          !lak::dsl::char_literal<U'"'>),
	                        lak::dsl::char_literal<U'"'>>;
	constexpr auto token_parser = lak::dsl::str_literal<u8"true"> |
	                              lak::dsl::str_literal<u8"false"> |
	                              lak::dsl::str_literal<u8"null">;

	RES_TRY((*whitespace).parse(rem).if_ok(move_str));

	if (rem.empty())
		return lak::ok_t<lak::dsl::parse_result<value_type>>{{
		  .consumed  = str.substr(str.size() - rem.size()),
		  .remaining = rem,
		  .value     = {},
		}};

	struct working_data
	{
		enum struct value_type
		{
			array,
			object,
			kvpair
		} type;

		size_t begin;
		size_t size;
	};

	lak::array<lak::json::value> working_values;
	lak::array<working_data> working_tree;

	auto pop_values = [&](size_t count) -> size_t
	{
		size_t begin = result.values().size();
		result.values().reserve(result.values().size() + count);
		for (size_t i = working_values.size() - count; i < working_values.size();
		     ++i)
			result.values().push_back(working_values[i]);
		working_values.erase(working_values.begin() +
		                       (working_values.size() - count),
		                     working_values.end());
		return begin;
	};

	auto pop_kvpair =
	  [&](bool allow_empty) -> lak::error_code<lak::dsl::err::parse>
	{
		if (working_tree.back().size == 2U)
		{
			working_tree.pop_back();
			working_tree.back().size += 2U;
		}
		else if (allow_empty && working_tree.back().size == 0U)
			working_tree.pop_back();
		else if (working_tree.back().size == 1U)
			return lak::err_t{u8"expected kvpair value"_str};
		else
			return lak::err_t{u8"invalid kvpair length"_str};

		return lak::ok_t{};
	};

	do
	{
		RES_TRY((*whitespace).parse(rem).if_ok(move_str));

		if (rem.empty()) return lak::err_t{u8"out of data"_str};

		if (lbrace.parse(rem).if_ok(move_str).is_ok())
		{
			working_tree.push_back({
			  .type  = working_data::value_type::object,
			  .begin = working_values.size(),
			  .size  = 0U,
			});
			working_tree.push_back({
			  .type  = working_data::value_type::kvpair,
			  .begin = working_values.size(),
			  .size  = 0U,
			});
		}
		else if (colon.parse(rem).if_ok(move_str).is_ok())
		{
			if (working_tree.back().type != working_data::value_type::kvpair)
				return lak::err_t{u8"unexpected ':'"_str};

			if (working_tree.back().begin + 1U > working_values.size())
				return lak::err_t{u8"expected kvpair key, got ':'"_str};

			if (working_tree.back().size >= 1U)
				return lak::err_t{u8"expected kvpair value, got ':'"_str};

			++working_tree.back().size;
		}
		else if (comma.parse(rem).if_ok(move_str).is_ok())
		{
			if (working_tree.back().type == working_data::value_type::array)
			{
				if (working_tree.back().begin + working_tree.back().size >=
				    working_values.size())
					return lak::err_t{u8"expected array value, got ','"_str};

				++working_tree.back().size;
			}
			else if (working_tree.back().type == working_data::value_type::kvpair)
			{
				if (working_tree.back().size == 0U)
					return lak::err_t{u8"expected kvpair key, got ','"_str};

				if (working_tree.back().begin + 2U > working_values.size())
					return lak::err_t{u8"expected kvpair value, got ','"_str};

				++working_tree.back().size;

				RES_TRY(pop_kvpair(false));

				working_tree.push_back({
				  .type  = working_data::value_type::kvpair,
				  .begin = working_values.size(),
				  .size  = 0U,
				});
			}
			else
				return lak::err_t{u8"unexpected ','"_str};
		}
		else if (rbrace.parse(rem).if_ok(move_str).is_ok())
		{
			if (working_tree.back().type != working_data::value_type::kvpair ||
			    working_tree.back().size >= 2U)
				return lak::err_t{u8"unexpected '}'"_str};

			if (working_tree.back().size == 1U)
			{
				if (working_tree.back().begin + 2U != working_values.size())
					return lak::err_t{u8"expected kvpair value, got ','"_str};
				else
					++working_tree.back().size;
			}

			RES_TRY(pop_kvpair(true));

			if (working_tree.back().type != working_data::value_type::object)
				return lak::err_t{u8"unexpected '}'"_str};

			size_t begin = pop_values(working_tree.back().size);
			working_values.push_back({
			  .value = lak::json::value::value_type::make<lak::json::object>(
			    result.objects().size()),
			});
			result.objects().push_back({
			  .values =
			    lak::json::object::value_type::make(begin, working_tree.back().size),
			});
			working_tree.pop_back();
		}
		else if (lbracket.parse(rem).if_ok(move_str).is_ok())
		{
			working_tree.push_back({
			  .type  = working_data::value_type::array,
			  .begin = working_values.size(),
			  .size  = 0U,
			});
		}
		else if (rbracket.parse(rem).if_ok(move_str).is_ok())
		{
			if (working_tree.back().type != working_data::value_type::array)
				return lak::err_t{u8"unexpected ']'"_str};

			if (working_tree.back().begin + working_tree.back().size + 1U ==
			    working_values.size())
				++working_tree.back().size;

			size_t begin = pop_values(working_tree.back().size);
			working_values.push_back({
			  .value = lak::json::value::value_type::make<lak::json::array>(
			    result.arrays().size()),
			});
			result.arrays().push_back({
			  .values =
			    lak::json::array::value_type::make(begin, working_tree.back().size),
			});
			working_tree.pop_back();
		}
		else if_let_ok (auto tok, token_parser.parse(rem).if_ok(move_str))
		{
			working_values.push_back({
			  .value = lak::json::value::value_type::make<lak::json::token>(
			    result.tokens().size()),
			});
			result.tokens().push_back({.value = tok.value});
		}
		else if_let_ok (auto num, number_parser.parse(rem).if_ok(move_str))
		{
			working_values.push_back({
			  .value = lak::json::value::value_type::make<lak::json::number>(
			    result.numbers().size()),
			});
			result.numbers().push_back({.value = num.value});
		}
		else if_let_ok (auto str, string_parser.parse(rem).if_ok(move_str))
		{
			working_values.push_back({
			  .value = lak::json::value::value_type::make<lak::json::string>(
			    result.strings().size()),
			});
			result.strings().push_back({.value = str.value});
		}
		else
			return lak::err_t{u8"unexpected data"_str};
	} while (!working_tree.empty());

	if (working_values.size() == 0U)
		return lak::err_t{u8"missing root value"_str};
	else if (working_values.size() > 1U)
		return lak::err_t{u8"unused working values"_str};

	result.values().push_back(working_values.back());

	return lak::ok_t<lak::dsl::parse_result<value_type>>{{
	  .consumed  = str.substr(str.size() - rem.size()),
	  .remaining = rem,
	  .value     = lak::move(result),
	}};
}
