#include "lak/memory.hpp"

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

/* --- shared_ptr --- */

template<typename T>
lak::shared_ptr<T>::shared_ptr(lak::_shared_ptr_metadata *d)
: _data(static_cast<internal_value_type *>(d))
{
}

template<typename T>
void lak::shared_ptr<T>::reset(lak::_shared_ptr_metadata *d)
{
	reset();
	_data = static_cast<internal_value_type *>(d);
}

template<typename T>
lak::_shared_ptr_metadata *lak::shared_ptr<T>::release()
{
	return lak::exchange(_data, nullptr);
}

template<typename T>
lak::_shared_ptr_metadata *lak::shared_ptr<T>::release_copy() const
{
	if (_data) ++_data->ref_count;
	return _data;
}

template<typename T>
template<typename... ARGS>
lak::shared_ptr<T> lak::shared_ptr<T>::make(ARGS &&...args)
{
	lak::shared_ptr<T> result;

	result._data = new internal_value_type{
	  {
	    .ref_count{.value{1U}},
	    .deleter{[](void *d)
	             {
		             if (auto data{static_cast<internal_value_type *>(
		                   reinterpret_cast<lak::_shared_ptr_metadata *>(d))};
		                 data && (--data->ref_count) == 0)
			             delete data;
	             }},
	    .data{nullptr},
	  },
	  T{lak::forward<ARGS>(args)...},
	};

	if (result._data)
	{
		if constexpr (lak::is_const_v<T>)
		{
			using vp  = void *;
			using cvp = const void *;

			result._data->data.~vp();
			new (&result._data->cdata) cvp(&result._data->value);
		}
		else
			result._data->data = &result._data->value;
	}

	return result;
}

template<typename T>
lak::shared_ptr<T>::shared_ptr(const shared_ptr &other)
: shared_ptr(other.release_copy())
{
}

template<typename T>
lak::shared_ptr<T> &lak::shared_ptr<T>::operator=(const shared_ptr &other)
{
	reset(other.release_copy());
	return *this;
}

template<typename T>
lak::shared_ptr<T>::shared_ptr(shared_ptr &&other)
: shared_ptr(other.release())
{
}

template<typename T>
lak::shared_ptr<T> &lak::shared_ptr<T>::operator=(shared_ptr &&other)
{
	reset(other.release());
	return *this;
}

template<typename T>
void lak::shared_ptr<T>::reset()
{
	if (_data) _data->deleter(_data);
	_data = nullptr;
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
