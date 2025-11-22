#ifndef LAK_UTILITY_HPP
#define LAK_UTILITY_HPP

#include "lak/system/compiler.hpp"

#include "lak/type_traits.hpp"

#include <functional>
#include <memory>
#include <utility>

namespace lak
{
	/* --- incomplete --- */

	struct incomplete;

	/* --- monostate --- */

	struct monostate
	{
	};
	static_assert(lak::is_default_constructible_v<lak::monostate>);

	/* --- bottom --- */

	struct bottom
	{
		bottom() = delete;
	};

	/* --- unreachable --- */

	[[noreturn]] inline void unreachable() { LAK_UNREACHABLE(); }

	/* --- reference_wrapper --- */

	template<typename T>
	using reference_wrapper = std::reference_wrapper<T>;

	template<typename T>
	constexpr lak::reference_wrapper<T> ref(T &t) noexcept
	{
		return lak::reference_wrapper<T>{t};
	}

	template<typename T>
	constexpr lak::reference_wrapper<T> ref(lak::reference_wrapper<T> t) noexcept
	{
		return lak::reference_wrapper<T>{t};
	}

	template<typename T>
	constexpr lak::reference_wrapper<lak::add_wconst_t<T>> cref(T &t) noexcept
	{
		return lak::reference_wrapper<lak::add_wconst_t<T>>{t};
	}

	template<typename T>
	constexpr lak::reference_wrapper<lak::add_wconst_t<T>> cref(
	  lak::reference_wrapper<T> t) noexcept
	{
		return lak::reference_wrapper<lak::add_wconst_t<T>>{t};
	}

	/* --- in_place_index --- */

	template<size_t I>
	struct in_place_index_t : public lak::size_type<I>
	{
	};
	template<size_t I>
	inline constexpr in_place_index_t<I> in_place_index{};

	struct in_place_t
	{
	};
	inline constexpr in_place_t in_place{};

	/* --- get_nth --- */

	template<size_t I, typename T, typename... U>
	requires(I != 0)
	force_inline constexpr lak::nth_type_t<I, T, U...> &get_nth(T &,
	                                                            U &...u) noexcept
	{
		return lak::get_nth<I - 1U, U...>(u...);
	}

	template<size_t I, typename T, typename... U>
	requires(I == 0)
	force_inline constexpr lak::nth_type_t<I, T, U...> &get_nth(T &t,
	                                                            U &...) noexcept
	{
		return t;
	}

	/* --- move --- */

	template<typename T>
	force_inline constexpr lak::remove_reference_t<T> &&move(T &&t) noexcept
	{
		return static_cast<lak::remove_reference_t<T> &&>(t);
	}

	/* --- forward --- */

	template<typename T>
	force_inline constexpr T &&forward(lak::remove_reference_t<T> &t) noexcept
	{
		return static_cast<T &&>(t);
	}

	template<typename T>
	force_inline constexpr T &&forward(lak::remove_reference_t<T> &&t) noexcept
	{
		return static_cast<T &&>(t);
	}

	/* --- swap --- */

	template<typename T>
	force_inline constexpr void swap(T &a, T &b)
	{
		T temp{lak::move(a)};
		a = lak::move(b);
		b = lak::move(temp);
	}

	template<typename T>
	force_inline constexpr void swap(T *&a, T *&b)
	{
		T *temp = a;
		a       = b;
		b       = temp;
	}

	template<typename T, size_t SIZE>
	force_inline constexpr void swap(T (&a)[SIZE], T (&b)[SIZE])
	{
		for (size_t i = 0; i < SIZE; ++i) lak::swap(a[i], b[i]);
	}

	/* --- exchange --- */

	template<typename T>
	inline constexpr T exchange(T &a, T &&n)
	{
		T result{lak::move(a)};
		a = lak::move(n);
		return result;
	}

	template<typename T, typename U>
	inline constexpr T exchange(T &a, U &&n)
	{
		T result{lak::move(a)};
		a = T(lak::forward<U>(n));
		return result;
	}

	/* --- addressof --- */

	template<typename T>
	auto addressof(T &&t)
	{
		return std::addressof(lak::forward<T>(t));
	}

	/* --- construct_at --- */

	template<typename T, typename... ARGS>
	constexpr T *construct_at(T *p, ARGS &&...args)
	{
		return ::new (const_cast<void *>(static_cast<const volatile void *>(p)))
		  T(lak::forward<ARGS>(args)...);
	}

	/* --- destroy_at --- */

	template<typename T>
	constexpr void destroy_at(T *p)
	{
		if constexpr (lak::is_array_v<T>)
			for (auto &elem : *p) lak::destroy_at(lak::addressof(elem));
		else
			p->~T();
	}

	/* --- destructive_move_construct --- */

	template<typename T>
	constexpr void destructive_move_construct(T *src, T *dst)
	{
		new (dst) T(lak::move(*src));
		if constexpr (!std::is_trivially_destructible_v<T>) src->~T();
	}

	/* --- as_ptr --- */

	// Converts pointers and pointer-like types to pointers.

	template<typename T>
	force_inline constexpr T *as_ptr(T *p)
	{
		return p;
	}

	template<typename T>
	force_inline T *as_ptr(lak::reference_wrapper<T> *p)
	{
		return p ? &p->get() : nullptr;
	}

	template<typename T>
	force_inline const T *as_ptr(const lak::reference_wrapper<T> *p)
	{
		return p ? &p->get() : nullptr;
	}

	// Converts references and reference-like types to pointers.

	template<typename T>
	force_inline constexpr T *as_ptr(lak::remove_refs_ptrs<T> &p)
	{
		lak::addressof(p);
	}

	template<typename T>
	force_inline T *as_ptr(lak::reference_wrapper<T> &p)
	{
		return &p->get();
	}

	template<typename T>
	force_inline const T *as_ptr(const lak::reference_wrapper<T> &p)
	{
		return &p->get();
	}
}

#endif