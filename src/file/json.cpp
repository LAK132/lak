#include "lak/file/json.hpp"

void lak::json::block::intern()
{
	// make repeated calls to intern safe
	auto old_internal = lak::exchange(_internal, {});

	size_t alloc_size = 0U;
	for (const auto &t : tokens) alloc_size += t.size();
	for (const auto &s : strings) alloc_size += s.size();
	for (const auto &n : numbers) alloc_size += n.size();
	_internal.reserve(alloc_size);
	auto internalise = [&](lak::u8string_view &view)
	{
		view = lak::u8string_view(_internal.push_back(view.begin(), view.end()));
	};
	for (auto &t : tokens) internalise(t);
	for (auto &s : strings) internalise(s);
	for (auto &n : numbers) internalise(n);
}

lak::json::value_proxy lak::json::block::root() const
{
	return lak::json::value_proxy{
	  .block = *this,
	  .value =
	    values.empty()
	      ? lak::json::value{.type =
	                           static_cast<lak::json::value::value_type>(-1),
	                         .index = static_cast<size_t>(-1)}
	      : values.front()};
}

lak::json::value_proxy lak::json::array_proxy::operator[](size_t index) const
{
	BOUNDS_ASSERT_LESS(index, size());
	return lak::json::value_proxy{.block = block,
	                              .value = block.values[array.begin + index]};
}

lak::pair<lak::u8string_view, lak::json::value_proxy>
lak::json::object_proxy::operator[](size_t index) const
{
	BOUNDS_ASSERT_LESS(index, size());
	const size_t ki = object.begin + (index * 2U);
	const size_t vi = ki + 1U;
	lak::u8string_view k;
	switch (block.values[ki].type)
	{
		case lak::json::value::value_type::token:
			k = block.tokens[block.values[ki].index];
			break;
		case lak::json::value::value_type::string:
			k = block.strings[block.values[ki].index];
			break;
		case lak::json::value::value_type::number:
			k = block.numbers[block.values[ki].index];
			break;
		default: ASSERT_UNREACHABLE();
	}
	return {k,
	        lak::json::value_proxy{.block = block, .value = block.values[vi]}};
}

lak::json::value_proxy lak::json::object_proxy::operator[](
  lak::u8string_view key) const
{
	for (size_t i = 0U; i < size(); ++i)
	{
		const size_t ki = object.begin + (i * 2U);
		const size_t vi = ki + 1U;
		lak::u8string_view k;
		switch (block.values[ki].type)
		{
			case lak::json::value::value_type::token:
				k = block.tokens[block.values[ki].index];
				break;
			case lak::json::value::value_type::string:
				k = block.strings[block.values[ki].index];
				break;
			case lak::json::value::value_type::number:
				k = block.numbers[block.values[ki].index];
				break;
			default: ASSERT_UNREACHABLE();
		}
		if (k == key)
			return lak::json::value_proxy{.block = block, .value = block.values[vi]};
	}
	return lak::json::value_proxy{
	  .block = block,
	  .value =
	    lak::json::value{.type  = static_cast<lak::json::value::value_type>(-1),
	                     .index = static_cast<size_t>(-1)}};
}

bool lak::json::value_proxy::is_none() const
{
	return value.type == static_cast<lak::json::value::value_type>(-1);
}

bool lak::json::value_proxy::is_token() const
{
	return value.type == lak::json::value::value_type::token;
}

bool lak::json::value_proxy::is_string() const
{
	return value.type == lak::json::value::value_type::string;
}

bool lak::json::value_proxy::is_number() const
{
	return value.type == lak::json::value::value_type::number;
}

bool lak::json::value_proxy::is_array() const
{
	return value.type == lak::json::value::value_type::array;
}

bool lak::json::value_proxy::is_object() const
{
	return value.type == lak::json::value::value_type::object;
}

lak::json::value_proxy::result_type<lak::u8string_view>
lak::json::value_proxy::token() const
{
	if (is_token())
		return lak::ok_t{block.tokens[value.index]};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected = lak::json::value::value_type::token, .got = value.type}};
}

lak::json::value_proxy::result_type<lak::u8string_view>
lak::json::value_proxy::string() const
{
	if (is_string())
		return lak::ok_t{block.strings[value.index]};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected = lak::json::value::value_type::string, .got = value.type}};
}

lak::json::value_proxy::result_type<lak::u8string_view>
lak::json::value_proxy::number_str() const
{
	if (is_number())
		return lak::ok_t{block.numbers[value.index]};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected = lak::json::value::value_type::number, .got = value.type}};
}

lak::json::value_proxy::result_type<lak::json::array_proxy>
lak::json::value_proxy::array() const
{
	if (is_array())
		return lak::ok_t{lak::json::array_proxy{
		  .block = block, .array = block.arrays[value.index]}};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected = lak::json::value::value_type::array, .got = value.type}};
}

lak::json::value_proxy::result_type<lak::json::object_proxy>
lak::json::value_proxy::object() const
{
	if (is_object())
		return lak::ok_t{lak::json::object_proxy{
		  .block = block, .object = block.objects[value.index]}};
	else
		return lak::err_t{lak::json::err::unexpected_type{
		  .expected = lak::json::value::value_type::object, .got = value.type}};
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
		size_t begin = result.values.size();
		result.values.reserve(result.values.size() + count);
		for (size_t i = working_values.size() - count; i < working_values.size();
		     ++i)
			result.values.push_back(working_values[i]);
		working_values.resize(working_values.size() - count);
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

	// this will be replaced with the last value in the working values once
	// parsing is complete
	result.values.emplace_back();

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
			  .type  = lak::json::value::value_type::object,
			  .index = result.objects.size(),
			});
			result.objects.push_back({
			  .begin = begin,
			  .end   = begin + working_tree.back().size,
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
			  .type  = lak::json::value::value_type::array,
			  .index = result.arrays.size(),
			});
			result.arrays.push_back({
			  .begin = begin,
			  .end   = begin + working_tree.back().size,
			});
			working_tree.pop_back();
		}
		else if_let_ok (auto tok, token_parser.parse(rem).if_ok(move_str))
		{
			working_values.push_back({
			  .type  = lak::json::value::value_type::token,
			  .index = result.tokens.size(),
			});
			result.tokens.push_back(tok.value);
		}
		else if_let_ok (auto num, number_parser.parse(rem).if_ok(move_str))
		{
			working_values.push_back({
			  .type  = lak::json::value::value_type::number,
			  .index = result.numbers.size(),
			});
			result.numbers.push_back(num.value);
		}
		else if_let_ok (auto str, string_parser.parse(rem).if_ok(move_str))
		{
			working_values.push_back({
			  .type  = lak::json::value::value_type::string,
			  .index = result.strings.size(),
			});
			result.strings.push_back(str.value);
		}
		else
			return lak::err_t{u8"unexpected data"_str};
	} while (!working_tree.empty());

	if (working_values.size() == 0U)
		return lak::err_t{u8"missing root value"_str};
	else if (working_values.size() > 1U)
		return lak::err_t{u8"unused working values"_str};

	result.values.front() = working_values.back();

	return lak::ok_t<lak::dsl::parse_result<value_type>>{{
	  .consumed  = str.substr(str.size() - rem.size()),
	  .remaining = rem,
	  .value     = lak::move(result),
	}};
}
