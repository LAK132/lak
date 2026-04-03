#ifndef LAK_UNINITIALISED_HPP
#define LAK_UNINITIALISED_HPP

#include "lak/utility.hpp"

namespace lak
{
	/* --- uninitialised --- */

	template<typename T>
	struct uninitialised
	{
		using value_type = T;
		union
		{
			lak::monostate _uninitialised_value;
			value_type _value;
		};

		uninitialised() : _uninitialised_value()
		{
			_uninitialised_value.~monostate();
		}

		template<typename... ARGS>
		requires requires { value_type(lak::declval<ARGS>()...); }
		uninitialised(lak::in_place_t, ARGS &&...args)
		: _value(lak::forward<ARGS>(args)...)
		{
		}

		~uninitialised() {}

		template<typename... ARGS>
		requires requires { value_type(lak::declval<ARGS>()...); }
		value_type &create(ARGS &&...args)
		{
			new (&_value) value_type(lak::forward<ARGS>(args)...);
			return _value;
		}

		void destroy() { _value.~value_type(); }

		value_type &value() { return _value; }

		const value_type &value() const { return _value; }
	};

	static_assert(lak::is_default_constructible_v<lak::uninitialised<int>>);
}

#define LAK_DEBUG_FORWARD_ONLY
#include "lak/debug.hpp"

namespace lak
{
	template<typename T>
	constexpr void destructive_move_construct(lak::uninitialised<T> *,
	                                          lak::uninitialised<T> *)
	{
		// :TODO: make array handle uninitialised better so this hack isn't
		// necessary.
		ABORTF_S(u8"Attempted to move an unmovable uninitialised<T>");
	}
}

#endif
