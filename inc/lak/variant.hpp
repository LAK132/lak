#ifndef LAK_VARIANT_HPP
#define LAK_VARIANT_HPP

#include "lak/type_pack.hpp"
#include "lak/type_traits.hpp"
#include "lak/utility.hpp"

namespace lak
{
	/* --- is_variant --- */

	template<typename T>
	struct is_variant : public lak::false_type
	{
	};
	template<typename T>
	static constexpr bool is_variant_v = lak::is_variant<T>::value;

	/* --- _var_t --- */

	template<size_t I, typename T>
	struct _var_t
	{
		T value;
	};

	template<size_t I, typename T>
	static constexpr force_inline _var_t<I, T &&> var_t(T &&v)
	{
		return {lak::forward<T>(v)};
	}

	template<size_t I, typename T>
	static constexpr force_inline _var_t<I, T &> var_t(T &v)
	{
		return {v};
	}

	/* --- uninitialised_union_flag_t --- */

	struct uninitialised_union_flag_t
	{
	};
	static constexpr uninitialised_union_flag_t uninitialised_union_flag;

	/* --- pack_union --- */

	template<typename... T>
	struct pack_union;

	template<typename T, typename... U>
	struct pack_union<T, U...>
	{
		static_assert(lak::is_same_v<lak::remove_reference_t<T>, T>);
		static_assert(!lak::is_void_v<T>);

		using next_type = pack_union<U...>;

		template<size_t I>
		using value_type =
		  lak::conditional_t<(I == 0),
		                     T,
		                     typename next_type::template value_type<(I - 1)>>;

		static constexpr size_t size = sizeof...(U) + 1;

		union
		{
			T value;
			next_type next;
		};

		template<typename V                                           = T,
		         lak::enable_if_i<lak::is_default_constructible_v<V>> = 0>
		pack_union() : value()
		{
		}

		template<typename... ARGS>
		force_inline pack_union(lak::in_place_index_t<0>, ARGS &&...args)
		: value(lak::forward<ARGS>(args)...)
		{
		}

		template<size_t I,
		         typename... ARGS,
		         lak::enable_if_i<(I > 0 && I < size)> = 0>
		force_inline pack_union(lak::in_place_index_t<I>, ARGS &&...args)
		: next(lak::in_place_index<I - 1>, lak::forward<ARGS>(args)...)
		{
		}

		force_inline pack_union(lak::uninitialised_union_flag_t)
		: next(lak::uninitialised_union_flag)
		{
		}

		// union members must be manually destroyed by the user
		~pack_union() {}

		template<size_t I>
		force_inline auto &get();

		template<size_t I>
		force_inline const auto &get() const;

		template<size_t I, typename... ARGS>
		force_inline void emplace(ARGS &&...args);

		template<typename... ARGS>
		force_inline bool emplace_dynamic(size_t i, ARGS &&...args);

		template<size_t I>
		force_inline void reset();

		force_inline bool reset_dynamic(size_t i);
	};

	template<typename T>
	struct pack_union<T>
	{
		static_assert(lak::is_same_v<lak::remove_reference_t<T>, T>);

		template<size_t I>
		using value_type = lak::conditional_t<I == 0, T, void>;

		static constexpr size_t size = 1;

		union
		{
			T value;
			lak::monostate _;
		};

		template<typename V                                           = T,
		         lak::enable_if_i<lak::is_default_constructible_v<V>> = 0>
		pack_union() : value()
		{
		}

		template<typename... ARGS>
		pack_union(lak::in_place_index_t<0>, ARGS &&...args)
		: value(lak::forward<ARGS>(args)...)
		{
		}

		pack_union(lak::uninitialised_union_flag_t);

		// union members must be manually destroyed by the user
		~pack_union() {}

		template<size_t I>
		auto &get();

		template<size_t I>
		const auto &get() const;

		template<size_t I, typename... ARGS>
		force_inline void emplace(ARGS &&...args);

		template<typename... ARGS>
		force_inline bool emplace_dynamic(size_t i, ARGS &&...args);

		template<size_t I>
		force_inline void reset();

		force_inline bool reset_dynamic(size_t i);
	};

	static_assert(lak::is_same_v<lak::pack_union<char>::value_type<0>, char>);

	/* --- variant --- */

	template<typename... T>
	struct variant
	{
		static_assert(((!lak::is_rvalue_reference_v<T>)&&...));

		using indices = lak::index_sequence_for<T...>;

		template<size_t I>
		using value_type = lak::nth_type_t<I, T...>;

		template<typename U>
		static constexpr size_t index_of = lak::index_of_element_v<U, T...>;

	private:
		using union_type = pack_union<lak::lvalue_to_ptr_t<T>...>;

		template<size_t I>
		static constexpr bool _is_ref = lak::is_lvalue_reference_v<value_type<I>>;

		static constexpr size_t _size = sizeof...(T);

		size_t _index = 0;
		union_type _value;

	public:
		template<typename V = value_type<0>,
		         lak::enable_if_i<lak::is_default_constructible_v<V>> = 0>
		variant() : _index(0), _value()
		{
		}

		template<size_t I, typename... ARGS>
		requires((I < _size) && !_is_ref<I>) //
		  variant(lak::in_place_index_t<I>, ARGS &&...args)
		: _index(I), _value(lak::in_place_index<I>, lak::forward<ARGS>(args)...)
		{
		}

		template<size_t I, typename... ARGS>
		requires((I < _size) && _is_ref<I>) //
		  variant(lak::in_place_index_t<I>, value_type<I> ref)
		: _index(I), _value(lak::in_place_index<I>, &ref)
		{
		}

		template<size_t I, typename U>
		variant(lak::_var_t<I, U> var)
		: variant(lak::in_place_index<I>, lak::forward<U>(var.value))
		{
		}

		template<lak::concepts::one_of<T...> U>
		variant(const U &val)
		: variant(lak::in_place_index<variant::index_of<U>>, val)
		{
		}

		template<lak::concepts::one_of<T...> U>
		variant(U &&val)
		: variant(lak::in_place_index<variant::index_of<U>>, lak::forward<U>(val))
		{
		}

		variant(const variant &other);

		variant(variant &&other);

		variant &operator=(const variant &other);

		variant &operator=(variant &&other);

		template<size_t I, typename... ARGS>
		auto &emplace(ARGS &&...args);

		template<size_t I, typename... ARGS>
		static variant make(ARGS &&...args);

		~variant();

		size_t index() const { return _index; }

		template<typename U>
		bool holds() const
		{
			return _index == index_of<U>;
		}

		constexpr size_t size() const { return _size; }

		template<size_t I>
		auto *get();

		template<size_t I>
		const auto *get() const;

		template<typename T>
		auto *get()
		{
			return get<index_of<T>>();
		}

		template<typename T>
		const auto *get() const
		{
			return get<index_of<T>>();
		}
	};

	static_assert(lak::is_same_v<lak::variant<char>::value_type<0>, char>);
}

#include "lak/variant.inl"

#endif
