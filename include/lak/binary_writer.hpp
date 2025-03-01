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

		template<typename T, lak::endian E, typename... ERR>
		using error_type = lak::bytes_errors_t<
		  ERR...,
		  typename lak::to_bytes_traits<lak::remove_const_t<T>, E>::error_type>;

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

		template<lak::endian E = lak::endian::little, typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>, E>)
		lak::error_code<error_type<T, E>> write(const T &value)
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
		auto write_le(const T &value)
		{
			return write<lak::endian::little, T>(value);
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::big>)
		auto write_be(const T &value)
		{
			return write<lak::endian::big, T>(value);
		}

		template<lak::endian E = lak::endian::little, typename T = void>
		requires(lak::concepts::to_bytes_writeable<lak::remove_const_t<T>, E>)
		lak::error_code<error_type<T, E>> write(lak::span<T> values)
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
		auto write_le(lak::span<T> values)
		{
			return write<lak::endian::little, T>(values);
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::big>)
		auto write_be(lak::span<T> values)
		{
			return write<lak::endian::big, T>(values);
		}

		template<lak::endian E = lak::endian::little, typename CHAR = void>
		requires(lak::to_bytes_traits<CHAR, E>::const_size)
		lak::error_code<error_type<CHAR, E, lak::out_of_data_error>> write_c_str(
		  lak::string_view<CHAR> string)
		{
			constexpr size_t char_size = lak::to_bytes_traits<CHAR, E>::size;
			const size_t req_size      = char_size * (string.size() + 1U);

			auto bytes{remaining()};

			if (req_size > bytes.size()) return lak::err_t<lak::out_of_data_error>{};

			bytes = bytes.last(req_size);
			lak::array_to_bytes<CHAR, E>(bytes.first(char_size * string.size()),
			                             lak::span(string))
			  .unwrap();
			lak::to_bytes<CHAR, E>(bytes.template last<char_size>(), CHAR(0));

			_cursor += req_size;

			return lak::ok_t{};
		}

		template<typename CHAR>
		auto write_c_str_le(lak::string_view<CHAR> string)
		{
			return write_c_str<lak::endian::little, CHAR>(string);
		}

		template<typename CHAR>
		auto write_c_str_be(lak::string_view<CHAR> string)
		{
			return write_c_str<lak::endian::big, CHAR>(string);
		}

#	define BINARY_SPAN_WRITER_MEMBERS(TYPE, NAME, ...)                         \
		template<lak::endian E = lak::endian::little>                             \
		inline auto write_##NAME(const TYPE &value)                               \
		{                                                                         \
			return write<E, TYPE>(value);                                           \
		}                                                                         \
		inline auto write_##NAME##le(const TYPE &value)                           \
		{                                                                         \
			return write_le<TYPE>(value);                                           \
		}                                                                         \
		inline auto write_##NAME##be(const TYPE &value)                           \
		{                                                                         \
			return write_be<TYPE>(value);                                           \
		}
		LAK_FOREACH_INTEGER(BINARY_SPAN_WRITER_MEMBERS)
		LAK_FOREACH_FLOAT(BINARY_SPAN_WRITER_MEMBERS)
