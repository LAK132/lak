#ifndef LAK_MEMMANIP_HPP
#define LAK_MEMMANIP_HPP

#define LAK_SPAN_MANIP_FORWARD_ONLY
#include "lak/span_manip.hpp"

#define LAK_RESULT_FORWARD_ONLY
#include "lak/result.hpp"

#define LAK_SPAN_FORWARD_ONLY
#include "lak/span.hpp"

#include <cstring>

namespace lak
{
	byte_t *align_ptr(byte_t *ptr, size_t align);
	uintptr_t align_ptr(uintptr_t ptr, size_t align);

	// see lak::binary_reader and lak::binary_writer for type safe manipulation
	// of types as bytes
	template<typename T>
	lak::span<lak::copy_const_t<T, byte_t>, sizeof(T)> as_bytes(T *v)
	{
		return lak::span<lak::copy_const_t<T, byte_t>, sizeof(T)>::from_ptr(
		  reinterpret_cast<lak::copy_const_t<T, byte_t> *>(v));
	}

	void memcpy(byte_t *dst, const byte_t *src, size_t count);

	void memmove(byte_t *dst, const byte_t *src, size_t count);

	void memcpy(lak::span<byte_t> dst, lak::span<const byte_t> src);

	void memmove(lak::span<byte_t> dst, lak::span<const byte_t> src);

	template<typename T>
	force_inline void memcpy(T *dst, const T *src)
	{
		lak::memcpy(lak::as_bytes(dst), lak::as_bytes(src));
	}

	template<typename T>
	force_inline void memmove(T *dst, const T *src)
	{
		lak::memmove(lak::as_bytes(dst), lak::as_bytes(src));
	}

	template<typename T>
	force_inline void bzero(T *dst)
	{
		lak::fill<byte_t>(lak::as_bytes(dst), byte_t(0));
	}

	template<typename T>
	void byte_swap(lak::span<T> v)
	{
		if constexpr (sizeof(T) != sizeof(byte_t))
			for (T &e : v) lak::reverse(lak::as_bytes(&e));
	}

	template<typename TO, typename FROM>
	TO bit_cast(const FROM &from)
	{
		TO result;
		lak::memcpy(lak::as_bytes(&result), lak::as_bytes(&from));
		return result;
	}

	void *aligned_alloc(size_t alignment, size_t size);

	void aligned_free(void *p);

	size_t page_size();

	size_t round_to_page_multiple(size_t size, size_t *page_size_out = nullptr);

	enum struct page_error
	{
	};
	template<typename OK = lak::monostate>
	using page_result_t = lak::result<OK, page_error>;

	lak::page_result_t<lak::span<void>> page_reserve(
	  size_t size, size_t *page_size_out = nullptr);

	lak::page_result_t<> page_commit(lak::span<void> pages);

	lak::page_result_t<> page_decommit(lak::span<void> pages);

	lak::page_result_t<> page_free(lak::span<void> pages);

#if 0
  lak::page_result_t<bool> pages_are_committed(lak::span<void> pages);

  lak::page_result_t<bool> pages_are_reserved(lak::span<void> pages);
#endif
}

#endif
