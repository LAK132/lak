#ifndef LAK_BANK_PTR_HPP
#define LAK_BANK_PTR_HPP

#include "lak/array.hpp"
#include "lak/railcar.hpp"

#ifdef LAK_NO_STD
#	error STL required
#endif
#include <mutex>

namespace lak
{
	template<typename T>
	struct bank
	{
	protected:
		template<typename U>
		friend struct shared_bank_ptr;
		template<typename U>
		friend struct unique_bank_ptr;

		static std::mutex _mutex;
		static lak::railcar<T> _container;
		static lak::array<size_t> _deleted;

		static void internal_flush();

		[[nodiscard]] static size_t internal_find_index(T *ptr);

		template<typename... ARGS>
		[[nodiscard]] static size_t internal_create(ARGS &&...args);

		static void internal_destroy(size_t index);

		template<typename FUNCTOR>
		[[nodiscard]] static size_t internal_find_if(FUNCTOR &&func);

	public:
		static void flush();

		[[nodiscard]] static T *create(const T &t);

		[[nodiscard]] static T *create(T &&t);

		template<typename... ARGS>
		[[nodiscard]] static T *create(ARGS &&...args);

		static void destroy(T *t);

		template<typename FUNCTOR>
		static void for_each(FUNCTOR &&func);

		template<typename FUNCTOR>
		static T *find_if(FUNCTOR &&func);
	};

	template<typename T>
	struct unique_bank_ptr : protected bank<T>
	{
	protected:
		template<typename U>
		friend struct shared_bank_ptr;

		size_t _index; // not thread safe!
		T *_value;

		unique_bank_ptr(size_t index) : _index(index)
		{
			_value = _index == std::numeric_limits<size_t>::max()
			           ? nullptr
			           : &bank<T>::_container[index];
		}

	public:
		using bank<T>::flush;
		using bank<T>::for_each;

		[[nodiscard]] static unique_bank_ptr create(const T &t);

		[[nodiscard]] static unique_bank_ptr create(T &&t);

		template<typename... ARGS>
		[[nodiscard]] static unique_bank_ptr create(ARGS &&...args);

		[[nodiscard]] static unique_bank_ptr from_raw_bank_ptr(T *ptr);

		unique_bank_ptr();

		unique_bank_ptr(lak::nullptr_t) : unique_bank_ptr() {}
		unique_bank_ptr &operator=(lak::nullptr_t);

		unique_bank_ptr(const unique_bank_ptr &other) = delete;
		unique_bank_ptr &operator=(const unique_bank_ptr &other) = delete;

		unique_bank_ptr(unique_bank_ptr &&other);
		unique_bank_ptr &operator=(unique_bank_ptr &&other);

		~unique_bank_ptr();

		void reset();

		T *release();

		T *get();
		const T *get() const;

		T *operator->();
		const T *operator->() const;

		T &operator*();
		const T &operator*() const;

		bool operator<(const unique_bank_ptr &rhs) const;
		bool operator<=(const unique_bank_ptr &rhs) const;
		bool operator>(const unique_bank_ptr &rhs) const;
		bool operator>=(const unique_bank_ptr &rhs) const;
		bool operator==(const unique_bank_ptr &rhs) const;
		bool operator!=(const unique_bank_ptr &rhs) const;

		bool operator==(lak::nullptr_t) const;
		bool operator!=(lak::nullptr_t) const;

		operator bool() const;
	};

	template<typename T>
	struct shared_bank_ptr : protected unique_bank_ptr<T>
	{
	protected:
		template<typename U>
		friend struct unique_bank_ptr;

		static lak::railcar<size_t> _reference_count;

		shared_bank_ptr(size_t index) : unique_bank_ptr<T>(index) {}

	public:
		using bank<T>::for_each;
		using unique_bank_ptr<T>::get;
		using unique_bank_ptr<T>::operator->;
		using unique_bank_ptr<T>::operator*;
		using unique_bank_ptr<T>::operator bool;

		static void flush();

		[[nodiscard]] static shared_bank_ptr create(const T &t);

		[[nodiscard]] static shared_bank_ptr create(T &&t);

		template<typename... ARGS>
		[[nodiscard]] static shared_bank_ptr create(ARGS &&...args);

		template<typename FUNCTOR>
		[[nodiscard]] static shared_bank_ptr find_if(FUNCTOR &&func);

		shared_bank_ptr() : unique_bank_ptr<T>() {}

		shared_bank_ptr(lak::nullptr_t) : unique_bank_ptr<T>() {}
		shared_bank_ptr &operator=(lak::nullptr_t);

		shared_bank_ptr(const shared_bank_ptr &other);
		shared_bank_ptr &operator=(const shared_bank_ptr &other);

		shared_bank_ptr(unique_bank_ptr<T> &&other);
		shared_bank_ptr &operator=(unique_bank_ptr<T> &&other);

		~shared_bank_ptr();

		void reset();

		bool operator<(const shared_bank_ptr &rhs) const;
		bool operator<=(const shared_bank_ptr &rhs) const;
		bool operator>(const shared_bank_ptr &rhs) const;
		bool operator>=(const shared_bank_ptr &rhs) const;
		bool operator==(const shared_bank_ptr &rhs) const;
		bool operator!=(const shared_bank_ptr &rhs) const;

		bool operator==(lak::nullptr_t) const;
		bool operator!=(lak::nullptr_t) const;
	};

	template<typename T>
	std::mutex lak::bank<T>::_mutex;
	template<typename T>
	lak::railcar<T> lak::bank<T>::_container;
	template<typename T>
	lak::array<size_t> lak::bank<T>::_deleted;
	template<typename T>
	lak::railcar<size_t> lak::shared_bank_ptr<T>::_reference_count;
}

#include "bank_ptr.inl"

#endif