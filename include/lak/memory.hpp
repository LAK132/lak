#ifndef LAK_MEMORY_HPP
#define LAK_MEMORY_HPP

#include "lak/result.hpp"

#include <atomic>

namespace lak
{
	template<typename T>
	struct unique_ptr
	{
	private:
		T *_value = nullptr;

	public:
		template<typename... ARGS>
		static unique_ptr make(ARGS &&...args);

		unique_ptr() = default;
		unique_ptr(unique_ptr &&other);
		unique_ptr &operator=(unique_ptr &&other);
		~unique_ptr();

		void reset();

		inline operator bool() const { return _value != nullptr; }

		inline T &operator*() const { return *_value; }

		inline T *operator->() const { return _value; }

		inline T *get() const { return _value; }
	};

	template<typename T>
	struct unique_ref
	{
	private:
		lak::unique_ptr<T> _value;

		unique_ref() = default;

	public:
		template<typename... ARGS>
		static lak::result<unique_ref> make(ARGS &&...args);

		unique_ref(unique_ref &&) = default;
		unique_ref &operator=(unique_ref &&) = default;

		inline T &operator*() const { return *_value; }

		inline T *operator->() const { return _value.get(); }

		inline T *get() const { return _value.get(); }
	};

	struct reference_count
	{
		std::atomic_size_t value = 0U;

		inline size_t operator++()
		{
			for (size_t s = value.load();;)
				if (value.compare_exchange_weak(s, s + 1)) return s + 1;
		}

		inline size_t operator++(int)
		{
			for (size_t s = value.load();;)
				if (value.compare_exchange_weak(s, s + 1)) return s;
		}

		inline size_t operator--()
		{
			for (size_t s = value.load();;)
				if (value.compare_exchange_weak(s, s - 1)) return s - 1;
		}

		inline size_t operator--(int)
		{
			for (size_t s = value.load();;)
				if (value.compare_exchange_weak(s, s - 1)) return s;
		}
	};

	template<typename T>
	struct shared_ptr
	{
	private:
		template<typename U>
		struct internal_value_type
		{
			lak::reference_count ref_count;
			U value;
		};
		void *_data                      = nullptr;
		lak::reference_count *_ref_count = nullptr;
		T *_value                        = nullptr;
		void (*_deleter)(void *)         = nullptr;

		template<typename U>
		friend struct shared_ptr;

	public:
		template<typename... ARGS>
		static shared_ptr make(ARGS &&...args);

		shared_ptr() = default;
		shared_ptr(const shared_ptr &other);
		shared_ptr &operator=(const shared_ptr &other);
		shared_ptr(shared_ptr &&other);
		shared_ptr &operator=(shared_ptr &&other);

		~shared_ptr() { reset(); }

		void reset();

		size_t use_count() const
		{
			return _ref_count ? _ref_count->value.load() : 0U;
		}

		inline operator bool() const { return _value != nullptr; }

		inline T &operator*() const { return *_value; }

		inline T *operator->() const { return _value; }

		inline T *get() const { return _value; }
	};

	template<>
	struct shared_ptr<void>
	{
	private:
		void *_data                      = nullptr;
		lak::reference_count *_ref_count = nullptr;
		void *_value                     = nullptr;
		void (*_deleter)(void *)         = nullptr;

		template<typename U>
		friend struct shared_ptr;

	public:
		shared_ptr() = default;

		shared_ptr(const shared_ptr &other)
		: _data(other._data),
		  _ref_count(other._ref_count),
		  _value(other._value),
		  _deleter(other._deleter)
		{
			if (_ref_count) ++*_ref_count;
		}

		template<typename T>
		shared_ptr(const shared_ptr<T> &other)
		: _data(other._data),
		  _ref_count(other._ref_count),
		  _value(other._value),
		  _deleter(other._deleter)
		{
			if (_ref_count) ++*_ref_count;
		}

		template<typename T>
		shared_ptr &operator=(const shared_ptr<T> &other)
		{
			_data      = other._data;
			_ref_count = other._ref_count;
			_value     = other._value;
			_deleter   = other._deleter;
			if (_ref_count) ++*_ref_count;
			return *this;
		}

