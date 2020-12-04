#ifndef LAK_VARIANT_HPP
#define LAK_VARIANT_HPP

#include "lak/type_pack.hpp"
#include "lak/type_traits.hpp"
#include "lak/type_utils.hpp"
#include "lak/utility.hpp"
#include "lak/visit.hpp"

namespace lak
{
  struct uninitialised_union_flag_t
  {
  };
  static constexpr lak::uninitialised_union_flag_t uninitialised_union_flag;

  struct monostate
  {
  };
  static_assert(lak::is_default_constructible_v<lak::monostate>);

  template<typename... T>
  struct pack_union;

  template<typename T, typename... U>
  struct pack_union<T, U...>
  {
    static_assert(lak::is_same_v<lak::remove_reference_t<T>, T>);

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
    force_inline pack_union(lak::in_place_index_t<0>, ARGS &&... args)
    : value(lak::forward<ARGS>(args)...)
    {
    }

    template<size_t I, typename... ARGS, lak::enable_if_i<(I < size)> = 0>
    force_inline pack_union(lak::in_place_index_t<I>, ARGS &&... args)
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
    force_inline auto &get()
    {
      if constexpr (I == 0)
        return value;
      else
        return next.get<I - 1>();
    }

    template<size_t I>
    force_inline const auto &get() const
    {
      if constexpr (I == 0)
        return value;
      else
        return next.get<I - 1>();
    }

    template<size_t I, typename... ARGS>
    force_inline void emplace(ARGS &&... args)
    {
      if constexpr (I == 0)
        new (&value) T(lak::forward<ARGS>(args)...);
      else
        next.emplace<I - 1>(lak::forward<ARGS>(args)...);
    }

    template<typename... ARGS>
    force_inline bool emplace_dynamic(size_t i, ARGS &&... args)
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

    template<size_t I>
    force_inline void reset()
    {
      if constexpr (I == 0)
        value.~T();
      else
        next.reset<I - 1>();
    }

    force_inline bool reset_dynamic(size_t i)
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
      lak::uninitialised_union_flag_t _;
    };

    template<typename V                                           = T,
             lak::enable_if_i<lak::is_default_constructible_v<V>> = 0>
    pack_union() : value()
    {
    }

    template<typename... ARGS>
    pack_union(lak::in_place_index_t<0>, ARGS &&... args)
    : value(lak::forward<ARGS>(args)...)
    {
    }

    pack_union(lak::uninitialised_union_flag_t) : _()
    {
      _.~uninitialised_union_flag_t();
    }

    // union members must be manually destroyed by the user
    ~pack_union() {}

    template<size_t I>
    auto &get()
    {
      static_assert(I == 0);
      return value;
    }

    template<size_t I>
    const auto &get() const
    {
      static_assert(I == 0);
      return value;
    }

    template<size_t I, typename... ARGS>
    force_inline void emplace(ARGS &&... args)
    {
      static_assert(I == 0);
      new (&value) T(lak::forward<ARGS>(args)...);
    }

    template<typename... ARGS>
    force_inline bool emplace_dynamic(size_t i, ARGS &&... args)
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

    template<size_t I>
    force_inline void reset()
    {
      static_assert(I == 0);
      value.~T();
    }

    force_inline bool reset_dynamic(size_t i)
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
  };

  static_assert(lak::is_same_v<lak::pack_union<char>::value_type<0>, char>);

  template<typename... T>
  struct variant
  {
    static_assert(((!lak::is_rvalue_reference_v<T>)&&...));

    template<size_t I>
    using value_type = lak::tuple_element_t<I, lak::type_pack<T...>>;

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

    template<size_t I,
             typename... ARGS,
             lak::enable_if_i<(I < _size) && !_is_ref<I>> = 0>
    variant(lak::in_place_index_t<I>, ARGS &&... args)
    : _index(I), _value(lak::in_place_index<I>, lak::forward<ARGS>(args)...)
    {
    }

    template<size_t I,
             typename... ARGS,
             lak::enable_if_i<(I < _size) && _is_ref<I>> = 0>
    variant(lak::in_place_index_t<I>, value_type<I> ref)
    : _index(I), _value(lak::in_place_index<I>, &ref)
    {
    }

    variant(const variant &other)
    : _index(other._index), _value(lak::uninitialised_union_flag)
    {
      ASSERT(lak::visit_switch(
        lak::make_index_sequence<_size>{}, _index, [&, this](auto index) {
          using I = lak::remove_cvref_t<decltype(index)>;
          if constexpr (_is_ref<I::value>)
            _value.emplace<I::value>(other.get<I::value>());
          else
            _value.emplace<I::value>(*other.get<I::value>());
        }));
    }

    variant(variant &&other)
    : _index(other._index), _value(lak::uninitialised_union_flag)
    {
      _index = other._index;
      ASSERT(lak::visit_switch(
        lak::make_index_sequence<_size>{}, _index, [&, this](auto index) {
          using I = lak::remove_cvref_t<decltype(index)>;
          if constexpr (_is_ref<I::value>)
            _value.emplace<I::value>(other.get<I::value>());
          else
            _value.emplace<I::value>(
              lak::forward<value_type<I::value>>(*other.get<I::value>()));
        }));
    }

    variant &operator=(const variant &other)
    {
      ASSERT(lak::visit_switch(lak::make_index_sequence<_size>{},
                               other._index,
                               [&, this](auto index) {
                                 using I =
                                   lak::remove_cvref_t<decltype(index)>;
                                 emplace<I::value>(*other.get<I::value>());
                               }));
      return *this;
    }

    variant &operator=(variant &&other)
    {
      ASSERT(lak::visit_switch(
        lak::make_index_sequence<_size>{},
        other._index,
        [&, this](auto index) {
          using I = lak::remove_cvref_t<decltype(index)>;
          emplace<I::value>(
            lak::forward<value_type<I::value>>(*other.get<I::value>()));
        }));
      return *this;
    }

    template<size_t I, typename... ARGS>
    auto &emplace(ARGS &&... args)
    {
      ASSERT(_value.reset_dynamic(_index));
      _index = I;
      _value.emplace<I>(lak::forward<ARGS>(args)...);
      return _value.get<I>();
    }

    template<size_t I, typename... ARGS>
    static variant make(ARGS &&... args)
    {
      static_assert(I < _size);
      return variant(lak::in_place_index<I>, lak::forward<ARGS>(args)...);
    }

    ~variant()
    {
      ASSERT(lak::visit_switch(
        lak::make_index_sequence<_size>{}, _index, [this](auto index) {
          using I = lak::remove_cvref_t<decltype(index)>;
          _value.reset<I::value>();
        }));
    }

    size_t index() const { return _index; }

    constexpr size_t size() const { return _size; }

    template<size_t I>
    auto *get()
    {
      // we store references as pointers.
      if constexpr (_is_ref<I>)
        return I == _index ? _value.get<I>() : nullptr;
      else
        return I == _index ? &_value.get<I>() : nullptr;
    }

    template<size_t I>
    const auto *get() const
    {
      // we store references as pointers.
      if constexpr (_is_ref<I>)
        return I == _index ? _value.get<I>() : nullptr;
      else
        return I == _index ? &_value.get<I>() : nullptr;
    }
  };

  static_assert(lak::is_same_v<lak::variant<char>::value_type<0>, char>);
}

#endif