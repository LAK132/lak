#ifndef LAK_FUTURE_HPP
#define LAK_FUTURE_HPP

#include "lak/memory.hpp"
#include "lak/thread.hpp"
#include "lak/tuple.hpp"
#include "lak/type_traits.hpp"
#include "lak/uninitialised.hpp"

#include <atomic>

namespace lak
{
	template<typename T>
	struct future;

	template<typename F, typename... ARGS>
	lak::future<lak::invoke_result_t<F, ARGS...>> async(F &&f, ARGS &&...args);

	template<typename T>
	struct future
	{
	private:
		struct future_impl
		{
			std::atomic_bool has_value = false;
			lak::thread thread;
			lak::uninitialised<lak::lvalue_to_ptr_t<T>> result;
			~future_impl()
			{
				if (thread.running()) thread.join();
				if (has_value) result.destroy();
			}
		};
		lak::shared_ptr<future_impl> _data;

		future() = default;

	public:
		future(future &&)                  = default;
		future(const future &)             = delete;
		future &operator=(future &&)       = default;
		future &operator=(const future &&) = delete;

		template<typename F, typename... A>
		friend lak::future<lak::invoke_result_t<F, A...>> async(F &&, A &&...);

		using value_type = lak::remove_lvalue_reference_t<T>;

		bool running() const { return _data->thread.running(); }
		bool has_value() const { return _data->has_value; }
		value_type *try_get() const
		{
			if constexpr (lak::is_lvalue_reference_v<T>)
				return has_value() ? &_data->result.value() : nullptr;
			else
				return has_value() ? _data->result.value() : nullptr;
		}
		value_type &wait() const
		{
			if (running()) _data->thread.join();
			return *try_get();
		}
	};

	template<>
	struct future<void>
	{
	private:
		struct future_impl
		{
			std::atomic_bool has_value = false;
			lak::thread thread;
			~future_impl()
			{
				if (thread.running()) thread.join();
			}
		};
		lak::shared_ptr<future_impl> _data;

		future() = default;

	public:
		future(future &&)                  = default;
		future(const future &)             = delete;
		future &operator=(future &&)       = default;
		future &operator=(const future &&) = delete;

		template<typename F, typename... A>
		friend lak::future<lak::invoke_result_t<F, A...>> async(F &&, A &&...);

		bool running() const { return _data->thread.running(); }
		bool has_value() const { return _data->has_value; }
		void wait() const
		{
			if (running()) _data->thread.join();
		}
	};

	template<typename F, typename... ARGS>
	lak::future<lak::invoke_result_t<F, ARGS...>> async(F &&f, ARGS &&...args)
	{
		using result_type = lak::invoke_result_t<F, ARGS...>;
		lak::future<result_type> result;
		result._data =
		  lak::shared_ptr<typename lak::future<result_type>::future_impl>::make();
		result._data->thread = lak::thread(
		  [data = result._data, f, ... args = args]()
		  {
			  if constexpr (!lak::is_void_v<result_type>)
				  data->result.create(f(lak::forward<ARGS>(args)...));
			  else
				  f(lak::forward<ARGS>(args)...);
			  data->has_value = true;
		  });
		return result;
	}
}

#endif
