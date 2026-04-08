#ifndef LAK_GENERATOR_HPP
#define LAK_GENERATOR_HPP

#include "lak/optional.hpp"
#include "lak/tuple.hpp"
#include "lak/type_traits.hpp"

namespace lak
{
	template<typename T, typename FUNC>
	struct generator
	{
		lak::optional<FUNC> func;
		mutable lak::optional<T> value;

		using value_type      = T;
		using difference_type = std::ptrdiff_t;

		void pump() const
		{
			if (func) value = (*func)();
		}

		value_type operator*() const
		{
			if (!value) pump();
			return *value;
		}

		generator &operator++()
		{
			pump();
			return *this;
		}

		void operator++(int) { pump(); }

		bool operator==(const generator &other) const
		{
			return !value.has_value() && !other.value.has_value();
		}

		bool operator!=(const generator &other) const
		{
			return value.has_value() || other.value.has_value();
		}
	};

	template<typename FUNC>
	auto make_generator(FUNC &&func)
	{
		using gen = lak::generator<typename decltype(func())::value_type, FUNC>;
		lak::pair<gen, gen> result;
		result.first.func = lak::forward<FUNC>(func);
		result.first.pump();
		return result;
	}
}

#endif
