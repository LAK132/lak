#include "lak/variant.hpp"

#include "lak/debug.hpp"

#include "lak/visit.hpp"

/* --- pack_union --- */

template<typename T, typename... U>
template<size_t I>
auto &lak::pack_union<T, U...>::get()
{
	if constexpr (I == 0)
		return value;
	else
		return next.template get<I - 1>();
}

template<typename T, typename... U>
template<size_t I>
const auto &lak::pack_union<T, U...>::get() const
{
	if constexpr (I == 0)
		return value;
	else
		return next.template get<I - 1>();
}

template<typename T, typename... U>
template<size_t I, typename... ARGS>
auto &lak::pack_union<T, U...>::emplace(ARGS &&...args)
{
	if constexpr (I == 0)
	{
		new (&value) T(lak::forward<ARGS>(args)...);
		return value;
	}
	else
		return next.template emplace<I - 1>(lak::forward<ARGS>(args)...);
}

template<typename T, typename... U>
template<typename... ARGS>
bool lak::pack_union<T, U...>::emplace_dynamic(size_t i, ARGS &&...args)
{
	if (i == 0)
	{
		new (&value) T(lak::forward<ARGS>(args)...);
		return true;
	}
	else
	{
		return next.emplace_dynamic(i, lak::forward<ARGS>(args)...);
	}
}

template<typename T, typename... U>
template<size_t I>
void lak::pack_union<T, U...>::reset()
{
	if constexpr (I == 0)
		value.~T();
	else
		next.template reset<I - 1>();
}

template<typename T, typename... U>
bool lak::pack_union<T, U...>::reset_dynamic(size_t i)
{
	if (i == 0)
	{
		value.~T();
		return true;
	}
	else
	{
		return next.reset_dynamic(i - 1);
	}
}

template<typename T>
lak::pack_union<T>::pack_union(lak::uninitialised_union_flag_t) : _()
{
	_.~monostate();
}

template<typename T>
template<size_t I>
auto &lak::pack_union<T>::get()
{
	static_assert(I == 0);
	return value;
}

template<typename T>
template<size_t I>
const auto &lak::pack_union<T>::get() const
{
	static_assert(I == 0);
	return value;
}

template<typename T>
template<size_t I, typename... ARGS>
auto &lak::pack_union<T>::emplace(ARGS &&...args)
{
	static_assert(I == 0);
	new (&value) T(lak::forward<ARGS>(args)...);
	return value;
}

