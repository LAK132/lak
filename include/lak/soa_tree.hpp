#ifndef LAK_SAO_TREE_HPP
#define LAK_SAO_TREE_HPP

#include "lak/array.hpp"
#include "lak/concepts.hpp"
#include "lak/index_set.hpp"
#include "lak/span.hpp"
#include "lak/tuple.hpp"

namespace lak
{
	template<typename... T>
	struct soa_tree
	{
		using index_type = lak::index_set_for<T...>;

		template<lak::concepts::one_of<T...> U>
		static constexpr size_t index_of = lak::index_of_element_v<U, T...>;

		template<lak::concepts::one_of<T...> U>
		struct pointer
		{
			size_t index;

			inline static pointer make(size_t index)
			{
				return pointer{.index = index};
			}
		};

		template<lak::concepts::one_of<T...> U>
		struct subspan
		{
			pointer<U> begin;
			size_t size;

			inline static subspan make(size_t begin, size_t size)
			{
				return subspan{.begin = pointer<U>::make(begin), .size = size};
			}
		};

		template<lak::concepts::one_of<T...>... U>
		struct limited_dyn_pointer
		{
			using type_index_type = lak::index_set<index_of<U>...>;
			type_index_type type_index;
			size_t index;

			limited_dyn_pointer()                                       = default;
			limited_dyn_pointer(const limited_dyn_pointer &)            = default;
			limited_dyn_pointer &operator=(const limited_dyn_pointer &) = default;

			template<lak::concepts::one_of<U...> V>
			limited_dyn_pointer(const pointer<V> &other)
			: type_index(lak::size_type<index_of<V>>{}), index(other.index)
			{
			}

			template<lak::concepts::one_of<U...> V>
			limited_dyn_pointer &operator=(const pointer<V> &other)
			{
				type_index = lak::size_type<index_of<V>>{};
				index      = other.index;
				return *this;
			}

			template<lak::concepts::one_of<U...> V>
			inline static limited_dyn_pointer make(size_t index)
			{
				return limited_dyn_pointer(pointer<V>::make(index));
			}
		};

		template<lak::concepts::one_of<T...>... U>
		struct limited_dyn_subspan
		{
			limited_dyn_pointer<U...> begin;
			size_t size;

			limited_dyn_subspan()                                       = default;
			limited_dyn_subspan(const limited_dyn_subspan &)            = default;
			limited_dyn_subspan &operator=(const limited_dyn_subspan &) = default;

			template<lak::concepts::one_of<U...> V>
			limited_dyn_subspan(const subspan<V> &other)
			: begin(other.begin), size(other.size)
			{
			}

			template<lak::concepts::one_of<U...> V>
			limited_dyn_subspan &operator=(const subspan<V> &other)
			{
				begin = other.begin;
				size  = other.size;
				return *this;
			}

			template<lak::concepts::one_of<U...> V>
			inline static limited_dyn_subspan make(size_t begin, size_t size)
			{
				return limited_dyn_subspan{
				  .begin = limited_dyn_pointer::template make<V>(begin),
				  .size  = size,
				};
			}
		};

		struct dyn_pointer
		{
			using type_index_type = lak::index_set_for<T...>;
			type_index_type type_index;
			size_t index;

			dyn_pointer()                               = default;
			dyn_pointer(const dyn_pointer &)            = default;
			dyn_pointer &operator=(const dyn_pointer &) = default;

			template<lak::concepts::one_of<T...> U>
			dyn_pointer(const pointer<U> &other)
			: type_index(lak::size_type<index_of<U>>{}), index(other.index)
			{
			}

			template<lak::concepts::one_of<T...>... U>
			dyn_pointer(const limited_dyn_pointer<U...> &other)
			: type_index(other.type_index), index(other.index)
			{
			}

			template<lak::concepts::one_of<T...> U>
			dyn_pointer &operator=(const pointer<U> &other)
			{
				type_index = lak::size_type<index_of<U>>{};
				index      = other.index;
				return *this;
			}

			template<lak::concepts::one_of<T...>... U>
			dyn_pointer &operator=(const limited_dyn_pointer<U...> &other)
			{
				type_index = other.type_index;
				index      = other.index;
				return *this;
			}

			template<lak::concepts::one_of<T...> U>
			inline static dyn_pointer make(size_t index)
			{
				return dyn_pointer{
				  .type_index = lak::size_type<index_of<U>>{},
				  .index      = index,
				};
			}
		};

		struct dyn_subspan
		{
			dyn_pointer begin;
			size_t size;

			dyn_subspan()                               = default;
			dyn_subspan(const dyn_subspan &)            = default;
			dyn_subspan &operator=(const dyn_subspan &) = default;

			template<lak::concepts::one_of<T...> U>
			dyn_subspan(const subspan<U> &other)
			: begin(other.begin), size(other.size)
			{
			}

			template<lak::concepts::one_of<T...>... U>
			dyn_subspan(const limited_dyn_subspan<U...> &other)
			: begin(other.begin), size(other.size)
			{
			}

			template<lak::concepts::one_of<T...> U>
			dyn_subspan &operator=(const subspan<U> &other)
			{
				begin = other.begin;
				size  = other.size;
				return *this;
			}

			template<lak::concepts::one_of<T...>... U>
			dyn_subspan &operator=(const limited_dyn_subspan<U...> &other)
			{
				begin = other.begin;
				size  = other.size;
				return *this;
			}

