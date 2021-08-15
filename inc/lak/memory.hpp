#ifndef LAK_MEMORY_HPP
#define LAK_MEMORY_HPP

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
		unique_ptr(unique_ptr &&);
		unique_ptr &operator=(unique_ptr &&);
		~unique_ptr();

		void reset();

		inline operator bool() const { return _value; }

		inline T &operator*() { return *_value; }
		inline const T &operator*() const { return *_value; }

		inline T *operator->() { return _value; }
		inline const T *operator->() const { return _value; }

		inline T *get() { return _value; }
		inline const T *get() const { return _value; }
	};
}

#include "lak/memory.inl"

#endif
