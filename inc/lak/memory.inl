#include "lak/memory.hpp"

#include "lak/utility.hpp"

template<typename T>
template<typename... ARGS>
lak::unique_ptr<T> lak::unique_ptr<T>::make(ARGS &&...args)
{
	lak::unique_ptr<T> result;
	result._value = new T(lak::forward<ARGS>(args)...);
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
