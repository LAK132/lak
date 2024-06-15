#include "lak/binary_traits.hpp"

#ifndef LAK_BINARY_WRITER_HPP
#	define LAK_BINARY_WRITER_HPP

#	include "lak/array.hpp"
#	include "lak/endian.hpp"
#	include "lak/result.hpp"
#	include "lak/span.hpp"
#	include "lak/stdint.hpp"
#	include "lak/type_traits.hpp"

namespace lak
{
	/* --- binary_span_writer --- */

	struct binary_span_writer
	{
		lak::span<byte_t> _data = {};
		size_t _cursor          = 0;

		binary_span_writer() = default;
		binary_span_writer(lak::span<byte_t> bytes) : _data(bytes), _cursor(0) {}
		binary_span_writer(const binary_span_writer &)            = default;
		binary_span_writer &operator=(const binary_span_writer &) = default;

		inline lak::span<byte_t> remaining() const
		{
			return _data.subspan(_cursor);
		}
		inline size_t remaining_size() const { return _data.size() - _cursor; }
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
		inline lak::result<> unwrite(size_t count)
		{
			if (count > _cursor) return lak::err_t{};
			_cursor -= count;
			return lak::ok_t{};
		}

		template<lak::endian E = lak::endian::little, typename T = void>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>, E>)
		lak::result<> write(const T &value)
		{
			RES_TRY_ASSIGN(auto unused =, lak::to_bytes<E>(remaining(), value));
			const size_t req_size = remaining().size() - unused.size();
			_cursor += req_size;
			return lak::ok_t{};
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::little>)
		lak::result<> write_le(const T &value)
		{
			return write<lak::endian::little, T>(value);
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::big>)
		lak::result<> write_be(const T &value)
		{
			return write<lak::endian::big, T>(value);
		}

		template<lak::endian E = lak::endian::little, typename T = void>
		requires(lak::concepts::to_bytes_writeable<lak::remove_const_t<T>, E>)
		lak::result<> write(lak::span<T> values)
		{
			RES_TRY_ASSIGN(auto unused =,
			               lak::array_to_bytes<E>(remaining(), values));
			const size_t req_size = remaining().size() - unused.size();
			_cursor += req_size;
			return lak::ok_t{};
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::little>)
		lak::result<> write_le(lak::span<T> values)
		{
			return write<lak::endian::little, T>(values);
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::big>)
		lak::result<> write_be(lak::span<T> values)
		{
			return write<lak::endian::big, T>(values);
		}

		template<lak::endian E = lak::endian::little, typename CHAR = void>
		requires(lak::to_bytes_traits<CHAR, E>::const_size)
		lak::result<> write_c_str(lak::string_view<CHAR> string)
		{
			constexpr size_t char_size = lak::to_bytes_traits<CHAR, E>::size;
			const size_t req_size      = char_size * (string.size() + 1U);

			auto bytes{remaining()};

			if (req_size > bytes.size()) return lak::err_t{};

			bytes = bytes.last(req_size);
			lak::array_to_bytes<CHAR, E>(bytes.first(char_size * string.size()),
			                             lak::span(string))
			  .unwrap();
			lak::to_bytes<CHAR, E>(bytes.template last<char_size>(), CHAR(0));

			_cursor += req_size;

			return lak::ok_t{};
		}

		template<typename CHAR>
		lak::result<> write_c_str_le(lak::string_view<CHAR> string)
		{
			return write_c_str<lak::endian::little, CHAR>(string);
		}

