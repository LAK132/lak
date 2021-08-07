#ifndef LAK_MEMMANIP_HPP
#define LAK_MEMMANIP_HPP

#include "lak/result.hpp"
#include "lak/span.hpp"

#include <cstring>

namespace lak
{
	byte_t *align_ptr(byte_t *ptr, size_t align);
	uintptr_t align_ptr(uintptr_t ptr, size_t align);

	// see lak::binary_reader and lak::binary_writer for type safe manipulation
	// of types as bytes
	template<typename T>
	lak::span<lak::copy_const_t<T, char>, sizeof(T)> as_bytes(T *v)
	{
		return lak::span<lak::copy_const_t<T, char>, sizeof(T)>::from_ptr(
		  reinterpret_cast<lak::copy_const_t<T, char> *>(v));
	}

	void memcpy(char *dst, const char *src, size_t count);

	void memmove(char *dst, const char *src, size_t count);

	void memcpy(lak::span<char> dst, lak::span<const char> src);

	void memmove(lak::span<char> dst, lak::span<const char> src);

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
		lak::fill<char>(lak::as_bytes(dst), char(0));
	}

	template<typename TO, typename FROM>
	TO bit_cast(const FROM &from)
	{
		TO result;
		lak::memcpy(lak::as_bytes(&result), lak::as_bytes(&from));
		return result;
	}

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