#include "lak/memmanip.hpp"

#include <bit>

template<typename T>
constexpr lak::span<lak::copy_const_t<T, byte_t>, sizeof(T)> lak::as_bytes(
  T *v)
{
	return lak::span<lak::copy_const_t<T, byte_t>, sizeof(T)>::from_ptr(
	  reinterpret_cast<lak::copy_const_t<T, byte_t> *>(v));
}

constexpr void lak::memcpy(byte_t *dst, const byte_t *src, size_t count)
{
	for (size_t i = 0; i < count; ++i) dst[i] = src[i];
}

constexpr void lak::memcpy(lak::span<byte_t> dst, lak::span<const byte_t> src)
{
	lak::memcpy(
	  dst.data(), src.data(), dst.size() < src.size() ? dst.size() : src.size());
}

template<typename T>
force_inline void lak::memcpy(T *dst, const T *src)
{
	lak::memcpy(lak::as_bytes(dst), lak::as_bytes(src));
}

template<typename T>
force_inline void lak::memmove(T *dst, const T *src)
{
	lak::memmove(lak::as_bytes(dst), lak::as_bytes(src));
}

template<typename T>
force_inline void lak::bzero(T *dst)
{
	lak::fill<byte_t>(lak::as_bytes(dst), byte_t(0));
}

template<size_t CHUNK_SIZE>
void lak::byte_swap(lak::span<byte_t> v)
{
	if constexpr (CHUNK_SIZE > 1U)
	{
		while (v.size() >= CHUNK_SIZE)
		{
			lak::reverse<byte_t>(v.first(CHUNK_SIZE));
			v = v.subspan(CHUNK_SIZE);
		}
	}
}

template<typename T>
void lak::byte_swap(lak::span<T> v)
{
	if constexpr (sizeof(T) > 1U)
		for (T &e : v) lak::reverse<byte_t>(lak::as_bytes(&e));
}

template<typename TO, typename FROM>
constexpr TO lak::bit_cast(const FROM &from)
{
	TO result;
	lak::memcpy(lak::as_bytes(&result), lak::as_bytes(&from));
	return result;
}
