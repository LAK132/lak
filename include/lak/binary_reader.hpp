#ifndef LAK_BINARY_READER_HPP
#define LAK_BINARY_READER_HPP

#include "lak/array.hpp"
#include "lak/binary_traits.hpp"
#include "lak/endian.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

#include "lak/debug.hpp"

namespace lak
{
	/* --- binary_reader --- */

	struct binary_reader
	{
		lak::span<const byte_t> _data = {};
		size_t _cursor                = 0;

		binary_reader() = default;
		binary_reader(lak::span<const byte_t> bytes) : _data(bytes), _cursor(0) {}
		binary_reader(const binary_reader &)            = default;
		binary_reader &operator=(const binary_reader &) = default;

		inline lak::span<const byte_t> remaining() const
		{
			return _data.subspan(_cursor);
		}
		inline bool empty() const { return _cursor >= _data.size(); }
		inline size_t position() const { return _cursor; }
		inline size_t size() const { return _data.size(); }
		inline lak::result<> seek(size_t pos)
		{
			if (pos > _data.size()) return lak::err_t{};
			_cursor = pos;
			return lak::ok_t{};
		}
		inline lak::result<> skip(size_t count)
		{
			if (_cursor + count > _data.size()) return lak::err_t{};
			_cursor += count;
			return lak::ok_t{};
		}
		inline lak::result<> unread(size_t count)
		{
			if (count > _cursor) return lak::err_t{};
			_cursor -= count;
			return lak::ok_t{};
		}

		lak::result<lak::span<const byte_t>> peek_bytes(size_t count) const
		{
			if (_cursor + count > _data.size()) return lak::err_t{};
			return lak::ok_t{_data.subspan(_cursor, count)};
		}

		lak::result<lak::span<const byte_t>> read_bytes(size_t count)
		{
			if (_cursor + count > _data.size()) return lak::err_t{};
			lak::span<const byte_t> result = _data.subspan(_cursor, count);
			_cursor += count;
			return lak::ok_t{result};
		}

		template<typename T, lak::endian E = lak::endian::little>
		lak::result<T> peek()
		{
			if constexpr (lak::from_bytes_traits<T, E>::const_size)
			{
				if (auto rem = remaining(); rem.size() < lak::from_bytes_size_v<T, E>)
					return lak::err_t{};
				else
					return lak::from_bytes<T, E>(
					  rem.first(lak::from_bytes_size_v<T, E>));
			}
			else
				return lak::from_bytes<T, E>(remaining())
				  .map([](lak::pair<T, lak::span<const byte_t>> &&v) -> T
				       { return lak::move(v.first); });
		}

		template<typename T, lak::endian E = lak::endian::little>
		lak::result<T> read()
		{
			if constexpr (lak::from_bytes_traits<T, E>::const_size)
				return peek<T, E>().if_ok(
				  [&](auto &&) { this->_cursor += lak::from_bytes_size_v<T, E>; });
			else
				return lak::from_bytes<T, E>(remaining())
				  .map(
				    [&](lak::pair<T, lak::span<const byte_t>> &&v) -> T
				    {
					    this->_cursor = v.second.begin() - this->_data.begin();
					    return lak::move(v.first);
				    });
		}

		template<typename T, lak::endian E = lak::endian::little>
		lak::result<lak::array<T>> peek(size_t count)
		{
			if constexpr (lak::from_bytes_traits<T, E>::const_size)
			{
				if (auto rem = remaining();
				    rem.size() < (lak::from_bytes_size_v<T, E> * count))
					return lak::err_t{};
				else
					return lak::array_from_bytes<T, E>(
					  rem.first(lak::from_bytes_size_v<T, E> * count), count);
			}
			else
				return lak::array_from_bytes<T, E>(remaining(), count)
				  .map([](lak::pair<lak::array<T>, lak::span<const byte_t>> &&v)
				       { return lak::move(v.first); });
		}

		template<typename T, lak::endian E = lak::endian::little>
		lak::result<lak::array<T>> read(size_t count)
		{
			if constexpr (lak::from_bytes_traits<T, E>::const_size)
				return peek<T, E>(count).if_ok(
				  [&](auto &&)
				  { this->_cursor += lak::from_bytes_size_v<T, E> * count; });
			else
				return lak::array_from_bytes<T, E>(remaining(), count)
				  .map(
				    [&](lak::pair<lak::array<T>, lak::span<const byte_t>> &&v)
				      -> lak::array<T>
				    {
					    ASSERT_LESS_OR_EQUAL(v.second.size(), this->_data.size());
					    this->_cursor = this->_data.size() - v.second.size();
					    return lak::move(v.first);
				    });
		}

		// always read count bytes even if the c string isn't the full count bytes
		template<typename CHAR, lak::endian E = lak::endian::little>
		lak::result<lak::string<CHAR>> read_exact_c_str(size_t count)
		{
			return read<CHAR, E>(count).map(
			  [](lak::array<CHAR> &&array)
			  {
				  size_t len = 0;
				  for (const auto &c : array)
				  {
					  if (c == CHAR(0)) break;
					  ++len;
				  }
				  return lak::string<CHAR>(array.data(), array.data() + len);
			  });
		}

		template<typename CHAR, lak::endian E = lak::endian::little>
		lak::result<lak::string<CHAR>> read_c_str(size_t max_size = SIZE_MAX)
		{
			lak::string<CHAR> result;
			const size_t old_cursor = _cursor;
			while (max_size-- > 0ULL)
			{
				if_let_ok (const CHAR c, read<CHAR, E>())
				{
					if (c == 0) return lak::move_ok(result);
					result += c;
				}
				else
				{
					ERROR("read failed");
					break;
				}
			}
			_cursor = old_cursor;
			if (max_size == SIZE_MAX) ERROR("max size reached");
			return lak::err_t{};
		}

		template<typename CHAR, lak::endian E = lak::endian::little>
		lak::string<CHAR> read_any_c_str(size_t max_size = SIZE_MAX)
		{
			lak::string<CHAR> result;
			while (max_size-- > 0ULL)
			{
				if_let_ok (const CHAR c, read<CHAR, E>())
				{
					if (c == 0) break;
					result += c;
				}
				else
					break;
			}
			return result;
		}

#define BINARY_READER_MEMBERS(TYPE, NAME, ...)                                \
	template<lak::endian E = lak::endian::little>                               \
	inline lak::result<TYPE> peek_##NAME()                                      \
	{                                                                           \
		return peek<TYPE, E>();                                                   \
	}                                                                           \
	template<lak::endian E = lak::endian::little>                               \
	inline lak::result<TYPE> read_##NAME()                                      \
	{                                                                           \
		return read<TYPE, E>();                                                   \
	}
		LAK_FOREACH_INTEGER(BINARY_READER_MEMBERS)
		LAK_FOREACH_FLOAT(BINARY_READER_MEMBERS)
#undef BINARY_READER_MEMBERS
	};
}

#endif
