#include "lak/binary_traits.hpp"

#include "lak/compiler.hpp"
#include "lak/memmanip.hpp"
#include "lak/result.hpp"
#include "lak/span_manip.hpp"
#include "lak/utility.hpp"

#include "lak/char.hpp"
#include "lak/stdint.hpp"

/* --- from_bytes --- */

template<typename T, lak::endian E>
requires(lak::from_bytes_traits<T, E>::const_size)
T lak::from_bytes(
  lak::span<const byte_t, lak::from_bytes_traits<T, E>::size> bytes)
{
	T result;
	lak::from_bytes_traits<T, E>::from_bytes(bytes, result);
	return result;
}

template<typename T, lak::endian E>
lak::result<lak::pair<T, lak::span<const byte_t>>> lak::from_bytes(
  lak::span<const byte_t> bytes)
{
	lak::pair<T, lak::span<const byte_t>> result;
	RES_TRY_ASSIGN(
	  result.second =,
	  lak::from_bytes_traits<T, E>::from_bytes(bytes, result.first));
	return lak::move_ok(result);
}

namespace lak
{
	template<lak::endian E, typename... T, size_t... OFFSETS>
	requires((sizeof...(T) > 0) &&
	         (lak::from_bytes_traits<T, E>::const_size && ...))
	void _from_bytes(
	  lak::span<const byte_t, (lak::from_bytes_traits<T, E>::size + ...)> bytes,
	  lak::index_sequence<OFFSETS...>,
	  T &...values)
	{
		((lak::from_bytes_traits<T, E>::from_bytes(
		   bytes.template subspan<OFFSETS, lak::from_bytes_traits<T, E>::size>(),
		   values)),
		 ...);
	}
}

template<lak::endian E, typename... T>
requires((sizeof...(T) > 0) &&
         (lak::from_bytes_traits<T, E>::const_size && ...))
void lak::from_bytes(
  lak::span<const byte_t, (lak::from_bytes_traits<T, E>::size + ...)> bytes,
  T &...values)
{
	lak::_from_bytes<E>(
	  bytes, lak::offset_sequence_for<size_t, sizeof(T)...>{}, values...);
}

template<lak::endian E, typename... T>
requires((sizeof...(T) > 0) &&
         (lak::from_bytes_traits<T, E>::const_size && ...))
lak::result<lak::span<const byte_t>> lak::from_bytes(
  lak::span<const byte_t> bytes, T &...values)
{
	constexpr size_t size = (lak::from_bytes_traits<T, E>::size + ...);
	if (bytes.size() < size) return lak::err_t{};
	lak::from_bytes<E>(bytes.template first<size>(), values...);
	return lak::ok_t{bytes.subspan(size)};
}

template<lak::endian E, typename... T>
requires((sizeof...(T) > 0) &&
         !(lak::from_bytes_traits<T, E>::const_size && ...))
lak::result<lak::span<const byte_t>> lak::from_bytes(
  lak::span<const byte_t> bytes, T &...values)
{
	if (bytes.empty()) return lak::err_t{};
	auto do_from_bytes = [&]<typename J>(J &v) -> lak::result<>
	{
		RES_TRY_ASSIGN(bytes =,
		               lak::from_bytes_traits<J, E>::from_bytes(bytes, v));
		return lak::ok_t{};
	};
	if (((do_from_bytes(values).is_ok()) && ...)) return lak::ok_t{bytes};
	return lak::err_t{};
}

/* --- array_from_bytes --- */

template<typename T, size_t S, lak::endian E>
requires(lak::from_bytes_traits<T, E>::const_size)
lak::array<T, S> lak::array_from_bytes(
  lak::span<const byte_t, lak::from_bytes_traits<T, E>::size * S> bytes)
{
	lak::array<T, S> result;
	for (size_t i = 0U; T & res : result)
		lak::from_bytes<T, E>(
		  bytes.subspan((i++) * lak::from_bytes_traits<T, E>::size)
		    .template first<lak::from_bytes_traits<T, E>::size>(),
		  res);
	return result;
}

