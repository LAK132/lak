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
	struct to_bytes_data;

	template<typename T, lak::endian E>
	struct to_bytes_traits
	{
		using value_type = lak::nonesuch;

		static constexpr bool const_size = true;

		static constexpr size_t size() { return 0; }

		static void to_bytes(lak::to_bytes_data<value_type, E> data) = delete;
	};

	template<typename T, lak::endian E>
	static constexpr bool has_to_bytes_traits =
	  !lak::is_same_v<typename lak::to_bytes_traits<T, E>::value_type,
	                  lak::nonesuch>;

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	constexpr size_t to_bytes_size()
	{
		return lak::to_bytes_traits<T, E>::size();
	}
	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	constexpr size_t to_bytes_size(const T &)
	{
		return lak::to_bytes_traits<T, E>::size();
	}
	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	constexpr size_t to_bytes_size(lak::span<const T> v)
	{
		return lak::to_bytes_traits<T, E>::size() * v.size();
	}
	template<typename T, lak::endian E = lak::endian::little>
	requires(!lak::to_bytes_traits<T, E>::const_size)
	size_t to_bytes_size(const T &v)
	{
		return lak::to_bytes_traits<T, E>::size(v);
	}
	template<typename T, lak::endian E = lak::endian::little>
	requires(!lak::to_bytes_traits<T, E>::const_size)
	size_t to_bytes_size(lak::span<const T> v)
	{
		size_t size = 0;
		for (const auto &e : v) size += lak::to_bytes_traits<T, E>::size(e);
		return size;
	}

	template<typename T, lak::endian E>
	requires(lak::to_bytes_traits<T, E>::const_size)
	struct to_bytes_data<T, E>
	{
		const lak::span<byte_t> dst;
		const lak::span<const T> src;

		static inline lak::result<to_bytes_data> maybe_make(lak::span<byte_t> dst,
		                                                    lak::span<const T> src)
		{
			if (dst.size() == src.size() * lak::to_bytes_size<T, E>())
				return lak::ok_t{to_bytes_data(dst, src)};
			else
				return lak::err_t{};
		}

		template<size_t S>
		static inline to_bytes_data make(
		  lak::span<byte_t, S * lak::to_bytes_size<T, E>()> dst,
		  lak::span<const T, S> src)
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
	requires(!lak::to_bytes_traits<T, E>::const_size)
	struct to_bytes_data<T, E>
	{
		const lak::span<byte_t> dst;
		const lak::span<const T> src;

		static inline lak::result<to_bytes_data> maybe_make(lak::span<byte_t> dst,
		                                                    lak::span<const T> src)
		{
			if (dst.size() == lak::to_bytes_size<T, E>(src))
				return lak::ok_t{to_bytes_data(dst, src)};
			else
				return lak::err_t{};
		}

		to_bytes_data(const to_bytes_data &)            = default;
		to_bytes_data &operator=(const to_bytes_data &) = default;

	private:
		to_bytes_data(lak::span<byte_t> d, lak::span<const T> s) : dst(d), src(s)
		{
		}
	};

	template<typename T, lak::endian E>
	struct to_bytes_traits_arr_impl
	{
		using value_type = T;

		template<lak::endian EE = E>
		static void to_bytes(lak::to_bytes_data<value_type, E> data)
		requires(EE == E && lak::to_bytes_traits<value_type, EE>::const_size)
		{
			auto dst{data.dst};
			for (const auto &val : data.src)
			{
				lak::array<byte_t, lak::to_bytes_size<value_type, EE>()> bytes{
				  lak::to_bytes_traits<value_type, EE>::single_to_bytes(val)};
				lak::memcpy(dst, bytes);
				dst = dst.subspan(lak::to_bytes_size<value_type, EE>());
			}
		}

		template<lak::endian EE = E>
		static void to_bytes(lak::to_bytes_data<value_type, E> data)
		requires(EE == E && !lak::to_bytes_traits<value_type, EE>::const_size)
		{
			auto dst{data.dst};
			for (const auto &val : data.src)
			{
				const size_t sz = lak::to_bytes_size<value_type, EE>(val);
				lak::to_bytes_traits<value_type, EE>::single_to_bytes(dst.first(sz),
				                                                      val);
				dst = dst.subspan(sz);
			}
		}
	};

	/* --- to_bytes --- */

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	void to_bytes(lak::span<byte_t, lak::to_bytes_size<T, E>()> bytes,
	              const T &value);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<> to_bytes(lak::span<byte_t> bytes, const T &value);

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	lak::array<byte_t, lak::to_bytes_size<T, E>()> to_bytes(const T &value);

	/* --- array_to_bytes --- */

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	lak::array<byte_t, S * lak::to_bytes_size<T, E>()> array_to_bytes(
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

#include "lak/binary_writer.inl"

#endif
