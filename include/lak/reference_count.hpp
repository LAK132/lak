#ifndef LAK_REFERENCE_COUNT_HPP
#define LAK_REFERENCE_COUNT_HPP

#include <atomic>

namespace lak
{
	template<typename T>
	struct reference_count
	{
	private:
		std::atomic<T> value = 0U;

	public:
		reference_count() = default;
		reference_count(T v) : value(v) {}

		inline T operator++()
		{
			for (T s = value.load();;)
				if (value.compare_exchange_weak(s, s + 1U)) return s + 1;
		}

		inline T operator++(int)
		{
			for (T s = value.load();;)
				if (value.compare_exchange_weak(s, s + 1U)) return s;
		}

		inline T non_zero_inc()
		{
			for (T s = value.load(); s != 0U;)
				if (value.compare_exchange_weak(s, s + 1U)) return s;
			return 0U;
		}

		inline T operator--()
		{
			for (T s = value.load();;)
				if (value.compare_exchange_weak(s, s - 1U)) return s - 1;
		}

		inline T operator--(int)
		{
			for (T s = value.load();;)
				if (value.compare_exchange_weak(s, s - 1U)) return s;
		}

		inline T load() { return value.load(); }
	};
}

#endif
