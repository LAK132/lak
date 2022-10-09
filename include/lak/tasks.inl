template<typename FUNC>
void lak::tasks::push(FUNC &&func)
{
	ASSERT_GREATER(_threads.size(), 0U);
	std::lock_guard lock{await_not_full(), std::adopt_lock};
	const size_t index{_active};
	if (_threads[index].joinable()) _threads[index].join();
	_threads[index] = std::thread(
	  [func, this]
	  {
		  func();
		  std::lock_guard lock{this->_mutex};
		  this->deactivate(std::this_thread::get_id());
	  });
	++_active;
}