template<typename T, size_t S, lak::endian E>
requires(lak::from_bytes_traits<T, E>::const_size)
lak::result<lak::array<T, S>> lak::array_from_bytes(
  lak::span<const byte_t> bytes)
{
	return lak::first_as_const_sized<lak::from_bytes_traits<T, E>::size * S>(
	         bytes)
	  .map(static_cast<lak::array<T, S> (*)(
	         lak::span<const byte_t, lak::from_bytes_traits<T, E>::size * S>)>(
	    &lak::array_from_bytes<T, S, E>));
}

template<typename T, lak::endian E>
requires(lak::from_bytes_traits<T, E>::const_size)
lak::result<lak::array<T>> lak::array_from_bytes(lak::span<const byte_t> bytes,
                                                 size_t count)
{
	constexpr size_t single_size = lak::from_bytes_traits<T, E>::size;
	const size_t req_size        = single_size * count;

	if (bytes.size() < req_size) return lak::err_t{};

	lak::array<T> result;
	result.reserve(count);

	for (size_t i = 0U; i < count; ++i)
	{
		lak::from_bytes<E>(bytes.template first<single_size>(),
		                   result.emplace_back());
		bytes = bytes.subspan(single_size);
	}

	return lak::move_ok(result);
}

template<lak::endian E, typename T>
requires(lak::from_bytes_traits<T, E>::const_size)
lak::result<lak::span<const byte_t>> lak::array_from_bytes(
  lak::span<const byte_t> bytes, lak::span<T> values)
{
	constexpr size_t size = lak::from_bytes_traits<T, E>::size;

	if (values.size() > bytes.size() * size) return lak::err_t{};

	for (size_t i = 0U; T & val : values)
		lak::from_bytes_traits<T, E>::from_bytes(
		  bytes.subspan((i++) * size).template first<size>(), val);

	return lak::ok_t{bytes.subspan(values.size() * size)};
}

template<lak::endian E, typename T, size_t S>
requires((S != lak::dynamic_extent) &&
         lak::from_bytes_traits<T, E>::const_size)
void lak::array_from_bytes(
  lak::span<const byte_t, S * lak::from_bytes_traits<T, E>::size> bytes,
  lak::span<T, S> values)
{
	constexpr size_t size = lak::from_bytes_traits<T, E>::size;
	for (size_t i = 0U; T & val : values)
		lak::from_bytes<E>(bytes.subspan((i++) * size).template first<size>(),
		                   val);
}

template<typename T, lak::endian E>
requires(!lak::from_bytes_traits<T, E>::const_size)
lak::result<lak::pair<lak::array<T>, lak::span<const byte_t>>>
lak::array_from_bytes(lak::span<const byte_t> bytes, size_t count)
{
	lak::pair<lak::array<T>, lak::span<const byte_t>> result;
	result.first.reserve(count);

	for (size_t i = 0U; i < count; ++i)
	{
		RES_TRY_ASSIGN(bytes =,
		               lak::from_bytes_traits<T, E>::from_bytes(
		                 bytes, result.first.emplace_back()));
	}

	return lak::move_ok(result);
}

template<lak::endian E, typename T>
requires(!lak::from_bytes_traits<T, E>::const_size)
lak::result<lak::span<const byte_t>> lak::array_from_bytes(
  lak::span<const byte_t> bytes, lak::span<T> values)
{
	for (T &v : values)
	{
		RES_TRY_ASSIGN(bytes =,
		               lak::from_bytes_traits<T, E>::from_bytes(bytes, v));
	}
	return lak::ok_t{bytes};
}

/* --- to_bytes --- */

template<lak::endian E, typename T>
requires(lak::to_bytes_traits<T, E>::const_size)
void lak::to_bytes(lak::span<byte_t, lak::to_bytes_traits<T, E>::size> bytes,
                   const T &value)
{
	lak::to_bytes_traits<T, E>::to_bytes(bytes, value);
}

