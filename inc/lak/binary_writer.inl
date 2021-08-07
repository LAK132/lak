#include "lak/binary_writer.hpp"
#include "lak/memmanip.hpp"

#include "lak/debug.hpp"

template<typename T, lak::endian E>
void lak::to_bytes(lak::span<uint8_t, lak::to_bytes_size_v<T, E>> bytes,
                   const T &value)
{
	lak::to_bytes_traits<T, E>::to_bytes(bytes, value);
}

template<typename T, lak::endian E>
lak::result<lak::span<uint8_t>> lak::to_bytes(lak::span<uint8_t> bytes,
                                              const T &value)
{
	static constexpr size_t min_size = lak::to_bytes_size_v<T, E>;
	if (bytes.size() < min_size)
	{
		ERROR("not enough bytes");
		return lak::err_t{};
	}

	lak::to_bytes<T, E>(bytes.first<min_size>(), value);
	return lak::ok_t{bytes.subspan(min_size)};
}

template<typename T, lak::endian E>
lak::result<lak::span<uint8_t>> lak::array_to_bytes(lak::span<uint8_t> bytes,
                                                    lak::span<const T> values)
{
	const size_t req_size = lak::to_bytes_size_v<T, E> * values.size();

	if (req_size > bytes.size()) return lak::err_t{};

	for (const T &value : values)
	{
		lak::to_bytes<T, E>(bytes.first<lak::to_bytes_size_v<T, E>>(), value);
		bytes = bytes.subspan(lak::to_bytes_size_v<T, E>);
	}

	return lak::ok_t{bytes};
}

/* --- uint8_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<uint8_t, E>
{
	using value_type             = uint8_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, uint8_t value)
	{
		bytes[0] = value;
	}
};

/* --- int8_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<int8_t, E>
{
	using value_type             = int8_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, int8_t value)
	{
		lak::to_bytes_traits<uint8_t, E>::to_bytes(bytes,
		                                           static_cast<uint8_t>(value));
	}
};

/* --- uint16_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<uint16_t, E>
{
	using value_type             = uint16_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, uint16_t value)
	{
		static_assert(E == lak::endian::little || E == lak::endian::big);

		if constexpr (E == lak::endian::little)
		{
			bytes[0] = static_cast<uint8_t>(value);
			bytes[1] = static_cast<uint8_t>(value >> 8);
		}
		else
		{
			bytes[1] = static_cast<uint8_t>(value);
			bytes[0] = static_cast<uint8_t>(value >> 8);
		}
	}
};

/* --- int16_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<int16_t, E>
{
	using value_type             = int16_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, int16_t value)
	{
		lak::to_bytes_traits<uint16_t, E>::to_bytes(bytes,
		                                            static_cast<uint16_t>(value));
	}
};

/* --- uint32_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<uint32_t, E>
{
	using value_type             = uint32_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, uint32_t value)
	{
		static_assert(E == lak::endian::little || E == lak::endian::big);

		if constexpr (E == lak::endian::little)
		{
			bytes[0] = static_cast<uint8_t>(value);
			bytes[1] = static_cast<uint8_t>(value >> 8);
			bytes[2] = static_cast<uint8_t>(value >> 16);
			bytes[3] = static_cast<uint8_t>(value >> 24);
		}
		else
		{
			bytes[3] = static_cast<uint8_t>(value);
			bytes[2] = static_cast<uint8_t>(value >> 8);
			bytes[1] = static_cast<uint8_t>(value >> 16);
			bytes[0] = static_cast<uint8_t>(value >> 24);
		}
	}
};

/* --- int32_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<int32_t, E>
{
	using value_type             = int32_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, int32_t value)
	{
		lak::to_bytes_traits<uint32_t, E>::to_bytes(bytes,
		                                            static_cast<uint32_t>(value));
	}
};

/* --- uint64_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<uint64_t, E>
{
	using value_type             = uint64_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, uint64_t value)
	{
		static_assert(E == lak::endian::little || E == lak::endian::big);

		if constexpr (E == lak::endian::little)
		{
			bytes[0] = static_cast<uint8_t>(value);
			bytes[1] = static_cast<uint8_t>(value >> 8);
			bytes[2] = static_cast<uint8_t>(value >> 16);
			bytes[3] = static_cast<uint8_t>(value >> 24);
			bytes[4] = static_cast<uint8_t>(value >> 32);
			bytes[5] = static_cast<uint8_t>(value >> 40);
			bytes[6] = static_cast<uint8_t>(value >> 48);
			bytes[7] = static_cast<uint8_t>(value >> 56);
		}
		else
		{
			bytes[7] = static_cast<uint8_t>(value);
			bytes[6] = static_cast<uint8_t>(value >> 8);
			bytes[5] = static_cast<uint8_t>(value >> 16);
			bytes[4] = static_cast<uint8_t>(value >> 24);
			bytes[3] = static_cast<uint8_t>(value >> 32);
			bytes[2] = static_cast<uint8_t>(value >> 40);
			bytes[1] = static_cast<uint8_t>(value >> 48);
			bytes[0] = static_cast<uint8_t>(value >> 56);
		}
	}
};

/* --- int64_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<int64_t, E>
{
	using value_type             = int64_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, int64_t value)
	{
		lak::to_bytes_traits<uint64_t, E>::to_bytes(bytes,
		                                            static_cast<uint64_t>(value));
	}
};

/* --- f32_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<f32_t, E>
{
	using value_type             = f32_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, f32_t value)
	{
		lak::to_bytes_traits<uint32_t, E>::to_bytes(
		  bytes, lak::bit_cast<uint32_t>(value));
	}
};

/* --- f64_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<f64_t, E>
{
	using value_type             = f64_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, f64_t value)
	{
		lak::to_bytes_traits<uint64_t, E>::to_bytes(
		  bytes, lak::bit_cast<uint64_t>(value));
	}
};

/* --- char --- */

template<lak::endian E>
struct lak::to_bytes_traits<char, E>
{
	using value_type             = char;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, char value)
	{
		lak::to_bytes_traits<uint8_t, E>::to_bytes(bytes,
		                                           static_cast<uint8_t>(value));
	}
};

#ifdef LAK_COMPILER_CPP20
/* --- char8_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<char8_t, E>
{
	using value_type             = char8_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, char8_t value)
	{
		lak::to_bytes_traits<uint8_t, E>::to_bytes(bytes,
		                                           static_cast<uint8_t>(value));
	}
};
#endif

/* --- char16_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<char16_t, E>
{
	using value_type             = char16_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, char16_t value)
	{
		lak::to_bytes_traits<uint16_t, E>::to_bytes(bytes,
		                                            static_cast<uint16_t>(value));
	}
};

/* --- char32_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<char32_t, E>
{
	using value_type             = char32_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, char32_t value)
	{
		lak::to_bytes_traits<uint32_t, E>::to_bytes(bytes,
		                                            static_cast<uint32_t>(value));
	}
};

/* --- wchar_t --- */

template<lak::endian E>
struct lak::to_bytes_traits<wchar_t, E>
{
	using value_type             = wchar_t;
	static constexpr size_t size = sizeof(value_type);

	static void to_bytes(lak::span<uint8_t, size> bytes, wchar_t value)
	{
		lak::to_bytes_traits<wchar_unicode_t, E>::to_bytes(
		  bytes, static_cast<wchar_unicode_t>(value));
	}
};
