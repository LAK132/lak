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

	struct out_of_data_error
	{
		inline friend std::ostream &operator<<(std::ostream &strm,
		                                       const out_of_data_error &)
		{
			return strm << "out of data";
		}
	};

	/* --- bytes_traits --- */

	template<typename T, lak::endian E>
	struct bytes_traits
	{
		/*
		using value_type = lak::nonesuch;
		using error_type = lak::nonesuch;

		static constexpr bool const_size = false;
		static constexpr size_t size     = lak::dynamic_extent;
		static_assert((size != lak::dynamic_extent) == const_size);

		static size_t dynamic_size(const T &) = delete;

		static void from_bytes(lak::span<const byte_t, size> bytes, T &value)
		requires(const_size)
		= delete;

		static lak::result<lak::span<const byte_t>, error_type> from_bytes(
		  lak::span<const byte_t> bytes, T &value) = delete;

		static void to_bytes(lak::span<byte_t, size> bytes, const T &value)
		requires(const_size)
		= delete;

		static lak::result<lak::span<byte_t>, error_type> to_bytes(
		  lak::span<byte_t> bytes, const T &value) = delete;
		*/
	};

	namespace concepts
	{
		template<typename T, lak::endian E>
		concept has_bytes_traits_to_traits = requires(T value) {
			typename lak::bytes_traits<T, E>::value_type;
			typename lak::bytes_traits<T, E>::error_type;
			lak::bytes_traits<T, E>::const_size;
			lak::bytes_traits<T, E>::size;

			{
				lak::bytes_traits<T, E>::dynamic_size(value)
			} -> lak::concepts::same_as<size_t>;

			{
				lak::bytes_traits<T, E>::to_bytes(lak::declval<lak::span<byte_t>>(),
				                                  value)
			} -> lak::concepts::same_as<
			  lak::result<lak::span<byte_t>,
			              typename lak::bytes_traits<T, E>::error_type>>;
		};

		template<typename T, lak::endian E>
		concept has_bytes_traits_from_traits = requires(T value) {
			typename lak::bytes_traits<T, E>::value_type;
			typename lak::bytes_traits<T, E>::error_type;
			lak::bytes_traits<T, E>::const_size;
			lak::bytes_traits<T, E>::size;

			{
				lak::bytes_traits<T, E>::from_bytes(
				  lak::declval<lak::span<const byte_t>>(), value)
			} -> lak::concepts::same_as<
			  lak::result<lak::span<const byte_t>,
			              typename lak::bytes_traits<T, E>::error_type>>;
		};

		template<typename T, lak::endian E>
		concept has_bytes_traits =
		  has_bytes_traits_to_traits<T, E> && has_bytes_traits_from_traits<T, E>;

		template<typename T, lak::endian E>
		concept has_const_size_bytes_traits_to_traits =
		  has_bytes_traits_to_traits<T, E> && requires(T value) {
			  {
				  lak::bytes_traits<T, E>::to_bytes(
				    lak::declval<lak::span<byte_t, lak::bytes_traits<T, E>::size>>(),
				    value)
			  } -> lak::concepts::same_as<void>;
		  };

		template<typename T, lak::endian E>
		concept has_const_size_bytes_traits_from_traits =
		  has_bytes_traits_from_traits<T, E> && requires(T value) {
			  {
				  lak::bytes_traits<T, E>::from_bytes(
				    lak::declval<
				      lak::span<const byte_t, lak::bytes_traits<T, E>::size>>(),
				    value)
			  } -> lak::concepts::same_as<void>;
		  };

		template<typename T, lak::endian E>
		concept has_const_size_bytes_traits =
		  has_const_size_bytes_traits_to_traits<T, E> &&
		  has_const_size_bytes_traits_from_traits<T, E>;
	}

	/* --- from_bytes_traits --- */

	template<typename T, lak::endian E>
	struct from_bytes_traits;

	template<lak::endian E, lak::concepts::has_bytes_traits_from_traits<E> T>
	struct from_bytes_traits<T, E>
	{
		using value_type = typename lak::bytes_traits<T, E>::value_type;
		using error_type = typename lak::bytes_traits<T, E>::error_type;

		static constexpr bool const_size = lak::bytes_traits<T, E>::const_size;
		static constexpr size_t size     = lak::bytes_traits<T, E>::size;
		static_assert((size != lak::dynamic_extent) == const_size);
		static_assert(
		  const_size ==
		  lak::concepts::has_const_size_bytes_traits_from_traits<T, E>);

		static void from_bytes(lak::span<const byte_t, size> bytes, T &value)
		requires(const_size)
		{
			return lak::bytes_traits<T, E>::from_bytes(bytes, value);
		}

		static lak::result<lak::span<const byte_t>, error_type> from_bytes(
		  lak::span<const byte_t> bytes, T &value)
		{
			return lak::bytes_traits<T, E>::from_bytes(bytes, value);
		}
	};

	template<typename V>
	struct _bytes_errors;
	template<typename ERR>
	struct _bytes_errors<lak::type_pack<ERR>> : lak::type_identity<ERR>
	{
	};
	template<typename... ERRS>
	struct _bytes_errors<lak::type_pack<ERRS...>>
	: lak::type_identity<lak::try_remove_variant_t<lak::create_from_pack_t<
	    lak::variant,
	    lak::remove_from_pack_t<lak::bottom, lak::type_pack<ERRS...>>>>>
	{
	};
	template<typename... ERR>
	using bytes_errors_t =
	  typename lak::_bytes_errors<lak::unique_pack_t<lak::flatten_pack_t<
	    lak::all_variants_to_type_pack_t<lak::variant<ERR...>>>>>::type;

	static_assert(lak::is_same_v<lak::bytes_errors_t<uint8_t, uint32_t>,
	                             lak::variant<uint8_t, uint32_t>>);
	static_assert(
	  lak::is_same_v<lak::bytes_errors_t<lak::variant<uint8_t, uint32_t>>,
	                 lak::variant<uint8_t, uint32_t>>);
	static_assert(
	  lak::is_same_v<
	    lak::bytes_errors_t<lak::variant<uint8_t, uint32_t>, lak::bottom>,
	    lak::variant<uint8_t, uint32_t>>);
	static_assert(lak::is_same_v<
	              lak::bytes_errors_t<uint8_t, lak::variant<uint8_t, uint32_t>>,
	              lak::variant<uint8_t, uint32_t>>);
	static_assert(lak::is_same_v<lak::bytes_errors_t<uint8_t>, uint8_t>);
	static_assert(
	  lak::is_same_v<lak::bytes_errors_t<lak::variant<uint8_t, lak::bottom>>,
	                 uint8_t>);
	static_assert(
	  lak::is_same_v<lak::bytes_errors_t<uint8_t, uint8_t>, uint8_t>);
	static_assert(
	  lak::is_same_v<lak::bytes_errors_t<lak::bottom, lak::variant<uint8_t>>,
	                 uint8_t>);

	template<lak::endian E, typename... T>
	using from_bytes_traits_errors_t = lak::bytes_errors_t<
	  typename lak::from_bytes_traits<lak::remove_const_t<T>, E>::error_type...>;

	namespace concepts
	{
		template<typename T, lak::endian E>
		concept from_bytes_readable = requires(T value) {
			{
				lak::from_bytes_traits<T, E>::from_bytes(
				  lak::declval<lak::span<const byte_t>>(), value)
			} -> lak::concepts::same_as<
			  lak::result<lak::span<const byte_t>,
			              typename lak::from_bytes_traits<T, E>::error_type>>;
		};
	}

	/* --- from_bytes --- */

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	T from_bytes(
	  lak::span<const byte_t, lak::from_bytes_traits<T, E>::size> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<lak::pair<T, lak::span<const byte_t>>,
	            typename lak::from_bytes_traits<T, E>::error_type>
	from_bytes(lak::span<const byte_t> bytes);

	template<lak::endian E = lak::endian::little, typename... T>
	requires((sizeof...(T) > 0) &&
	         (lak::from_bytes_traits<T, E>::const_size && ...))
	void from_bytes(
	  lak::span<const byte_t, (lak::from_bytes_traits<T, E>::size + ...)> bytes,
	  T &...values);

	template<lak::endian E = lak::endian::little, typename... T>
	requires((sizeof...(T) > 0) &&
	         (lak::from_bytes_traits<T, E>::const_size && ...))
	lak::result<lak::span<const byte_t>, lak::out_of_data_error> from_bytes(
	  lak::span<const byte_t> bytes, T &...values);

	template<lak::endian E = lak::endian::little, typename... T>
	requires((sizeof...(T) > 0) &&
	         !(lak::from_bytes_traits<T, E>::const_size && ...))
	lak::result<lak::span<const byte_t>,
	            lak::from_bytes_traits_errors_t<E, T...>>
	from_bytes(lak::span<const byte_t> bytes, T &...values);

	/* --- array_from_bytes --- */

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::array<T, S> array_from_bytes(
	  lak::span<const byte_t, lak::from_bytes_traits<T, E>::size * S> bytes);

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::array<T, S>, lak::out_of_data_error> array_from_bytes(
	  lak::span<const byte_t> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::array<T>, lak::out_of_data_error> array_from_bytes(
	  lak::span<const byte_t> bytes, size_t count);

	template<lak::endian E = lak::endian::little, typename T>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::span<const byte_t>, lak::out_of_data_error>
	array_from_bytes(lak::span<const byte_t> bytes, lak::span<T> values);

	template<lak::endian E = lak::endian::little, typename T, size_t S>
	requires((S != lak::dynamic_extent) &&
	         lak::from_bytes_traits<T, E>::const_size)
	void array_from_bytes(
	  lak::span<const byte_t, S * lak::from_bytes_traits<T, E>::size> bytes,
	  lak::span<T, S> values);

	template<typename T, lak::endian E = lak::endian::little>
	requires(!lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::pair<lak::array<T>, lak::span<const byte_t>>,
	            typename lak::from_bytes_traits<T, E>::error_type>
	array_from_bytes(lak::span<const byte_t> bytes, size_t count);

	template<lak::endian E = lak::endian::little, typename T>
	requires(!lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::span<const byte_t>,
	            typename lak::from_bytes_traits<T, E>::error_type>
	array_from_bytes(lak::span<const byte_t> bytes, lak::span<T> values);

	/* --- to_bytes_traits --- */

	template<typename T, lak::endian E>
	struct to_bytes_traits;

	template<lak::endian E, lak::concepts::has_bytes_traits_to_traits<E> T>
	struct to_bytes_traits<T, E>
	{
		using value_type = typename lak::bytes_traits<T, E>::value_type;
		using error_type = typename lak::bytes_traits<T, E>::error_type;

		static constexpr bool const_size = lak::bytes_traits<T, E>::const_size;
		static constexpr size_t size     = lak::bytes_traits<T, E>::size;
		static_assert((size != lak::dynamic_extent) == const_size);
		static_assert(const_size ==
		              lak::concepts::has_const_size_bytes_traits_to_traits<T, E>);

		static size_t dynamic_size(const T &value)
		{
			return lak::bytes_traits<T, E>::dynamic_size(value);
		}

		static void to_bytes(lak::span<byte_t, size> bytes, const T &value)
		requires(const_size)
		{
			return lak::bytes_traits<T, E>::to_bytes(bytes, value);
		}

		static lak::result<lak::span<byte_t>, error_type> to_bytes(
		  lak::span<byte_t> bytes, const T &value)
		{
			return lak::bytes_traits<T, E>::to_bytes(bytes, value);
		}
	};

	template<typename T, lak::endian E>
	struct to_bytes_traits<const T, E> : to_bytes_traits<T, E>
	{
	};

	template<lak::endian E, typename... T>
	using to_bytes_traits_errors_t = lak::bytes_errors_t<
	  typename lak::to_bytes_traits<lak::remove_const_t<T>, E>::error_type...>;

	namespace concepts
	{
		template<typename T, lak::endian E>
		concept to_bytes_writeable = requires(const T value) {
			{
				lak::to_bytes_traits<T, E>::to_bytes(lak::declval<lak::span<byte_t>>(),
				                                     value)
			} -> lak::concepts::same_as<
			  lak::result<lak::span<byte_t>,
			              typename lak::to_bytes_traits<T, E>::error_type>>;
		};
	}

	/* --- to_bytes --- */

	template<lak::endian E = lak::endian::little, typename T>
	requires(lak::to_bytes_traits<T, E>::const_size)
	void to_bytes(lak::span<byte_t, lak::to_bytes_traits<T, E>::size> bytes,
	              const T &value);

	template<lak::endian E = lak::endian::little, typename T>
	lak::result<lak::span<byte_t>,
	            typename lak::to_bytes_traits<T, E>::error_type>
	to_bytes(lak::span<byte_t> bytes, const T &value);

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
	lak::result<lak::span<byte_t>, lak::to_bytes_traits_errors_t<E, T...>>
	to_bytes(lak::span<byte_t> bytes, const T &...values);

	/* --- array_to_bytes --- */

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	lak::array<byte_t, S * lak::to_bytes_traits<T, E>::size> array_to_bytes(
	  lak::span<const T, S> values);

	// template<typename T, lak::endian E = lak::endian::little>
	// lak::array<byte_t> array_to_bytes(lak::span<const T> values);

	template<lak::endian E = lak::endian::little, typename T>
	lak::result<lak::span<byte_t>,
	            typename lak::to_bytes_traits<T, E>::error_type>
	array_to_bytes(lak::span<byte_t> bytes, lak::span<const T> values);

	template<lak::endian E = lak::endian::little, typename T, size_t S>
	requires((S != lak::dynamic_extent) &&
	         lak::to_bytes_traits<T, E>::const_size)
	void array_to_bytes(
	  lak::span<byte_t, S * lak::to_bytes_traits<T, E>::size> bytes,
	  lak::span<const T, S> values);

	/* --- bytes_traits_fixed_struct_impl --- */

	template<typename T, lak::endian E, auto... MEMBERS>
	struct to_bytes_traits_fixed_struct_impl;
	template<typename T, lak::endian E, auto... MEMBERS>
	struct from_bytes_traits_fixed_struct_impl;

#define LAK_FIXED_STRUCT_TO_BYTES_TRAITS(TYPE, ...)                           \
	template<lak::endian E>                                                     \
	struct lak::to_bytes_traits<TYPE, E>                                        \
	: lak::to_bytes_traits_fixed_struct_impl<TYPE, E, __VA_ARGS__>              \
	{                                                                           \
	};
#define LAK_FIXED_STRUCT_FROM_BYTES_TRAITS(TYPE, ...)                         \
	template<lak::endian E>                                                     \
	struct lak::from_bytes_traits<TYPE, E>                                      \
	: lak::from_bytes_traits_fixed_struct_impl<TYPE, E, __VA_ARGS__>            \
	{                                                                           \
	};
#define LAK_FIXED_STRUCT_BYTES_TRAITS(TYPE, ...)                              \
	LAK_FIXED_STRUCT_TO_BYTES_TRAITS(TYPE, __VA_ARGS__)                         \
	LAK_FIXED_STRUCT_FROM_BYTES_TRAITS(TYPE, __VA_ARGS__)

#define LAK_FIXED_STRUCT_TO_BYTES_TRAITS_EX(ENDIAN, TYPE, ...)                \
	template<lak::endian E>                                                     \
	struct lak::to_bytes_traits<TYPE, E>                                        \
	: lak::to_bytes_traits_fixed_struct_impl<TYPE, ENDIAN, __VA_ARGS__>         \
	{                                                                           \
	};
#define LAK_FIXED_STRUCT_FROM_BYTES_TRAITS_EX(ENDIAN, TYPE, ...)              \
	template<lak::endian E>                                                     \
	struct lak::from_bytes_traits<TYPE, E>                                      \
	: lak::from_bytes_traits_fixed_struct_impl<TYPE, ENDIAN, __VA_ARGS__>       \
	{                                                                           \
	};
#define LAK_FIXED_STRUCT_BYTES_TRAITS_EX(ENDIAN, TYPE, ...)                   \
	LAK_FIXED_STRUCT_TO_BYTES_TRAITS_EX(ENDIAN, TYPE, __VA_ARGS__)              \
	LAK_FIXED_STRUCT_FROM_BYTES_TRAITS_EX(ENDIAN, TYPE, __VA_ARGS__)

#define LAK_FIXED_TEMPLATE_STRUCT_TO_BYTES_TRAITS(TEMPL, TYPE, ...)           \
	template<TEMPL, lak::endian E>                                              \
	struct lak::to_bytes_traits<TYPE, E>                                        \
	: lak::to_bytes_traits_fixed_struct_impl<TYPE, E, __VA_ARGS__>              \
	{                                                                           \
	};
#define LAK_FIXED_TEMPLATE_STRUCT_FROM_BYTES_TRAITS(TEMPL, TYPE, ...)         \
	template<TEMPL, lak::endian E>                                              \
	struct lak::from_bytes_traits<TYPE, E>                                      \
	: lak::from_bytes_traits_fixed_struct_impl<TYPE, E, __VA_ARGS__>            \
	{                                                                           \
	};
#define LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS(TEMPL, TYPE, ...)              \
	LAK_FIXED_TEMPLATE_STRUCT_TO_BYTES_TRAITS(TEMPL, TYPE, __VA_ARGS__)         \
	LAK_FIXED_TEMPLATE_STRUCT_FROM_BYTES_TRAITS(TEMPL, TYPE, __VA_ARGS__)

#define LAK_FIXED_TEMPLATE_STRUCT_TO_BYTES_TRAITS_EX(                         \
  TEMPL, ENDIAN, TYPE, ...)                                                   \
	template<TEMPL, lak::endian E>                                              \
	struct lak::to_bytes_traits<TYPE, E>                                        \
	: lak::to_bytes_traits_fixed_struct_impl<TYPE, ENDIAN, __VA_ARGS__>         \
	{                                                                           \
	};
#define LAK_FIXED_TEMPLATE_STRUCT_FROM_BYTES_TRAITS_EX(                       \
  TEMPL, ENDIAN, TYPE, ...)                                                   \
	template<TEMPL, lak::endian E>                                              \
	struct lak::from_bytes_traits<TYPE, E>                                      \
	: lak::from_bytes_traits_fixed_struct_impl<TYPE, ENDIAN, __VA_ARGS__>       \
	{                                                                           \
	};
#define LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS_EX(TEMPL, ENDIAN, TYPE, ...)   \
	LAK_FIXED_TEMPLATE_STRUCT_TO_BYTES_TRAITS_EX(                               \
	  TEMPL, ENDIAN, TYPE, __VA_ARGS__)                                         \
	LAK_FIXED_TEMPLATE_STRUCT_FROM_BYTES_TRAITS_EX(                             \
	  TEMPL, ENDIAN, TYPE, __VA_ARGS__)
}

#define LAK_MEMCPY_BYTE_TRAITS_IMPL(TYPE, ...)                                \
	template<lak::endian E>                                                     \
	struct lak::bytes_traits<TYPE, E>                                           \
	{                                                                           \
		using value_type                 = TYPE;                                  \
		using error_type                 = lak::out_of_data_error;                \
		static constexpr bool const_size = true;                                  \
		static constexpr size_t size     = sizeof(value_type);                    \
		static_assert((size != lak::dynamic_extent) == const_size);               \
		static_assert(E == lak::endian::native || ~E == lak::endian::native);     \
                                                                              \
		static size_t dynamic_size(const value_type &) { return size; }           \
                                                                              \
		static void from_bytes(lak::span<const byte_t, size> bytes,               \
		                       value_type &value)                                 \
		{                                                                         \
			lak::memcpy(lak::as_bytes(&value), bytes);                              \
			if constexpr (E != lak::endian::native)                                 \
				lak::byte_swap<size>(lak::as_bytes(&value));                          \
		}                                                                         \
                                                                              \
		static lak::result<lak::span<const byte_t>, error_type> from_bytes(       \
		  lak::span<const byte_t> bytes, value_type &value)                       \
		{                                                                         \
			if (bytes.size() < size) return lak::err_t<lak::out_of_data_error>{};   \
			from_bytes(bytes.template first<size>(), value);                        \
			return lak::ok_t{bytes.subspan(size)};                                  \
		}                                                                         \
                                                                              \
		static void to_bytes(lak::span<byte_t, size> bytes,                       \
		                     const value_type &value)                             \
		{                                                                         \
			lak::memcpy(bytes, lak::as_bytes(&value));                              \
			if constexpr (E != lak::endian::native)                                 \
				lak::byte_swap<size>(lak::span<byte_t>(bytes));                       \
		}                                                                         \
                                                                              \
		static lak::result<lak::span<byte_t>, error_type> to_bytes(               \
		  lak::span<byte_t> bytes, const value_type &value)                       \
		{                                                                         \
			if (bytes.size() < size) return lak::err_t<lak::out_of_data_error>{};   \
			to_bytes(bytes.template first<size>(), value);                          \
			return lak::ok_t{bytes.subspan(size)};                                  \
		}                                                                         \
	};

LAK_MEMCPY_BYTE_TRAITS_IMPL(byte_t)
LAK_FOREACH_FLOAT(LAK_MEMCPY_BYTE_TRAITS_IMPL)
LAK_FOREACH_CHAR(LAK_MEMCPY_BYTE_TRAITS_IMPL)
LAK_FOREACH_INTEGER(LAK_MEMCPY_BYTE_TRAITS_IMPL)

static_assert(lak::concepts::from_bytes_readable<byte_t, lak::endian::native>);
static_assert(lak::concepts::from_bytes_readable<byte_t, lak::endian::native>);
static_assert(lak::concepts::from_bytes_readable<char, lak::endian::native>);
static_assert(
  lak::concepts::from_bytes_readable<uint8_t, lak::endian::native>);
static_assert(
  lak::concepts::from_bytes_readable<uint16_t, lak::endian::native>);
static_assert(
  lak::concepts::from_bytes_readable<uint32_t, lak::endian::native>);
static_assert(lak::concepts::from_bytes_readable<float, lak::endian::native>);

static_assert(lak::concepts::to_bytes_writeable<byte_t, lak::endian::native>);
static_assert(lak::concepts::to_bytes_writeable<byte_t, lak::endian::native>);
static_assert(lak::concepts::to_bytes_writeable<char, lak::endian::native>);
static_assert(lak::concepts::to_bytes_writeable<uint8_t, lak::endian::native>);
static_assert(
  lak::concepts::to_bytes_writeable<uint16_t, lak::endian::native>);
static_assert(
  lak::concepts::to_bytes_writeable<uint32_t, lak::endian::native>);
static_assert(lak::concepts::to_bytes_writeable<float, lak::endian::native>);

#include "lak/binary_reader.hpp"
#include "lak/binary_writer.hpp"

#include "binary_traits.inl"

#endif