		template<typename CHAR>
		lak::result<> write_c_str_be(lak::string_view<CHAR> string)
		{
			return write_c_str<lak::endian::big, CHAR>(string);
		}

#	define BINARY_SPAN_WRITER_MEMBERS(TYPE, NAME, ...)                         \
		template<lak::endian E = lak::endian::little>                             \
		inline lak::result<> write_##NAME(const TYPE &value)                      \
		{                                                                         \
			return write<E, TYPE>(value);                                           \
		}                                                                         \
		inline lak::result<> write_##NAME##le(const TYPE &value)                  \
		{                                                                         \
			return write_##NAME<lak::endian::little>(value);                        \
		}                                                                         \
		inline lak::result<> write_##NAME##be(const TYPE &value)                  \
		{                                                                         \
			return write_##NAME<lak::endian::big>(value);                           \
		}
		LAK_FOREACH_INTEGER(BINARY_SPAN_WRITER_MEMBERS)
		LAK_FOREACH_FLOAT(BINARY_SPAN_WRITER_MEMBERS)
#	undef BINARY_SPAN_WRITER_MEMBERS
	};

	/* --- binary_array_writer --- */

	struct binary_array_writer
	{
		lak::array<byte_t> data = {};

		inline void reserve(size_t size) { data.reserve(size); }
		inline lak::array<byte_t> &&release() { return lak::move(data); }
		inline size_t size() const { return data.size(); }
		inline lak::result<> unwrite(size_t count)
		{
			if (count > data.size()) return lak::err_t{};
			data.resize(data.size() - count);
			return lak::ok_t{};
		}

		template<lak::endian E = lak::endian::little, typename T = void>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>, E>)
		lak::result<> write(const T &value)
		{
			using value_type = lak::remove_cvref_t<T>;

			const size_t req_size =
			  lak::to_bytes_traits<value_type, E>::dynamic_size(value);
			const size_t new_size = data.size() + req_size;
			data.resize(new_size);
			ASSERT_EQUAL(data.size(), new_size);
			if (lak::to_bytes_traits<value_type, E>::to_bytes(
			      lak::span(data).last(req_size), value)
			      .is_err())
			{
				data.resize(data.size() - req_size);
				return lak::err_t{};
			}
			return lak::ok_t{};
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::little>)
		lak::result<> write_le(const T &value)
		{
			return write<lak::endian::little, T>(value);
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::big>)
		lak::result<> write_be(const T &value)
		{
			return write<lak::endian::big, T>(value);
		}

		template<lak::endian E = lak::endian::little, typename T = void>
		requires(lak::concepts::to_bytes_writeable<lak::remove_const_t<T>, E>)
		lak::result<> write(lak::span<T> values)
		{
			using value_type = lak::remove_const_t<T>;

			if constexpr (lak::to_bytes_traits<value_type, E>::const_size)
			{
				const size_t req_size =
				  lak::to_bytes_traits<value_type, E>::size * values.size();
				const size_t new_size = data.size() + req_size;
				data.resize(new_size);
				ASSERT_EQUAL(data.size(), new_size);
				if (lak::array_to_bytes<E>(lak::span(data).last(req_size), values)
				      .is_err())
				{
					data.resize(data.size() - req_size);
					return lak::err_t{};
				}
			}
			else
			{
				for (const auto &v : values)
				{
					RES_TRY(write(v));
				}
			}
			return lak::ok_t{};
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::little>)
		lak::result<> write_le(lak::span<T> values)
		{
			return write<lak::endian::little, T>(values);
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::big>)
		lak::result<> write_be(lak::span<T> values)
		{
			return write<lak::endian::big, T>(values);
		}

		template<lak::endian E = lak::endian::little, typename CHAR = void>
		requires(lak::to_bytes_traits<CHAR, E>::const_size)
		void write_c_str(lak::string_view<CHAR> string)
		{
			constexpr size_t char_size = lak::to_bytes_traits<CHAR, E>::size;
			const size_t req_size      = char_size * (string.size() + 1U);
			const size_t new_size      = data.size() + req_size;
			data.resize(new_size);
			ASSERT_EQUAL(data.size(), new_size);
			auto bytes{lak::span(data).last(req_size)};
			lak::array_to_bytes<CHAR, E>(bytes.first(char_size * string.size()),
			                             lak::span(string))
			  .unwrap();
			lak::to_bytes<CHAR, E>(bytes.template last<char_size>(), CHAR(0));
		}

		template<typename CHAR>
		lak::result<> write_c_str_le(lak::string_view<CHAR> string)
		{
			return write_c_str<lak::endian::little, CHAR>(string);
		}

		template<typename CHAR>
		lak::result<> write_c_str_be(lak::string_view<CHAR> string)
		{
			return write_c_str<lak::endian::big, CHAR>(string);
		}

#	define BINARY_ARRAY_WRITER_MEMBERS(TYPE, NAME, ...)                        \
		template<lak::endian E = lak::endian::little>                             \
		inline void write_##NAME(const TYPE &value)                               \
		{                                                                         \
			write<E, TYPE>(value);                                                  \
		}                                                                         \
		inline void write_##NAME##le(const TYPE &value)                           \
		{                                                                         \
			write_##NAME<lak::endian::little>(value);                               \
		}                                                                         \
		inline void write_##NAME##be(const TYPE &value)                           \
		{                                                                         \
			write_##NAME<lak::endian::big>(value);                                  \
		}
		LAK_FOREACH_INTEGER(BINARY_ARRAY_WRITER_MEMBERS)
		LAK_FOREACH_FLOAT(BINARY_ARRAY_WRITER_MEMBERS)
#	undef BINARY_ARRAY_WRITER_MEMBERS
	};
}

template<typename T, lak::endian E>
requires requires(const T value) {
	{
		value.template write_size<E>()
	} -> lak::concepts::same_as<size_t>;
	{
		value.template write<E>(lak::declval<lak::binary_span_writer &>())
	} -> lak::concepts::same_as<lak::result<>>;
}
struct lak::to_bytes_traits<T, E>
{
	using value_type                 = T;
	static constexpr bool const_size = false;
	static constexpr size_t size     = lak::dynamic_extent;

	static size_t dynamic_size(const T &value)
	{
		return value.template write_size<E>();
	}

	static lak::result<lak::span<byte_t>> to_bytes(lak::span<byte_t> bytes,
	                                               const T &value)
	{
		lak::binary_span_writer strm{bytes};
		RES_TRY(value.template write<E>(strm));
		return lak::ok_t{strm.remaining()};
	}
};

#endif
