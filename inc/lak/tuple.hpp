#ifndef LAK_TUPLE_HPP
#define LAK_TUPLE_HPP

#ifndef LAK_NO_STD
#  include <tuple>
#endif

namespace lak
{
#ifdef LAK_NO_STD
#  define LAK_TUPLE_ELEMENT lak::tuple_element
#  define LAK_TUPLE_SIZE    lak::tuple_size
    template<size_t I, typename T>
    struct tuple_element;
    template<typename T>
    struct tuple_size;
#else
#  define LAK_TUPLE_ELEMENT std::tuple_element
#  define LAK_TUPLE_SIZE    std::tuple_size
    template<size_t I, typename T>
    struct tuple_element : std::tuple_element<I, T>
  {
  };
    template<typename T>
    struct tuple_size : std::tuple_size<T>
  {
  };
#endif

  template<size_t I, typename T>
  using tuple_element_t = typename lak::tuple_element<I, T>::type;

  template<typename T>
  constexpr inline size_t tuple_size_v = lak::tuple_size<T>::value;

  template<typename... T>
  struct tuple;

  /* --- pair --- */

  template<typename T, typename U>
  struct pair
  {
    T first;
    U second;

    pair() = default;
    pair(const pair &p) : first(p.first), second(p.second) {}
    pair(pair &&p) : first(lak::move(p.first)), second(lak::move(p.second)) {}

    template<typename V, typename W>
    pair(V &&f, W &&s) : first(lak::forward<V>(f)), second(lak::forward<W>(s))
    {
    }

    pair &operator=(const pair &p)
    {
      first  = p.first;
      second = p.second;
      return *this;
    }
    pair &operator=(pair &&p)
    {
      first  = lak::move(p.first);
      second = lak::move(p.second);
      return *this;
    }

    template<typename... V>
    pair &operator=(const tuple<V...> &p)
    {
      static_assert(sizeof...(V) == 2);
      first  = p.first;
      second = p.second;
      return *this;
    }
    template<typename... V>
    pair &operator=(tuple<V...> &&p)
    {
      static_assert(sizeof...(V) == 2);
      first  = lak::move(p.first);
      second = lak::move(p.second);
      return *this;
    }

    template<size_t I>
    auto &get()
    {
      static_assert(I < 2);
      if constexpr (I == 0)
        return first;
      else if constexpr (I == 1)
        return second;
    }
    template<size_t I>
    auto &get() const
    {
      static_assert(I < 2);
      if constexpr (I == 0)
        return first;
      else if constexpr (I == 1)
        return second;
    }
  };

  template<typename T, typename U>
  pair(const T &, const U &) -> pair<T, U>;

  template<typename T, typename U>
  struct lak::is_lak_type<lak::pair<T, U>> : lak::true_type
  {
  };

  template<typename T, typename U>
  struct LAK_TUPLE_SIZE<lak::pair<T, U>>
  {
    static constexpr size_t value = 2;
  };

  template<typename T, typename U>
  struct LAK_TUPLE_ELEMENT<0, lak::pair<T, U>>
  {
    using type = T;
  };

  template<typename T, typename U>
  struct LAK_TUPLE_ELEMENT<1, lak::pair<T, U>>
  {
    using type = T;
  };

  /* --- tuple --- */

  template<typename T, typename... U>
  struct tuple<T, U...>
  {
    T value;
    lak::tuple<U...> next;

    tuple() = default;
    tuple(const tuple &p) : value(p.value), next(p.next) {}
    tuple(tuple &&p) : value(lak::move(p.value)), next(lak::move(p.next)) {}

    template<typename V, typename... W>
    tuple(V &&v, W &&... n)
    : value(lak::forward<V>(v)), next(lak::forward<W>(n)...)
    {
    }

    tuple &operator=(const tuple &p)
    {
      value = p.value;
      next  = p.next;
      return *this;
    }
    tuple &operator=(tuple &&p)
    {
      value = lak::move(p.value);
      next  = lak::move(p.next);
      return *this;
    }

    template<typename... V>
    tuple &operator=(const pair<V...> &p)
    {
      static_assert(sizeof...(U) == 1);
      value      = p.first;
      next.value = p.second;
      return *this;
    }

    template<typename... V>
    tuple &operator=(pair<V...> &&p)
    {
      static_assert(sizeof...(U) == 1);
      value      = lak::move(p.first);
      next.value = lak::move(p.second);
      return *this;
    }

    template<size_t I>
    auto &get()
    {
      static_assert(I <= sizeof...(U));
      if constexpr (I == 0)
        return value;
      else
        return next.get<I - 1>();
    }
    template<size_t I>
    auto &get() const
    {
      static_assert(I <= sizeof...(U));
      if constexpr (I == 0)
        return value;
      else
        return next.get<I - 1>();
    }
  };

  template<>
  struct tuple<>
  {
  };

  template<typename... T>
  tuple(const T &...) -> tuple<T...>;

  template<typename... T>
  struct lak::is_lak_type<lak::tuple<T...>> : lak::true_type
  {
  };

  template<typename... T>
  struct LAK_TUPLE_SIZE<lak::tuple<T...>>
  {
    static constexpr size_t value = sizeof(T...);
  };

  template<typename T, typename... U>
  struct LAK_TUPLE_ELEMENT<0, lak::tuple<T, U...>>
  {
    using type = T;
  };

  template<size_t I, typename T, typename... U>
  struct LAK_TUPLE_ELEMENT<I, lak::tuple<T, U...>>
  {
    using type = typename LAK_TUPLE_ELEMENT<I - 1, lak::tuple<U...>>::type;
  };

  /* --- free functions --- */

  template<typename... T>
  lak::tuple<T &...> tie(T &... args)
  {
    return {args...};
  }
}

#endif