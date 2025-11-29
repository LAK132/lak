#ifndef LAK_C_ARRAY_HPP
#define LAK_C_ARRAY_HPP

#include "lak/system/compiler.hpp"

#include <cstddef>

namespace lak
{
	template<typename T, size_t SIZE>
	struct c_array
	{
		using value_type      = T;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;
		using reference       = T &;
		using const_reference = const T &;
		using pointer         = T *;
		using const_pointer   = const T *;
		using iterator        = T *;
		using const_iterator  = const T *;

		T data[SIZE] = {};

		constexpr size_t size() const { return SIZE; }
		constexpr T &operator[](size_t i) { return data[i]; };
		constexpr const T &operator[](size_t i) const { return data[i]; };
		constexpr T *begin() { return data; }
		constexpr T *end() { return begin() + SIZE; }
		constexpr const T *begin() const { return data; }
		constexpr const T *end() const { return begin() + SIZE; }
		constexpr const T *cbegin() const { return cbegin(); }
		constexpr const T *cend() const { return cend(); }
	};

	template<typename T>
	struct c_array<T, 0U>
	{
		using value_type      = T;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;
		using reference       = T &;
		using const_reference = const T &;
		using pointer         = T *;
		using const_pointer   = const T *;
		using iterator        = T *;
		using const_iterator  = const T *;

		byte_t _data[sizeof(T)] = {};

		constexpr size_t size() const { return 0U; }
		constexpr T &operator[](size_t)
		{
			LAK_UNREACHABLE();
			return *begin();
		};
		constexpr const T &operator[](size_t) const
		{
			LAK_UNREACHABLE();
			return *begin();
		};
		constexpr T *begin() { return reinterpret_cast<T *>(_data); }
		constexpr T *end() { return reinterpret_cast<T *>(_data); }
		constexpr const T *begin() const
		{
			return reinterpret_cast<const T *>(_data);
		}
		constexpr const T *end() const
		{
			return reinterpret_cast<const T *>(_data);
		}
		constexpr const T *cbegin() const { return begin(); }
		constexpr const T *cend() const { return end(); }
	};
}

#endif
