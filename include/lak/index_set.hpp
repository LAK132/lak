#ifndef LAK_INDEX_SET_HPP
#define LAK_INDEX_SET_HPP

#define LAK_RESULT_FORWARD_ONLY
#include "lak/result.hpp"

#include "lak/type_traits.hpp"

namespace lak
{
	template<size_t... I>
	struct index_set
	{
	private:
		size_t _value;
		index_set(size_t index) : _value(index) {}

	public:
		index_set()                             = delete;
		index_set(const index_set &)            = default;
		index_set &operator=(const index_set &) = default;

		template<size_t INDEX>
		constexpr index_set(lak::size_type<INDEX>);

		template<size_t INDEX>
		constexpr index_set &operator=(lak::size_type<INDEX>);

		constexpr static lak::result<index_set> make(size_t index);

		size_t value() const { return _value; }
		operator size_t() const { return _value; }
	};

	static_assert(std::is_standard_layout_v<lak::index_set<0, 1, 2>>);

	template<typename T>
	struct index_set_from_sequence;
	template<size_t... I>
	struct index_set_from_sequence<lak::index_sequence<I...>>
	: lak::type_identity<lak::index_set<I...>>
	{
	};

	template<typename... T>
	using index_set_for =
	  typename index_set_from_sequence<lak::index_sequence_for<T...>>::type;

	template<size_t... I>
	lak::result<lak::index_set<I...>> make_index_set(lak::index_sequence<I...>,
	                                                 size_t index);

	template<size_t... I, size_t INDEX>
	lak::index_set<I...> make_index_set(lak::index_sequence<I...>,
	                                    lak::size_type<INDEX> index);
}

#include "index_set.inl"

#endif