template<typename T>
template<typename... ARGS>
bool lak::pack_union<T>::emplace_dynamic(size_t i, ARGS &&...args)
{
	if (i == 0)
	{
		new (&value) T(lak::forward<ARGS>(args)...);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename T>
template<size_t I>
void lak::pack_union<T>::reset()
{
	static_assert(I == 0);
	value.~T();
}

template<typename T>
bool lak::pack_union<T>::reset_dynamic(size_t i)
{
	if (i == 0)
	{
		value.~T();
		return true;
	}
	else
	{
		return false;
	}
}

/* --- variant --- */

template<typename... T>
lak::variant<T...>::variant(const variant &other)
: _index(other._index), _value(lak::uninitialised_union_flag)
{
	ASSERT(
	  lak::visit_switch(lak::make_index_sequence<_size>{},
	                    _index,
	                    [&, this]<size_t I>(lak::size_type<I>)
	                    {
		                    if constexpr (_is_ref<I>)
			                    _value.template emplace<I>(other.template get<I>());
		                    else
			                    _value.template emplace<I>(*other.template get<I>());
	                    }));
}

template<typename... T>
lak::variant<T...>::variant(variant &&other)
: _index(other._index), _value(lak::uninitialised_union_flag)
{
	_index = other._index;
	ASSERT(lak::visit_switch(
	  lak::make_index_sequence<_size>{},
	  _index,
	  [&, this]<size_t I>(lak::size_type<I>)
	  {
		  if constexpr (_is_ref<I>)
			  _value.template emplace<I>(other.template get<I>());
		  else
			  _value.template emplace<I>(
			    lak::forward<value_type<I>>(*other.template get<I>()));
	  }));
}

template<typename... T>
lak::variant<T...> &lak::variant<T...>::operator=(const variant &other)
{
	ASSERT(lak::visit_switch(lak::make_index_sequence<_size>{},
	                         other._index,
	                         [&, this]<size_t I>(lak::size_type<I>)
	                         { emplace<I>(*other.template get<I>()); }));
	return *this;
}

template<typename... T>
lak::variant<T...> &lak::variant<T...>::operator=(variant &&other)
{
	ASSERT(lak::visit_switch(
	  lak::make_index_sequence<_size>{},
	  other._index,
	  [&]<size_t I>(lak::size_type<I>)
	  { emplace<I>(lak::forward<value_type<I>>(*other.template get<I>())); }));
	return *this;
}

template<typename... T>
template<size_t I, typename... ARGS>
auto &lak::variant<T...>::emplace(ARGS &&...args)
{
	ASSERT(_value.reset_dynamic(_index));
	_index = I;
	_value.template emplace<I>(lak::forward<ARGS>(args)...);
	return _value.template get<I>();
}

template<typename... T>
template<size_t I, typename... ARGS>
lak::variant<T...> lak::variant<T...>::make(ARGS &&...args)
{
	static_assert(I < _size);
	return variant(lak::in_place_index<I>, lak::forward<ARGS>(args)...);
}

template<typename... T>
template<typename U, typename... ARGS>
lak::variant<T...> lak::variant<T...>::make(ARGS &&...args)
{
	static_assert(index_of<U> < _size);
	return variant(lak::in_place_index<index_of<U>>,
	               lak::forward<ARGS>(args)...);
}

template<typename... T>
lak::variant<T...>::variant::~variant()
{
	ASSERT(lak::visit_switch(lak::make_index_sequence<_size>{},
	                         _index,
	                         [this]<size_t I>(lak::size_type<I>)
	                         { _value.template reset<I>(); }));
}

template<typename... T>
template<size_t I>
auto *lak::variant<T...>::get()
{
	// we store references as pointers.
	if constexpr (_is_ref<I>)
		return I == _index ? _value.template get<I>() : nullptr;
	else
		return I == _index ? &_value.template get<I>() : nullptr;
}

template<typename... T>
template<size_t I>
const auto *lak::variant<T...>::get() const
{
	// we store references as pointers.
	if constexpr (_is_ref<I>)
		return I == _index ? _value.template get<I>() : nullptr;
	else
		return I == _index ? &_value.template get<I>() : nullptr;
}

template<typename... T>
template<typename F>
auto lak::variant<T...>::visit(F &&func)
{
	return lak::visit(*this, lak::forward<F>(func));
}

template<typename... T>
template<typename F>
auto lak::variant<T...>::visit(F &&func) const
{
	return lak::visit(*this, lak::forward<F>(func));
}

template<typename... T>
template<typename F>
auto lak::variant<T...>::flat_visit(F &&func)
{
	return visit(lak::forward<F>(func));
}

template<typename... T>
template<typename F>
auto lak::variant<T...>::flat_visit(F &&func) const
{
	return visit(lak::forward<F>(func));
}

/* --- variant<variant<T...>, U...> --- */

template<typename... T, typename... U>
requires((lak::is_standard_layout_v<lak::variant<T..., U...>>)) //
  lak::variant<lak::variant<T...>, U...>::variant(const variant &other)
: _value(lak::uninitialised_union_flag)
{
	ASSERT(lak::visit_switch(
	  lak::make_index_sequence<_size>{},
	  other.index(),
	  [&, this]<size_t I>(lak::size_type<I>)
	  {
		  if constexpr (I == 0U)
			  _value.template emplace<0U>(other.template unsafe_get<0U>());
		  else if constexpr (_is_ref<I>)
			  _value.template emplace<1U>(lak::in_place_index<I + _internal_offset>,
			                              *other.template unsafe_get<I>());
		  else
			  _value.template emplace<1U>(lak::in_place_index<I + _internal_offset>,
			                              other.template unsafe_get<I>());
	  }));
}

template<typename... T, typename... U>
requires((lak::is_standard_layout_v<lak::variant<T..., U...>>)) //
  lak::variant<lak::variant<T...>, U...>::variant(variant &&other)
: _value(lak::uninitialised_union_flag)
{
	ASSERT(lak::visit_switch(
	  lak::make_index_sequence<_size>{},
	  other.index(),
	  [&, this]<size_t I>(lak::size_type<I>)
	  {
		  if constexpr (I == 0U)
			  _value.template emplace<0U>(
			    lak::move(other.template unsafe_get<0U>()));
		  else if constexpr (_is_ref<I>)
			  _value.template emplace<1U>(lak::in_place_index<I + _internal_offset>,
			                              *other.template unsafe_get<I>());
		  else
			  _value.template emplace<1U>(lak::in_place_index<I + _internal_offset>,
			                              lak::move(other.template unsafe_get<I>()));
	  }));
}

template<typename... T, typename... U>
requires((lak::is_standard_layout_v<lak::variant<T..., U...>>)) //
  lak::variant<lak::variant<T...>, U...>
&lak::variant<lak::variant<T...>, U...>::operator=(const variant &other)
{
	ASSERT(lak::visit_switch(lak::make_index_sequence<_size>{},
	                         other.index(),
	                         [&, this]<size_t I>(lak::size_type<I>)
	                         {
		                         if constexpr (_is_ref<I>)
			                         emplace<I>(*other.template unsafe_get<I>());
		                         else
			                         emplace<I>(other.template unsafe_get<I>());
	                         }));
	return *this;
}

template<typename... T, typename... U>
requires((lak::is_standard_layout_v<lak::variant<T..., U...>>)) //
  lak::variant<lak::variant<T...>, U...>
&lak::variant<lak::variant<T...>, U...>::operator=(variant &&other)
{
	ASSERT(lak::visit_switch(lak::make_index_sequence<_size>{},
	                         other.index(),
	                         [&]<size_t I>(lak::size_type<I>)
	                         {
		                         if constexpr (_is_ref<I>)
			                         emplace<I>(*other.template unsafe_get<I>());
		                         else
			                         emplace<I>(
			                           lak::move(other.template unsafe_get<I>()));
	                         }));
	return *this;
}

template<typename... T, typename... U>
requires((lak::is_standard_layout_v<lak::variant<T..., U...>>)) //
  lak::variant<lak::variant<T...>, U...>::variant::~variant()
{
	if (internal_index() <= _internal_offset)
		_value.template reset<0U>();
	else
		_value.template reset<1U>();
}
