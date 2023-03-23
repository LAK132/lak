#include "lak/functional.hpp"

#include "lak/ptr_intrin.hpp"

/* --- equal_to<T, P...> --- */

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<size_t... Is>
constexpr force_inline bool lak::equal_to<T, P...>::operator()(
  const T &lhs, const T &rhs, lak::index_sequence<Is...>) const
{
	return ((lak::equal_to<>{}(lhs.*(lak::nth_value_v<Is, P...>),
	                           rhs.*(lak::nth_value_v<Is, P...>))) &&
	        ...);
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
constexpr inline bool lak::equal_to<T, P...>::operator()(const T &lhs,
                                                         const T &rhs) const
{
	return (*this)(lhs, rhs, lak::make_index_sequence<sizeof...(P)>{});
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::equal_to<T, P...>::operator()(const T &lhs,
                                                         const U &rhs) const
{
	return lak::equal_to<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::equal_to<T, P...>::operator()(const U &lhs,
                                                         const T &rhs) const
{
	return lak::equal_to<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
}

/* --- equal_to<void> --- */

template<typename T>
constexpr bool lak::equal_to<void>::operator()(const T &lhs,
                                               const T &rhs) const
{
	return lak::equal_to<T>{}(lhs, rhs);
}

template<class T, class U>
requires(!lak::is_same_v<T, U>)
constexpr auto lak::equal_to<void>::operator()(T &&lhs, U &&rhs) const
  -> decltype(lak::forward<T>(lhs) == lak::forward<U>(rhs))
{
	return lak::forward<T>(lhs) == lak::forward<U>(rhs);
}

/* --- equal_to<T> --- */

template<typename T>
constexpr inline bool lak::equal_to<T>::operator()(const T &lhs,
                                                   const T &rhs) const
{
	return lhs == rhs;
}

/* --- equal_to<T *> --- */

template<typename T>
constexpr inline bool lak::equal_to<T *>::operator()(const T *lhs,
                                                     const T *rhs) const
{
	return lak::ptr_compare(lhs, rhs) == lak::strong_ordering::equal;
}

/* --- not_equal_to<T, P...> --- */

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<size_t... Is>
constexpr force_inline bool lak::not_equal_to<T, P...>::operator()(
  const T &lhs, const T &rhs, lak::index_sequence<Is...>) const
{
	return ((lak::not_equal_to<>{}(lhs.*(lak::nth_value_v<Is, P...>),
	                               rhs.*(lak::nth_value_v<Is, P...>))) ||
	        ...);
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
constexpr inline bool lak::not_equal_to<T, P...>::operator()(
  const T &lhs, const T &rhs) const
{
	return (*this)(lhs, rhs, lak::make_index_sequence<sizeof...(P)>{});
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::not_equal_to<T, P...>::operator()(
  const T &lhs, const U &rhs) const
{
	return lak::not_equal_to<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::not_equal_to<T, P...>::operator()(
  const U &lhs, const T &rhs) const
{
	return lak::not_equal_to<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
}

/* --- not_equal_to<void> --- */

template<typename T>
constexpr bool lak::not_equal_to<void>::operator()(const T &lhs,
                                                   const T &rhs) const
{
	return lak::not_equal_to<T>{}(lhs, rhs);
}

template<class T, class U>
requires(!lak::is_same_v<T, U>)
constexpr auto lak::not_equal_to<void>::operator()(T &&lhs, U &&rhs) const
  -> decltype(lak::forward<T>(lhs) != lak::forward<U>(rhs))
{
	return lak::forward<T>(lhs) != lak::forward<U>(rhs);
}

/* --- not_equal_to<T> --- */

template<typename T>
constexpr inline bool lak::not_equal_to<T>::operator()(const T &lhs,
                                                       const T &rhs) const
{
	return lhs != rhs;
}

/* --- not_equal_to<T *> --- */

template<typename T>
constexpr inline bool lak::not_equal_to<T *>::operator()(const T *lhs,
                                                         const T *rhs) const
{
	return lak::ptr_compare(lhs, rhs) != lak::strong_ordering::equal;
}

/* --- less<T, P...> --- */

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<size_t I, size_t... Is>
constexpr force_inline bool lak::less<T, P...>::operator()(
  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const
{
	if constexpr (sizeof...(Is) == 0)
		return lak::less<>{}(lhs.*(lak::nth_value_v<I, P...>),
		                     rhs.*(lak::nth_value_v<I, P...>));
	else
		return lak::less<>{}(lhs.*(lak::nth_value_v<I, P...>),
		                     rhs.*(lak::nth_value_v<I, P...>)) ||
		       (!lak::less<>{}(rhs.*(lak::nth_value_v<I, P...>),
		                       lhs.*(lak::nth_value_v<I, P...>)) &&
		        operator()(lhs, rhs, lak::index_sequence<Is...>{}));
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
constexpr inline bool lak::less<T, P...>::operator()(const T &lhs,
                                                     const T &rhs) const
{
	return (*this)(lhs, rhs, lak::make_index_sequence<sizeof...(P)>{});
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::less<T, P...>::operator()(const T &lhs,
                                                     const U &rhs) const
{
	return lak::less<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::less<T, P...>::operator()(const U &lhs,
                                                     const T &rhs) const
{
	return lak::less<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
}

/* --- less<void> --- */

template<typename T>
constexpr bool lak::less<void>::operator()(const T &lhs, const T &rhs) const
{
	return lak::less<T>{}(lhs, rhs);
}

template<class T, class U>
requires(!lak::is_same_v<T, U>)
constexpr auto lak::less<void>::operator()(T &&lhs, U &&rhs) const
  -> decltype(lak::forward<T>(lhs) < lak::forward<U>(rhs))
{
	return lak::forward<T>(lhs) < lak::forward<U>(rhs);
}

/* --- less<T> --- */

template<typename T>
constexpr inline bool lak::less<T>::operator()(const T &lhs,
                                               const T &rhs) const
{
	return lhs < rhs;
}

/* --- less<T *> --- */

template<typename T>
constexpr inline bool lak::less<T *>::operator()(const T *lhs,
                                                 const T *rhs) const
{
	return lak::ptr_compare(lhs, rhs) == lak::strong_ordering::less;
}

/* --- greater<T, P...> --- */

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<size_t I, size_t... Is>
constexpr force_inline bool lak::greater<T, P...>::operator()(
  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const
{
	if constexpr (sizeof...(Is) == 0)
		return lak::greater<>{}(lhs.*(lak::nth_value_v<I, P...>),
		                        rhs.*(lak::nth_value_v<I, P...>));
	else
		return lak::greater<>{}(lhs.*(lak::nth_value_v<I, P...>),
		                        rhs.*(lak::nth_value_v<I, P...>)) ||
		       (!lak::greater<>{}(rhs.*(lak::nth_value_v<I, P...>),
		                          lhs.*(lak::nth_value_v<I, P...>)) &&
		        operator()(lhs, rhs, lak::index_sequence<Is...>{}));
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
constexpr inline bool lak::greater<T, P...>::operator()(const T &lhs,
                                                        const T &rhs) const
{
	return (*this)(lhs, rhs, lak::make_index_sequence<sizeof...(P)>{});
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::greater<T, P...>::operator()(const T &lhs,
                                                        const U &rhs) const
{
	return lak::greater<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::greater<T, P...>::operator()(const U &lhs,
                                                        const T &rhs) const
{
	return lak::greater<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
}

/* --- greater<void> --- */

template<typename T>
constexpr bool lak::greater<void>::operator()(const T &lhs, const T &rhs) const
{
	return lak::greater<T>{}(lhs, rhs);
}

template<class T, class U>
requires(!lak::is_same_v<T, U>)
constexpr auto lak::greater<void>::operator()(T &&lhs, U &&rhs) const
  -> decltype(lak::forward<T>(lhs) > lak::forward<U>(rhs))
{
	return lak::forward<T>(lhs) > lak::forward<U>(rhs);
}

/* --- greater<T> --- */

template<typename T>
constexpr inline bool lak::greater<T>::operator()(const T &lhs,
                                                  const T &rhs) const
{
	return lhs > rhs;
}

/* --- greater<T *> --- */

template<typename T>
constexpr inline bool lak::greater<T *>::operator()(const T *lhs,
                                                    const T *rhs) const
{
	return lak::ptr_compare(lhs, rhs) == lak::strong_ordering::greater;
}

/* --- less_equal<T, P...> --- */

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<size_t I, size_t... Is>
constexpr force_inline bool lak::less_equal<T, P...>::operator()(
  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const
{
	if constexpr (sizeof...(Is) == 0)
		return lak::less_equal<>{}(lhs.*(lak::nth_value_v<I, P...>),
		                           rhs.*(lak::nth_value_v<I, P...>));
	else
		return lak::less_equal<>{}(lhs.*(lak::nth_value_v<I, P...>),
		                           rhs.*(lak::nth_value_v<I, P...>)) &&
		       (!lak::less_equal<>{}(rhs.*(lak::nth_value_v<I, P...>),
		                             lhs.*(lak::nth_value_v<I, P...>)) ||
		        operator()(lhs, rhs, lak::index_sequence<Is...>{}));
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
constexpr inline bool lak::less_equal<T, P...>::operator()(const T &lhs,
                                                           const T &rhs) const
{
	return (*this)(lhs, rhs, lak::make_index_sequence<sizeof...(P)>{});
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::less_equal<T, P...>::operator()(const T &lhs,
                                                           const U &rhs) const
{
	return lak::less_equal<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::less_equal<T, P...>::operator()(const U &lhs,
                                                           const T &rhs) const
{
	return lak::less_equal<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
}

/* --- less_equal<void> --- */

template<typename T>
constexpr bool lak::less_equal<void>::operator()(const T &lhs,
                                                 const T &rhs) const
{
	return lak::less_equal<T>{}(lhs, rhs);
}

template<class T, class U>
requires(!lak::is_same_v<T, U>)
constexpr auto lak::less_equal<void>::operator()(T &&lhs, U &&rhs) const
  -> decltype(lak::forward<T>(lhs) <= lak::forward<U>(rhs))
{
	return lak::forward<T>(lhs) <= lak::forward<U>(rhs);
}

/* --- less_equal<T> --- */

template<typename T>
constexpr inline bool lak::less_equal<T>::operator()(const T &lhs,
                                                     const T &rhs) const
{
	return lhs <= rhs;
}

/* --- less_equal<T *> --- */

template<typename T>
constexpr inline bool lak::less_equal<T *>::operator()(const T *lhs,
                                                       const T *rhs) const
{
	return lak::ptr_compare(lhs, rhs) != lak::strong_ordering::greater;
}

/* --- greater_equal<T, P...> --- */

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<size_t I, size_t... Is>
constexpr force_inline bool lak::greater_equal<T, P...>::operator()(
  const T &lhs, const T &rhs, lak::index_sequence<I, Is...>) const
{
	if constexpr (sizeof...(Is) == 0)
		return lak::greater_equal<>{}(lhs.*(lak::nth_value_v<I, P...>),
		                              rhs.*(lak::nth_value_v<I, P...>));
	else
		return lak::greater_equal<>{}(lhs.*(lak::nth_value_v<I, P...>),
		                              rhs.*(lak::nth_value_v<I, P...>)) &&
		       (!lak::greater_equal<>{}(rhs.*(lak::nth_value_v<I, P...>),
		                                lhs.*(lak::nth_value_v<I, P...>)) ||
		        operator()(lhs, rhs, lak::index_sequence<Is...>{}));
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
constexpr inline bool lak::greater_equal<T, P...>::operator()(
  const T &lhs, const T &rhs) const
{
	return (*this)(lhs, rhs, lak::make_index_sequence<sizeof...(P)>{});
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::greater_equal<T, P...>::operator()(
  const T &lhs, const U &rhs) const
{
	return lak::greater_equal<U>{}(lhs.*(lak::nth_value_v<0, P...>), rhs);
}

#ifdef LAK_CAN_USE_CONCEPT_AUTO
template<typename T, lak::concepts::member_pointer auto... P>
#else
template<typename T, auto... P>
#endif
template<typename U>
requires(sizeof...(P) == 1)
constexpr inline bool lak::greater_equal<T, P...>::operator()(
  const U &lhs, const T &rhs) const
{
	return lak::greater_equal<U>{}(lhs, rhs.*(lak::nth_value_v<0, P...>));
}

/* --- greater_equal<void> --- */

template<typename T>
constexpr bool lak::greater_equal<void>::operator()(const T &lhs,
                                                    const T &rhs) const
{
	return lak::greater_equal<T>{}(lhs, rhs);
}

template<class T, class U>
requires(!lak::is_same_v<T, U>)
constexpr auto lak::greater_equal<void>::operator()(T &&lhs, U &&rhs) const
  -> decltype(lak::forward<T>(lhs) >= lak::forward<U>(rhs))
{
	return lak::forward<T>(lhs) >= lak::forward<U>(rhs);
}

/* --- greater_equal<T> --- */

template<typename T>
constexpr inline bool lak::greater_equal<T>::operator()(const T &lhs,
                                                        const T &rhs) const
{
	return lhs >= rhs;
}

/* --- greater_equal<T *> --- */

template<typename T>
constexpr inline bool lak::greater_equal<T *>::operator()(const T *lhs,
                                                          const T *rhs) const
{
	return lak::ptr_compare(lhs, rhs) != lak::strong_ordering::less;
}
