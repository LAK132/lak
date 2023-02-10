#ifndef LAK_VARIANT_HPP
#	define LAK_VARIANT_HPP

#	include "lak/type_pack.hpp"
#	include "lak/type_traits.hpp"
#	include "lak/utility.hpp"

namespace lak
{
	/* --- is_variant --- */

	template<typename T>
	struct is_variant : public lak::false_type
	{
	};

	template<typename T>
	static constexpr bool is_variant_v = lak::is_variant<T>::value;

	/* --- var_t --- */

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

	template<size_t I, typename T>
	static constexpr force_inline _var_t<I, T &&> move_var(T &v)
	{
		return {lak::move(v)};
	}

	/* --- uninitialised_union_flag_t --- */

	struct uninitialised_union_flag_t
	{
	};
	static constexpr uninitialised_union_flag_t uninitialised_union_flag;

	/* --- pack_union --- */

	template<typename... T>
	struct pack_union;

	/* --- variant --- */

	template<typename... T>
	struct variant;

	template<typename... T>
	struct is_variant<lak::variant<T...>> : public lak::true_type
	{
	};
}

#endif

#ifdef LAK_VARIANT_FORWARD_ONLY
#	undef LAK_VARIANT_FORWARD_ONLY
#else
#	ifndef LAK_VARIANT_HPP_IMPL
#		define LAK_VARIANT_HPP_IMPL

#		include "lak/index_set.hpp"
#		include "lak/concepts.hpp"

namespace lak
{
	/* --- pack_union --- */

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
		force_inline auto &emplace(ARGS &&...args);

		template<typename... ARGS>
		force_inline void emplace_dynamic(lak::index_set_for<T, U...> i,
		                                  ARGS &&...args);

		template<size_t I>
		force_inline void reset();

		force_inline void reset_dynamic(lak::index_set_for<T, U...> i);
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
		force_inline auto &emplace(ARGS &&...args);

		template<typename... ARGS>
		force_inline void emplace_dynamic(lak::index_set<0>, ARGS &&...args);

		template<size_t I>
		force_inline void reset();

		force_inline void reset_dynamic(lak::index_set<0>);
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

		template<typename... U>
		friend struct variant;

		using index_type = lak::index_set_for<T...>;

	private:
		using union_type = lak::pack_union<lak::lvalue_to_ptr_t<T>...>;

		template<size_t I>
		static constexpr bool _is_ref = lak::is_lvalue_reference_v<value_type<I>>;

		static constexpr size_t _size          = sizeof...(T);
		static constexpr size_t _internal_size = _size;

		index_type _index = lak::size_type<0U>{};
		union_type _value;

		template<size_t I>
		force_inline auto &unsafe_get()
		{
			return _value.template get<I>();
		}

		template<size_t I>
		force_inline const auto &unsafe_get() const
		{
			return _value.template get<I>();
		}

		force_inline index_type internal_index() const { return _index; }

	public:
		template<typename V = value_type<0U>,
		         lak::enable_if_i<lak::is_default_constructible_v<V>> = 0>
		variant() : _index(lak::size_type<0U>{}), _value()
		{
		}

		template<size_t I, typename... ARGS>
		requires((I < _size) && !_is_ref<I>)
		variant(lak::in_place_index_t<I>, ARGS &&...args)
		: _index(lak::size_type<I>{}),
		  _value(lak::in_place_index<I>, lak::forward<ARGS>(args)...)
		{
		}

		template<size_t I, typename... ARGS>
		requires((I < _size) && _is_ref<I>)
		variant(lak::in_place_index_t<I>, value_type<I> ref)
		: _index(lak::size_type<I>{}), _value(lak::in_place_index<I>, &ref)
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

		template<typename V, typename... ARGS>
		static variant make(ARGS &&...args);

		~variant();

		size_t index() const { return _index; }
		index_type index_set() const { return _index; }

		template<size_t I>
		requires((I < _size))
		bool holds() const
		{
			return _index.value() == I;
		}

		template<typename U>
		bool holds() const
		{
			return _index.value() == index_of<U>;
		}

		constexpr size_t size() const { return _size; }

		template<size_t I>
		auto *get();

		template<size_t I>
		const auto *get() const;

