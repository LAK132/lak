#ifndef LAK_SPAN_FORWARD_HPP
#define LAK_SPAN_FORWARD_HPP

#include "lak/char.hpp"
#include "lak/concepts.hpp"
#include "lak/stdint.hpp"
#include "lak/tuple.hpp"
#include "lak/type_traits.hpp"

#include <array>
#include <vector>

// When we move to C++20 we need to allow ASSERTF to work in constexpr
// functions.
#ifndef LAK_COMPILER_CPP20
#	define ASSERT_CONSTEXPR
#else
#	define ASSERT_CONSTEXPR constexpr
#endif

namespace lak
{
	template<typename T, size_t SIZE>
	struct array;

	template<typename T>
	struct is_span : lak::false_type
	{
	};

	template<typename T>
	inline constexpr bool is_span_v = is_span<T>::value;

	/* --- Compile time sized spans --- */

	template<typename T, size_t SIZE = lak::dynamic_extent>
	struct span
	{
	private:
		T *_data = nullptr;

		template<typename U, size_t S>
		friend struct span;

	public:
		constexpr span()             = default;
		constexpr span(const span &) = default;
		constexpr span &operator=(const span &) = default;

		template<lak::concepts::fixed_contiguous_range_of<T, SIZE> RANGE>
		inline constexpr span(RANGE &range) noexcept : _data(range.data())
		{
		}

		template<lak::concepts::fixed_contiguous_range_of<T, SIZE> RANGE>
		inline constexpr span(const RANGE &range) noexcept : _data(range.data())
		{
		}

		explicit inline constexpr span(
		  std::initializer_list<std::remove_const_t<T>> list) noexcept
		: _data(list.begin())
		{
		}

		template<size_t S = SIZE>
		requires(S == SIZE) //
		  inline constexpr span(T (&data)[S]) noexcept
		: _data(data)
		{
		}

		static inline constexpr span from_ptr(T *data) noexcept;

		inline ASSERT_CONSTEXPR T &operator[](size_t index) const noexcept;

		inline constexpr T *data() const noexcept;

		[[nodiscard]] inline constexpr bool empty() const noexcept;

		inline constexpr size_t size() const noexcept;

		inline constexpr size_t size_bytes() const noexcept;

		inline constexpr T *begin() const noexcept;

		inline constexpr T *end() const noexcept;

		template<size_t offset, size_t count = lak::dynamic_extent>
		inline ASSERT_CONSTEXPR span<T, count> subspan() const;

		inline ASSERT_CONSTEXPR span<T, lak::dynamic_extent> subspan(
		  size_t offset, size_t count = lak::dynamic_extent) const;

		template<size_t count>
		inline constexpr span<T, count> first() const;

		inline ASSERT_CONSTEXPR span<T, lak::dynamic_extent> first(
		  size_t count) const;

		template<size_t count>
		inline constexpr span<T, count> last() const;

		inline ASSERT_CONSTEXPR span<T, lak::dynamic_extent> last(
		  size_t count) const;

		inline auto to_string() const;
		inline auto stringify() const;

		// add const
		constexpr operator span<const T, SIZE>() const noexcept;

		// reinterpret cast to U
		template<lak::concepts::ptr_reinterpret_castable_from<T> U>
		requires(!lak::concepts::same_as<U, T> && !lak::is_void_v<U>) //
		  constexpr explicit
		  operator span<U, (SIZE * sizeof(T)) / sizeof(U)>() const noexcept;

		// reinterpret cast to void
		template<lak::concepts::void_type V>
		requires(lak::is_reinterpret_castable_v<T *, V *>) //
		  constexpr
		  operator span<V, SIZE * sizeof(T)>() const noexcept;

		// static cast to dynamic size
		template<lak::concepts::ptr_static_castable_from<T> U>
		constexpr operator span<U, lak::dynamic_extent>() const noexcept;
	};

	template<size_t SIZE>
	struct span<void, SIZE>
	{
	private:
		void *_data = nullptr;

		template<typename U, size_t S>
		friend struct span;

	public:
		constexpr span()             = default;
		constexpr span(const span &) = default;
		constexpr span &operator=(const span &) = default;

		template<
		  typename T,
		  std::enable_if_t<!std::is_same_v<lak::remove_cvref_t<T>, char> &&
		                     !std::is_same_v<lak::remove_cvref_t<T>, wchar_t> &&
		                     !std::is_same_v<lak::remove_cvref_t<T>, char8_t> &&
		                     !std::is_same_v<lak::remove_cvref_t<T>, char16_t> &&
		                     !std::is_same_v<lak::remove_cvref_t<T>, char32_t>,
		                   int> = 0>
		inline constexpr span(T (&data)[SIZE / sizeof(T)]) noexcept : _data(data)
		{
		}

		inline constexpr void *data() const noexcept;

		[[nodiscard]] inline constexpr bool empty() const noexcept;

		inline constexpr size_t size() const noexcept;

