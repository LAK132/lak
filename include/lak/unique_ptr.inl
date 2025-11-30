#include "lak/unique_ptr.hpp"

#include "lak/utility.hpp"

/* --- unique_ptr --- */

template<typename T>
template<typename... ARGS>
lak::unique_ptr<T> lak::unique_ptr<T>::make(ARGS &&...args)
{
	lak::unique_ptr<T> result;
	result._deleter = [](T *t) { delete t; };
	result._value   = new T{lak::forward<ARGS>(args)...};
	return result;
}

template<typename T>
lak::unique_ptr<T>::unique_ptr(T *value)
: _deleter([](T *t) { delete t; }), _value(value)
{
}

template<typename T>
lak::unique_ptr<T>::unique_ptr(T *value, void (*deleter)(T *))
: _deleter(deleter), _value(value)
{
}

template<typename T>
lak::unique_ptr<T>::unique_ptr(unique_ptr &&other)
: _deleter(lak::exchange(other._deleter, nullptr)),
  _value(lak::exchange(other._value, nullptr))
{
}

template<typename T>
lak::unique_ptr<T> &lak::unique_ptr<T>::operator=(unique_ptr &&other)
{
	lak::swap(_deleter, other._deleter);
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
	if (_value) _deleter(_value);
	_deleter = nullptr;
	_value   = nullptr;
}

template<typename T>
template<typename... ARGS>
lak::unique_ptr<T[]> lak::unique_ptr<T[]>::make(size_t count, ARGS &&...args)
{
	lak::unique_ptr<T[]> result;
	result._deleter = [](lak::span<T> v) { delete[] v.data(); };
	result._value =
	  lak::span<T>(new T[count]{lak::forward<ARGS>(args)...}, count);
	return result;
}

template<typename T>
lak::unique_ptr<T[]>::unique_ptr(lak::span<T> value)
: _deleter([](lak::span<T> v) { delete[] v.data(); }), _value(value)
{
}

template<typename T>
lak::unique_ptr<T[]>::unique_ptr(lak::span<T> value,
                                 void (*deleter)(lak::span<T>))
: _deleter(deleter), _value(value)
{
}

template<typename T>
lak::unique_ptr<T[]>::unique_ptr(unique_ptr &&other)
: _deleter(lak::exchange(other._deleter, nullptr)),
  _value(lak::exchange(other._value, lak::span<T>{}))
{
}

template<typename T>
lak::unique_ptr<T[]> &lak::unique_ptr<T[]>::operator=(unique_ptr &&other)
{
	lak::swap(_deleter, other._deleter);
	lak::swap(_value, other._value);
	return *this;
}

template<typename T>
lak::unique_ptr<T[]>::unique_ptr::~unique_ptr()
{
	reset();
}

template<typename T>
void lak::unique_ptr<T[]>::reset()
{
	if (!_value.empty()) _deleter(_value);
	_deleter = nullptr;
	_value   = lak::span<T>{};
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