		template<typename U>
		auto *get()
		{
			return get<index_of<U>>();
		}

		template<typename U>
		const auto *get() const
		{
			return get<index_of<U>>();
		}

		template<typename F>
		auto visit(F &&func);
		template<typename F>
		auto visit(F &&func) const;

		template<typename F>
		auto flat_visit(F &&func);
		template<typename F>
		auto flat_visit(F &&func) const;
	};

	/* --- variant<variant<T...>, U...> --- */

	template<typename... T, typename... U>
	requires((lak::is_standard_layout_v<lak::variant<T..., U...>>))
	struct variant<variant<T...>, U...>
	{
		static_assert(((!lak::is_rvalue_reference_v<U>)&&...));

		using indices = lak::index_sequence_for<variant<T...>, U...>;

		template<size_t I>
		using value_type = lak::nth_type_t<I, variant<T...>, U...>;

		template<typename V>
		static constexpr size_t index_of =
		  lak::index_of_element_v<V, variant<T...>, U...>;

		template<typename... V>
		friend struct variant;

		using index_type = lak::index_set_for<T..., U...>;

	private:
		using union_type = lak::pack_union<variant<T...>, variant<T..., U...>>;

		template<size_t I>
		static constexpr bool _is_ref = lak::is_lvalue_reference_v<value_type<I>>;

		static constexpr size_t _size = sizeof...(U) + 1U;
		static constexpr size_t _internal_size =
		  variant<T...>::_internal_size + sizeof...(U);
		static constexpr size_t _internal_offset =
		  variant<T...>::_internal_size - 1U;

		union_type _value;

		template<size_t I>
		force_inline auto &unsafe_get()
		{
			if constexpr (I == 0U)
				return _value.value;
			else
				return _value.next.value.template unsafe_get<I + _internal_offset>();
		}

		template<size_t I>
		force_inline const auto &unsafe_get() const
		{
			if constexpr (I == 0U)
				return _value.value;
			else
				return _value.next.value.template unsafe_get<I + _internal_offset>();
		}

		force_inline index_type internal_index() const
		{
			return _value.next.value.internal_index();
		}

	public:
		template<typename V = value_type<0U>,
		         lak::enable_if_i<lak::is_default_constructible_v<V>> = 0>
		variant() : _value()
		{
		}

		template<typename... ARGS>
		variant(lak::in_place_index_t<0U>, ARGS &&...args)
		: _value(lak::in_place_index<0U>, lak::forward<ARGS>(args)...)
		{
		}

		template<size_t I, typename... ARGS>
		requires((I > 0) && (I < _size) && !_is_ref<I>)
		variant(lak::in_place_index_t<I>, ARGS &&...args)
		: _value(lak::in_place_index<1U>,
		         lak::in_place_index<I + _internal_offset>,
		         lak::forward<ARGS>(args)...)
		{
		}

		template<size_t I, typename... ARGS>
		requires((I > 0) && (I < _size) && _is_ref<I>)
		variant(lak::in_place_index_t<I>, value_type<I> ref)
		: _value(lak::in_place_index<1U>,
		         lak::in_place_index<I + _internal_offset>,
		         &ref)
		{
		}

		template<size_t I, typename V>
		variant(lak::_var_t<I, V> var)
		: variant(lak::in_place_index<I>, lak::forward<V>(var.value))
		{
		}

		template<lak::concepts::one_of<variant<T...>, U...> V>
		variant(const V &val)
		: variant(lak::in_place_index<variant::index_of<V>>, val)
		{
		}

		template<lak::concepts::one_of<variant<T...>, U...> V>
		variant(V &&val)
		: variant(lak::in_place_index<variant::index_of<V>>, lak::forward<V>(val))
		{
		}

		variant(const variant &other);

		variant(variant &&other);

		variant &operator=(const variant &other);

		variant &operator=(variant &&other);

		template<size_t I, typename... ARGS>
		auto &emplace(ARGS &&...args)
		{
			if (internal_index() <= _internal_offset)
				_value.template reset<0U>();
			else
				_value.template reset<1U>();

			if constexpr (I <= _internal_offset)
				return _value.template emplace<0U>(lak::forward<ARGS>(args)...);
			else
				return _value.template emplace<1U>(
				  lak::in_place_index<I + _internal_offset>,
				  lak::forward<ARGS>(args)...);
		}

		template<size_t I, typename... ARGS>
		requires((I < _size))
		static variant make(ARGS &&...args)
		{
			return variant(lak::in_place_index<I>, lak::forward<ARGS>(args)...);
		}

		template<typename V, typename... ARGS>
		requires((index_of<V> < _size))
		static variant make(ARGS &&...args)
		{
			return variant(lak::in_place_index<index_of<V>>,
			               lak::forward<ARGS>(args)...);
		}

		~variant();

		size_t index() const
		{
			const size_t ind = internal_index();
			return ind <= _internal_offset ? 0U : ind - _internal_offset;
		}
		lak::index_set_from_sequence<indices> index_set() const
		{
			const auto &i{internal_index()};
			if (i.value() <= _internal_offset)
				return lak::index_set_from_sequence<indices>(lak::size_type<0U>{});
			else
				return lak::index_set_from_sequence<indices>::make(i.value() -
				                                                   _internal_offset)
				  .unsafe_unwrap();
		}

		template<size_t I>
		requires((I < _size))
		bool holds() const
		{
			if constexpr (I == 0)
				return internal_index() <= _internal_offset;
			else
				return internal_index() == (I + _internal_offset);
		}

		template<typename V>
		bool holds() const
		{
			return index() == index_of<V>;
		}

		constexpr size_t size() const { return _size; }

		template<size_t I>
		auto *get()
		{
			if constexpr (I == 0)
				return internal_index() <= _internal_offset ? &_value.value : nullptr;
			else
				return internal_index() > _internal_offset
				         ? _value.next.value.template get<I + _internal_offset>()
				         : nullptr;
		}

		template<size_t I>
		const auto *get() const
		{
			if constexpr (I == 0)
				return internal_index() <= _internal_offset ? &_value.value : nullptr;
			else
				return internal_index() > _internal_offset
				         ? _value.next.value.template get<I + _internal_offset>()
				         : nullptr;
		}

		template<typename V>
		auto *get()
		{
			return get<index_of<V>>();
		}

		template<typename V>
		const auto *get() const
		{
			return get<index_of<V>>();
		}

		template<typename F>
		auto visit(F &&func)
		{
			if (internal_index() <= _internal_offset)
				return func(_value.value);
			else
				return _value.next.value.visit(lak::forward<F>(func));
		}
		template<typename F>
		auto visit(F &&func) const
		{
			if (internal_index() <= _internal_offset)
				return func(_value.value);
			else
				return _value.next.value.visit(lak::forward<F>(func));
		}

		template<typename F>
		auto flat_visit(F &&func)
		{
			if (internal_index() <= _internal_offset)
				return _value.value.flat_visit(lak::forward<F>(func));
			else
				return _value.next.value.flat_visit(lak::forward<F>(func));
		}
		template<typename F>
		auto flat_visit(F &&func) const
		{
			if (internal_index() <= _internal_offset)
				return _value.value.flat_visit(lak::forward<F>(func));
			else
				return _value.next.value.flat_visit(lak::forward<F>(func));
		}
	};

	static_assert(lak::is_standard_layout_v<lak::variant<int>>);

	struct _variant_standard_layout_test_public
	{
	public:
		int value;
	};
	struct _variant_standard_layout_test_private
	{
	private:
		int value;
	};
	static_assert(lak::is_standard_layout_v<
	              lak::variant<lak::_variant_standard_layout_test_public,
	                           lak::_variant_standard_layout_test_private,
	                           int>>);
	static_assert(
	  lak::is_standard_layout_v<
	    lak::variant<lak::variant<lak::_variant_standard_layout_test_public,
	                              lak::_variant_standard_layout_test_private,
	                              int>,
	                 lak::_variant_standard_layout_test_public,
	                 lak::_variant_standard_layout_test_private,
	                 int>>);

	static_assert(sizeof(lak::variant<lak::monostate>) ==
	              sizeof(lak::variant<lak::variant<lak::monostate>>));

	static_assert(lak::is_same_v<lak::variant<char>::value_type<0>, char>);
}

#		include "lak/variant.inl"

#	endif
#endif
