#ifndef LAK_BINARY_WRITER_HPP
#define LAK_BINARY_WRITER_HPP

#include "lak/array.hpp"
#include "lak/binary_traits.hpp"
#include "lak/endian.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/type_traits.hpp"

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
		         lak::has_to_bytes_traits<lak::remove_cvref_t<T>, E>)
		lak::result<> write(const T &value)
		{
			using value_type = lak::remove_cvref_t<T>;

			const size_t req_size = lak::to_bytes_size<value_type, E>(value);
			if (req_size > remaining_size()) return lak::err_t{};
			lak::to_bytes<T, E>(remaining().first(req_size), value).unwrap();
			_cursor += req_size;
			return lak::ok_t{};
		}

		template<lak::endian E = lak::endian::little, typename T = void>
		requires(lak::has_to_bytes_traits<lak::remove_const_t<T>, E>)
		lak::result<> write(lak::span<T> values)
		{
			using value_type = lak::remove_const_t<T>;

			const size_t req_size = lak::to_bytes_size<value_type, E>(values);
			if (req_size > remaining_size()) return lak::err_t{};
			return lak::array_to_bytes<value_type, E>(remaining().first(req_size),
			                                          values)
			  .map(
			    [&, this](auto &&)
			    {
				    this->_cursor += req_size;
				    return lak::monostate{};
			    });
		}

		template<lak::endian E = lak::endian::little, typename CHAR = void>
		lak::result<> write_c_str(lak::string_view<CHAR> string)
		{
			constexpr size_t char_size = lak::to_bytes_size<CHAR, E>();
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

#define BINARY_SPAN_WRITER_MEMBERS(TYPE, NAME, ...)                           \
	template<lak::endian E = lak::endian::little>                               \
	inline lak::result<> write_##NAME(const TYPE &value)                        \
	{                                                                           \
		return write<E, TYPE>(value);                                             \
	}
		LAK_FOREACH_INTEGER(BINARY_SPAN_WRITER_MEMBERS)
		LAK_FOREACH_FLOAT(BINARY_SPAN_WRITER_MEMBERS)
#undef BINARY_SPAN_WRITER_MEMBERS
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
		         lak::has_to_bytes_traits<lak::remove_cvref_t<T>, E>)
		void write(const T &value)
		{
			using value_type = lak::remove_cvref_t<T>;

			const size_t req_size = lak::to_bytes_size<value_type, E>(value);
			const size_t new_size = data.size() + req_size;
			data.resize(new_size);
			ASSERT_EQUAL(data.size(), new_size);
			lak::to_bytes<value_type, E>(lak::span(data).last(req_size), value)
			  .unwrap();
		}

		template<lak::endian E = lak::endian::little, typename T = void>
		requires(lak::has_to_bytes_traits<lak::remove_const_t<T>, E>)
		void write(lak::span<T> values)
		{
			using value_type = lak::remove_const_t<T>;

			const size_t req_size = lak::to_bytes_size<value_type, E>(values);
			const size_t new_size = data.size() + req_size;
			data.resize(new_size);
			ASSERT_EQUAL(data.size(), new_size);
			lak::array_to_bytes<value_type, E>(lak::span(data).last(req_size),
			                                   values)
			  .unwrap();
		}

		template<lak::endian E = lak::endian::little, typename CHAR = void>
		void write_c_str(lak::string_view<CHAR> string)
		{
			constexpr size_t char_size = lak::to_bytes_size<CHAR, E>();
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

#define BINARY_ARRAY_WRITER_MEMBERS(TYPE, NAME, ...)                          \
	template<lak::endian E = lak::endian::little>                               \
	inline void write_##NAME(const TYPE &value)                                 \
	{                                                                           \
		write<E, TYPE>(value);                                                    \
	}
		LAK_FOREACH_INTEGER(BINARY_ARRAY_WRITER_MEMBERS)
		LAK_FOREACH_FLOAT(BINARY_ARRAY_WRITER_MEMBERS)
#undef BINARY_ARRAY_WRITER_MEMBERS
	};
}

#endif
