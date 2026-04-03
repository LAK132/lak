# Binary reader

```cpp
#include <lak/system/file.hpp>
#include <lak/binary_reader.hpp>

int main()
{
	lak::array<byte_t> file = lak::read_file("somefile.bin").unwrap();
	lak::binary_reader strm{lak::span(file)};

	// template-less calls for simple integer types
	uint8_t some_u8 = strm.read_u8le().unwrap();

	// *_le and *_be map to lak::endian::little and lak::endian::big respectively
	byte_t peeked_byte = strm.peek_le<byte_t>().unwrap();

	// templated read call allows reading any types with binary reader traits specialised
	// and passing endianness as a constant argument
	lak::array<int32_t> some_i32s = strm.read<int32_t, lak::endian::big>(10).unwrap();
}
```

# Binary span writer

```cpp
#include <lak/binary_writer.hpp>

int main()
{
	lak::array<byte_t> backing_array;
	backing_array.resize(100U);
	lak::binary_span_writer strm{lak::span(backing_array)};
	// writing to strm limited to size of the backing array
}

```

# Binary array writer

```cpp
#include <lak/binary_writer.hpp>

int main()
{
	lak::binary_array_writer strm{};
	strm.reserve(100U);
	// writing to strm will grow the internal array as needed
	lak::array<byte_t> result = strm.release();
}

```

# Bytes traits

Traits objects that allow the binary reader and writers to read and write your own types

## Memcpy

Useful for very simple data types such as enums

```cpp
#include <lak/binary_traits.hpp>

enum struct my_enum {};

// impl read and write traits
LAK_MEMCPY_BYTE_TRAITS_IMPL(my_enum)

#include <lak/system/file.hpp>
#include <lak/binary_reader.hpp>

int main()
{
	lak::array<byte_t> file = lak::read_file("somefile.bin").unwrap();
	lak::binary_reader strm{lak::span(file)};
	my_enum my_object = strm.read<my_enum, lak::endian::little>().unwrap();
}

```

## Fixed struct

```cpp
#include <lak/binary_traits.hpp>

struct my_type
{
	uint8_t my_u8;
	uint32_t my_u32;
};

LAK_FIXED_STRUCT_BYTES_TRAITS(
	my_type,
	&my_type::my_u8,
	&my_type::my_u32);
// use LAK_FIXED_STRUCT_TO_BYTES_TRAITS for only to-bytes traits
// use LAK_FIXED_STRUCT_FROM_BYTES_TRAITS for only from-bytes traits

struct my_always_le_type
{
	uint8_t my_u8;
	uint32_t my_u32;
};

// _EX allows overriding endianness. in this case calls to both
// strm.read_le<my_always_le_type>() and strm.read_be<my_always_le_type>()
// always read my_always_le_type as little endian
LAK_FIXED_STRUCT_BYTES_TRAITS_EX(
	lak::endian::little,
	my_type,
	&my_type::my_u8,
	&my_type::my_u32);
// use LAK_FIXED_STRUCT_TO_BYTES_TRAITS_EX for only to-bytes traits
// use LAK_FIXED_STRUCT_FROM_BYTES_TRAITS_EX for only from-bytes traits

template<typename T>
struct my_templated_type
{
	T my_value;
};

LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS(
	typename T,
	my_templated_type<T>,
	&my_templated_type<T>::my_value);
// use LAK_FIXED_TEMPLATE_STRUCT_TO_BYTES_TRAITS for only to-bytes traits
// use LAK_FIXED_TEMPLATE_STRUCT_FROM_BYTES_TRAITS for only from-bytes traits
// use LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS_EX(typename T, endian, type, ...)
// for overriding endianness

#include <lak/system/file.hpp>
#include <lak/binary_reader.hpp>

int main()
{
	lak::array<byte_t> file = lak::read_file("somefile.bin").unwrap();
	lak::binary_reader strm{lak::span(file)};
	my_type my_object =
		strm.read_be<my_type>().unwrap();
	my_always_le_type my_le_object =
		strm.read_be<my_always_le_type>().unwrap();
	my_templated_type<uint8_t> my_u8_object =
		strm.read_be<my_templated_type<uint8_t>>().unwrap();
}

```

