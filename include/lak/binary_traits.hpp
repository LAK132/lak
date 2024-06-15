#ifndef LAK_BINARY_TRAITS_HPP
#define LAK_BINARY_TRAITS_HPP

#include "lak/array.hpp"
#include "lak/endian.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/type_traits.hpp"

namespace lak
{
	struct binary_reader;
	struct binary_span_writer;

	/* --- bytes_traits --- */

	template<typename T, lak::endian E>
	struct bytes_traits
	{
		using value_type = lak::nonesuch;

		static constexpr bool const_size = false;
		static constexpr size_t size     = lak::dynamic_extent;
		static_assert((size != lak::dynamic_extent) == const_size);

		static size_t dynamic_size(const T &) = delete;

		static void from_bytes(lak::span<const byte_t, size> bytes, T &value)
		requires(const_size)
		= delete;

		static lak::result<lak::span<const byte_t>> from_bytes(
		  lak::span<const byte_t> bytes, T &value) = delete;

		static void to_bytes(lak::span<byte_t, size> bytes, const T &value)
		requires(const_size)
		= delete;

		static lak::result<lak::span<byte_t>> to_bytes(lak::span<byte_t> bytes,
		                                               const T &value) = delete;
	};

	/* --- from_bytes_traits --- */

	template<typename T, lak::endian E>
	struct from_bytes_traits
	{
		using value_type = typename lak::bytes_traits<T, E>::value_type;

		static constexpr bool const_size = lak::bytes_traits<T, E>::const_size;
		static constexpr size_t size     = lak::bytes_traits<T, E>::size;
		static_assert((size != lak::dynamic_extent) == const_size);

		static void from_bytes(lak::span<const byte_t, size> bytes, T &value)
		requires(const_size)
		{
			return lak::bytes_traits<T, E>::from_bytes(bytes, value);
		}

		static lak::result<lak::span<const byte_t>> from_bytes(
		  lak::span<const byte_t> bytes, T &value)
		{
			return lak::bytes_traits<T, E>::from_bytes(bytes, value);
		}
	};

	namespace concepts
	{
		template<typename T, lak::endian E>
		concept from_bytes_readable = requires(T value) {
			{
				lak::from_bytes_traits<T, E>::from_bytes(
				  lak::declval<lak::span<const byte_t>>(), value)
			} -> lak::concepts::same_as<lak::result<lak::span<const byte_t>>>;
		};
	}

