#include "lak/binary_writer.hpp"
#include "lak/memmanip.hpp"
#include "lak/span_manip.hpp"

#include "lak/debug.hpp"

/* --- to_bytes --- */

template<typename T, lak::endian E>
requires(lak::to_bytes_traits<T, E>::const_size)
void lak::to_bytes(lak::span<byte_t, lak::to_bytes_size<T, E>()> bytes,
                   const T &value)
{
	static_assert(!lak::is_const_v<T>);
	lak::to_bytes_traits<T, E>::to_bytes(lak::to_bytes_data<T, E>::make(
	  bytes, lak::span<const T, 1>::from_ptr(&value)));
}

template<typename T, lak::endian E>
lak::result<> lak::to_bytes(lak::span<byte_t> bytes, const T &value)
{
	static_assert(!lak::is_const_v<T>);
	return lak::to_bytes_data<T, E>::maybe_make(
	         bytes, lak::span<const T, 1>::from_ptr(&value))
	  .map(
	    [](auto data)
	    {
		    lak::to_bytes_traits<T, E>::to_bytes(data);
		    return lak::monostate{};
	    });
}

template<typename T, lak::endian E>
requires(lak::to_bytes_traits<T, E>::const_size)
lak::array<byte_t, lak::to_bytes_size<T, E>()> lak::to_bytes(const T &value)
{
	static_assert(!lak::is_const_v<T>);
	lak::array<byte_t, lak::to_bytes_size<T, E>()> result;
	lak::to_bytes<T, E>(lak::span<byte_t, lak::to_bytes_size<T, E>()>(result),
	                    value);
	return result;
}

/* --- array_to_bytes --- */

template<typename T, size_t S, lak::endian E>
requires(lak::to_bytes_traits<T, E>::const_size)
lak::array<byte_t, S * lak::to_bytes_size<T, E>()> lak::array_to_bytes(
  lak::span<const T, S> values)
{
	static_assert(!lak::is_const_v<T>);
	lak::array<byte_t, S * lak::to_bytes_size<T, E>()> result;
	lak::to_bytes_traits<T, E>::to_bytes(lak::to_bytes_data<T, E>::make(
	  lak::span<byte_t, S * lak::to_bytes_size<T, E>()>(result),
	  lak::span<const T, S>(values)));
	return result;
}

template<typename T, lak::endian E>
lak::array<byte_t> lak::array_to_bytes(lak::span<const T> values)
{
	static_assert(!lak::is_const_v<T>);
	lak::array<byte_t> result;
	result.resize(lak::to_bytes_size<T, E>(values));
	lak::to_bytes_traits<T, E>::to_bytes(
	  lak::to_bytes_data<T, E>::maybe_make(lak::span<byte_t>(result), values)
	    .unwrap());
	return result;
}

template<typename T, lak::endian E>
lak::result<> lak::array_to_bytes(lak::span<byte_t> bytes,
                                  lak::span<const T> values)
{
	static_assert(!lak::is_const_v<T>);
	return lak::to_bytes_data<T, E>::maybe_make(bytes, values)
	  .map(
	    [](lak::to_bytes_data<T, E> data)
	    {
		    lak::to_bytes_traits<T, E>::to_bytes(data);
		    return lak::monostate{};
	    });
}

/* --- _basic_memcpy_to_bytes --- */

namespace lak
{
	template<typename T, lak::endian E>
	inline void _basic_memcpy_to_bytes(lak::to_bytes_data<T, E> data)
	{
		static_assert(E == lak::endian::native || ~E == lak::endian::native);
		lak::memcpy(data.dst, lak::span<const byte_t>(data.src));
		if constexpr (E != lak::endian::native)
			lak::byte_swap<sizeof(T)>(data.dst);
	}
}

/* --- byte_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<byte_t, E>
{
	using value_type                 = byte_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::memcpy(data.dst, data.src);
	}
};

/* --- uint8_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<uint8_t, E>
{
	using value_type                 = uint8_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::memcpy(data.dst, lak::span<const byte_t>(data.src));
	}
};

/* --- int8_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<int8_t, E>
{
	using value_type                 = int8_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::memcpy(data.dst, lak::span<const byte_t>(data.src));
	}
};

/* --- uint16_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<uint16_t, E>
{
	using value_type                 = uint16_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- int16_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<int16_t, E>
{
	using value_type                 = int16_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- uint32_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<uint32_t, E>
{
	using value_type                 = uint32_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- int32_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<int32_t, E>
{
	using value_type                 = int32_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- uint64_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<uint64_t, E>
{
	using value_type                 = uint64_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- int64_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<int64_t, E>
{
	using value_type                 = int64_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- f32_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<f32_t, E>
{
	using value_type                 = f32_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- f64_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<f64_t, E>
{
	using value_type                 = f64_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- char --- */

template<lak::endian E>
struct lak::to_bytes_traits<char, E>
{
	using value_type                 = char;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::memcpy(data.dst, lak::span<const byte_t>(data.src));
	}
};

#ifdef LAK_COMPILER_CPP20
/* --- char8_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<char8_t, E>
{
	using value_type                 = char8_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::memcpy(data.dst, lak::span<const byte_t>(data.src));
	}
};
#endif

/* --- char16_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<char16_t, E>
{
	using value_type                 = char16_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- char32_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<char32_t, E>
{
	using value_type                 = char32_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};

/* --- wchar_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<wchar_t, E>
{
	using value_type                 = wchar_t;
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(value_type); }

	static void to_bytes(lak::to_bytes_data<value_type, E> data)
	{
		lak::_basic_memcpy_to_bytes(data);
	}
};