		template<typename T>
		shared_ptr(shared_ptr<T> &&other)
		: _data(lak::exchange(other._data, nullptr)),
		  _ref_count(lak::exchange(other._ref_count, nullptr)),
		  _value(lak::exchange(other._value, nullptr)),
		  _deleter(lak::exchange(other._deleter, nullptr))
		{
		}

		~shared_ptr() { reset(); }

		void reset()
		{
			if (_deleter) _deleter(_data);
			_data      = nullptr;
			_ref_count = nullptr;
			_value     = nullptr;
			_deleter   = nullptr;
		}

		size_t use_count() const
		{
			return _ref_count ? _ref_count->value.load() : 0U;
		}

		inline operator bool() const { return _value != nullptr; }

		inline void *get() const { return _value; }

		template<typename T>
		operator shared_ptr<T>() const
		{
			lak::shared_ptr<T> result;
			if (_ref_count)
			{
				result._data      = _data;
				result._ref_count = _ref_count;
				result._value     = static_cast<T *>(_value);
				result._deleter   = _deleter;
				++*_ref_count;
			}
			return result;
		}
	};

	template<>
	struct shared_ptr<const void>
	{
	private:
		void *_data                      = nullptr;
		lak::reference_count *_ref_count = nullptr;
		const void *_value               = nullptr;
		void (*_deleter)(void *)         = nullptr;

		template<typename U>
		friend struct shared_ptr;

	public:
		shared_ptr() = default;

		shared_ptr(const shared_ptr &other)
		: _data(other._data),
		  _ref_count(other._ref_count),
		  _value(other._value),
		  _deleter(other._deleter)
		{
			if (_ref_count) ++*_ref_count;
		}

		template<typename T>
		shared_ptr(const shared_ptr<T> &other)
		: _data(other._data),
		  _ref_count(other._ref_count),
		  _value(other._value),
		  _deleter(other._deleter)
		{
			if (_ref_count) ++*_ref_count;
		}

		template<typename T>
		shared_ptr &operator=(const shared_ptr<T> &other)
		{
			_data      = other._data;
			_ref_count = other._ref_count;
			_value     = other._value;
			_deleter   = other._deleter;
			if (_ref_count) ++*_ref_count;
			return *this;
		}

		template<typename T>
		shared_ptr(shared_ptr<T> &&other)
		: _data(lak::exchange(other._data, nullptr)),
		  _ref_count(lak::exchange(other._ref_count, nullptr)),
		  _value(lak::exchange(other._value, nullptr)),
		  _deleter(lak::exchange(other._deleter, nullptr))
		{
		}

		~shared_ptr() { reset(); }

		void reset()
		{
			if (_deleter) _deleter(_data);
			_data      = nullptr;
			_ref_count = nullptr;
			_value     = nullptr;
			_deleter   = nullptr;
		}

		size_t use_count() const
		{
			return _ref_count ? _ref_count->value.load() : 0U;
		}

		inline operator bool() const { return _value != nullptr; }

		inline const void *get() const { return _value; }

		template<typename T>
		operator shared_ptr<const T>() const
		{
			lak::shared_ptr<const T> result;
			if (_ref_count)
			{
				result._data      = _data;
				result._ref_count = _ref_count;
				result._value     = static_cast<const T *>(_value);
				result._deleter   = _deleter;
				++*_ref_count;
			}
			return result;
		}
	};

	template<typename T>
	struct shared_ref
	{
	private:
		lak::shared_ptr<T> _value;

		shared_ref(lak::shared_ptr<T> &&val);

	public:
		template<typename... ARGS>
		static lak::result<shared_ref> make(ARGS &&...args);

		shared_ref()                   = delete;
		shared_ref(const shared_ref &) = default;
		shared_ref(shared_ref &&)      = default;
		shared_ref &operator=(const shared_ref &) = default;
		shared_ref &operator=(shared_ref &&) = default;

		size_t use_count() const { return _value.use_count(); }

		inline T &operator*() const { return *_value; }

		inline T *operator->() const { return _value.get(); }

		inline T *get() const { return _value.get(); }
	};
}

#include "lak/memory.inl"

#endif