			template<lak::concepts::one_of<T...> U>
			inline static dyn_subspan make(size_t begin, size_t size)
			{
				return dyn_subspan{
				  .begin = dyn_pointer::template make<U>(begin),
				  .size  = size,
				};
			}
		};

		lak::tuple<lak::array<T>...> arrays;

		template<size_t I>
		requires((I < sizeof...(T)))
		lak::array<lak::nth_type_t<I, T...>> &get()
		{
			return arrays.template get<I>();
		}

		template<size_t I>
		requires((I < sizeof...(T)))
		const lak::array<lak::nth_type_t<I, T...>> &get() const
		{
			return arrays.template get<I>();
		}

		template<lak::concepts::one_of<T...> U>
		lak::array<U> &get()
		{
			return arrays.template get<index_of<U>>();
		}

		template<lak::concepts::one_of<T...> U>
		const lak::array<U> &get() const
		{
			return arrays.template get<index_of<U>>();
		}

		template<lak::concepts::one_of<T...> U>
		U &operator[](const pointer<U> &ptr)
		{
			return arrays.template get<index_of<U>>()[ptr.index];
		}

		template<lak::concepts::one_of<T...> U>
		const U &operator[](const pointer<U> &ptr) const
		{
			return arrays.template get<index_of<U>>()[ptr.index];
		}

		template<lak::concepts::one_of<T...> U>
		lak::span<U> operator[](const subspan<U> &sub)
		{
			return lak::span(arrays.template get<index_of<U>>())
			  .subspan(sub.begin.index, sub.size);
		}

		template<lak::concepts::one_of<T...> U>
		lak::span<const U> operator[](const subspan<U> &sub) const
		{
			return lak::span(arrays.template get<index_of<U>>())
			  .subspan(sub.begin.index, sub.size);
		}

		inline lak::variant<lak::add_lvalue_reference_t<T>...> operator[](
		  const dyn_pointer &ptr)
		{
			return lak::visit_switch(
			  ptr.type_index,
			  [&]<size_t I>(lak::size_type<I>)
			  {
				  return lak::variant<lak::add_lvalue_reference_t<T>...>(
				    lak::in_place_index_t<I>{}, arrays.template get<I>()[ptr.index]);
			  });
		}

		inline lak::variant<lak::add_lvalue_reference_t<const T>...> operator[](
		  const dyn_pointer &ptr) const
		{
			return lak::visit_switch(
			  ptr.type_index,
			  [&]<size_t I>(lak::size_type<I>)
			  {
				  return lak::variant<lak::add_lvalue_reference_t<T>...>(
				    lak::in_place_index_t<I>{}, arrays.template get<I>()[ptr.index]);
			  });
		}

		inline lak::variant<lak::span<T>...> operator[](const dyn_subspan &sub)
		{
			return lak::visit_switch(sub.type_index,
			                         [&]<size_t I>(lak::size_type<I>)
			                         {
				                         return lak::variant<lak::span<T>...>(
				                           lak::in_place_index_t<I>{},
				                           lak::span(arrays.template get<I>())
				                             .subspan(sub.begin.index, sub.size));
			                         });
		}

		inline lak::variant<lak::span<const T>...> operator[](
		  const dyn_subspan &sub) const
		{
			return lak::visit_switch(sub.type_index,
			                         [&]<size_t I>(lak::size_type<I>)
			                         {
				                         return lak::variant<lak::span<T>...>(
				                           lak::in_place_index_t<I>{},
				                           lak::span(arrays.template get<I>())
				                             .subspan(sub.begin.index, sub.size));
			                         });
		}

		template<lak::concepts::one_of<T...>... U>
		lak::variant<lak::add_lvalue_reference_t<U>...> operator[](
		  const limited_dyn_pointer<U...> &ptr)
		{
			return lak::visit_switch(
			  ptr.type_index,
			  [&]<size_t I>(lak::size_type<I>)
			  {
				  return lak::variant<lak::add_lvalue_reference_t<U>...>(
				    lak::in_place_index_t<I>{}, arrays.template get<I>()[ptr.index]);
			  });
		}

		template<lak::concepts::one_of<T...>... U>
		lak::variant<lak::add_lvalue_reference_t<const U>...> operator[](
		  const limited_dyn_pointer<U...> &ptr) const
		{
			return lak::visit_switch(
			  ptr.type_index,
			  [&]<size_t I>(lak::size_type<I>)
			  {
				  return lak::variant<lak::add_lvalue_reference_t<const U>...>(
				    lak::in_place_index_t<I>{}, arrays.template get<I>()[ptr.index]);
			  });
		}

		template<lak::concepts::one_of<T...>... U>
		lak::variant<lak::span<U>...> operator[](
		  const limited_dyn_subspan<U...> &sub)
		{
			return lak::visit_switch(sub.type_index,
			                         [&]<size_t I>(lak::size_type<I>)
			                         {
				                         return lak::variant<lak::span<U>...>(
				                           lak::in_place_index_t<I>{},
				                           lak::span(arrays.template get<I>())
				                             .subspan(sub.begin.index, sub.size));
			                         });
		}

		template<lak::concepts::one_of<T...>... U>
		lak::variant<lak::span<const U>...> operator[](
		  const limited_dyn_subspan<U...> &sub) const
		{
			return lak::visit_switch(sub.type_index,
			                         [&]<size_t I>(lak::size_type<I>)
			                         {
				                         return lak::variant<lak::span<U>...>(
				                           lak::in_place_index_t<I>{},
				                           lak::span(arrays.template get<I>())
				                             .subspan(sub.begin.index, sub.size));
			                         });
		}
	};
}

#endif
