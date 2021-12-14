#ifndef LAK_BINARY_READER_HPP
#define LAK_BINARY_READER_HPP

#include "lak/array.hpp"
#include "lak/endian.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

#include "lak/debug.hpp"

namespace lak
{
	/* --- traits --- */

	template<typename T, lak::endian E>
	struct from_bytes_traits
	{
		using value_type = lak::nonesuch;

		static constexpr size_t size = 0;
	};

	template<typename T, lak::endian E>
	struct from_bytes_data
	{
		const lak::span<T> dst;
		const lak::span<const byte_t> src;
		static constexpr size_t bytes_per_element =
		  lak::from_bytes_traits<T, E>::size;

		static inline lak::result<from_bytes_data> maybe_make(
		  lak::span<T> dst, lak::span<const byte_t> src)
		{
			if (dst.size() * bytes_per_element == src.size())
				return lak::ok_t{from_bytes_data(dst, src)};
			else
				return lak::err_t{};
		}

		template<size_t S>
		static inline from_bytes_data make(
		  lak::span<T, S> dst, lak::span<const byte_t, S * bytes_per_element> src)
		{
			return from_bytes_data(dst, src);
		}

		from_bytes_data(const from_bytes_data &) = default;
		from_bytes_data &operator=(const from_bytes_data &) = default;

	private:
		from_bytes_data(lak::span<T> d, lak::span<const byte_t> s) : dst(d), src(s)
		{
		}
	};

	template<typename T, lak::endian E>
	static constexpr bool has_from_bytes_traits =
	  !lak::is_same_v<typename lak::from_bytes_traits<T, E>::value_type,
	                  lak::nonesuch>;

	template<typename T, lak::endian E = lak::endian::little>
	static constexpr size_t from_bytes_size_v =
	  lak::from_bytes_traits<T, E>::size;

	template<typename T, lak::endian E = lak::endian::little>
	using from_bytes_value_type_t =
	  typename lak::from_bytes_traits<T, E>::value_type;

	/* --- from_bytes --- */

	template<typename T, lak::endian E = lak::endian::little>
	T from_bytes(lak::span<const byte_t, lak::from_bytes_size_v<T, E>> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<T> from_bytes(lak::span<const byte_t> bytes);

	/* --- array_from_bytes --- */

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	lak::array<T, S> array_from_bytes(
	  lak::span<const byte_t, lak::from_bytes_size_v<T, E> * S> bytes);

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	lak::result<lak::array<T, S>> array_from_bytes(
	  lak::span<const byte_t> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<lak::array<T>> array_from_bytes(lak::span<const byte_t> bytes,
	                                            size_t count);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<> array_from_bytes(lak::span<T> values,
	                               lak::span<const byte_t> bytes);

	/* --- binary_reader --- */

	struct binary_reader
	{
		lak::span<const byte_t> _data = {};
		size_t _cursor                = 0;

		binary_reader() = default;
		binary_reader(lak::span<const byte_t> bytes) : _data(bytes), _cursor(0) {}
		binary_reader(const binary_reader &) = default;
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
			return lak::from_bytes<T, E>(remaining());
		}

		template<typename T, lak::endian E = lak::endian::little>
		lak::result<T> read()
		{
			return peek<T, E>().if_ok(
			  [&](auto &&) { this->_cursor += lak::from_bytes_size_v<T, E>; });
		}

		template<typename T, lak::endian E = lak::endian::little>
		lak::result<lak::array<T>> peek(size_t count)
		{
			return lak::array_from_bytes<T, E>(remaining(), count);
		}

		template<typename T, lak::endian E = lak::endian::little>
		lak::result<lak::array<T>> read(size_t count)
		{
			return peek<T, E>(count).if_ok(
			  [&](auto &&)
			  { this->_cursor += lak::from_bytes_size_v<T, E> * count; });
		}

		// always read count bytes even if the c string isn't the full count bytes
		template<typename CHAR, lak::endian E = lak::endian::little>
		lak::result<lak::string<CHAR>> read_exact_c_str(size_t count)
		{
			return read<CHAR>(count).map(
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
					if (c == 0) return lak::ok_t{lak::move(result)};
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
				else break;
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

#include "lak/binary_reader.inl"

#endif
