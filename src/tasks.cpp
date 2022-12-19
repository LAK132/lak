#include "lak/tasks.hpp"

#include "lak/span_manip.hpp"

std::mutex &lak::tasks::await_not_full()
{
	for (;;)
	{
		while (_active >= _threads.size()) std::this_thread::yield();
		_mutex.lock();
		if (_active < _threads.size()) return _mutex;
		_mutex.unlock();
	}
}

void lak::tasks::deactivate(size_t index)
{
	const size_t active = _active;
	ASSERT_GREATER(active, index);
	lak::rotate_left(lak::span(_threads).subspan(index, active - index));
	--_active;
}

void lak::tasks::deactivate(std::thread::id id)
{
	size_t index = 0;
	for (const auto &t : lak::span(_threads).first(_active))
		if (t.get_id() == id)
			return deactivate(index);
		else
			++index;
}

lak::tasks::tasks(tasks &&other)
{
	std::scoped_lock lock{_mutex, other._mutex};
	lak::swap(_threads, other._threads);
	_active       = size_t(other._active);
	other._active = 0U;
}

lak::tasks &lak::tasks::operator=(tasks &&other)
{
	std::scoped_lock lock{_mutex, other._mutex};
	lak::swap(_threads, other._threads);
	size_t active = _active;
	_active       = size_t(other._active);
	other._active = active;
	return *this;
}

lak::tasks::~tasks()
{
	while (_active > 0) std::this_thread::yield();
	std::lock_guard lock{_mutex};
	for (std::thread &t : _threads)
		if (t.joinable()) t.join();
}
