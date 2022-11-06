#ifndef LAK_AWAIT_HPP
#define LAK_AWAIT_HPP

#include "lak/debug.hpp"
#include "lak/result.hpp"

#include <atomic>
#include <ostream>
#include <thread>

namespace lak
{
	enum struct await_error
	{
		running = 0,
		failed  = 1
	};

	template<typename T>
	using await_result = lak::result<T, await_error>;

	template<typename T>
	struct await
	{
	private:
		std::thread _thread;
		std::atomic_bool _finished   = false;
		lak::await_result<T> _result = lak::err_t{await_error::failed};

	public:
		await() = default;

		await(const await &) = delete;
		await(await &&)      = delete;

		await &operator=(const await &) = delete;
		await &operator=(await &&) = delete;

		template<typename FUNCTOR, typename... ARGS>
		lak::await_result<T> operator()(FUNCTOR &&functor, ARGS &&...args)
		{
			if (!_thread.joinable())
			{
				_finished = false;
				_thread   = std::thread(
          [](auto functor,
             std::atomic_bool &finished,
             lak::await_result<T> &result,
             auto... arg)
          {
            try
            {
              result = lak::ok_t{functor(arg...)};
            }
            catch (const std::exception &e)
            {
              ERROR("Uncaught Exception: ", e.what());
              result = lak::err_t{await_error::failed};
            }
            catch (...)
            {
              ERROR("Uncaught Exception");
              result = lak::err_t{await_error::failed};
            }
            finished = true;
          },
          functor,
          std::ref(_finished),
          std::ref(_result),
          lak::forward<ARGS>(args)...);
			}
			if (_finished)
			{
				_thread.join();
				_finished = false;
				if (_result.is_err())
					ASSERT_EQUAL(_result.unwrap_err(), await_error::failed);
				return _result;
			}
			else
			{
				return lak::err_t{await_error::running};
			}
		}
	};
}

inline std::ostream &operator<<(std::ostream &strm,
                                const lak::await_error &err)
{
	switch (err)
	{
		case lak::await_error::running: strm << "await running"; break;
		case lak::await_error::failed: strm << "await failed"; break;
		default: ASSERT_NYI(); break;
	}
	return strm;
}

#endif
