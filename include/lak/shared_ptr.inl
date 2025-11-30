#include "lak/shared_ptr.hpp"

#include "lak/utility.hpp"

/* --- _tiny_shared_ptr_metadata --- */

void lak::_tiny_shared_ptr_metadata::dec_ref()
{
	if (--ref_count == 0U)
		deleter(this, _shared_ptr_df::do_destroy | _shared_ptr_df::do_delete);
}

/* --- _shared_ptr_metadata --- */

void lak::_shared_ptr_metadata::dec_ref()
{
	if (--ref_count == 0U)
		deleter(this, _shared_ptr_df::do_destroy | _shared_ptr_df::do_delete);
}

/* --- basic_shared_ptr --- */

template<typename T, typename META>
lak::basic_shared_ptr<T, META>::basic_shared_ptr(META *d) : _data(d)
{
}

template<typename T, typename META>
void lak::basic_shared_ptr<T, META>::reset(META *d)
{
	reset();
	_data = d;
}

template<typename T, typename META>
META *lak::basic_shared_ptr<T, META>::release()
{
	return lak::exchange(_data, nullptr);
}

template<typename T, typename META>
META *lak::basic_shared_ptr<T, META>::release_copy() const
{
	if (_data) _data->inc_ref();
	return _data;
}

template<typename T, typename META>
template<typename... ARGS>
lak::basic_shared_ptr<T, META> lak::basic_shared_ptr<T, META>::make(
  ARGS &&...args)
{
	lak::basic_shared_ptr<T, META> result;

	result._data = new internal_value_type(
	  [](lak::basic_shared_ptr_metadata *p, _shared_ptr_df df)
	  {
		  auto d{static_cast<internal_value_type *>(p)};
		  if ((df & _shared_ptr_df::do_destroy) == _shared_ptr_df::do_destroy)
			  d->value.destroy();
		  if ((df & _shared_ptr_df::do_delete) == _shared_ptr_df::do_delete)
			  delete d;
	  },
	  lak::forward<ARGS>(args)...);

	if (result._data) result._data->set_data(&result._get()->value);

	return result;
}

template<typename T, typename META>
lak::basic_shared_ptr<T, META>::basic_shared_ptr(const basic_shared_ptr &other)
: basic_shared_ptr(other.release_copy())
{
}

template<typename T, typename META>
lak::basic_shared_ptr<T, META> &lak::basic_shared_ptr<T, META>::operator=(
  const basic_shared_ptr &other)
{
	reset(other.release_copy());
	return *this;
}

template<typename T, typename META>
lak::basic_shared_ptr<T, META>::basic_shared_ptr(basic_shared_ptr &&other)
: basic_shared_ptr(other.release())
{
}

template<typename T, typename META>
lak::basic_shared_ptr<T, META> &lak::basic_shared_ptr<T, META>::operator=(
  basic_shared_ptr &&other)
{
	reset(other.release());
	return *this;
}

template<typename T, typename META>
void lak::basic_shared_ptr<T, META>::reset()
{
	if (_data) _data->dec_ref();
	_data = nullptr;
}

/* --- basic_shared_ref --- */

template<typename T, typename META>
lak::basic_shared_ref<T, META>::basic_shared_ref(
  lak::basic_shared_ptr<T, META> &&val)
: _value(lak::move(val))
{
	ASSERT(_value);
}

template<typename T, typename META>
lak::result<lak::basic_shared_ref<T, META>>
lak::basic_shared_ref<T, META>::make(lak::basic_shared_ptr<T, META> ptr)
{
	if (ptr)
		return lak::ok_t(lak::basic_shared_ref<T, META>{lak::move(ptr)});
	else
		return lak::err_t{};
}

template<typename T, typename META>
template<typename... ARGS>
lak::result<lak::basic_shared_ref<T, META>>
lak::basic_shared_ref<T, META>::make(ARGS &&...args)
{
	lak::basic_shared_ptr<T, META> result;
	result = lak::basic_shared_ptr<T, META>::make(lak::forward<ARGS>(args)...);
	if (result)
		return lak::ok_t(lak::basic_shared_ref<T, META>{lak::move(result)});
	else
		return lak::err_t{};
}

/* --- _strong_shared_ptr_metadata --- */

void lak::_strong_shared_ptr_metadata::dec_ref()
{
	if (--ref_count == 0U)
	{
		deleter(this, _shared_ptr_df::do_destroy);
		if (weak_ref_count.non_zero_inc() == 0U || --weak_ref_count == 0U)
			deleter(this, _shared_ptr_df::do_delete);
	}
}

void lak::_strong_shared_ptr_metadata::dec_weak_ref()
{
	if (ref_count.non_zero_inc() != 0U)
	{
		--weak_ref_count;
		dec_ref();
	}
	else if (--weak_ref_count == 0U)
	{
		deleter(this, _shared_ptr_df::do_delete);
	}
}

/* --- basic_weak_ptr --- */

template<typename T, typename META>
lak::basic_weak_ptr<T, META>::basic_weak_ptr(META *d) : _data(d)
{
}

template<typename T, typename META>
void lak::basic_weak_ptr<T, META>::reset(META *d)
{
	reset();
	_data = d;
}

template<typename T, typename META>
META *lak::basic_weak_ptr<T, META>::release()
{
	return lak::exchange(_data, nullptr);
}

template<typename T, typename META>
META *lak::basic_weak_ptr<T, META>::release_copy() const
{
	if (_data) _data->inc_weak_ref();
	return _data;
}

template<typename T, typename META>
lak::basic_weak_ptr<T, META>::basic_weak_ptr(const basic_weak_ptr &other)
: basic_shared_ptr(other.release_copy())
{
}

template<typename T, typename META>
lak::basic_weak_ptr<T, META> &lak::basic_weak_ptr<T, META>::operator=(
  const basic_weak_ptr &other)
{
	reset(other.release_copy());
	return *this;
}

template<typename T, typename META>
lak::basic_weak_ptr<T, META>::basic_weak_ptr(basic_weak_ptr &&other)
: basic_weak_ptr(other.release())
{
}

template<typename T, typename META>
lak::basic_weak_ptr<T, META> &lak::basic_weak_ptr<T, META>::operator=(
  basic_weak_ptr &&other)
{
	reset(other.release());
	return *this;
}

template<typename T, typename META>
lak::basic_weak_ptr<T, META>::basic_weak_ptr(
  const lak::basic_shared_ptr<T, META> &other)
{
	reset(other._data);
	if (_data) _data->inc_weak_ref();
}

template<typename T, typename META>
lak::basic_weak_ptr<T, META> &lak::basic_weak_ptr<T, META>::operator=(
  const lak::basic_shared_ptr<T, META> &other)
{
	reset(other._data);
	if (_data) _data->inc_weak_ref();
	return *this;
}

template<typename T, typename META>
void lak::basic_weak_ptr<T, META>::reset()
{
	if (_data) _data->dec_ref();
	_data = nullptr;
}

template<typename T, typename META>
lak::basic_shared_ptr<T, META> lak::basic_weak_ptr<T, META>::get() const
{
	if (!_data) return {};
	if (!_data->try_inc_ref()) return {};
	return lak::basic_shared_ptr<T, META>(_data);
}