template<lak::endian E, typename T>
lak::result<lak::span<byte_t>> lak::to_bytes(lak::span<byte_t> bytes,
                                             const T &value)
{
	return lak::to_bytes_traits<T, E>::to_bytes(bytes, value);
}

template<lak::endian E, typename T>
requires(lak::to_bytes_traits<T, E>::const_size)
lak::array<byte_t, lak::to_bytes_traits<T, E>::size> lak::to_bytes(
  const T &value)
{
	lak::array<byte_t, lak::to_bytes_traits<T, E>::size> result;
	lak::to_bytes_traits<T, E> to_bytes(
	  lak::span<byte_t, lak::to_bytes_traits<T, E>::size>(result), value);
	return result;
}

namespace lak
{
	template<lak::endian E, typename... T, size_t... OFFSETS>
	requires((lak::to_bytes_traits<T, E>::const_size && ...))
	void _to_bytes(
	  lak::span<byte_t, (lak::to_bytes_traits<T, E>::size + ...)> bytes,
	  lak::index_sequence<OFFSETS...>,
	  const T &...values)
	{
		((lak::to_bytes<T, E>(
		   bytes.template subspan<OFFSETS, lak::to_bytes_traits<T, E>::size>(),
		   values)),
		 ...);
	}
}

template<lak::endian E, typename... T>
requires((sizeof...(T) > 0) && (lak::to_bytes_traits<T, E>::const_size && ...))
void lak::to_bytes(
  lak::span<byte_t, (lak::to_bytes_traits<T, E>::size + ...)> bytes,
  const T &...values)
{
	lak::_to_bytes<E>(
	  bytes, lak::offset_sequence_for<size_t, sizeof(T)...>{}, values...);
}

template<lak::endian E, typename... T>
requires((sizeof...(T) > 0) && (lak::to_bytes_traits<T, E>::const_size && ...))
lak::array<byte_t, (lak::to_bytes_traits<T, E>::size + ...)> lak::to_bytes(
  const T &...values)
{
	lak::array<byte_t, (lak::to_bytes_traits<T, E>::size + ...)> bytes;
	lak::to_bytes(bytes, values...);
	return bytes;
}

template<lak::endian E, typename... T>
requires((sizeof...(T) > 0) &&
         !(lak::to_bytes_traits<T, E>::const_size && ...))
lak::result<lak::span<byte_t>> lak::to_bytes(lak::span<byte_t> bytes,
                                             const T &...values)
{
	auto do_to_bytes = [&]<typename J>(const J &v) -> lak::result<>
	{
		RES_TRY_ASSIGN(bytes =, lak::to_bytes<J, E>(bytes, v));
		return lak::ok_t{};
	};
	RES_TRY(((do_to_bytes(values)).is_ok() && ...));
	return lak::ok_t{bytes};
}

/* --- array_to_bytes --- */

template<typename T, size_t S, lak::endian E>
requires(lak::to_bytes_traits<T, E>::const_size)
lak::array<byte_t, S * lak::to_bytes_traits<T, E>::size> lak::array_to_bytes(
  lak::span<const T, S> values)
{
	lak::array<byte_t, S * lak::to_bytes_traits<T, E>::size> result;
	auto bytes{lak::span(result)};

	for (size_t i = 0U; const T &val : values)
		lak::to_bytes<T, E>(bytes.subspan((i++) * lak::to_bytes_traits<T, E>::size)
		                      .template first<lak::to_bytes_traits<T, E>::size>(),
		                    val);

	return result;
}

// template<typename T, lak::endian E>
// lak::array<byte_t> lak::array_to_bytes(lak::span<const T> values)
// {
// 	static_assert(!lak::is_const_v<T>);
// 	lak::array<byte_t> result;
// 	result.resize(lak::to_bytes_size<T, E>(values));
// 	lak::to_bytes_traits<T, E>::to_bytes(
// 	  lak::to_bytes_data<T, E>::maybe_make(lak::span<byte_t>(result), values)
// 	    .unwrap());
// 	return result;
// }