	/* --- from_bytes --- */

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	T from_bytes(
	  lak::span<const byte_t, lak::from_bytes_traits<T, E>::size> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<lak::pair<T, lak::span<const byte_t>>> from_bytes(
	  lak::span<const byte_t> bytes);

	template<lak::endian E = lak::endian::little, typename... T>
	requires((sizeof...(T) > 0) &&
	         (lak::from_bytes_traits<T, E>::const_size && ...))
	void from_bytes(
	  lak::span<const byte_t, (lak::from_bytes_traits<T, E>::size + ...)> bytes,
	  T &...values);

	template<lak::endian E = lak::endian::little, typename... T>
	requires((sizeof...(T) > 0) &&
	         (lak::from_bytes_traits<T, E>::const_size && ...))
	lak::result<lak::span<const byte_t>> from_bytes(
	  lak::span<const byte_t> bytes, T &...values);

	template<lak::endian E = lak::endian::little, typename... T>
	requires((sizeof...(T) > 0) &&
	         !(lak::from_bytes_traits<T, E>::const_size && ...))
	lak::result<lak::span<const byte_t>> from_bytes(
	  lak::span<const byte_t> bytes, T &...values);

	/* --- array_from_bytes --- */

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::array<T, S> array_from_bytes(
	  lak::span<const byte_t, lak::from_bytes_traits<T, E>::size * S> bytes);

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::array<T, S>> array_from_bytes(
	  lak::span<const byte_t> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::array<T>> array_from_bytes(lak::span<const byte_t> bytes,
	                                            size_t count);

	template<lak::endian E = lak::endian::little, typename T>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::span<const byte_t>> array_from_bytes(
	  lak::span<const byte_t> bytes, lak::span<T> values);

	template<lak::endian E = lak::endian::little, typename T, size_t S>
	requires((S != lak::dynamic_extent) &&
	         lak::from_bytes_traits<T, E>::const_size)
	void array_from_bytes(
	  lak::span<const byte_t, S * lak::from_bytes_traits<T, E>::size> bytes,
	  lak::span<T, S> values);

	template<typename T, lak::endian E = lak::endian::little>
	requires(!lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::pair<lak::array<T>, lak::span<const byte_t>>>
	array_from_bytes(lak::span<const byte_t> bytes, size_t count);

	template<lak::endian E = lak::endian::little, typename T>
	requires(!lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::span<const byte_t>> array_from_bytes(
	  lak::span<const byte_t> bytes, lak::span<T> values);

	/* --- to_bytes_traits --- */

	template<typename T, lak::endian E>
	struct to_bytes_traits
	{
		using value_type = typename lak::bytes_traits<T, E>::value_type;

		static constexpr bool const_size = lak::bytes_traits<T, E>::const_size;
		static constexpr size_t size     = lak::bytes_traits<T, E>::size;
		static_assert((size != lak::dynamic_extent) == const_size);

		static size_t dynamic_size(const T &value)
		{
			return lak::bytes_traits<T, E>::dynamic_size(value);
		}

		static lak::result<lak::span<byte_t>> to_bytes(lak::span<byte_t> bytes,
		                                               const T &value)
		{
			return lak::bytes_traits<T, E>::to_bytes(bytes, value);
		}

		static void to_bytes(lak::span<byte_t, size> bytes, const T &value)
		requires(const_size)
		{
			return lak::bytes_traits<T, E>::to_bytes(bytes, value);
		}
	};

	namespace concepts
	{
		template<typename T, lak::endian E>
		concept to_bytes_writeable = requires(const T value) {
			{
				lak::to_bytes_traits<T, E>::to_bytes(lak::declval<lak::span<byte_t>>(),
				                                     value)
			} -> lak::concepts::same_as<lak::result<lak::span<byte_t>>>;
		};
	}

	/* --- to_bytes --- */

	template<lak::endian E = lak::endian::little, typename T>
	requires(lak::to_bytes_traits<T, E>::const_size)
	void to_bytes(lak::span<byte_t, lak::to_bytes_traits<T, E>::size> bytes,
	              const T &value);

	template<lak::endian E = lak::endian::little, typename T>
	lak::result<lak::span<byte_t>> to_bytes(lak::span<byte_t> bytes,
	                                        const T &value);

	template<lak::endian E = lak::endian::little, typename T>
	requires(lak::to_bytes_traits<T, E>::const_size)
	lak::array<byte_t, lak::to_bytes_traits<T, E>::size> to_bytes(
	  const T &value);

	template<lak::endian E = lak::endian::little, typename... T>
	requires((sizeof...(T) > 0) &&
	         (lak::to_bytes_traits<T, E>::const_size && ...))
	void to_bytes(
	  lak::span<byte_t, (lak::to_bytes_traits<T, E>::size + ...)> bytes,
	  const T &...values);

	template<lak::endian E = lak::endian::little, typename... T>
	requires((sizeof...(T) > 0) &&
	         (lak::to_bytes_traits<T, E>::const_size && ...))
	lak::array<byte_t, (lak::to_bytes_traits<T, E>::size + ...)> to_bytes(
	  const T &...values);

	template<lak::endian E = lak::endian::little, typename... T>
	requires((sizeof...(T) > 0) &&
	         !(lak::to_bytes_traits<T, E>::const_size && ...))
	lak::result<lak::span<byte_t>> to_bytes(lak::span<byte_t> bytes,
	                                        const T &...values);

	/* --- array_to_bytes --- */

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	lak::array<byte_t, S * lak::to_bytes_traits<T, E>::size> array_to_bytes(
	  lak::span<const T, S> values);

	// template<typename T, lak::endian E = lak::endian::little>
	// lak::array<byte_t> array_to_bytes(lak::span<const T> values);

	template<lak::endian E = lak::endian::little, typename T>
	lak::result<lak::span<byte_t>> array_to_bytes(lak::span<byte_t> bytes,
	                                              lak::span<const T> values);

	template<lak::endian E = lak::endian::little, typename T, size_t S>
	requires((S != lak::dynamic_extent) &&
	         lak::to_bytes_traits<T, E>::const_size)
	void array_to_bytes(
	  lak::span<byte_t, S * lak::to_bytes_traits<T, E>::size> bytes,
	  lak::span<const T, S> values);

	/* --- bytes_traits_fixed_struct_impl --- */

	template<typename T, lak::endian E, lak::endian E2, auto... MEMBERS>
	struct to_bytes_traits_fixed_struct_impl;
	template<typename T, lak::endian E, lak::endian E2, auto... MEMBERS>
	struct from_bytes_traits_fixed_struct_impl;

#define LAK_FIXED_STRUCT_BYTES_TRAITS(TYPE, ...)                              \
	template<lak::endian E>                                                     \
	struct lak::to_bytes_traits<TYPE, E>                                        \
	: lak::to_bytes_traits_fixed_struct_impl<TYPE, E, E, __VA_ARGS__>           \
	{                                                                           \
	};                                                                          \
	template<lak::endian E>                                                     \
	struct lak::from_bytes_traits<TYPE, E>                                      \
	: lak::from_bytes_traits_fixed_struct_impl<TYPE, E, E, __VA_ARGS__>         \
	{                                                                           \
	};
#define LAK_FIXED_STRUCT_BYTES_TRAITS_EX(ENDIAN, TYPE, ...)                   \
	template<lak::endian E>                                                     \
	struct lak::to_bytes_traits<TYPE, E>                                        \
	: lak::to_bytes_traits_fixed_struct_impl<TYPE, E, ENDIAN, __VA_ARGS__>      \
	{                                                                           \
	};                                                                          \
	template<lak::endian E>                                                     \
	struct lak::from_bytes_traits<TYPE, E>                                      \
	: lak::from_bytes_traits_fixed_struct_impl<TYPE, E, ENDIAN, __VA_ARGS__>    \
	{                                                                           \
	};

#define LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS(TEMPL, TYPE, ...)              \
	template<TEMPL, lak::endian E>                                              \
	struct lak::to_bytes_traits<TYPE, E>                                        \
	: lak::to_bytes_traits_fixed_struct_impl<TYPE, E, E, __VA_ARGS__>           \
	{                                                                           \
	};                                                                          \
	template<TEMPL, lak::endian E>                                              \
	struct lak::from_bytes_traits<TYPE, E>                                      \
	: lak::from_bytes_traits_fixed_struct_impl<TYPE, E, E, __VA_ARGS__>         \
	{                                                                           \
	};
#define LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS_EX(TEMPL, ENDIAN, TYPE, ...)   \
	template<TEMPL, lak::endian E>                                              \
	struct lak::to_bytes_traits<TYPE, E>                                        \
	: lak::to_bytes_traits_fixed_struct_impl<TYPE, E, ENDIAN, __VA_ARGS__>      \
	{                                                                           \
	};                                                                          \
	template<TEMPL, lak::endian E>                                              \
	struct lak::from_bytes_traits<TYPE, E>                                      \
	: lak::from_bytes_traits_fixed_struct_impl<TYPE, E, ENDIAN, __VA_ARGS__>    \
	{                                                                           \
	};

}

#include "lak/binary_reader.hpp"
#include "lak/binary_writer.hpp"

#include "binary_traits.inl"

#endif
