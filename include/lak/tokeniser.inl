#include "lak/char_utils.hpp"
#include "lak/defer.hpp"
#include "lak/string_view.hpp"
#include "lak/tokeniser.hpp"

#include <tuple>

/* --- codepoint_position --- */

template<typename CHAR>
lak::string_view<CHAR> lak::codepoint_position::find(
  lak::string_view<CHAR> str) const
{
	ASSERT_GREATER(line, 0U);
	ASSERT_GREATER(column, 0U);

	for (size_t l = line; --l > 0;)
	{
		for (const auto &[c, len] : lak::codepoint_range(str))
		{
			str = str.substr(len);
			if (c == U'\n') break;
		}
	}

	size_t col = 0;
	for (const auto &[c, len] : lak::codepoint_range(str))
	{
		if (++col == column) break;
		ASSERT(c != U'\n');
		str = str.substr(len);
	}

	return str;
}

/* --- token --- */

template<typename CHAR>
bool lak::token<CHAR>::operator==(const token &other) const
{
	return source == other.source && position == other.position;
}

template<typename CHAR>
bool lak::token<CHAR>::operator!=(const token &other) const
{
	return !operator==(other);
}

template<typename CHAR>
template<size_t I>
auto &lak::token<CHAR>::get()
{
	if constexpr (I == 0)
		return source;
	else if constexpr (I == 1)
		return position;
}

template<typename CHAR>
template<size_t I>
const auto &lak::token<CHAR>::get() const
{
	if constexpr (I == 0)
		return source;
	else if constexpr (I == 1)
		return position;
}

/* --- token_buffer --- */

template<typename CHAR>
lak::token_buffer<CHAR>::token_buffer(lak::string_view<CHAR> data,
                                      size_t max_size,
                                      const lak::token<CHAR> &start)
: _source(data),
  _max_size(
    std::min(max_size, lak::span(start.source.begin(), data.end()).size()))
{
	_buffer.reserve(_max_size);
	_buffer_token.source   = start.source.first(0);
	_buffer_token.position = {start.position.begin, start.position.begin};

	// Prefill up to _max_size characters into the buffer.
	while (_buffer.size() < _max_size) ++*this;
}

template<typename CHAR>
lak::u32string_view lak::token_buffer<CHAR>::buffer() const
{
	return lak::string_view(lak::span(_buffer));
}

template<typename CHAR>
lak::string_view<CHAR> lak::token_buffer<CHAR>::source() const
{
	return _buffer_token.source;
}

template<typename CHAR>
const lak::token_position &lak::token_buffer<CHAR>::position() const
{
	return _buffer_token.position;
}

template<typename CHAR>
std::optional<lak::token<CHAR>> lak::token_buffer<CHAR>::match(
  lak::span<const lak::u32string> operators, const lak::token<CHAR> &token)
{
	for (const auto &op : operators)
	{
		ASSERT(!op.empty());

		if (_buffer.size() < op.size()) continue;

		if (lak::string_view(lak::span(_buffer)).first(op.size()) ==
		    lak::string_view(op))
		{
			// :TRICKY: token.position.end may not be correct here!

			// token.source is tracking the entire length of the current token,
			// whereas buffer_source_begin is only tracking where the next
			// operator might be starting from. This means that
			// buffer_source_begin can be between token.source.begin() and
			// token.source.end() if _longest_operator is less than
			// token.source.size(). This also means that buffer_source_begin can
			// be before token.source.begin() if _longest_operator is greater
			// than token.source.size() (this should only happen while skipping
			// whitespace at the start of the token).
			if (token.source.begin() < _buffer_token.source.begin())
			{
				// We hit the start of an operator part way through a token.
				// Return the token up to the operator.

				return lak::token<CHAR>{
				  lak::string_view(token.source.begin(), _buffer_token.source.begin()),
				  lak::token_position{token.position.begin,
				                      _buffer_token.position.begin}};
			}
			else
			{
				// _buffer_token.source.begin() <= token.source.begin()

				// We matched an operator.

				lak::codepoint_position end_position = _buffer_token.position.begin;

				size_t converted_length = 0;
				for (const auto &c : op)
				{
					converted_length += lak::codepoint_length<CHAR>(c);
					end_position += c;
				}

				if (_buffer_token.source.begin() == token.source.begin())
					ASSERT_EQUAL(_buffer_token.position.begin, token.position.begin);

				return lak::token<CHAR>{
				  lak::string_view(_buffer_token.source.begin(), converted_length),
				  lak::token_position{_buffer_token.position.begin, end_position}};
			}
		}
	}

	// No matches.
	return std::nullopt;
}