		inline constexpr size_t size_bytes() const noexcept;

		// add const
		constexpr operator span<const void, SIZE>() const noexcept;

		// reinterpret cast to T
		template<lak::concepts::ptr_reinterpret_castable_from<void> T>
		requires(!lak::is_void_v<T>) //
		  constexpr explicit
		  operator span<T, (SIZE / sizeof(T))>() const noexcept;

		// static cast to dynamic size
		template<lak::concepts::void_type V>
		constexpr operator span<V, lak::dynamic_extent>() const noexcept;
	};

	template<size_t SIZE>
	struct span<const void, SIZE>
	{
	private:
		const void *_data = nullptr;

		template<typename U, size_t S>
		friend struct span;

	public:
		constexpr span()             = default;
		constexpr span(const span &) = default;
		constexpr span &operator=(const span &) = default;

		template<typename T>
		inline constexpr span(T (&data)[SIZE / sizeof(T)]) noexcept : _data(data)
		{
		}

		inline constexpr const void *data() const noexcept;

		[[nodiscard]] inline constexpr bool empty() const noexcept;

		inline constexpr size_t size() const noexcept;

		inline constexpr size_t size_bytes() const noexcept;

		// reinterpret cast to T
		template<lak::concepts::ptr_reinterpret_castable_from<const void> T>
		requires(!lak::is_void_v<T>) //
		  constexpr explicit
		  operator span<T, SIZE / sizeof(T)>() const noexcept;

		// static cast to dynamic size
		constexpr operator span<const void, lak::dynamic_extent>() const noexcept;
	};

	/* --- Runtime sized spans --- */

	template<typename T>
	struct span<T, lak::dynamic_extent>
	{
	private:
		T *_data     = nullptr;
		size_t _size = 0;

		template<typename U, size_t S>
		friend struct span;

	public:
		constexpr span()             = default;
		constexpr span(const span &) = default;
		constexpr span &operator=(const span &) = default;

		template<lak::concepts::contiguous_range_of<T> RANGE>
		inline constexpr span(RANGE &range) noexcept
		: _data(range.data()), _size(range.size())
		{
		}

		template<lak::concepts::contiguous_range_of<T> RANGE>
		inline constexpr span(const RANGE &range) noexcept
		: _data(range.data()), _size(range.size())
		{
		}

		inline constexpr span(
		  std::initializer_list<std::remove_const_t<T>> list) noexcept
		: _data(list.begin()), _size(list.size())
		{
		}

		template<size_t SIZE>
		inline constexpr span(T (&data)[SIZE]) noexcept : _data(data), _size(SIZE)
		{
		}

		inline constexpr span(T *data, size_t size) noexcept
		: _data(data), _size(size)
		{
		}

		explicit inline ASSERT_CONSTEXPR span(T *begin, T *end) noexcept;

		inline ASSERT_CONSTEXPR T &operator[](size_t index) const noexcept;

		inline constexpr T *data() const noexcept;

		[[nodiscard]] inline constexpr bool empty() const noexcept;

		inline constexpr size_t size() const noexcept;

		inline constexpr size_t size_bytes() const noexcept;

		inline constexpr T *begin() const noexcept;

		inline constexpr T *end() const noexcept;

		template<size_t offset, size_t count = lak::dynamic_extent>
		inline ASSERT_CONSTEXPR span<T, count> subspan() const;

		inline ASSERT_CONSTEXPR span<T, lak::dynamic_extent> subspan(
		  size_t offset, size_t count = lak::dynamic_extent) const;

		template<size_t count>
		inline ASSERT_CONSTEXPR span<T, count> first() const;

		inline ASSERT_CONSTEXPR span<T, lak::dynamic_extent> first(
		  size_t count) const;

		template<size_t count>
		inline ASSERT_CONSTEXPR span<T, count> last() const;

		inline ASSERT_CONSTEXPR span<T, lak::dynamic_extent> last(
		  size_t count) const;

		inline auto to_string() const;
		inline auto stringify() const;

		// add const
		constexpr operator span<const T, lak::dynamic_extent>() const noexcept;

		// reinterpret cast to U
		template<lak::concepts::ptr_reinterpret_castable_from<T> U>
		requires(!lak::concepts::same_as<U, T> && !lak::is_void_v<U>) //
		  constexpr explicit
		  operator span<U, lak::dynamic_extent>() const noexcept;

		// reinterpret cast to void
		template<lak::concepts::void_type V>
		requires(lak::is_reinterpret_castable_v<T *, V *>) //
		  constexpr
		  operator span<V, lak::dynamic_extent>() const noexcept;
	};

	template<>
	struct span<void, lak::dynamic_extent>
	{
	private:
		void *_data  = nullptr;
		size_t _size = 0;

		template<typename U, size_t S>
		friend struct span;

	public:
		constexpr span()             = default;
		constexpr span(const span &) = default;
		constexpr span &operator=(const span &) = default;

