#ifndef LAK_MEMORY_HPP
#define LAK_MEMORY_HPP

#include "lak/math.hpp"
#include "lak/memmanip.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

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

		unique_ref(unique_ref &&)            = default;
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

	struct _shared_ptr_metadata
	{
		lak::reference_count ref_count;
		void (*deleter)(void *);
		union
		{
			void *data;
			const void *cdata;
		};
	};

	template<typename T>
	struct _shared_ptr_value_type : public lak::_shared_ptr_metadata
	{
		T value;
	};

	template<typename T>
	struct shared_ptr
	{
	private:
		using internal_value_type = lak::_shared_ptr_value_type<T>;

		internal_value_type *_data = nullptr;

		template<typename U>
		friend struct shared_ptr;

		shared_ptr(lak::_shared_ptr_metadata *d);

		void reset(lak::_shared_ptr_metadata *d);

		lak::_shared_ptr_metadata *release();

		// increments the ref count and returns a copy of the data pointer
		lak::_shared_ptr_metadata *release_copy() const;

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
			return _data ? _data->ref_count.value.load() : 0U;
		}

		inline operator bool() const { return _data != nullptr; }

		inline T &operator*() const { return _data->value; }

		inline T *operator->() const { return &_data->value; }

		inline T *get() const { return _data ? &_data->value : nullptr; }
	};

	template<typename T>
	struct shared_ptr<T[]>
	{
	private:
		using internal_value_type = lak::_shared_ptr_value_type<lak::span<T>>;

		internal_value_type *_data = nullptr;

		template<typename U>
		friend struct shared_ptr;

		shared_ptr(lak::_shared_ptr_metadata *d)
		: _data(static_cast<internal_value_type *>(d))
		{
		}

		void reset(lak::_shared_ptr_metadata *d)
		{
			reset();
			_data = static_cast<internal_value_type *>(d);
		}

		lak::_shared_ptr_metadata *release()
		{
			return lak::exchange(_data, nullptr);
		}

		// increments the ref count and returns a copy of the data pointer
		lak::_shared_ptr_metadata *release_copy() const
		{
			if (_data) ++_data->ref_count;
			return _data;
		}

	public:
		template<typename... ARGS>
		static shared_ptr make(size_t count, ARGS &&...args)
		{
			lak::shared_ptr<T[]> result;

			do
			{
				byte_t *pre;
				byte_t *fam;
				if constexpr (alignof(internal_value_type) >= alignof(T))
				{
					auto p = reinterpret_cast<byte_t *>(lak::aligned_alloc(
					  alignof(internal_value_type),
					  lak::to_multiple(sizeof(internal_value_type) + (sizeof(T) * count),
					                   alignof(internal_value_type))));

					if (!p) break;

					pre = p;
					fam = p + sizeof(internal_value_type);
				}
				else
				{
					constexpr size_t aligned_size =
					  lak::to_multiple(sizeof(internal_value_type), alignof(T));

					auto p = reinterpret_cast<byte_t *>(lak::aligned_alloc(
					  alignof(T), aligned_size + (sizeof(T) * count)));

					if (!p) break;

					pre = p;
					fam = p + aligned_size;
				}

				result._data = new (pre) internal_value_type{
				  {
				    .ref_count{.value{1U}},
				    .deleter{
				      [](void *d)
				      {
					      if (auto data{static_cast<internal_value_type *>(
					            reinterpret_cast<lak::_shared_ptr_metadata *>(d))};
					          data && (--data->ref_count) == 0)
					      {
						      for (auto &v : data->value) v.~T();
						      data->~internal_value_type();
						      lak::aligned_free(data);
					      }
				      }},
				    .data{nullptr},
				  },
				  lak::span<T>(new (fam) T[count]{lak::forward<ARGS>(args)...}, count),
				};
			} while (false);

			if (result._data)
			{
				result._data->data = result._data->value.data();
			}

			return result;
		}

		shared_ptr() = default;
		shared_ptr(const shared_ptr &other) : shared_ptr(other.release_copy()) {}
		shared_ptr &operator=(const shared_ptr &other)
		{
			reset(other.release_copy());
			return *this;
		}
		shared_ptr(shared_ptr &&other) : shared_ptr(other.release()) {}
		shared_ptr &operator=(shared_ptr &&other)
		{
			reset(other.release());
			return *this;
		}

		~shared_ptr() { reset(); }

		void reset()
		{
			if (_data) _data->deleter(_data);
			_data = nullptr;
		}

		size_t use_count() const
		{
			return _data ? _data->ref_count.value.load() : 0U;
		}

		inline operator bool() const { return _data != nullptr; }

		inline lak::span<T> &operator*() const { return _data->value; }

		inline lak::span<T> *operator->() const { return &_data->value; }

		inline lak::span<T> *get() const
		{
			return _data ? &_data->value : nullptr;
		}
	};

	template<>
	struct shared_ptr<void>
	{
	private:
		lak::_shared_ptr_metadata *_data = nullptr;

		template<typename U>
		friend struct shared_ptr;

		shared_ptr(lak::_shared_ptr_metadata *d) : _data(d) {}

		void reset(lak::_shared_ptr_metadata *d)
		{
			reset();
			_data = d;
		}

		lak::_shared_ptr_metadata *release()
		{
			return lak::exchange(_data, nullptr);
		}

		lak::_shared_ptr_metadata *release_copy() const
		{
			if (_data) ++_data->ref_count;
			return _data;
		}

	public:
		shared_ptr() = default;

		shared_ptr(const shared_ptr &other) : shared_ptr(other.release_copy())
		{
			if (_data) ++_data->ref_count;
		}

		shared_ptr(shared_ptr &&other) : shared_ptr(other.release()) {}

		shared_ptr &operator=(shared_ptr &&other)
		{
			reset(other.release());
			return *this;
		}

		template<typename T>
		requires(!lak::is_const_v<T>)
		shared_ptr(const shared_ptr<T> &other) : shared_ptr(other.release_copy())
		{
		}

		template<typename T>
		requires(!lak::is_const_v<T>)
		shared_ptr &operator=(const shared_ptr<T> &other)
		{
			reset(other.release_copy());
			return *this;
		}

		template<typename T>
		requires(requires { lak::declval<T>().release(); } && !lak::is_const_v<T>)
		shared_ptr(shared_ptr<T> &&other) : shared_ptr(other.release())
		{
		}

		~shared_ptr() { reset(); }

		void reset()
		{
			if (_data) _data->deleter(_data);
			_data = nullptr;
		}

		size_t use_count() const
		{
			return _data ? _data->ref_count.value.load() : 0U;
		}

		inline operator bool() const { return _data != nullptr; }

		inline void *get() const { return _data ? _data->data : nullptr; }

		template<typename T>
		explicit operator shared_ptr<T>() const
		{
			return lak::shared_ptr<T>(release_copy());
		}
	};

	template<>
	struct shared_ptr<const void>
	{
	private:
		lak::_shared_ptr_metadata *_data = nullptr;

		template<typename U>
		friend struct shared_ptr;

		shared_ptr(lak::_shared_ptr_metadata *d) : _data(d) {}

		void reset(lak::_shared_ptr_metadata *d)
		{
			reset();
			_data = d;
		}

		lak::_shared_ptr_metadata *release()
		{
			return lak::exchange(_data, nullptr);
		}

		lak::_shared_ptr_metadata *release_copy() const
		{
			if (_data) ++_data->ref_count;
			return _data;
		}

	public:
		shared_ptr() = default;

		shared_ptr(const shared_ptr &other) : shared_ptr(other.release_copy()) {}

		shared_ptr(shared_ptr &&other) : shared_ptr(other.release()) {}

		shared_ptr &operator=(shared_ptr &&other)
		{
			reset(other.release());
			return *this;
		}

		template<typename T>
		shared_ptr(const shared_ptr<T> &other) : shared_ptr(other.release_copy())
		{
		}

		template<typename T>
		shared_ptr &operator=(const shared_ptr<T> &other)
		{
			reset(other.release_copy());
			return *this;
		}

		template<typename T>
		requires(requires { lak::declval<T>().release(); })
		shared_ptr(shared_ptr<T> &&other) : shared_ptr(other.release())
		{
		}

		~shared_ptr() { reset(); }

		void reset()
		{
			if (_data) _data->deleter(_data);
			_data = nullptr;
		}

		size_t use_count() const
		{
			return _data ? _data->ref_count.value.load() : 0U;
		}

		inline operator bool() const { return _data != nullptr; }

		inline const void *get() const { return _data ? _data->cdata : nullptr; }

		template<typename T>
		operator shared_ptr<const T>() const
		{
			return lak::shared_ptr<const T>(release_copy());
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

		shared_ref()                              = delete;
		shared_ref(const shared_ref &)            = default;
		shared_ref(shared_ref &&)                 = default;
		shared_ref &operator=(const shared_ref &) = default;
		shared_ref &operator=(shared_ref &&)      = default;

		size_t use_count() const { return _value.use_count(); }

		inline T &operator*() const { return *_value; }

		inline T *operator->() const { return _value.get(); }

		inline T *get() const { return _value.get(); }
	};
}

#include "lak/memory.inl"

#endif
