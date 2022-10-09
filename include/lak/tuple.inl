#include "lak/apply.hpp"

/* --- pair --- */

template<typename T, typename U>
lak::pair<T, U>::pair(const pair &p) : first(p.first), second(p.second)
{
}

template<typename T, typename U>
lak::pair<T, U>::pair(pair &&p)
: first(lak::move(p.first)), second(lak::move(p.second))
{
}

template<typename T, typename U>
template<typename V, typename W>
lak::pair<T, U>::pair(V &&f, W &&s)
: first(lak::forward<V>(f)), second(lak::forward<W>(s))
{
}

template<typename T, typename U>
lak::pair<T, U> &lak::pair<T, U>::operator=(const pair &p)
{
	first  = p.first;
	second = p.second;
	return *this;
}

template<typename T, typename U>
lak::pair<T, U> &lak::pair<T, U>::operator=(pair &&p)
{
	first  = lak::move(p.first);
	second = lak::move(p.second);
	return *this;
}

template<typename T, typename U>
template<typename... V>
lak::pair<T, U> &lak::pair<T, U>::operator=(const tuple<V...> &p)
{
	static_assert(sizeof...(V) == 2);
	first  = p.first;
	second = p.second;
	return *this;
}

template<typename T, typename U>
template<typename... V>
lak::pair<T, U> &lak::pair<T, U>::operator=(tuple<V...> &&p)
{
	static_assert(sizeof...(V) == 2);
	first  = lak::move(p.first);
	second = lak::move(p.second);
	return *this;
}

template<typename T, typename U>
template<size_t I>
auto &lak::pair<T, U>::get()
{
	static_assert(I < 2);
	if constexpr (I == 0)
		return first;
	else if constexpr (I == 1)
		return second;
}

template<typename T, typename U>
template<size_t I>
auto &lak::pair<T, U>::get() const
{
	static_assert(I < 2);
	if constexpr (I == 0)
		return first;
	else if constexpr (I == 1)
		return second;
}

/* --- tuple --- */

template<typename T, typename... U>
lak::tuple<T, U...>::tuple(const tuple &p) : value(p.value), next(p.next)
{
}

template<typename T, typename... U>
lak::tuple<T, U...>::tuple(tuple &&p)
: value(lak::move(p.value)), next(lak::move(p.next))
{
}

template<typename T, typename... U>
template<typename V, typename... W>
lak::tuple<T, U...>::tuple(V &&v, W &&...n)
: value(lak::forward<V>(v)), next(lak::forward<W>(n)...)
{
}

template<typename T, typename... U>
lak::tuple<T, U...> &lak::tuple<T, U...>::operator=(const tuple &p)
{
	value = p.value;
	next  = p.next;
	return *this;
}

template<typename T, typename... U>
lak::tuple<T, U...> &lak::tuple<T, U...>::operator=(tuple &&p)
{
	value = lak::move(p.value);
	next  = lak::move(p.next);
	return *this;
}

template<typename T, typename... U>
template<typename... V>
lak::tuple<T, U...> &lak::tuple<T, U...>::operator=(const pair<V...> &p)
{
	static_assert(sizeof...(U) == 1);
	value      = p.first;
	next.value = p.second;
	return *this;
}

template<typename T, typename... U>
template<typename... V>
lak::tuple<T, U...> &lak::tuple<T, U...>::operator=(pair<V...> &&p)
{
	static_assert(sizeof...(U) == 1);
	value      = lak::move(p.first);
	next.value = lak::move(p.second);
	return *this;
}

template<typename T, typename... U>
template<typename F>
auto lak::tuple<T, U...>::apply(F &&func)
{
	return lak::apply(lak::forward<F>(func), *this);
}

template<typename T, typename... U>
template<typename F>
auto lak::tuple<T, U...>::apply(F &&func) const
{
	return lak::apply(lak::forward<F>(func), *this);
}

template<typename T, typename... U>
template<typename F>
void lak::tuple<T, U...>::foreach(F &&func)
{
	func(value);
	next.foreach(func);
}

template<typename T, typename... U>
template<typename F>
void lak::tuple<T, U...>::foreach(F &&func) const
{
	func(value);
	next.foreach(func);
}

template<typename T, typename... U>
template<size_t I>
auto &lak::tuple<T, U...>::get()
{
	static_assert(I <= sizeof...(U));
	if constexpr (I == 0)
		return value;
	else
		return next.template get<I - 1>();
}

template<typename T, typename... U>
template<size_t I>
auto &lak::tuple<T, U...>::get() const
{
	static_assert(I <= sizeof...(U));
	if constexpr (I == 0)
		return value;
	else
		return next.template get<I - 1>();
}