## Member functions

Binary reader and writer have partially specialised bytes traits objects that
allow your type to be from-bytes readable and to-bytes writable by implementing
a few simple member functions

```cpp
#include <lak/binary_reader.hpp> // from-bytes partially specialised traits
#include <lak/binary_writer.hpp> // to-bytes partially specialised traits

struct my_type
{
	uint8_t my_u8;
	uint32_t my_u32;

	// to-bytes

	template<lak::endian E>
	size_t write_size() const
	{
		return lak::to_bytes_traits<uint8_t, E>::size
			+ lak::to_bytes_traits<uint32_t, E>::size;
	}

	template<lak::endian E>
	lak::error_code<lak::err::out_of_data> write(lak::binary_span_writer &strm) const
	{
		RES_TRY(strm.write<E>(my_u8));
		RES_TRY(strm.write<E>(my_u32));
		return lak::ok_t{};
	}

	// from-bytes

	template<lak::endian E>
	lak::error_code<lak::err::out_of_data> read(lak::binary_reader &strm)
	{
		RES_TRY_ASSIGN(my_u8 =, strm.read<uint8_t, E>());
		RES_TRY_ASSIGN(my_u32 =, strm.read<uint32_t, E>());
		return lak::ok_t{};
	}
};

#include <lak/system/file.hpp>

int main()
{
	lak::array<byte_t> file = lak::read_file("somefile.bin").unwrap();
	lak::binary_reader strm{lak::span(file)};
	my_type my_object =
		strm.read_be<my_type>().unwrap();
	my_always_le_type my_le_object =
		strm.read_be<my_always_le_type>().unwrap();
	my_templated_type<uint8_t> my_u8_object =
		strm.read_be<my_templated_type<uint8_t>>().unwrap();
}

```

## Manual trait implementation

