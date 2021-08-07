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
	template<typename T, lak::endian E>
	struct to_bytes_traits
	{
		using value_type = lak::nonesuch;

		static constexpr size_t size = 0;
	};

	template<typename T, lak::endian E>
	static constexpr bool has_to_bytes_traits =
	  !lak::is_same_v<typename lak::to_bytes_traits<T, E>::value_type,
	                  lak::nonesuch>;

	template<typename T, lak::endian E = lak::endian::little>
	static constexpr size_t to_bytes_size_v = lak::to_bytes_traits<T, E>::size;

	template<typename T, lak::endian E = lak::endian::little>
	void to_bytes(lak::span<uint8_t, lak::to_bytes_size_v<T, E>> bytes,
	              const T &value);

	// returns bytes.subspan(to_bytes_traits::size) on success
	template<typename T, lak::endian E = lak::endian::little>
	lak::result<lak::span<uint8_t>> to_bytes(lak::span<uint8_t> bytes,
	                                         const T &value);

	// returns bytes.subspan(to_bytes_traits::size * values.size()) on success
	template<typename T, lak::endian E = lak::endian::little>
	lak::result<lak::span<uint8_t>> array_to_bytes(lak::span<uint8_t> bytes,
	                                               lak::span<const T> values);

	struct binary_span_writer
	{
		lak::span<uint8_t> _data = {};
		size_t _cursor           = 0;

		binary_span_writer() = default;
		binary_span_writer(lak::span<uint8_t> bytes) : _data(bytes), _cursor(0) {}
		binary_span_writer(const binary_span_writer &) = default;
		binary_span_writer &operator=(const binary_span_writer &) = default;

		lak::span<uint8_t> remaining() const { return _data.subspan(_cursor); }
		bool empty() const { return _cursor >= _data.size(); }
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
			if (empty()) return lak::err_t{};
			return lak::to_bytes<T, E>(remaining(), value)
			  .if_ok([this](auto &&)
			         { this->_cursor += lak::to_bytes_size_v<T, E>; })
			  .map([](auto &&) -> lak::monostate { return {}; });
		}

		template<typename T, lak::endian E = lak::endian::little>
		lak::result<> write(lak::span<T> values)
		{
			if (empty()) return lak::err_t{};
			using value_type = lak::remove_const_t<T>;
			return lak::array_to_bytes<value_type, E>(remaining(), values)
			  .if_ok(
			    [&, this](auto &&) {
				    this->_cursor +=
				      lak::to_bytes_size_v<value_type, E> * values.size();
			    })
			  .map([](auto &&) -> lak::monostate { return {}; });
		}

		template<typename CHAR, lak::endian E = lak::endian::little>
		lak::result<> write_c_str(const lak::string<CHAR> &string)
		{
			if (empty()) return lak::err_t{};

			const size_t req_size =
			  lak::to_bytes_size_v<CHAR, E> * (string.size() + 1);

			auto bytes = remaining();

			if (req_size > bytes.size()) return lak::err_t{};

			for (const CHAR &c : string)
			{
				lak::to_bytes<CHAR, E>(bytes.first<lak::to_bytes_size_v<CHAR, E>>(),
				                       c);
				bytes = bytes.subspan(lak::to_bytes_size_v<CHAR, E>);
			}
			lak::to_bytes<CHAR, E>(bytes.first<lak::to_bytes_size_v<CHAR, E>>(),
			                       CHAR(0));
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

	struct binary_array_writer
	{
		lak::array<uint8_t> data = {};

		inline void reserve(size_t size) { data.reserve(size); }
		inline lak::array<uint8_t> &&release() { return lak::move(data); }
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
			  lak::span(data).last<lak::to_bytes_traits<T, E>::size>(), value);
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
				  bytes.first<lak::to_bytes_traits<value_type, E>::size>(), value);
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
				  bytes.first<lak::to_bytes_traits<CHAR, E>::size>(), value);
				bytes = bytes.subspan(lak::to_bytes_traits<CHAR, E>::size);
			}
			lak::to_bytes<CHAR, E>(
			  bytes.first<lak::to_bytes_traits<CHAR, E>::size>(), CHAR(0));
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
