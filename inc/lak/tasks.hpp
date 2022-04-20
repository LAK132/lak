#ifndef LAK_TASKS_HPP
#define LAK_TASKS_HPP

#include "lak/array.hpp"

#include <mutex>
#include <thread>

namespace lak
{
	struct tasks
	{
	private:
		lak::array<std::thread> _threads;
		std::atomic_size_t _active = 0;
		std::mutex _mutex;

		// leaves _mutex locked
		std::mutex &await_not_full();

		void deactivate(size_t index);

		void deactivate(std::thread::id id);

	public:
		inline tasks() = default;
		inline tasks(size_t threads) { _threads.resize(threads); }

		inline static tasks hardware_max()
		{
			return tasks(std::thread::hardware_concurrency());
		}

		tasks(tasks &&other);

		tasks &operator=(tasks &&other);

		~tasks();

		template<typename FUNC>
		void push(FUNC &&func);
	};
}

#include "lak/tasks.inl"

#endif