```cpp
#include <lak/binary_traits.hpp>
#include <lak/array.hpp>
#include <lak/variant.hpp>
#include <lak/optional.hpp>

struct my_const_size_type
{
	lak::array<uint8_t, 4U> value;
};

template<lak::endian E>
struct lak::bytes_traits<my_const_size_type, E>
{
	using value_type = my_const_size_type;
	using error_type = lak::err::out_of_data;

	static constexpr bool const_size = true;
	static constexpr size_t size     = 4U;

	// const size traits

	static void to_bytes(lak::span<byte_t, size> bytes, const value_type &value)
	{
		lak::memcpy(bytes, lak::span<const byte_t>(lak::span(value.value)));
	}

	static void from_bytes(lak::span<const byte_t, size> bytes, value_type &value)
	{
		lak::memcpy(lak::span<byte_t>(lak::span(value.value)), bytes);
	}

	// dynamic size traits (implement even if your type is constant size)

	static size_t dynamic_size(const value_type &) { return size; }

	static lak::result<lak::span<byte_t>, error_type> to_bytes(
		lak::span<byte_t> bytes, const value_type &value)
	{
		if (bytes.size() < size) return lak::err_t{lak::err::out_of_data{}};
		to_bytes(bytes.first<size>(), value);
		return lak::ok_t{bytes.subspan(size)};
	}

	static lak::result<lak::span<const byte_t>, error_type> from_bytes(
		lak::span<const byte_t> bytes, value_type &value)
	{
		if (bytes.size() < size) return lak::err_t{lak::err::out_of_data{}};
		from_bytes(bytes.first<size>(), value);
		return lak::ok_t{bytes.subspan(size)};
	}
};

struct my_dynamic_sized_type
{
	lak::array<char8_t> value;
};

template<lak::endian E>
struct lak::bytes_traits<my_dynamic_size_type, E>
{
	using value_type = my_dynamic_size_type;
	using error_type = lak::err::out_of_data;

	static constexpr bool const_size = false;
	static constexpr size_t size     = lak::dynamic_extent;
	static_assert((size != lak::dynamic_extent) == const_size);

	static size_t dynamic_size(const value_type &v)
	{
		return lak::to_bytes_traits<uint64_t, E>::size
			+ (lak::to_bytes_traits<char8_t, E>::size * v.value.size());
	}

	static lak::result<lak::span<byte_t>, error_type> to_bytes(
		lak::span<byte_t> bytes, const value_type &value)
	{
		RES_TRY_ASSIGN(bytes =, lak::to_bytes<E>(bytes, static_cast<uint64_t>(value.value.size())));
		RES_TRY_ASSIGN(bytes =, lak::to_bytes<E>(bytes, lak::span(value.value)));
		return lak::ok_t{bytes};
	}

	static lak::result<lak::span<const byte_t>, error_type> from_bytes(
		lak::span<const byte_t> bytes, value_type &value)
	{
		uint64_t size;
		RES_TRY_ASSIGN(lak::tie(size, bytes) =, lak::from_bytes<uint64_t, E>(bytes));
		RES_TRY_ASSIGN(lak::tie(value.value, bytes) =,
		               lak::from_bytes<char8_t, E>(bytes, static_cast<size_t>(size)));
		return lak::ok_t{bytes};
	}
};

template<typename T>
struct my_maybe_const_sized_type
{
	T value;
};

// to and from bytes traits objects can be specialised independently

template<typename T, lak::endian E>
struct lak::to_bytes_traits<my_maybe_const_size_type<T>, E>
{
	using value_type = my_maybe_const_size_type<T>;
	using error_type = typename lak::to_bytes_traits<T>::error_type;

	static constexpr bool const_size = lak::to_bytes_traits<T>::const_size;
	static constexpr size_t size     = lak::to_bytes_traits<T>::size;
	static_assert((size != lak::dynamic_extent) == const_size);

	static size_t dynamic_size(const value_type &v)
	{
		return lak::to_bytes_traits<T>::dynamic_size(v.value);
	}

	static void to_bytes(lak::span<byte_t, size> bytes, const value_type &value)
	requires(const_size)
	{
		lak::to_bytes_traits<T>::to_bytes(bytes, value.value);
	}

	static lak::result<lak::span<byte_t>, error_type> to_bytes(
		lak::span<byte_t> bytes, const value_type &value)
	{
		return lak::to_bytes_traits<T>::to_bytes(bytes, value.value);
	}
};

template<typename T, lak::endian E>
struct lak::from_bytes_traits<my_maybe_const_size_type<T>, E>
{
	using value_type = my_maybe_const_size_type<T>;
	using error_type = typename lak::from_bytes_traits<T>::error_type;

	static constexpr bool const_size = lak::from_bytes_traits<T>::const_size;
	static constexpr size_t size     = lak::from_bytes_traits<T>::size;
	static_assert((size != lak::dynamic_extent) == const_size);

	static void from_bytes(lak::span<const byte_t, size> bytes, value_type &value)
	requires(const_size)
	{
		lak::from_bytes_traits<T, E>::from_bytes(bytes, value.value);
	}

	static lak::result<lak::span<const byte_t>, error_type> from_bytes(
		lak::span<const byte_t> bytes, value_type &value)
	{
		return lak::from_bytes_traits<T, E>::from_bytes(bytes, value.value);
	}
};

#include <lak/system/file.hpp>
#include <lak/binary_reader.hpp>

int main()
{
	lak::array<byte_t> file = lak::read_file("somefile.bin").unwrap();
	lak::binary_reader strm{lak::span(file)};
	my_const_size_type my_const_object =
		strm.read<my_const_size_type, lak::endian::little>().unwrap();
	my_dynamic_size_type my_dynamic_object =
		strm.read<my_dynamic_size_type, lak::endian::little>().unwrap();
	my_maybe_const_size_type<my_const_size_type> my_const_maybe_const_object =
		strm.read<my_maybe_const_size_type<my_const_size_type>, lak::endian::little>().unwrap();
	my_maybe_const_size_type<my_dynamic_sized_type> my_dynamic_maybe_const_object =
		strm.read<my_maybe_const_size_type<my_dynamic_sized_type>, lak::endian::little>().unwrap();
}

```

