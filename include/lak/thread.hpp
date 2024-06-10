#ifndef LAK_THREAD_HPP
#define LAK_THREAD_HPP

#include "lak/optional.hpp"

#include <thread>

namespace lak
{
	struct thread
	{
	private:
		lak::optional<std::thread> _thread;

	public:
		thread()                          = default;
		thread(thread &&)                 = default;
		thread(const thread &)            = delete;
		thread &operator=(thread &&)      = default;
		thread &operator=(const thread &) = delete;

		template<typename F, typename... ARGS>
		explicit thread(F &&f, ARGS &&...args)
		: _thread(std::thread(lak::forward<F>(f), lak::forward<ARGS>(args)...))
		{
		}

		~thread()
		{
			if (_thread && _thread->joinable()) _thread->join();
		}

		template<typename F, typename... ARGS>
		lak::optional<std::thread> reset(F &&f, ARGS &&...args)
		{
			return lak::exchange(
			  _thread, std::thread(lak::forward<F>(f), lak::forward<ARGS>(args)...));
		}

		bool running() const { return _thread.has_value(); }

		void join()
		{
			_thread->join();
			_thread.reset();
		}

		const std::thread *operator->() const { return _thread.get(); }
	};
}

#endif
