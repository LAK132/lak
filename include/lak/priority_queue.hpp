#ifndef LAK_PRIORITY_QUEUE_HPP
#define LAK_PRIORITY_QUEUE_HPP

#include "lak/array.hpp"
#include "lak/concepts.hpp"
#include "lak/functional.hpp"
#include "lak/optional.hpp"

namespace lak
{
	template<typename T, typename CMP = lak::less<T>>
	struct priority_queue
	{
	private:
		lak::array<T> _data;

	public:
		using value_type = const T;
		using size_type  = size_t;

		inline const T *data() const { return _data.data(); }
		inline size_t size() const { return _data.size(); }
		inline lak::span<const T> sorted();

		bool empty() const { return _data.empty(); }

		void push(const T &value)
		requires std::copy_constructible<T>;

		void push(T &&value);

		void push(std::initializer_list<T> list)
		requires std::copy_constructible<T>;

		const T &top() const { return _data.front(); }

		T pop();

		// fast pops a low priority item off the queue, makes no guarantee about
		// how low
		T pop_low();

		template<lak::concepts::invocable_result_of<bool, const T &> F>
		lak::optional<T> pop_if_breadth_first(F &&predicate);

		template<lak::concepts::invocable_result_of<bool, const T &> F>
		lak::optional<T> pop_if_depth_first(F &&predicate);

		template<lak::concepts::
		           invocable_result_of<lak::optional<T>, const T &, const T &> F>
		void compact(F &&compactor);
	};
}

#include "lak/priority_queue.inl"

#endif
