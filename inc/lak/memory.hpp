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

	template<typename T>
	struct shared_ptr
	{
	private:
		template<typename U>
		struct internal_value_type
		{
			std::atomic_uint64_t ref_count;
			U value;
		};
		void *_data                      = nullptr;
		std::atomic_uint64_t *_ref_count = nullptr;
		T *_value                        = nullptr;
		void (*_deleter)(void *)         = nullptr;

	public:
		template<typename... ARGS>
		static shared_ptr make(ARGS &&...args);

		shared_ptr() = default;
		shared_ptr(const shared_ptr &other);
		shared_ptr &operator=(const shared_ptr &other);
		shared_ptr(shared_ptr &&other);
		shared_ptr &operator=(shared_ptr &&other);
		~shared_ptr();

		void reset();

		size_t use_count() const { return size_t(*_ref_count); }

		inline operator bool() const { return _value != nullptr; }

		inline T &operator*() const { return *_value; }

		inline T *operator->() const { return _value; }

		inline T *get() const { return _value; }
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