template<lak::endian E, typename T>
lak::result<lak::span<byte_t>> lak::array_to_bytes(lak::span<byte_t> bytes,
                                                   lak::span<const T> values)
{
	for (const T &val : values)
	{
		RES_TRY_ASSIGN(bytes =, lak::to_bytes_traits<T, E>::to_bytes(bytes, val));
	}
	return lak::ok_t{bytes};
}

template<lak::endian E, typename T, size_t S>
requires((S != lak::dynamic_extent) && lak::to_bytes_traits<T, E>::const_size)
void lak::array_to_bytes(
  lak::span<byte_t, S * lak::to_bytes_traits<T, E>::size> bytes,
  lak::span<const T, S> values)
{
	constexpr size_t size = lak::to_bytes_traits<T, E>::size;
	for (size_t i = 0U; const T &val : values)
		lak::to_bytes_traits<T, E>::to_bytes(
		  bytes.subspan((i++) * size).template first<size>(), val);
}

/* --- bytes_traits_fixed_struct_impl --- */

namespace
{
	namespace local
	{
		template<lak::endian E, auto... MEMBERS>
		static constexpr bool _members_are_const_size_v =
		  ((lak::to_bytes_traits<
		     lak::remove_reference_t<lak::remove_member_pointer_decl_t<MEMBERS>>,
		     E>::const_size) &&
		   ...);

		template<lak::endian E, typename T, auto... MEMBERS>
		static constexpr size_t _members_total_to_bytes_size_v =
		  local::_members_are_const_size_v<E, MEMBERS...>
		    ? ((lak::to_bytes_traits<
		         lak::remove_reference_t<decltype(lak::declval<T>().*MEMBERS)>,
		         E>::size) +
		       ...)
		    : lak::dynamic_extent;

		template<lak::endian E, typename T, auto... MEMBERS>
		static constexpr size_t _members_total_from_bytes_size_v =
		  local::_members_are_const_size_v<E, MEMBERS...>
		    ? ((lak::from_bytes_traits<
		         lak::remove_reference_t<decltype(lak::declval<T>().*MEMBERS)>,
		         E>::size) +
		       ...)
		    : lak::dynamic_extent;
	}
}

template<typename T, lak::endian E, auto... MEMBERS>
requires((sizeof...(MEMBERS) > 0) &&
         ((lak::is_member_pointer_for_v<decltype(MEMBERS), T>)&&...))
struct lak::to_bytes_traits_fixed_struct_impl<T, E, MEMBERS...>
{
	using value_type = T;
	static constexpr bool const_size =
	  local::_members_are_const_size_v<E, MEMBERS...>;
	static constexpr size_t size =
	  local::_members_total_to_bytes_size_v<E, T, MEMBERS...>;

	static size_t dynamic_size(const T &value)
	{
		if constexpr (const_size)
			return size;
		else
			return (
			  (lak::to_bytes_traits<
			    lak::remove_reference_t<lak::remove_member_pointer_decl_t<MEMBERS>>,
			    E>::dynamic_size(value.*MEMBERS)) +
			  ...);
	}

	static void to_bytes(lak::span<byte_t, size> bytes, const T &value)
	requires(const_size)
	{
		return lak::to_bytes<E>(bytes, value.*MEMBERS...);
	}

	static lak::result<lak::span<byte_t>> to_bytes(lak::span<byte_t> bytes,
	                                               const T &value)
	{
		return lak::to_bytes<E>(bytes, value.*MEMBERS...);
	}
};

template<typename T, lak::endian E, auto... MEMBERS>
requires((sizeof...(MEMBERS) > 0) &&
         ((lak::is_member_pointer_for_v<decltype(MEMBERS), T>)&&...))
struct lak::from_bytes_traits_fixed_struct_impl<T, E, MEMBERS...>
{
	using value_type = T;
	static constexpr bool const_size =
	  local::_members_are_const_size_v<E, MEMBERS...>;
	static constexpr size_t size =
	  local::_members_total_from_bytes_size_v<E, T, MEMBERS...>;

