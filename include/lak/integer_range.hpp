#ifndef LAK_INTEGER_RANGE_HPP
#define LAK_INTEGER_RANGE_HPP

#include "lak/stdint.hpp"

namespace lak
{
	template<typename INTEGER>
	struct integer_range
	{
	private:
		INTEGER _value;
		INTEGER _end;

		integer_range(INTEGER value, INTEGER end) : _value(value), _end(end) {}

	public:
		integer_range()                                 = delete;
		integer_range(const integer_range &)            = default;
		integer_range &operator=(const integer_range &) = default;

		static integer_range from_count(INTEGER from, INTEGER count)
		{
			return integer_range{from, from + count};
		}

		static integer_range from_to(INTEGER from, INTEGER to)
		{
			return integer_range{from, to};
		}

		INTEGER operator*() const { return _value; }

		explicit operator INTEGER() const { return _value; }

		integer_range &begin() { return *this; }

		INTEGER end() const { return _end; }

		integer_range &operator++()
		{
			++_value;
			return *this;
		}

		integer_range operator++(int) { return integer_range{_value++, _end}; }

		bool operator==(INTEGER rhs) const { return _value == rhs; }

		bool operator!=(INTEGER rhs) const { return _value != rhs; }
	};

	using size_range = integer_range<size_t>;

	inline auto size_range_count(size_t count)
	{
		return lak::size_range::from_count(size_t(0), count);
	}

	template<typename INTEGER>
	inline auto integer_range_count(INTEGER count)
	{
		return lak::integer_range<INTEGER>::from_count(INTEGER(0), count);
	}

	template<typename T>
	inline auto pointer_range(T *begin, T *end)
	{
		return lak::integer_range<T *>::from_to(begin, end);
	}

	template<typename T>
	inline auto pointer_range(T *begin, size_t count)
	{
		return lak::integer_range<T *>::from_to(begin, begin + count);
	}
}

#endif
