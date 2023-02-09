#include "index_set.hpp"

#include "lak/result.hpp"

template<size_t... I>
template<size_t INDEX>
constexpr lak::index_set<I...>::index_set(lak::size_type<INDEX>)
: _value(INDEX)
{
	static_assert(((INDEX == I) || ...));
}

template<size_t... I>
template<size_t INDEX>
constexpr lak::index_set<I...> &lak::index_set<I...>::operator=(
  lak::size_type<INDEX>)
{
	static_assert(((INDEX == I) || ...));
	_value = INDEX;
	return *this;
}

template<size_t... I>
constexpr lak::result<lak::index_set<I...>> lak::index_set<I...>::make(
  size_t index)
{
	if (((index == I) || ...))
		return lak::ok_t{index_set(index)};
	else
		return lak::err_t{};
}

template<size_t... I>
lak::result<lak::index_set<I...>> lak::make_index_set(
  lak::index_sequence<I...>, size_t index)
{
	return lak::index_set<I...>::make(index);
}

template<size_t... I, size_t INDEX>
lak::index_set<I...> lak::make_index_set(lak::index_sequence<I...>,
                                         lak::size_type<INDEX> index)
{
	return lak::index_set<I...>(index);
}