template<typename CHAR>
void lak::token_buffer<CHAR>::operator++()
{
	if (_max_size == 0) return;

	// slow as fuck
	// ASSERT_EQUAL(_buffer_token.source.begin(),
	//              _buffer_token.position.begin.find(_source).begin());

	auto pre_buffer       = _buffer; // this is a full buffer copy.
	auto pre_buffer_token = _buffer_token;

	auto remaining = lak::string_view(_buffer_token.source.end(), _source.end());

	auto next_len            = lak::character_length(remaining);
	const char32_t next_code = lak::codepoint(remaining);

	ASSERT_GREATER_OR_EQUAL(_max_size, _buffer.size());
	if (_buffer.size() == _max_size) --*this;

	if (remaining.size() == 0) return;
	ASSERT_GREATER(next_len, 0);

	_buffer.push_back(next_code);
	_buffer_token.source = lak::string_view(
	  _buffer_token.source.begin(), _buffer_token.source.end() + next_len);
	_buffer_token.position.end += next_code;

	if (pre_buffer.size() > 0)
	{
		auto u8buffer = lak::to_u8string(lak::string_view(lak::span(_buffer)));
		auto u8source = lak::to_u8string(_buffer_token.source);
		ASSERT_EQUAL(u8buffer, u8source);

		/* These test don't work when the buffers contain the same data despite
		referring to different parts of the input string.
		auto max_buffer = std::max(pre_buffer.size(), _buffer.size());
		auto buffcmp    = lak::compare<char32_t>(pre_buffer, _buffer);
		ASSERTF_GREATER(max_buffer,
		                buffcmp,
		                "'" << lak::to_astring(lak::span(pre_buffer)) << "' '"
		                    << lak::to_astring(lak::span(_buffer)) << "'");
		*/
		ASSERT(
		  !lak::same_span<CHAR>(pre_buffer_token.source, _buffer_token.source));
		ASSERT_NOT_EQUAL(pre_buffer_token.position, _buffer_token.position);
	}
}

template<typename CHAR>
void lak::token_buffer<CHAR>::operator--()
{
	ASSERT(_buffer.size());
	ASSERT(_buffer_token.source.size());

	// slow as fuck
	// ASSERT_EQUAL(_buffer_token.source.begin(),
	//              _buffer_token.position.begin.find(_source).begin());

	const char32_t front = _buffer.front();

	ASSERT_EQUAL(lak::codepoint_length<CHAR>(front),
	             lak::character_length(_buffer_token.source));

	_buffer.erase(_buffer.begin());

	_buffer_token.source =
	  _buffer_token.source.substr(lak::codepoint_length<CHAR>(front));

	_buffer_token.position.begin += front;

	// ASSERT_EQUAL(_buffer_token.source.begin(),
	//              _buffer_token.position.begin.find(_source).begin());
}

/* --- tokeniser --- */

template<typename CHAR>
lak::tokeniser<CHAR>::tokeniser(lak::string_view<CHAR> str,
                                lak::array<lak::u32string> operators) noexcept
: _data(str), _operators(lak::move(operators)), _longest_operator(0)
{
	for (auto it = _operators.begin(); it != _operators.end();)
	{
		if (it->empty())
		{
			WARNING("Empty operator (", it - _operators.begin(), ")");
			it = _operators.erase(it);
		}
		else
			++it;
	}

	for (const auto &op : _operators)
		if (op.size() > _longest_operator) _longest_operator = op.size();

	reset();
}

