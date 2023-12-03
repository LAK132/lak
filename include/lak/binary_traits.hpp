#ifndef LAK_BINARY_TRAITS_HPP
#define LAK_BINARY_TRAITS_HPP

#include "lak/array.hpp"
#include "lak/endian.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/type_traits.hpp"

namespace lak
{
	/* --- from_bytes_traits --- */

	template<typename T, lak::endian E>
	struct from_bytes_traits
	{
		using value_type = lak::nonesuch;

		static constexpr bool const_size = false;

		static constexpr size_t size = lak::dynamic_extent;

		// static void from_bytes(lak::from_bytes_data<value_type, E> data)
		// requires(const_size);

		// static lak::result<lak::span<const byte_t>>
		// from_bytes(lak::from_bytes_data<value_type, E> data)
		// requires(!const_size);
	};

	template<typename T, lak::endian E>
	static constexpr bool has_from_bytes_traits =
	  !lak::is_same_v<typename lak::from_bytes_traits<T, E>::value_type,
	                  lak::nonesuch>;

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	static constexpr size_t from_bytes_size_v =
	  lak::from_bytes_traits<T, E>::size;

	template<typename T, lak::endian E = lak::endian::little>
	using from_bytes_value_type_t =
	  typename lak::from_bytes_traits<T, E>::value_type;

	template<typename T, lak::endian E>
	struct from_bytes_data
	{
		const lak::span<T> dst;
		const lak::span<const byte_t> src;
		static constexpr size_t bytes_per_element =
		  lak::from_bytes_traits<T, E>::size;

		static inline lak::result<from_bytes_data> maybe_make(
		  lak::span<T> dst, lak::span<const byte_t> src)
		requires(lak::from_bytes_traits<T, E>::const_size)
		{
			if (dst.size() * bytes_per_element == src.size())
				return lak::ok_t{from_bytes_data(dst, src)};
			else
				return lak::err_t{};
		}

		static inline lak::result<from_bytes_data> maybe_make(
		  lak::span<T> dst, lak::span<const byte_t> src)
		requires(!lak::from_bytes_traits<T, E>::const_size)
		{
			return lak::ok_t{from_bytes_data(dst, src)};
		}

		template<size_t S>
		requires(lak::from_bytes_traits<T, E>::const_size)
		static inline from_bytes_data make(
		  lak::span<T, S> dst, lak::span<const byte_t, S * bytes_per_element> src)
		{
			return from_bytes_data(dst, src);
		}

		from_bytes_data(const from_bytes_data &)            = default;
		from_bytes_data &operator=(const from_bytes_data &) = default;

	private:
		from_bytes_data(lak::span<T> d, lak::span<const byte_t> s) : dst(d), src(s)
		{
		}
	};

