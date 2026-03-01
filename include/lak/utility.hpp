#ifndef LAK_UTILITY_HPP
#define LAK_UTILITY_HPP

#include "lak/system/compiler.hpp"

namespace lak
{
	/* --- incomplete --- */

	struct incomplete;

	/* --- monostate --- */

	struct monostate
	{
	};

	template<typename...>
	using monostate_t = monostate;

	/* --- bottom --- */

	struct bottom
	{
		bottom() = delete;
	};

	/* --- nonsuch --- */

	struct nonesuch
	{
		~nonesuch()                      = delete;
		nonesuch(const nonesuch &)       = delete;
		void operator=(const nonesuch &) = delete;
	};

	/* --- unreachable --- */

	[[noreturn]] inline void unreachable() { LAK_UNREACHABLE(); }

	/* --- void_t --- */

	template<typename...>
	using void_t = void;

	/* --- function_ptr_t --- */

	struct function_ptr_t
	{
		template<typename R, typename... T>
		using pointer = R (*)(T...);

	private:
		pointer<void, lak::incomplete> _ptr = nullptr;

	public:
		function_ptr_t()                                  = default;
		function_ptr_t(const function_ptr_t &)            = default;
		function_ptr_t &operator=(const function_ptr_t &) = default;
		~function_ptr_t()                                 = default;

		template<typename R, typename... T>
		function_ptr_t(pointer<R, T...> f)
		: _ptr(reinterpret_cast<pointer<void, lak::incomplete>>(f))
		{
		}

		template<typename R, typename... T>
		operator pointer<R, T...>()
		{
			return reinterpret_cast<pointer<R, T...>>(_ptr);
		}
	};
}

#include "lak/type_traits.hpp"

#include <functional>
#include <memory>
#include <utility>

namespace lak
{
	static_assert(lak::is_default_constructible_v<lak::monostate>);
	static_assert(!lak::is_constructible_v<lak::bottom>);
	static_assert(!lak::is_constructible_v<lak::nonesuch>);

	/* --- detected_or --- */

	template<typename DEFAULT,
	         typename V,
	         template<typename...> typename OP,
	         typename... ARGS>
	struct _detected
	{
		using value_t = lak::false_type;
		using type    = DEFAULT;
	};

	template<typename DEFAULT,
	         template<typename...> typename OP,
	         typename... ARGS>
	struct _detected<DEFAULT, void_t<OP<ARGS...>>, OP, ARGS...>
	{
		using value_t = lak::true_type;
		using type    = OP<ARGS...>;
	};

	template<typename DEFAULT,
	         template<typename...> typename OP,
	         typename... ARGS>
	using detected_or = lak::_detected<DEFAULT, void, OP, ARGS...>;

	template<typename DEFAULT,
	         template<typename...> typename OP,
	         typename... ARGS>
	using detected_or_t = typename lak::detected_or<DEFAULT, OP, ARGS...>::type;

	template<template<typename...> typename OP, typename... ARGS>
	using detected_t =
	  typename lak::detected_or<lak::nonesuch, OP, ARGS...>::type;

	/* --- is_detected --- */

	template<template<typename...> typename OP, typename... ARGS>
	using is_detected =
	  typename lak::detected_or<lak::nonesuch, OP, ARGS...>::value_t;

	template<template<typename...> typename OP, typename... ARGS>
	constexpr inline bool is_detected_v = lak::is_detected<OP, ARGS...>::value;

	/* --- is_detected_exact --- */

	template<typename EXPECTED,
	         template<typename...> typename OP,
	         typename... ARGS>
	using is_detected_exact =
	  lak::is_same<EXPECTED, lak::detected_t<OP, ARGS...>>;

	template<typename EXPECTED,
	         template<typename...> typename OP,
	         typename... ARGS>
	constexpr inline bool is_detected_exact_v =
	  lak::is_detected_exact<EXPECTED, OP, ARGS...>::value;

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
		static_assert(I <= sizeof...(U));
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

	/* --- ref_or_move --- */

	template<typename T>
	using ref_or_move_t = lak::conditional_t<lak::is_lvalue_reference_v<T>,
	                                         T,
	                                         lak::remove_reference_t<T>>;

	template<typename T>
	force_inline constexpr lak::ref_or_move_t<T> ref_or_move(T &&t) noexcept
	{
		return static_cast<lak::ref_or_move_t<T> &&>(t);
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

	/* --- destructive_move --- */

	template<typename T>
	constexpr T destructive_move(T *src)
	{
		T result{lak::move(*src)};
		lak::destroy_at(src);
		return result;
	}

	/* --- destructive_move_construct --- */

	template<typename T>
	constexpr void destructive_move_construct(T *src, T *dst)
	{
		::new (dst) T(lak::destructive_move(src));
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