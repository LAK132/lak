#include "lak/binary_reader.hpp"
#include "lak/compiler.hpp"
#include "lak/memmanip.hpp"

template<typename T, lak::endian E>
T lak::from_bytes(lak::span<const uint8_t, lak::from_bytes_size_v<T, E>> bytes)
{
  return lak::from_bytes_traits<T, E>::from_bytes(bytes);
}

template<typename T, lak::endian E>
lak::result<T> lak::from_bytes(lak::span<const uint8_t> bytes)
{
  static constexpr size_t min_size = lak::from_bytes_size_v<T, E>;
  if (bytes.size() < min_size) return lak::err_t{};
  return lak::ok_t{lak::from_bytes<T, E>(bytes.first<min_size>())};
}

/* --- uint8_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<uint8_t, E>
{
  using value_type             = uint8_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return bytes[0];
  }
};

/* --- int8_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<int8_t, E>
{
  using value_type             = int8_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return value_type(lak::from_bytes_traits<uint8_t, E>::from_bytes(bytes));
  }
};

/* --- uint16_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<uint16_t, E>
{
  using value_type             = uint16_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    static_assert(E == lak::endian::little || E == lak::endian::big);

    if constexpr (E == lak::endian::little)
      return value_type(bytes[0]) | (value_type(bytes[1]) << 8);
    else
      return value_type(bytes[1]) | (value_type(bytes[0]) << 8);
  }
};

/* --- int16_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<int16_t, E>
{
  using value_type             = int16_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return value_type(lak::from_bytes_traits<uint16_t, E>::from_bytes(bytes));
  }
};

/* --- uint32_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<uint32_t, E>
{
  using value_type             = uint32_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    static_assert(E == lak::endian::little || E == lak::endian::big);

    if constexpr (E == lak::endian::little)
      return value_type(bytes[0]) | (value_type(bytes[1]) << 8) |
             (value_type(bytes[2]) << 16) | (value_type(bytes[3]) << 24);
    else
      return value_type(bytes[3]) | (value_type(bytes[2]) << 8) |
             (value_type(bytes[1]) << 16) | (value_type(bytes[0]) << 24);
  }
};

/* --- int32_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<int32_t, E>
{
  using value_type             = int32_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return value_type(lak::from_bytes_traits<uint32_t, E>::from_bytes(bytes));
  }
};

/* --- uint64_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<uint64_t, E>
{
  using value_type             = uint64_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    static_assert(E == lak::endian::little || E == lak::endian::big);

    if constexpr (E == lak::endian::little)
      return value_type(bytes[0]) | (value_type(bytes[1]) << 8) |
             (value_type(bytes[2]) << 16) | (value_type(bytes[3]) << 24) |
             (value_type(bytes[4]) << 32) | (value_type(bytes[5]) << 40) |
             (value_type(bytes[6]) << 48) | (value_type(bytes[7]) << 56);
    else
      return value_type(bytes[7]) | (value_type(bytes[6]) << 8) |
             (value_type(bytes[5]) << 16) | (value_type(bytes[4]) << 24) |
             (value_type(bytes[3]) << 32) | (value_type(bytes[2]) << 40) |
             (value_type(bytes[1]) << 48) | (value_type(bytes[0]) << 56);
  }
};

/* --- int64_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<int64_t, E>
{
  using value_type             = int64_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return value_type(lak::from_bytes_traits<uint64_t, E>::from_bytes(bytes));
  }
};

/* --- f32_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<f32_t, E>
{
  using value_type             = f32_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    uint32_t value = lak::from_bytes_traits<uint32_t, E>::from_bytes(bytes);
    f32_t result;
    lak::memcpy(reinterpret_cast<char *>(&result),
                reinterpret_cast<const char *>(&value),
                size);

    return result;
  }
};

/* --- f64_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<f64_t, E>
{
  using value_type             = f64_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    uint64_t value = lak::from_bytes_traits<uint64_t, E>::from_bytes(bytes);
    f64_t result;
    lak::memcpy(reinterpret_cast<char *>(&result),
                reinterpret_cast<const char *>(&value),
                size);

    return result;
  }
};

/* --- char --- */

template<lak::endian E>
struct lak::from_bytes_traits<char, E>
{
  using value_type             = char;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return value_type(lak::from_bytes_traits<uint8_t, E>::from_bytes(bytes));
  }
};

#ifdef LAK_COMPILER_CPP20
/* --- char8_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<char8_t, E>
{
  using value_type             = char8_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return value_type(lak::from_bytes_traits<uint8_t, E>::from_bytes(bytes));
  }
};
#endif

/* --- char16_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<char16_t, E>
{
  using value_type             = char16_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return value_type(lak::from_bytes_traits<uint16_t, E>::from_bytes(bytes));
  }
};

/* --- char32_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<char32_t, E>
{
  using value_type             = char32_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return value_type(lak::from_bytes_traits<uint32_t, E>::from_bytes(bytes));
  }
};

/* --- wchar_t --- */

template<lak::endian E>
struct lak::from_bytes_traits<wchar_t, E>
{
  using value_type             = wchar_t;
  static constexpr size_t size = sizeof(value_type);

  static value_type from_bytes(lak::span<const uint8_t, size> bytes)
  {
    return value_type(
      lak::from_bytes_traits<wchar_unicode_t, E>::from_bytes(bytes));
  }
};
