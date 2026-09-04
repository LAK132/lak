#ifndef LAK_AWAIT_HPP
#define LAK_AWAIT_HPP

#include "lak/debug.hpp"
#include "lak/format.hpp"
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

	template<typename CHAR>
	struct format_traits<lak::await_error, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(const lak::await_error &err)
		{
			switch (err)
			{
				case lak::await_error::running:
					return lak::strconv<CHAR>("await running"_view);
				case lak::await_error::failed:
					return lak::strconv<CHAR>("await failed"_view);
				default: BOUNDS_ASSERT_UNREACHABLE(return {});
			}
		}
	};

	template<typename T>
	using await_result = lak::result<T, lak::await_error>;

	template<typename T>
	struct await
	{
	private:
		std::thread _thread;
		std::atomic_bool _finished   = false;
		lak::await_result<T> _result = lak::err_t{lak::await_error::failed};

	public:
		await() = default;

		await(const await &) = delete;
		await(await &&)      = delete;

		await &operator=(const await &) = delete;
		await &operator=(await &&)      = delete;

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
              result = lak::ok_t<T>{functor(arg...)};
            }
            catch (const std::exception &e)
            {
              ERROR("Uncaught Exception: ", e.what());
              result = lak::err_t<lak::await_error>{lak::await_error::failed};
            }
            catch (...)
            {
              ERROR("Uncaught Exception");
              result = lak::err_t<lak::await_error>{lak::await_error::failed};
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
					ASSERT_EQUAL(_result.unwrap_err(), lak::await_error::failed);
				return lak::exchange(_result, lak::err_t{lak::await_error::failed});
			}
			else
			{
				return lak::err_t{lak::await_error::running};
			}
		}
	};
}

#endif
