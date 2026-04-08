#ifndef LAK_ATOMIC_OPTIONAL_HPP
#define LAK_ATOMIC_OPTIONAL_HPP

#include "lak/optional.hpp"
#include "lak/uninitialised.hpp"

#include <atomic>
#include <mutex>
#include <thread>

namespace lak
{
	template<typename T>
	struct atomic_optional
	{
	private:
		std::mutex _mutex;
		std::condition_variable _cond;
		std::atomic_bool _has_value = false;
		std::atomic_bool _stopped   = false;
		lak::uninitialised<T> _value;

		template<typename... ARGS>
		void internal_emplace(ARGS &&...args)
		{
			_stopped = false;
			_value.create(lak::forward<ARGS>(args)...);
			_has_value = true;
		}

		T internal_release()
		{
			T result   = _value.release();
			_has_value = false;
			return result;
		}

		void internal_reset(bool st)
		{
			_stopped = st;
			if (_has_value.load())
			{
				_value.destroy();
				_has_value = false;
			}
		}

	public:
		atomic_optional()                                   = default;
		atomic_optional(const atomic_optional &)            = delete;
		atomic_optional &operator=(const atomic_optional &) = delete;
		atomic_optional(atomic_optional &&other)
		{
			if (other.has_value()) internal_emplace(other.internal_release());
		}
		atomic_optional &operator=(atomic_optional &&other)
		{
			if (has_value())
			{
				if (other.has_value())
				{
					_value.value() = other.internal_release();
					_stopped       = other.stopped();
				}
				else
					internal_reset(other.stopped());
			}
			else if (other.has_value())
			{
				internal_emplace(other.internal_release());
				_stopped = other.stopped();
			}
			return *this;
		}
		template<typename... ARGS>
		atomic_optional(lak::in_place_t, ARGS &&...args)
		{
			internal_emplace(lak::forward<ARGS>(args)...);
		}

		~atomic_optional() { internal_reset(true); }

		bool has_value() const { return _has_value.load(); }

		T &value() { _value.value(); }
		const T &value() const { _value.value(); }

		void stop()
		{
			{
				std::lock_guard lock{_mutex};
				internal_reset(true);
			}
			_cond.notify_all();
		}
		bool stopped() const { return _stopped.load(); }

		template<typename... ARGS>
		void emplace(ARGS &&...args)
		{
			std::unique_lock lock{_mutex};
			for (;; lock.unlock())
			{
				_cond.wait(lock, [&]() { return !has_value(); });
				if (!has_value()) break;
			}
			internal_emplace(lak::forward<ARGS>(args)...);
			lock.unlock();
			_cond.notify_all();
		}

		T release()
		{
			std::unique_lock lock{_mutex};
			for (;; lock.unlock())
			{
				_cond.wait(lock, [&]() { return has_value(); });
				if (has_value()) break;
			}
			T result = internal_release();
			lock.unlock();
			_cond.notify_all();
			return result;
		}

		template<typename... ARGS>
		bool try_emplace(ARGS &&...args)
		{
			if (stopped()) return lak::nullopt;
			std::unique_lock lock{_mutex};
			for (;; lock.unlock())
			{
				_cond.wait(lock, [&]() { return stopped() || !has_value(); });
				if (stopped()) return false;
				if (!has_value()) break;
			}
			internal_emplace(lak::forward<ARGS>(args)...);
			lock.unlock();
			_cond.notify_all();
			return true;
		}

		lak::optional<T> try_release()
		{
			if (stopped()) return lak::nullopt;
			std::unique_lock lock{_mutex};
			for (;; lock.unlock())
			{
				_cond.wait(lock, [&]() { return stopped() || has_value(); });
				if (stopped()) return lak::nullopt;
				if (has_value()) break;
			}
			lak::optional<T> result{lak::in_place, internal_release()};
			lock.unlock();
			_cond.notify_all();
			return result;
		}
	};
}

#endif
