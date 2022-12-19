#include "lak/memory.hpp"

#include "lak/utility.hpp"

/* --- unique_ptr --- */

template<typename T>
template<typename... ARGS>
lak::unique_ptr<T> lak::unique_ptr<T>::make(ARGS &&...args)
{
	lak::unique_ptr<T> result;
	result._value = new T{lak::forward<ARGS>(args)...};
	return result;
}

template<typename T>
lak::unique_ptr<T>::unique_ptr(unique_ptr &&other)
: _value(lak::exchange(other._value, nullptr))
{
}

template<typename T>
lak::unique_ptr<T> &lak::unique_ptr<T>::operator=(unique_ptr &&other)
{
	lak::swap(_value, other._value);
	return *this;
}

template<typename T>
lak::unique_ptr<T>::unique_ptr::~unique_ptr()
{
	reset();
}

template<typename T>
void lak::unique_ptr<T>::reset()
{
	if (_value) delete _value;
	_value = nullptr;
}

/* --- unique_ref --- */

template<typename T>
template<typename... ARGS>
lak::result<lak::unique_ref<T>> lak::unique_ref<T>::make(ARGS &&...args)
{
	lak::unique_ref<T> result;
	result._value = lak::unique_ptr<T>::make(lak::forward<ARGS>(args)...);
	if (result._value)
		return lak::move_ok(result);
	else
		return lak::err_t{};
}

/* --- shared_ptr --- */

template<typename T>
template<typename... ARGS>
lak::shared_ptr<T> lak::shared_ptr<T>::make(ARGS &&...args)
{
	lak::shared_ptr<T> result;
	auto d{new internal_value_type<T>{.ref_count{.value{1U}},
	                                  .value{lak::forward<ARGS>(args)...}}};
	ASSERT(d);
	if (d)
	{
		result._data      = d;
		result._ref_count = &d->ref_count;
		result._value     = &d->value;
		result._deleter   = [](void *d)
		{
			if (auto data{reinterpret_cast<internal_value_type<T> *>(d)};
			    data && (--data->ref_count) == 0)
				delete data;
		};
	}
	return result;
}

template<typename T>
lak::shared_ptr<T>::shared_ptr(const shared_ptr &other)
: _data(other._data),
  _ref_count(other._ref_count),
  _value(other._value),
  _deleter(other._deleter)
{
	if (_ref_count) ++*_ref_count;
}

template<typename T>
lak::shared_ptr<T> &lak::shared_ptr<T>::operator=(const shared_ptr &other)
{
	_data      = other._data;
	_ref_count = other._ref_count;
	_value     = other._value;
	_deleter   = other._deleter;
	if (_ref_count) ++*_ref_count;
	return *this;
}

template<typename T>
lak::shared_ptr<T>::shared_ptr(shared_ptr &&other)
: _data(lak::exchange(other._data, nullptr)),
  _ref_count(lak::exchange(other._ref_count, nullptr)),
  _value(lak::exchange(other._value, nullptr)),
  _deleter(lak::exchange(other._deleter, nullptr))
{
}

template<typename T>
lak::shared_ptr<T> &lak::shared_ptr<T>::operator=(shared_ptr &&other)
{
	lak::swap(_data, other._data);
	lak::swap(_ref_count, other._ref_count);
	lak::swap(_value, other._value);
	lak::swap(_deleter, other._deleter);
	return *this;
}

template<typename T>
void lak::shared_ptr<T>::reset()
{
	if (_deleter) _deleter(_data);
	_data      = nullptr;
	_ref_count = nullptr;
	_value     = nullptr;
	_deleter   = nullptr;
}

/* --- shared_ref --- */

template<typename T>
lak::shared_ref<T>::shared_ref(lak::shared_ptr<T> &&val)
: _value(lak::move(val))
{
	ASSERT(_value);
}

template<typename T>
template<typename... ARGS>
lak::result<lak::shared_ref<T>> lak::shared_ref<T>::make(ARGS &&...args)
{
	lak::shared_ptr<T> result;
	result = lak::shared_ptr<T>::make(lak::forward<ARGS>(args)...);
	if (result)
		return lak::ok_t(lak::shared_ref<T>{lak::move(result)});
	else
		return lak::err_t{};
}
