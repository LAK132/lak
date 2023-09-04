#include "lak/priority_queue.hpp"

#include "lak/algorithm.hpp"

template<typename T, typename CMP>
lak::span<const T> lak::priority_queue<T, CMP>::sorted()
{
	if (_data.size() > 2U)
		lak::reverse_sort_heap(_data.begin(), _data.end(), CMP{});
	return lak::span(_data);
}

template<typename T, typename CMP>
void lak::priority_queue<T, CMP>::push(const T &value)
requires std::copy_constructible<T>
{
	_data.push_back(value);
	lak::push_heap(_data.begin(), _data.end(), CMP{});
}

template<typename T, typename CMP>
void lak::priority_queue<T, CMP>::push(T &&value)
{
	_data.push_back(lak::move(value));
	lak::push_heap(_data.begin(), _data.end(), CMP{});
}

template<typename T, typename CMP>
void lak::priority_queue<T, CMP>::push(std::initializer_list<T> list)
requires std::copy_constructible<T>
{
	const size_t old_size = _data.size();
	_data.insert(_data.end(), list);
	for (size_t i = old_size; i <= _data.size(); ++i)
		lak::push_heap(_data.begin(), _data.begin() + i, CMP{});
}

template<typename T, typename CMP>
T lak::priority_queue<T, CMP>::pop()
{
	lak::pop_heap(_data.begin(), _data.end(), CMP{});
	return _data.popped_back();
}

template<typename T, typename CMP>
T lak::priority_queue<T, CMP>::pop_low()
{
	return _data.popped_back();
}

template<typename T, typename CMP>
template<lak::concepts::invocable_result_of<bool, const T &> F>
lak::optional<T> lak::priority_queue<T, CMP>::pop_if_breadth_first(
  F &&predicate)
{
	auto it =
	  lak::breadth_first_search_heap(_data.begin(), _data.end(), predicate);

	if (it == _data.end()) return lak::nullopt;

	if (auto last = _data.end() - 1; it != last)
	{
		lak::swap(*it, *last);
		lak::sift_down_heap(_data.begin(), it, last, CMP{});
	}

	return lak::optional<T>(_data.popped_back());
}

template<typename T, typename CMP>
template<lak::concepts::invocable_result_of<bool, const T &> F>
lak::optional<T> lak::priority_queue<T, CMP>::pop_if_depth_first(F &&predicate)
{
	auto it =
	  lak::depth_first_search_heap(_data.begin(), _data.end(), predicate);

	if (it == _data.end()) return lak::nullopt;

	if (auto last = _data.end() - 1; it != last)
	{
		lak::swap(*it, *last);
		lak::sift_down_heap(_data.begin(), it, last, CMP{});
	}

	return lak::optional<T>(_data.popped_back());
}

template<typename T, typename CMP>
template<
  lak::concepts::invocable_result_of<lak::optional<T>, const T &, const T &> F>
void lak::priority_queue<T, CMP>::compact(F &&compactor)
{
	if (size() <= 1U) return;

	lak::sort_heap(_data.begin(), _data.end(), CMP{});

	{
		size_t head = 0U;
		size_t tail = head + 1U;

		while (tail < size())
		{
			lak::optional<T> result = compactor(_data[head], _data[tail]);
			if (result)
			{
				_data[head] = lak::move(*result);
				++tail;
			}
			else
			{
				_data.erase(_data.data() + head + 1, _data.data() + tail);
				++head;
				tail = head + 1U;
			}
		}

		if (head + 1 < _data.size())
			_data.erase(_data.data() + head + 1,
			            tail < _data.size() ? _data.data() + tail : _data.end());
	}

	// if the compaction kept things mostly in-order, then reversing should
	// result in the data reforming a valid heap, making make_heap
	// considerably cheaper.
	lak::reverse(_data.begin(), _data.end());
	lak::make_heap(_data.begin(), _data.end(), CMP{});
}