#	undef BINARY_SPAN_WRITER_MEMBERS
	};

	/* --- binary_array_writer --- */

	struct binary_array_writer
	{
		lak::array<byte_t> data = {};

		template<typename T, lak::endian E, typename... ERR>
		using error_type = lak::bytes_errors_t<
		  ERR...,
		  typename lak::to_bytes_traits<lak::remove_const_t<T>, E>::error_type>;

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
		lak::error_code<error_type<T, E>> write(const T &value)
		{
			using value_type = lak::remove_cvref_t<T>;

			const size_t req_size =
			  lak::to_bytes_traits<value_type, E>::dynamic_size(value);
			const size_t new_size = data.size() + req_size;
			data.resize(new_size);
			ASSERT_EQUAL(data.size(), new_size);
			RES_TRY(lak::to_bytes_traits<value_type, E>::to_bytes(
			          lak::span(data).last(req_size), value)
			          .if_err([&](auto) { data.resize(data.size() - req_size); }));
			return lak::ok_t{};
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::little>)
		auto write_le(const T &value)
		{
			return write<lak::endian::little, T>(value);
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::big>)
		auto write_be(const T &value)
		{
			return write<lak::endian::big, T>(value);
		}

		template<lak::endian E = lak::endian::little, typename T = void>
		requires(lak::concepts::to_bytes_writeable<lak::remove_const_t<T>, E>)
		lak::error_code<error_type<T, E, lak::out_of_data_error>> write(
		  lak::span<T> values)
		{
			using value_type = lak::remove_const_t<T>;

			if constexpr (lak::to_bytes_traits<value_type, E>::const_size)
			{
				const size_t req_size =
				  lak::to_bytes_traits<value_type, E>::size * values.size();
				const size_t new_size = data.size() + req_size;
				data.resize(new_size);
				RES_TRY(lak::array_to_bytes<E, lak::remove_const_t<T>>(
				          lak::span(data).last(req_size), values)
				          .if_err([&](auto) { data.resize(data.size() - req_size); }));
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
		auto write_le(lak::span<T> values)
		{
			return write<lak::endian::little, T>(values);
		}

		template<typename T>
		requires(!lak::is_span_v<lak::remove_cvref_t<T>> &&
		         lak::concepts::to_bytes_writeable<lak::remove_cvref_t<T>,
		                                           lak::endian::big>)
		auto write_be(lak::span<T> values)
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
		auto write_c_str_le(lak::string_view<CHAR> string)
		{
			return write_c_str<lak::endian::little, CHAR>(string);
		}

		template<typename CHAR>
		auto write_c_str_be(lak::string_view<CHAR> string)
		{
			return write_c_str<lak::endian::big, CHAR>(string);
		}

#	define BINARY_ARRAY_WRITER_MEMBERS(TYPE, NAME, ...)                        \
		template<lak::endian E = lak::endian::little>                             \
		inline auto write_##NAME(const TYPE &value)                               \
		{                                                                         \
			return write<E, TYPE>(value);                                           \
		}                                                                         \
		inline auto write_##NAME##le(const TYPE &value)                           \
		{                                                                         \
			return write_le<TYPE>(value);                                           \
		}                                                                         \
		inline auto write_##NAME##be(const TYPE &value)                           \
		{                                                                         \
			return write_be<TYPE>(value);                                           \
		}
		LAK_FOREACH_INTEGER(BINARY_ARRAY_WRITER_MEMBERS)
		LAK_FOREACH_FLOAT(BINARY_ARRAY_WRITER_MEMBERS)
#	undef BINARY_ARRAY_WRITER_MEMBERS
	};
}

template<typename T, lak::endian E>
requires requires(const T value) {
	{ value.template write_size<E>() } -> lak::concepts::same_as<size_t>;
	{
		value.template write<E>(lak::declval<lak::binary_span_writer &>())
	} -> lak::concepts::of_template<lak::result>;
}
struct lak::to_bytes_traits<T, E>
{
	using _result_type = decltype(lak::declval<const T &>().template write<E>(
	  lak::declval<lak::binary_span_writer &>()));
	using value_type   = T;
	using error_type   = lak::result_err_type_t<_result_type>;
	static constexpr bool const_size = false;
	static constexpr size_t size     = lak::dynamic_extent;
	static_assert(lak::is_same_v<
	              lak::monostate,
	              typename decltype(lak::declval<const T &>().template write<E>(
	                lak::declval<lak::binary_span_writer &>()))::ok_type>);
	static_assert(!lak::is_trivial_variant_v<error_type>);

	static size_t dynamic_size(const T &value)
	{
		return value.template write_size<E>();
	}

	static lak::result<lak::span<byte_t>, error_type> to_bytes(
	  lak::span<byte_t> bytes, const T &value)
	{
		lak::binary_span_writer strm{bytes};
		RES_TRY(value.template write<E>(strm));
		return lak::ok_t{strm.remaining()};
	}
};

#endif
