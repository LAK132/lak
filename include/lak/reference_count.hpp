#ifndef LAK_REFERENCE_COUNT_HPP
#define LAK_REFERENCE_COUNT_HPP

#include <atomic>

namespace lak
{
	struct reference_count
	{
	private:
		std::atomic_uintmax_t value = 0U;

	public:
		reference_count() = default;
		reference_count(uintmax_t v) : value(v) {}

		inline uintmax_t operator++()
		{
			for (uintmax_t s = value.load();;)
				if (value.compare_exchange_weak(s, s + 1U)) return s + 1;
		}

		inline uintmax_t operator++(int)
		{
			for (uintmax_t s = value.load();;)
				if (value.compare_exchange_weak(s, s + 1U)) return s;
		}

		inline uintmax_t non_zero_inc()
		{
			for (uintmax_t s = value.load(); s != 0U;)
				if (value.compare_exchange_weak(s, s + 1U)) return s;
			return 0U;
		}

		inline uintmax_t operator--()
		{
			for (uintmax_t s = value.load();;)
				if (value.compare_exchange_weak(s, s - 1U)) return s - 1;
		}

		inline uintmax_t operator--(int)
		{
			for (uintmax_t s = value.load();;)
				if (value.compare_exchange_weak(s, s - 1U)) return s;
		}

		inline uintmax_t load() { return value.load(); }
	};
}

#endif