template<typename CHAR>
void lak::tokeniser<CHAR>::internal_pump() noexcept
{
	_current             = _next;
	_next.source         = lak::string_view(_next.source.end(), size_t(0));
	_next.position.begin = _next.position.end;

	ASSERT_EQUAL(_next.source.begin(), _current.source.end());
	ASSERT_EQUAL(_next.position.begin, _current.position.end);
}

template<typename CHAR>
lak::tokeniser<CHAR> &lak::tokeniser<CHAR>::operator++() noexcept
{
	peek();
	internal_pump();
	return *this;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::peek_char() const noexcept
{
	// Don't bother caching this.
	lak::token<CHAR> result{
	  .source{lak::string_view<CHAR>{_current.source.end(), _data.end()}},
	  .position{
	    .begin{_current.position.end},
	    .end{_current.position.end},
	  },
	};

	// the following causes unreachable code warnings:
	// for (const auto &[c, len] : lak::codepoint_range(result.source))
	// {
	// 	result.source = lak::string_view{result.source.begin(), len};
	// 	result.position.end += c;
	// 	break;
	// }

	if (lak::codepoint_range range{result.source}; range.begin() != range.end())
	{
		const auto &[c, len] = *range.begin();
		result.source        = lak::string_view{result.source.begin(), len};
		result.position.end += c;
	}

	return result;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::read_char() noexcept
{
	_next = peek_char();
	internal_pump();
	return _current;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::peek() const noexcept
{
	if (_next.source.size() > 0 || _next.source.begin() == _data.end())
		return _next;

	// _next is currently empty, move its end to the end of _data.
	_next.source = lak::string_view(_next.source.begin(), _data.end());

	ASSERT_EQUAL(_current.source.end(), _next.source.begin());

	lak::token_buffer<CHAR> buffer(_data, _longest_operator, _next);

	// Scan until we find the first non-whitespace character.
	lak::string_view<CHAR> previous_source;
	for (const auto &[c, len] : lak::codepoint_range(_next.source))
	{
		if (!lak::is_whitespace(c)) break;

		if (!buffer.source().empty())
		{
			ASSERT(!lak::same_span<CHAR>(previous_source, buffer.source()));

			ASSERT_EQUAL(_next.source.size(),
			             lak::string_view(buffer._buffer_token.source.begin(),
			                              buffer._source.end())
			               .size());

			if (auto match = buffer.match(_operators, _next); match)
			{
				if (_next.source.begin() == match->source.begin())
				{
					ASSERT_EQUAL(_next.position.begin, match->position.begin);
				}
				else
				{
					ASSERT_NOT_EQUAL(_next.position.begin, match->position.begin);
				}
				_next = *match;
				return _next;
			}
		}

		previous_source = buffer.source();
		++buffer;

		_next.position.begin += c;

		_next.source = _next.source.substr(len);
	}

	auto whitespace_begin = _next;
	DEFER(ASSERT_EQUAL(whitespace_begin.source.begin(), _next.source.begin()));
	DEFER(ASSERT_EQUAL(whitespace_begin.position.begin, _next.position.begin));

	_next.position.end = _next.position.begin;

	size_t count = 0;

	// Continue scanning until we run out of non-whitespace characters.
	for (const auto &[c, len] : lak::codepoint_range(_next.source))
	{
		if (lak::is_whitespace(c)) break;

		if (!buffer._buffer_token.source.empty())
			ASSERT_EQUAL(_next.source.begin() + count,
			             buffer._buffer_token.source.begin());
		if (auto match = buffer.match(_operators, _next); match)
		{
			if (_next.source.begin() == match->source.begin())
			{
				ASSERT_EQUAL(_next.position.begin, match->position.begin);
			}
			else
			{
				ASSERT_NOT_EQUAL(_next.position.begin, match->position.begin);
			}
			_next = *match;
			return _next;
		}

		auto before = buffer.source();
		++buffer;
		if (!buffer.source().empty())
			ASSERT(!lak::same_span<CHAR>(before, buffer.source()));

		_next.position.end += c;

		count += len;
	}

	_next.source = _next.source.first(count);

	// Flush out the buffer.
	while (buffer.buffer().size() > 0 &&
	       buffer.source().begin() < _next.source.end())
	{
		if (auto match = buffer.match(_operators, _next); match)
		{
			ASSERT_EQUAL(match->source.begin(), _next.source.begin());
			ASSERT_GREATER(match->source.end(), _next.source.begin());
			ASSERT_GREATER(_next.source.end(), match->source.end());
			_next = *match;
			return _next;
		}

		--buffer;
	}

	return _next;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::until(char32_t codepoint) noexcept
{
	_next.source = lak::string_view(_next.source.begin(), _data.end());

	ASSERT_EQUAL(_current.source.end(), _next.source.begin());

	size_t count = 0;

	for (const auto &[c, len] : lak::codepoint_range(_next.source))
	{
		if (c == codepoint) break;

		count += len;

		_next.position.end += c;
	}

	_next.source = _next.source.first(count);

	internal_pump();

	return _current;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::until(lak::u32string_view str) noexcept
{
	ASSERT(str.size() > 0);

	_next.source = lak::string_view(_next.source.begin(), _data.end());

	ASSERT_EQUAL(_current.source.end(), _next.source.begin());

	size_t count                = 0;
	size_t pre_match_count      = 0;
	size_t match_len            = 0;
	codepoint_position end_line = _next.position.end;

	if (str.size() <= _next.source.size())
	{
		for (const auto &[c, len] : lak::codepoint_range(_next.source))
		{
			count += len;

			end_line += c;

			ASSERT(match_len < str.size());

			if (c == str[match_len])
			{
				++match_len;
			}
			else
			{
				pre_match_count    = count;
				match_len          = 0;
				_next.position.end = end_line;
			}

			if (match_len == str.size()) break;
		}

		if (match_len != str.size())
		{
			// We never *fully* matched str.
			pre_match_count    = count;
			_next.position.end = end_line;
		}
	}
	else
	{
		// str is longer than the remaining data, so we can never find it.
		// Instead we just update the end position.
		for (const auto &[c, len] : lak::codepoint_range(_next.source))
		{
			_next.position.end += c;
		}
		pre_match_count = _next.source.size();
	}

	_next.source = _next.source.first(pre_match_count);

	internal_pump();

	return _current;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::skip(char32_t codepoint) noexcept
{
	_next.source = lak::string_view(_next.source.begin(), _data.end());

	ASSERT_EQUAL(_current.source.end(), _next.source.begin());

	size_t count = 0;

	for (const auto &[c, len] : lak::codepoint_range(_next.source))
	{
		count += len;

		_next.position.end += c;

		if (c == codepoint) break;
	}

	_next.source = _next.source.first(count);

	internal_pump();

	return _current;
}

template<typename CHAR>
lak::token<CHAR> lak::tokeniser<CHAR>::skip(lak::u32string_view str) noexcept
{
	ASSERT(str.size() > 0);

	_next.source = lak::string_view(_next.source.begin(), _data.end());

	size_t count     = 0;
	size_t match_len = 0;

	if (str.size() <= _next.source.size())
	{
		for (const auto &[c, len] : lak::codepoint_range(_next.source))
		{
			count += len;

			_next.position.end += c;

			if (c == str[match_len])
				++match_len;
			else
				match_len = 0;

			ASSERT(match_len <= str.size());

			if (match_len == str.size()) break;
		}
	}
	else
	{
		// str is longer than the remaining data, so we can never find it.
		// Instead we just update the end position.
		for (const auto &[c, len] : lak::codepoint_range(_next.source))
		{
			_next.position.end += c;
		}
		count = _next.source.size();
	}

	_next.source = _next.source.first(count);

	internal_pump();

	return _current;
}

template<typename CHAR>
inline void lak::tokeniser<CHAR>::reset() noexcept
{
	_current.source   = _data.first(0);
	_current.position = token_position{};
	_next             = _current;
	operator++();
}
