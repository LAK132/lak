#ifndef LAK_MEMMANIP_HPP
#define LAK_MEMMANIP_HPP

#define LAK_SPAN_MANIP_FORWARD_ONLY
#include "lak/span_manip.hpp"

#define LAK_RESULT_FORWARD_ONLY
#include "lak/result.hpp"

#define LAK_SPAN_FORWARD_ONLY
#include "lak/span.hpp"

#include "lak/stdint.hpp"

#include <cstring>

namespace lak
{
	byte_t *align_ptr(byte_t *ptr, size_t align);
	uintptr_t align_ptr(uintptr_t ptr, size_t align);

	// see lak::binary_reader and lak::binary_writer for type safe manipulation
	// of types as bytes
	template<typename T>
	constexpr lak::span<lak::copy_const_t<T, byte_t>, sizeof(T)> as_bytes(T *v);

	constexpr void memcpy(byte_t *dst, const byte_t *src, size_t count);

	void memmove(byte_t *dst, const byte_t *src, size_t count);

	constexpr void memcpy(lak::span<byte_t> dst, lak::span<const byte_t> src);

	void memmove(lak::span<byte_t> dst, lak::span<const byte_t> src);

	template<typename T>
	force_inline void memcpy(T *dst, const T *src);

	template<typename T>
	force_inline void memmove(T *dst, const T *src);

	template<typename T>
	force_inline void bzero(T *dst);

	template<size_t CHUNK_SIZE>
	void byte_swap(lak::span<byte_t> v);

	template<typename T>
	void byte_swap(lak::span<T> v);

	template<typename TO, typename FROM>
	constexpr TO bit_cast(const FROM &from);

	void *aligned_alloc(size_t alignment, size_t size);

	void aligned_free(void *p);
}

#include "lak/memmanip.inl"

#endif