	static void from_bytes(lak::span<const byte_t, size> bytes, T &value)
	requires(const_size)
	{
		return lak::from_bytes<E>(bytes, value.*MEMBERS...);
	}

	static lak::result<lak::span<const byte_t>> from_bytes(
	  lak::span<const byte_t> bytes, T &value)
	{
		return lak::from_bytes<E>(bytes, value.*MEMBERS...);
	}
};

namespace lak
{
	template<typename T, lak::endian E>
	struct _memcpy_bytes_traits
	{
		using value_type                 = T;
		static constexpr bool const_size = true;
		static constexpr size_t size     = sizeof(T);
		static_assert((size != lak::dynamic_extent) == const_size);
		static_assert(E == lak::endian::native || ~E == lak::endian::native);

		static size_t dynamic_size(const T &) { return size; }

		static void from_bytes(lak::span<const byte_t, size> bytes, T &value)
		{
			lak::memcpy(lak::as_bytes(&value), bytes);
			if constexpr (E != lak::endian::native)
				lak::byte_swap<size>(lak::as_bytes(&value));
		}

		static lak::result<lak::span<const byte_t>> from_bytes(
		  lak::span<const byte_t> bytes, T &value)
		{
			if (bytes.size() < size) return lak::err_t{};
			from_bytes(bytes.template first<size>(), value);
			return lak::ok_t{bytes.subspan(size)};
		}

		static void to_bytes(lak::span<byte_t, size> bytes, const T &value)
		{
			lak::memcpy(bytes, lak::as_bytes(&value));
			if constexpr (E != lak::endian::native)
				lak::byte_swap<size>(lak::span<byte_t>(bytes));
		}

		static lak::result<lak::span<byte_t>> to_bytes(lak::span<byte_t> bytes,
		                                               const T &value)
		{
			if (bytes.size() < size) return lak::err_t{};
			to_bytes(bytes.template first<size>(), value);
			return lak::ok_t{bytes.subspan(size)};
		}
	};
}

#define LAK_MEMCPY_BYTE_TRAITS_IMPL(TYPE, ...)                                \
	template<lak::endian E>                                                     \
	struct lak::bytes_traits<TYPE, E> : lak::_memcpy_bytes_traits<TYPE, E>      \
	{                                                                           \
	};

LAK_MEMCPY_BYTE_TRAITS_IMPL(byte_t)
LAK_FOREACH_INTEGER(LAK_MEMCPY_BYTE_TRAITS_IMPL)
LAK_FOREACH_FLOAT(LAK_MEMCPY_BYTE_TRAITS_IMPL)
LAK_FOREACH_CHAR(LAK_MEMCPY_BYTE_TRAITS_IMPL)

#undef LAK_MEMCPY_BYTE_TRAITS_IMPL

/* --- lak::span --- */

template<typename T, size_t S, lak::endian E>
requires(S != lak::dynamic_extent && lak::from_bytes_traits<T, E>::const_size)
struct lak::from_bytes_traits<lak::span<T, S>, E>
{
	using value_type                 = lak::span<T, S>;
	static constexpr bool const_size = true;
	static constexpr size_t size     = lak::from_bytes_traits<T, E>::size * S;

	static void from_bytes(lak::span<const byte_t, size> bytes,
	                       lak::span<T, S> &value)
	{
		return lak::array_from_bytes<E, T, S>(bytes, value);
	}

	static lak::result<lak::span<const byte_t>> from_bytes(
	  lak::span<const byte_t> bytes, lak::span<T, S> &value)
	{
		return lak::array_from_bytes<E, T>(bytes, value);
	}
};

template<typename T, size_t S, lak::endian E>
requires(S != lak::dynamic_extent && lak::to_bytes_traits<T, E>::const_size)
struct lak::to_bytes_traits<lak::span<T, S>, E>
{
	using value_type                 = lak::span<T, S>;
	static constexpr bool const_size = true;
	static constexpr size_t size =
	  lak::to_bytes_traits<lak::remove_const_t<T>, E>::size * S;