		inline constexpr span(void *data, size_t size) noexcept
		: _data(data), _size(size)
		{
		}

		inline constexpr void *data() const noexcept;

		[[nodiscard]] inline constexpr bool empty() const noexcept;

		inline constexpr size_t size() const noexcept;

		inline constexpr size_t size_bytes() const noexcept;

		// add const
		constexpr operator span<const void, lak::dynamic_extent>() const noexcept;

		// reinterpret cast to U
		template<lak::concepts::ptr_reinterpret_castable_from<void> U>
		requires(!lak::is_void_v<U>) //
		  constexpr explicit
		  operator span<U, lak::dynamic_extent>() const noexcept;
	};

	template<>
	struct span<const void, lak::dynamic_extent>
	{
	private:
		const void *_data = nullptr;
		size_t _size      = 0;

		template<typename U, size_t S>
		friend struct span;

	public:
		constexpr span()             = default;
		constexpr span(const span &) = default;
		constexpr span &operator=(const span &) = default;

		inline constexpr span(const void *data, size_t size) noexcept
		: _data(data), _size(size)
		{
		}

		inline constexpr const void *data() const noexcept;

		[[nodiscard]] inline constexpr bool empty() const noexcept;

		inline constexpr size_t size() const noexcept;

		inline constexpr size_t size_bytes() const noexcept;

		// reinterpret cast to U
		template<lak::concepts::ptr_reinterpret_castable_from<const void> U>
		requires(!lak::is_void_v<U>) //
		  constexpr explicit
		  operator span<U, lak::dynamic_extent>() const noexcept;
	};

	template<lak::concepts::contiguous_range RANGE>
	span(RANGE &r)
	  -> span<lak::remove_pointer_t<decltype(r.data())>, lak::dynamic_extent>;

	template<lak::concepts::contiguous_range RANGE>
	span(const RANGE &r)
	  -> span<lak::remove_pointer_t<decltype(r.data())>, lak::dynamic_extent>;

	template<typename T, size_t N>
	span(const lak::array<T, N> &) -> span<const T, N>;

	template<typename T, size_t N>
	span(lak::array<T, N> &) -> span<T, N>;

	template<typename T, size_t N>
	span(T (&)[N]) -> span<T, N>;

	template<typename T>
	span(T *, size_t) -> span<T, lak::dynamic_extent>;

	template<typename T>
	span(T *, T *) -> span<T, lak::dynamic_extent>;

	template<typename T, size_t S>
	struct is_span<lak::span<T, S>> : lak::true_type
	{
	};

	template<typename T>
	using dynamic_span = lak::span<T, lak::dynamic_extent>;

	/* --- helper functions --- */

	// return s.end() if v is not found
	template<typename T>
	constexpr T *find_first(lak::span<T> s, const T &v);

	template<typename T>
	constexpr bool contains(lak::span<const T> s, const T &v);

	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split(lak::span<T> s, T *at);

	template<typename T, lak::enable_if_i<!lak::is_const_v<T>> = 0>
	lak::pair<lak::span<T>, lak::span<T>> split(lak::span<T> s, const T *at);

	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split(lak::span<T> s, size_t at);

	// {s, s.last(0)} if s does not contain a v
	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split_before(lak::span<T> s,
	                                                   const T &v);

	// {s.first(0), s} if s does not contain a v
	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split_after(lak::span<T> s,
	                                                  const T &v);

	template<typename T>
	void fill(lak::span<T> span, const T &value);

	// Find the subspan of source that is equal to sub.
	template<typename T>
	lak::span<T> find_subspan(lak::span<T> source, lak::span<T> sub);

	template<typename T>
	lak::span<T> common_initial_sequence(lak::span<T> a, lak::span<T> b);

	template<typename T>
	lak::span<T> rotate_left(lak::span<T> data, size_t distance = 1);

	template<typename T>
	lak::span<T> rotate_right(lak::span<T> data, size_t distance = 1);

	template<typename T>
	struct shift_result
	{
		lak::span<T> moved_to;
		lak::span<T> unmodified;
		lak::span<T> moved_from;
	};

	template<typename T>
	lak::shift_result<T> shift_left(lak::span<T> data, size_t distance = 1);

	template<typename T>
	lak::shift_result<T> shift_right(lak::span<T> data, size_t distance = 1);

	template<typename T>
	size_t compare(lak::span<const T> a, lak::span<const T> b);

	// Check if the spans are pointing to the same underlying data.
	template<typename T>
	bool same_span(lak::span<const T> a, lak::span<const T> b);
}

// Check if each element of the data pointed to by the spans compare equal (not
// that the spans point to the exact same data).
template<typename T, size_t S1, typename U, size_t S2>
bool operator==(lak::span<T, S1> a, lak::span<U, S2> b);

template<typename T, size_t S1, typename U, size_t S2>
bool operator!=(lak::span<T, S1> a, lak::span<U, S2> b);

#undef ASSERT_CONSTEXPR

#endif
