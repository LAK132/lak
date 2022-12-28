#ifndef LAK_OPTIONAL_HPP
#define LAK_OPTIONAL_HPP

#include "lak/type_traits.hpp"
#include "lak/uninitialised.hpp"
#include "lak/utility.hpp"

#ifndef LAK_NO_STD
#	include <optional>
#endif

namespace lak
{
	/* --- nullopt --- */

	struct nullopt_t
	{
	};
	static constexpr lak::nullopt_t nullopt;

	/* --- some_t --- */

	template<typename T = lak::monostate>
	struct some_t
	{
		T value;
	};

	template<typename T>
	some_t(T &&) -> some_t<T &&>;

	template<typename T>
	some_t(T &) -> some_t<T &>;

	template<typename T>
	lak::some_t<T &&> move_some(T &v)
	{
		return lak::some_t<T &&>{lak::move(v)};
	}

	/* --- optional --- */

	template<typename T = void>
	struct optional
	{
		using value_type = T;

	private:
		bool _has_value = false;
		lak::uninitialised<value_type> _value;

	public:
		optional() = default;
		optional(const optional &other) : _has_value(other._has_value)
		{
			if (_has_value) _value.create(other._value.value());
		}
		optional(optional &&other) : _has_value(other._has_value)
		{
			if (_has_value) _value.create(lak::move(other._value.value()));
		}

		optional(lak::nullopt_t) {}

		template<typename U>
		requires(!lak::is_same_v<optional, lak::remove_cvref_t<U>> &&
		         requires { value_type(lak::declval<U>()); }) //
		  optional(U &&other)
		: _has_value(true), _value(lak::forward<U>(other))
		{
		}

		template<typename... ARGS>
		optional(lak::in_place_t, ARGS &&...args)
		: _has_value(true), _value(lak::forward<ARGS>(args)...)
		{
		}

		template<typename U>
		optional(lak::some_t<U> value) : optional(lak::forward<U>(value.value))
		{
		}

		~optional() { reset(); }

		optional &operator=(const optional &other)
		{
			if (other._has_value)
				emplace(other._value.value());
			else
				reset();
			return *this;
		}
		optional &operator=(optional &&other)
		{
			if (other._has_value)
				emplace(lak::move(other._value.value()));
			else
				reset();
			return *this;
		}

		optional &operator=(lak::nullopt_t)
		{
			reset();
			return *this;
		}
		template<typename U>
		optional &operator=(U &&other)
		{
			emplace(lak::forward<U>(other));
			return *this;
		}

		inline bool has_value() const { return _has_value; }

		inline operator bool() const { return _has_value; }

		template<typename U>
		value_type &emplace(U &&other)
		{
			reset();
			_value.create(lak::forward<U>(other));
			_has_value = true;
			return _value.value();
		}

		void reset()
		{
			if (_has_value)
			{
				_value.destroy();
				_has_value = false;
			}
		}

		T *get() { return _has_value ? &_value.value() : nullptr; }
		const T *get() const { return _has_value ? &_value.value() : nullptr; }

		T &operator*() { return *get(); }
		const T &operator*() const { return *get(); }

		T *operator->() { return get(); }
		const T *operator->() const { return get(); }

		inline friend optional operator|(optional a, optional b)
		{
			return a ? a : b;
		}
	};

	/* --- optional<void> --- */

	template<>
	struct optional<void>
	{
		using value_type = void;

	private:
		bool _value = false;

	public:
		optional()                 = default;
		optional(const optional &) = default;
		optional(optional &&)      = default;

		optional(lak::nullopt_t) : _value(false) {}
		optional(lak::in_place_t) : _value(true) {}
		template<bool B>
		optional(lak::bool_type<B>) : _value(B)
		{
		}

		optional &operator=(const optional &) = default;
		optional &operator=(optional &&) = default;

		optional &operator=(lak::nullopt_t)
		{
			_value = false;
			return *this;
		}
		template<bool B>
		optional &operator=(lak::bool_type<B>)
		{
			_value = B;
			return *this;
		}

		inline bool has_value() const { return _value; }

		inline operator bool() const { return _value; }

		void emplace() { _value = true; }

		void reset() { _value = false; }

		bool get() const { return _value; }

		void operator*() const {}
	};

	/* --- optional<T&> --- */

	template<typename T>
	struct optional<T &>
	{
		using value_type = T;

	private:
		T *_value;

	public:
		optional() : _value(nullptr) {}
		optional(const optional &) = default;
		optional(optional &&)      = default;

		optional(lak::nullopt_t) : _value(nullptr) {}
		optional(T &other) : _value(&other) {}
		template<typename ARGS>
		optional(lak::in_place_t, T &other) : _value(other)
		{
		}

		optional &operator=(const optional &) = default;
		optional &operator=(optional &&) = default;

		optional &operator=(lak::nullopt_t)
		{
			reset();
			return *this;
		}
		optional &operator=(T &other)
		{
			emplace(other);
			return *this;
		}

		inline bool has_value() const { return _value; }

		inline operator bool() const { return _value; }

		value_type &emplace(T &other)
		{
			_value = &other;
			return *_value;
		}

		void reset() { _value = nullptr; }

		T *get() { return _value; }
		const T *get() const { return _value; }

		T &operator*() { return *get(); }
		const T &operator*() const { return *get(); }

		T *operator->() { return get(); }
		const T *operator->() const { return get(); }
	};

	/* --- as_ptr --- */

	template<typename T>
	force_inline lak::remove_reference_t<T> *as_ptr(lak::optional<T> &p)
	{
		return lak::as_ptr(p.operator->());
	}

	template<typename T>
	force_inline const lak::remove_reference_t<T> *as_ptr(
	  const lak::optional<T> &p)
	{
		return lak::as_ptr(p.operator->());
	}

#ifndef LAK_NO_STD
	template<typename T>
	force_inline lak::remove_reference_t<T> *as_ptr(std::optional<T &> p)
	{
		return lak::as_ptr(p.operator->());
	}
#endif

	template<typename T>
	lak::optional<lak::remove_cvref_t<T>> make_optional(T &&t)
	{
		return {t};
	}
}

#endif