	/* --- from_bytes --- */

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	T from_bytes(lak::span<const byte_t, lak::from_bytes_size_v<T, E>> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<T> from_bytes(lak::span<const byte_t> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	requires(!lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::pair<T, lak::span<const byte_t>>> from_bytes(
	  lak::span<const byte_t> bytes);

	/* --- array_from_bytes --- */

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::array<T, S> array_from_bytes(
	  lak::span<const byte_t, lak::from_bytes_size_v<T, E> * S> bytes);

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::array<T, S>> array_from_bytes(
	  lak::span<const byte_t> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::array<T>> array_from_bytes(lak::span<const byte_t> bytes,
	                                            size_t count);

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::from_bytes_traits<T, E>::const_size)
	lak::result<> array_from_bytes(lak::span<T> values,
	                               lak::span<const byte_t> bytes);

	template<typename T, lak::endian E = lak::endian::little>
	requires(!lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::pair<lak::array<T>, lak::span<const byte_t>>>
	array_from_bytes(lak::span<const byte_t> bytes, size_t count);

	template<typename T, lak::endian E = lak::endian::little>
	requires(!lak::from_bytes_traits<T, E>::const_size)
	lak::result<lak::span<const byte_t>> array_from_bytes(
	  lak::span<T> values, lak::span<const byte_t> bytes);

	/* --- to_bytes_traits --- */

	template<typename T, lak::endian E>
	struct to_bytes_data;

	template<typename T, lak::endian E>
	struct to_bytes_traits
	{
		using value_type = lak::nonesuch;

		static constexpr bool const_size = true;

		static constexpr size_t size() { return 0; }

		static void to_bytes(lak::to_bytes_data<value_type, E> data) = delete;
	};

	template<typename T, lak::endian E>
	static constexpr bool has_to_bytes_traits =
	  !lak::is_same_v<typename lak::to_bytes_traits<T, E>::value_type,
	                  lak::nonesuch>;

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	constexpr size_t to_bytes_size()
	{
		return lak::to_bytes_traits<T, E>::size();
	}
	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	constexpr size_t to_bytes_size(const T &)
	{
		return lak::to_bytes_traits<T, E>::size();
	}
	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	constexpr size_t to_bytes_size(lak::span<const T> v)
	{
		return lak::to_bytes_traits<T, E>::size() * v.size();
	}
	template<typename T, lak::endian E = lak::endian::little>
	requires(!lak::to_bytes_traits<T, E>::const_size)
	size_t to_bytes_size(const T &v)
	{
		return lak::to_bytes_traits<T, E>::size(v);
	}
	template<typename T, lak::endian E = lak::endian::little>
	requires(!lak::to_bytes_traits<T, E>::const_size)
	size_t to_bytes_size(lak::span<const T> v)
	{
		size_t size = 0;
		for (const auto &e : v) size += lak::to_bytes_traits<T, E>::size(e);
		return size;
	}

	template<typename T, lak::endian E>
	requires(lak::to_bytes_traits<T, E>::const_size)
	struct to_bytes_data<T, E>
	{
		const lak::span<byte_t> dst;
		const lak::span<const T> src;

		static inline lak::result<to_bytes_data> maybe_make(lak::span<byte_t> dst,
		                                                    lak::span<const T> src)
		{
			if (dst.size() == src.size() * lak::to_bytes_size<T, E>())
				return lak::ok_t{to_bytes_data(dst, src)};
			else
				return lak::err_t{};
		}

		template<size_t S>
		static inline to_bytes_data make(
		  lak::span<byte_t, S * lak::to_bytes_size<T, E>()> dst,
		  lak::span<const T, S> src)
		{
			return to_bytes_data(dst, src);
		}

		to_bytes_data(const to_bytes_data &)            = default;
		to_bytes_data &operator=(const to_bytes_data &) = default;

	private:
		to_bytes_data(lak::span<byte_t> d, lak::span<const T> s) : dst(d), src(s)
		{
		}
	};

	template<typename T, lak::endian E>
	requires(!lak::to_bytes_traits<T, E>::const_size)
	struct to_bytes_data<T, E>
	{
		const lak::span<byte_t> dst;
		const lak::span<const T> src;

		static inline lak::result<to_bytes_data> maybe_make(lak::span<byte_t> dst,
		                                                    lak::span<const T> src)
		{
			if (dst.size() == lak::to_bytes_size<T, E>(src))
				return lak::ok_t{to_bytes_data(dst, src)};
			else
				return lak::err_t{};
		}

		to_bytes_data(const to_bytes_data &)            = default;
		to_bytes_data &operator=(const to_bytes_data &) = default;

	private:
		to_bytes_data(lak::span<byte_t> d, lak::span<const T> s) : dst(d), src(s)
		{
		}
	};

	template<typename T, lak::endian E>
	struct to_bytes_traits_arr_impl
	{
		using value_type = T;

		template<lak::endian EE = E>
		static void to_bytes(lak::to_bytes_data<value_type, E> data)
		requires(EE == E && lak::to_bytes_traits<value_type, EE>::const_size)
		{
			auto dst{data.dst};
			for (const auto &val : data.src)
			{
				lak::array<byte_t, lak::to_bytes_size<value_type, EE>()> bytes{
				  lak::to_bytes_traits<value_type, EE>::single_to_bytes(val)};
				lak::memcpy(dst, bytes);
				dst = dst.subspan(lak::to_bytes_size<value_type, EE>());
			}
		}

		template<lak::endian EE = E>
		static void to_bytes(lak::to_bytes_data<value_type, E> data)
		requires(EE == E && !lak::to_bytes_traits<value_type, EE>::const_size)
		{
			auto dst{data.dst};
			for (const auto &val : data.src)
			{
				const size_t sz = lak::to_bytes_size<value_type, EE>(val);
				lak::to_bytes_traits<value_type, EE>::single_to_bytes(dst.first(sz),
				                                                      val);
				dst = dst.subspan(sz);
			}
		}
	};

	/* --- to_bytes --- */

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	void to_bytes(lak::span<byte_t, lak::to_bytes_size<T, E>()> bytes,
	              const T &value);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<> to_bytes(lak::span<byte_t> bytes, const T &value);

	template<typename T, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	lak::array<byte_t, lak::to_bytes_size<T, E>()> to_bytes(const T &value);

	/* --- array_to_bytes --- */

	template<typename T, size_t S, lak::endian E = lak::endian::little>
	requires(lak::to_bytes_traits<T, E>::const_size)
	lak::array<byte_t, S * lak::to_bytes_size<T, E>()> array_to_bytes(
	  lak::span<const T, S> values);

	template<typename T, lak::endian E = lak::endian::little>
	lak::array<byte_t> array_to_bytes(lak::span<const T> values);

	template<typename T, lak::endian E = lak::endian::little>
	lak::result<> array_to_bytes(lak::span<byte_t> bytes,
	                             lak::span<const T> values);
}

#include "binary_traits.inl"

#endif
