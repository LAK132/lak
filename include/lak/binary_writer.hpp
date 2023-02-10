#ifndef LAK_BINARY_WRITER_HPP
#define LAK_BINARY_WRITER_HPP

#include "lak/array.hpp"
#include "lak/endian.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/type_traits.hpp"

namespace lak
{
	/* --- traits --- */

	template<typename T, lak::endian E>
	struct to_bytes_traits
	{
		using value_type = lak::nonesuch;

		static constexpr size_t size = 0;
	};

	template<typename T, lak::endian E>
	struct to_bytes_data
	{
		const lak::span<byte_t> dst;
		const lak::span<const T> src;
		static constexpr size_t bytes_per_element =
		  lak::to_bytes_traits<T, E>::size;

		static inline lak::result<to_bytes_data> maybe_make(lak::span<byte_t> dst,
		                                                    lak::span<const T> src)
		{
			if (dst.size() == src.size() * bytes_per_element)
				return lak::ok_t{to_bytes_data(dst, src)};
			else
				return lak::err_t{};
		}

		template<size_t S>
		static inline to_bytes_data make(
		  lak::span<byte_t, S * bytes_per_element> dst, lak::span<const T, S> src)
		{
			return to_bytes_data(dst, src);
		}

		to_bytes_data(const to_bytes_data &)            = default;
		to_bytes_data &operator=(const to_bytes_data &) = default;

	private:
		to_bytes_data(lak::span<byte_t> d, lak::span<const T> s) : dst(d), src(s)
		{
		}
	};

	template<typename T, lak::endian E>
	static constexpr bool has_to_bytes_traits =
	  !lak::is_same_v<typename lak::to_bytes_traits<T, E>::value_type,
	                  lak::nonesuch>;

	template<typename T, lak::endian E = lak::endian::little>
	static constexpr size_t to_bytes_size_v = lak::to_bytes_traits<T, E>::size;

	/* --- to_bytes --- */

	template<typename T, lak::endian E = lak::endian::little>
	void to_bytes(lak::span<byte_t, lak::to_bytes_size_v<T, E>> bytes,
	              const T &value);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<> to_bytes(lak::span<byte_t> bytes, const T &value);

	template<typename T, lak::endian E = lak::endian::little>
	lak::array<byte_t, lak::to_bytes_size_v<T, E>> to_bytes(const T &value);

	/* --- array_to_bytes --- */

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	lak::array<byte_t, S * lak::to_bytes_size_v<T, E>> array_to_bytes(
	  lak::span<const T, S> values);

	template<typename T, lak::endian E = lak::endian::little>
	lak::array<byte_t> array_to_bytes(lak::span<const T> values);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<> array_to_bytes(lak::span<byte_t> bytes,
	                             lak::span<const T> values);

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

		template<typename T,
		         lak::endian E = lak::endian::little,
		         lak::enable_if_i<lak::has_to_bytes_traits<T, E>> = 0>
		lak::result<> write(const T &value)
		{
			using value_type          = lak::remove_const_t<T>;
			constexpr size_t req_size = lak::to_bytes_size_v<value_type, E>;
			if (req_size > remaining_size()) return lak::err_t{};
			lak::to_bytes<T, E>(remaining().template first<req_size>(), value);
			_cursor += req_size;
			return lak::ok_t{};
		}

		template<typename T, lak::endian E = lak::endian::little>
		lak::result<> write(lak::span<T> values)
		{
			using value_type = lak::remove_const_t<T>;
			const size_t req_size =
			  lak::to_bytes_size_v<value_type, E> * values.size();
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

		template<typename CHAR, lak::endian E = lak::endian::little>
		lak::result<> write_c_str(const lak::string<CHAR> &string)
		{
			const size_t req_size =
			  lak::to_bytes_size_v<CHAR, E> * (string.size() + 1);

			auto bytes{remaining()};

			if (req_size > bytes.size()) return lak::err_t{};

			for (const CHAR &c : string)
			{
				lak::to_bytes<CHAR, E>(
				  bytes.template first<lak::to_bytes_size_v<CHAR, E>>(), c);
				bytes = bytes.subspan(lak::to_bytes_size_v<CHAR, E>);
			}
			lak::to_bytes<CHAR, E>(
			  bytes.template first<lak::to_bytes_size_v<CHAR, E>>(), CHAR(0));
			bytes = bytes.subspan(lak::to_bytes_size_v<CHAR, E>);

			_cursor += req_size;

			return lak::ok_t{};
		}

#define BINARY_SPAN_WRITER_MEMBERS(TYPE, NAME, ...)                           \
	template<lak::endian E = lak::endian::little>                               \
	inline lak::result<> write_##NAME(const TYPE &value)                        \
	{                                                                           \
		return write<TYPE, E>(value);                                             \
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

		template<typename T,
		         lak::endian E = lak::endian::little,
		         lak::enable_if_i<lak::has_to_bytes_traits<T, E>> = 0>
		void write(const T &value)
		{
			const size_t new_size = data.size() + lak::to_bytes_traits<T, E>::size;
			data.resize(new_size);
			ASSERT_EQUAL(data.size(), new_size);
			lak::to_bytes<T, E>(
			  lak::span(data).template last<lak::to_bytes_traits<T, E>::size>(),
			  value);
		}

		template<typename T, lak::endian E = lak::endian::little>
		void write(lak::span<T> values)
		{
			using value_type = lak::remove_const_t<T>;

			const size_t new_size =
			  data.size() +
			  lak::to_bytes_traits<value_type, E>::size * values.size();
			data.resize(new_size);
			ASSERT_EQUAL(data.size(), new_size);
			auto bytes = lak::span(data).last(
			  lak::to_bytes_traits<value_type, E>::size * values.size());
			for (const value_type &value : values)
			{
				lak::to_bytes<value_type, E>(
				  bytes.template first<lak::to_bytes_traits<value_type, E>::size>(),
				  value);
				bytes = bytes.subspan(lak::to_bytes_traits<value_type, E>::size);
			}
		}

		template<typename CHAR, lak::endian E = lak::endian::little>
		void write_c_str(const lak::string<CHAR> &values)
		{
			const size_t new_size =
			  data.size() +
			  lak::to_bytes_traits<CHAR, E>::size * (values.size() + 1);
			data.resize(new_size);
			ASSERT_EQUAL(data.size(), new_size);
			auto bytes = lak::span(data).last(lak::to_bytes_traits<CHAR, E>::size *
			                                  (values.size() + 1));
			for (const CHAR &value : values)
			{
				lak::to_bytes<CHAR, E>(
				  bytes.template first<lak::to_bytes_traits<CHAR, E>::size>(), value);
				bytes = bytes.subspan(lak::to_bytes_traits<CHAR, E>::size);
			}
			lak::to_bytes<CHAR, E>(
			  bytes.template first<lak::to_bytes_traits<CHAR, E>::size>(), CHAR(0));
		}

#define BINARY_ARRAY_WRITER_MEMBERS(TYPE, NAME, ...)                          \
	template<lak::endian E = lak::endian::little>                               \
	inline void write_##NAME(const TYPE &value)                                 \
	{                                                                           \
		write<TYPE, E>(value);                                                    \
	}
		LAK_FOREACH_INTEGER(BINARY_ARRAY_WRITER_MEMBERS)
		LAK_FOREACH_FLOAT(BINARY_ARRAY_WRITER_MEMBERS)
#undef BINARY_ARRAY_WRITER_MEMBERS
	};
}

#include "lak/binary_writer.inl"

#endif