	static void to_bytes(lak::span<byte_t, size> bytes,
	                     const lak::span<T, S> &value)
	{
		return lak::array_to_bytes<E, T, S>(bytes, value);
	}

	static lak::result<lak::span<byte_t>> to_bytes(lak::span<byte_t> bytes,
	                                               const lak::span<T, S> &value)
	{
		return lak::array_to_bytes<E, T>(bytes, value);
	}
};

/* --- lak::array --- */

template<typename T, size_t S, lak::endian E>
requires(S != lak::dynamic_extent && lak::from_bytes_traits<T, E>::const_size)
struct lak::from_bytes_traits<lak::array<T, S>, E>
{
	using value_type                 = lak::array<T, S>;
	static constexpr bool const_size = true;
	static constexpr size_t size     = lak::from_bytes_traits<T, E>::size * S;

	static void from_bytes(lak::span<const byte_t, size> bytes,
	                       lak::array<T, S> &value)
	{
		return lak::array_from_bytes<E, T, S>(bytes, lak::span<T, S>(value));
	}

	static lak::result<lak::span<const byte_t>> from_bytes(
	  lak::span<const byte_t> bytes, lak::array<T, S> &value)
	{
		return lak::array_from_bytes<E, T>(bytes, lak::span<T>(value));
	}
};

template<typename T, size_t S, lak::endian E>
requires(S != lak::dynamic_extent && lak::to_bytes_traits<T, E>::const_size)
struct lak::to_bytes_traits<lak::array<T, S>, E>
{
	using value_type                 = lak::array<T, S>;
	static constexpr bool const_size = true;
	static constexpr size_t size =
	  lak::to_bytes_traits<lak::remove_const_t<T>, E>::size * S;

	static void to_bytes(lak::span<byte_t, size> bytes,
	                     const lak::array<T, S> &value)
	{
		return lak::array_to_bytes<E, T, S>(bytes, lak::span<const T, S>(value));
	}

	static lak::result<lak::span<byte_t>> to_bytes(lak::span<byte_t> bytes,
	                                               const lak::array<T, S> &value)
	{
		return lak::array_to_bytes<E, T>(bytes, lak::span<const T>(value));
	}
};

/* --- lak::reference_wrapper --- */

template<typename T, lak::endian E>
requires(!lak::is_const_v<T>)
struct lak::from_bytes_traits<lak::reference_wrapper<T>, E>
{
	using value_type = lak::reference_wrapper<T>;

	static constexpr bool const_size = lak::from_bytes_traits<T, E>::const_size;

	static constexpr size_t size = lak::from_bytes_traits<T, E>::size;

	static void from_bytes(lak::span<const byte_t, size> bytes,
	                       lak::reference_wrapper<T> &value)
	{
		return lak::from_bytes_traits<T, E>::from_bytes(bytes, value.get());
	}

	static lak::result<lak::span<const byte_t>> from_bytes(
	  lak::span<const byte_t> bytes, lak::reference_wrapper<T> &value)
	{
		return lak::from_bytes_traits<T, E>::from_bytes(bytes, value.get());
	}
};

template<typename T, lak::endian E>
struct lak::to_bytes_traits<lak::reference_wrapper<T>, E>
{
	using value_type = lak::reference_wrapper<T>;

	static constexpr bool const_size =
	  lak::to_bytes_traits<lak::remove_const_t<T>, E>::const_size;

	static constexpr size_t size =
	  lak::to_bytes_traits<lak::remove_const_t<T>, E>::size;

	static void to_bytes(lak::span<byte_t, size> bytes,
	                     const lak::reference_wrapper<T> &value)
	{
		return lak::to_bytes_traits<lak::remove_const_t<T>, E>::to_bytes(
		  bytes, value.get());
	}

	static lak::result<lak::span<byte_t>> to_bytes(
	  lak::span<byte_t> bytes, const lak::reference_wrapper<T> &value)
	{
		return lak::to_bytes_traits<lak::remove_const_t<T>, E>::to_bytes(
		  bytes, value.get());
	}
};
