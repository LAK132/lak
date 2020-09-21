
template<typename T>
void lak::bank_ptr<T>::internal_flush()
{
  if (_deleted.empty()) return;
  std::sort(_deleted.begin(), _deleted.end(), std::greater<size_t>{});
  size_t destroyed = 0;
  while (destroyed < _deleted.size() &&
         _deleted[destroyed] == _container.size() - 1)
  {
    ASSERT_EQUAL(_reference_count.back(), 0);
    _container.pop_back();
    _reference_count.pop_back();
    ++destroyed;
  }
  _deleted.erase(_deleted.begin(), _deleted.begin() + destroyed);
}

template<typename T>
void lak::bank_ptr<T>::flush()
{
  std::lock_guard lock(_mutex);
  internal_flush();
}

template<typename T>
template<typename... ARGS>
lak::bank_ptr<T> lak::bank_ptr<T>::internal_create(ARGS &&... args)
{
  std::lock_guard lock(_mutex);
  if (_deleted.size() > 0)
  {
    auto index = _deleted.back();
    ASSERT_EQUAL(_reference_count[index], 0);
    _deleted.pop_back();
    _container[index].~T();
    new (&_container[index]) T(std::forward<ARGS>(args)...);
    ++_reference_count[index];
    return {index};
  }
  else
  {
    _container.emplace_back(std::forward<ARGS>(args)...);
    _reference_count.push_back(1);
    return {_container.size() - 1};
  }
}

template<typename T>
lak::bank_ptr<T> lak::bank_ptr<T>::create(const T &t)
{
  return internal_create(t);
}

template<typename T>
lak::bank_ptr<T> lak::bank_ptr<T>::create(T &&t)
{
  return internal_create(std::move(t));
}

template<typename T>
template<typename... ARGS>
lak::bank_ptr<T> lak::bank_ptr<T>::create(ARGS &&... args)
{
  return internal_create(std::forward<ARGS>(args)...);
}

template<typename T>
template<typename FUNCTOR>
void lak::bank_ptr<T>::for_each(FUNCTOR &&func)
{
  std::lock_guard lock(_mutex);
  for (size_t i = 0; i < _container.size(); ++i)
  {
    if (_reference_count[i] > 0) func(_container[i]);
  }
}

template<typename T>
template<typename FUNCTOR>
lak::bank_ptr<T> lak::bank_ptr<T>::find_if(FUNCTOR &&func)
{
  std::lock_guard lock(_mutex);
  for (size_t i = 0; i < _container.size(); ++i)
  {
    if (_reference_count[i] > 0 && func(_container[i]))
    {
      ++_reference_count[i];
      return {i};
    }
  }
  return {};
}

template<typename T>
lak::bank_ptr<T>::bank_ptr() : _index(std::numeric_limits<size_t>::max())
{
}

template<typename T>
lak::bank_ptr<T>::bank_ptr(std::nullptr_t)
: _index(std::numeric_limits<size_t>::max())
{
}

template<typename T>
lak::bank_ptr<T> &lak::bank_ptr<T>::operator=(std::nullptr_t)
{
  reset();
};

template<typename T>
lak::bank_ptr<T>::bank_ptr(const bank_ptr &other)
{
  std::lock_guard lock(_mutex);
  _index = other._index;
  ++_reference_count[other._index];
};

template<typename T>
lak::bank_ptr<T> &lak::bank_ptr<T>::operator=(const bank_ptr &other)
{
  std::lock_guard lock(_mutex);
  _index = other._index;
  ++_reference_count[other._index];
  return *this;
};

template<typename T>
lak::bank_ptr<T>::~bank_ptr()
{
  reset();
}

template<typename T>
void lak::bank_ptr<T>::reset()
{
  if (!*this) return;
  std::lock_guard lock(_mutex);
  if (--_reference_count[_index] == 0)
  {
    if (_index == _container.size() - 1)
    {
      _container.pop_back();
      _reference_count.pop_back();
    }
    else
    {
      _deleted.push_back(_index);
      internal_flush();
    }
  }
  _index = std::numeric_limits<size_t>::max();
}

template<typename T>
T *lak::bank_ptr<T>::operator->()
{
  return &_container[_index];
}

template<typename T>
const T *lak::bank_ptr<T>::operator->() const
{
  return &_container[_index];
}

template<typename T>
T &lak::bank_ptr<T>::operator*()
{
  return _container[_index];
}

template<typename T>
const T &lak::bank_ptr<T>::operator*() const
{
  return _container[_index];
}

template<typename T>
bool lak::bank_ptr<T>::operator==(const lak::bank_ptr<T> &rhs) const
{
  return _index == rhs._index;
}

template<typename T>
bool lak::bank_ptr<T>::operator!=(const lak::bank_ptr<T> &rhs) const
{
  return !operator==(rhs);
}

template<typename T>
bool lak::bank_ptr<T>::operator==(std::nullptr_t) const
{
  return !*this;
}

template<typename T>
bool lak::bank_ptr<T>::operator!=(std::nullptr_t) const
{
  return !!*this;
}

template<typename T>
lak::bank_ptr<T>::operator bool() const
{
  return _index != std::numeric_limits<size_t>::max();
}