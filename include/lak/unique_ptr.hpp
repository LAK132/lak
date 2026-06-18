#ifndef LAK_UNIQUE_PTR_HPP
#define LAK_UNIQUE_PTR_HPP

#include "lak/math.hpp"
#include "lak/memmanip.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

namespace lak
{
	template<typename T>
	struct unique_ptr
	{
	private:
		void (*_deleter)(T *) = nullptr;
		T *_value             = nullptr;

	public:
		template<typename... ARGS>
		static unique_ptr make(ARGS &&...args);

		unique_ptr() = default;
		unique_ptr(T *value);
		unique_ptr(T *value, void (*deleter)(T *));
		unique_ptr(unique_ptr &&other);
		unique_ptr &operator=(unique_ptr &&other);
		~unique_ptr();

		void reset();

		explicit inline operator bool() const { return _value != nullptr; }

		inline T &operator*() const { return *_value; }

		inline T *operator->() const { return _value; }

		inline T *get() const { return _value; }

		inline T *release()
		{
			_deleter = nullptr;
			return lak::exchange(_value, nullptr);
		}
	};

	template<typename T>
	struct unique_ptr<T[]>
	{
	private:
		void (*_deleter)(lak::span<T>) = nullptr;
		lak::span<T> _value            = {};

	public:
		template<typename... ARGS>
		static unique_ptr make(size_t count, ARGS &&...args);

		unique_ptr() = default;
		unique_ptr(lak::span<T> value);
		unique_ptr(lak::span<T> value, void (*deleter)(lak::span<T>));
		unique_ptr(unique_ptr &&other);
		unique_ptr &operator=(unique_ptr &&other);
		~unique_ptr();

		void reset();

		explicit inline operator bool() const { return _value != nullptr; }

		inline const lak::span<T> &operator*() const { return _value; }

		inline const lak::span<T> *operator->() const { return &_value; }

		inline const lak::span<T> *get() const { return &_value; }

		inline lak::span<T> release()
		{
			_deleter = nullptr;
			return lak::exchange(_value, {});
		}
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

		unique_ref(unique_ref &&)            = default;
		unique_ref &operator=(unique_ref &&) = default;

		inline T &operator*() const { return *_value; }

		inline T *operator->() const { return _value.get(); }

		inline T *get() const { return _value.get(); }
	};
}

#include "lak/unique_ptr.inl"

#endif
