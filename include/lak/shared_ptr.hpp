#ifndef LAK_SHARED_PTR_HPP
#define LAK_SHARED_PTR_HPP

#include "lak/math.hpp"
#include "lak/memmanip.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

#include "lak/reference_count.hpp"

namespace lak
{
	template<typename T, template<typename> typename META>
	struct basic_shared_ptr;
	template<typename T, template<typename> typename META>
	struct basic_shared_ref;
	template<typename T>
	struct weak_ptr;

	enum struct _shared_ptr_df : uint8_t
	{
		dealloc = 1U,
		destroy = 2U,
	};
	inline _shared_ptr_df operator|(_shared_ptr_df A, _shared_ptr_df B)
	{
		return static_cast<_shared_ptr_df>(static_cast<uint8_t>(A) |
		                                   static_cast<uint8_t>(B));
	}
	inline _shared_ptr_df operator&(_shared_ptr_df A, _shared_ptr_df B)
	{
		return static_cast<_shared_ptr_df>(static_cast<uint8_t>(A) &
		                                   static_cast<uint8_t>(B));
	}

	struct basic_shared_ptr_deleter
	{
		basic_shared_ptr_deleter(void (*del)(basic_shared_ptr_deleter *,
		                                     lak::_shared_ptr_df))
		: deleter(del)
		{
		}

		void destroy() { deleter(this, lak::_shared_ptr_df::destroy); }
		void dealloc() { deleter(this, lak::_shared_ptr_df::dealloc); }

	private:
		void (*deleter)(basic_shared_ptr_deleter *, lak::_shared_ptr_df);
	};

	struct basic_shared_ptr_ref_count
	{
		inline void inc_ref() { ++ref_count; }
		inline bool try_inc_ref() { return ref_count.non_zero_inc() != 0U; }
		inline uintptr_t internal_dec_ref() { return --ref_count; }
		inline uintptr_t load_ref() { return ref_count.load(); }

		basic_shared_ptr_ref_count() : ref_count(1U) {}

	private:
		lak::reference_count<uintptr_t> ref_count;
	};

	struct basic_shared_ptr_weak_ref_count
	{
		inline void inc_weak_ref() { ++ref_count; }
		inline bool internal_try_inc_weak_ref()
		{
			return ref_count.non_zero_inc() != 0U;
		}
		inline uintptr_t internal_dec_weak_ref() { return --ref_count; }
		inline uintptr_t load_weak_ref() { return ref_count.load(); }

		basic_shared_ptr_weak_ref_count() : ref_count(0U) {}

	private:
		lak::reference_count<uintptr_t> ref_count;
	};

	struct basic_shared_ptr_metadata : protected lak::basic_shared_ptr_deleter,
	                                   protected lak::basic_shared_ptr_ref_count
	{
		basic_shared_ptr_metadata(void (*del)(lak::basic_shared_ptr_deleter *,
		                                      lak::_shared_ptr_df))
		: lak::basic_shared_ptr_deleter(del)
		{
		}

		using lak::basic_shared_ptr_ref_count::inc_ref;
		using lak::basic_shared_ptr_ref_count::load_ref;
		inline void dec_ref()
		{
			if (internal_dec_ref() == 0U)
			{
				destroy();
				dealloc();
			}
		}
	};

	struct basic_strong_ptr_metadata
	: protected lak::basic_shared_ptr_deleter,
	  protected lak::basic_shared_ptr_ref_count,
	  protected lak::basic_shared_ptr_weak_ref_count
	{
		basic_strong_ptr_metadata(void (*del)(lak::basic_shared_ptr_deleter *,
		                                      lak::_shared_ptr_df))
		: lak::basic_shared_ptr_deleter(del)
		{
		}

		using lak::basic_shared_ptr_ref_count::inc_ref;
		using lak::basic_shared_ptr_ref_count::load_ref;
		using lak::basic_shared_ptr_ref_count::try_inc_ref;
		inline void dec_ref()
		{
			inc_weak_ref();
			if (internal_dec_ref() == 0U) destroy();
			if (internal_dec_weak_ref() == 0U && load_ref() == 0U) dealloc();
		}

		using lak::basic_shared_ptr_weak_ref_count::inc_weak_ref;
		using lak::basic_shared_ptr_weak_ref_count::load_weak_ref;
		inline bool try_inc_weak_ref()
		{
			if (try_inc_ref())
			{
				inc_weak_ref();
				if (internal_dec_ref() == 0U)
				{
					destroy();
					if (internal_dec_weak_ref() == 0U) dealloc();
					return false;
				}
				return true;
			}
			else
				return false;
		}
		inline void dec_weak_ref()
		{
			if (try_inc_ref())
			{
				if (internal_dec_ref() == 0U) destroy();
				if (internal_dec_weak_ref() == 0U && load_ref() == 0U) dealloc();
			}
			else if (internal_dec_weak_ref() == 0U)
				dealloc();
		}
	};

	template<typename T, typename META>
	struct basic_shared_ptr_allocator : public META
	{
		lak::uninitialised<T> _value;

		template<typename... ARGS>
		static basic_shared_ptr_allocator *make(ARGS &&...args)
		{
			return new basic_shared_ptr_allocator(
			  [](lak::basic_shared_ptr_deleter *p, _shared_ptr_df df)
			  {
				  auto d{static_cast<basic_shared_ptr_allocator *>(p)};

				  if ((df & _shared_ptr_df::destroy) == _shared_ptr_df::destroy)
					  d->_value.destroy();

				  if ((df & _shared_ptr_df::dealloc) == _shared_ptr_df::dealloc)
					  delete d;
			  },
			  lak::forward<ARGS>(args)...);
		}

		T *get() { return &_value.value(); }

	protected:
		template<typename... ARGS>
		basic_shared_ptr_allocator(void (*del)(lak::basic_shared_ptr_deleter *,
		                                       lak::_shared_ptr_df),
		                           ARGS &&...args)
		: META(del), _value(lak::in_place, lak::forward<ARGS>(args)...)
		{
		}
	};

	template<typename T, typename META>
	struct basic_shared_ptr_allocator<T[], META> : public META
	{
		lak::span<T> _value;

		template<typename... ARGS>
		static basic_shared_ptr_allocator *make(size_t count, ARGS &&...args)
		{
			byte_t *pre;
			byte_t *fam;
			if constexpr (alignof(basic_shared_ptr_allocator) >= alignof(T))
			{
				auto p = reinterpret_cast<byte_t *>(lak::aligned_alloc(
				  alignof(basic_shared_ptr_allocator),
				  lak::to_multiple(sizeof(basic_shared_ptr_allocator) +
				                     (sizeof(T) * count),
				                   alignof(basic_shared_ptr_allocator))));

				if (!p) return nullptr;

				pre = p;
				fam = p + sizeof(basic_shared_ptr_allocator);
			}
			else
			{
				constexpr size_t aligned_size =
				  lak::to_multiple(sizeof(basic_shared_ptr_allocator), alignof(T));

				auto p = reinterpret_cast<byte_t *>(
				  lak::aligned_alloc(alignof(T), aligned_size + (sizeof(T) * count)));

				if (!p) return nullptr;

				pre = p;
				fam = p + aligned_size;
			}

			return new (pre) basic_shared_ptr_allocator(
			  [](lak::basic_shared_ptr_deleter *p, _shared_ptr_df df)
			  {
				  auto d{static_cast<basic_shared_ptr_allocator *>(p)};
				  if ((df & _shared_ptr_df::destroy) == _shared_ptr_df::destroy)
					  for (auto &v : d->_value) v.~T();

				  if ((df & _shared_ptr_df::dealloc) == _shared_ptr_df::dealloc)
				  {
					  d->~basic_shared_ptr_allocator();
					  lak::aligned_free(d);
				  }
			  },
			  lak::span<T>(new (fam) T[count]{lak::forward<ARGS>(args)...}, count));
		}

		lak::span<T> *get() { return &_value; }

	protected:
		basic_shared_ptr_allocator(void (*del)(lak::basic_shared_ptr_deleter *,
		                                       lak::_shared_ptr_df),
		                           lak::span<T> data)
		: META(del), _value(data)
		{
		}
	};

	template<typename T, size_t N, typename META>
	struct basic_shared_ptr_allocator<T[N], META> : public META
	{
		lak::span<T, N> _value;

		template<typename... ARGS>
		static basic_shared_ptr_allocator *make(ARGS &&...args)
		{
			byte_t *pre;
			byte_t *fam;
			if constexpr (alignof(basic_shared_ptr_allocator) >= alignof(T))
			{
				auto p = reinterpret_cast<byte_t *>(lak::aligned_alloc(
				  alignof(basic_shared_ptr_allocator),
				  lak::to_multiple(sizeof(basic_shared_ptr_allocator) +
				                     (sizeof(T) * N),
				                   alignof(basic_shared_ptr_allocator))));

				if (!p) return nullptr;

				pre = p;
				fam = p + sizeof(basic_shared_ptr_allocator);
			}
			else
			{
				constexpr size_t aligned_size =
				  lak::to_multiple(sizeof(basic_shared_ptr_allocator), alignof(T));

				auto p = reinterpret_cast<byte_t *>(
				  lak::aligned_alloc(alignof(T), aligned_size + (sizeof(T) * N)));

				if (!p) return nullptr;

				pre = p;
				fam = p + aligned_size;
			}

			return new (pre) basic_shared_ptr_allocator(
			  [](lak::basic_shared_ptr_deleter *p, _shared_ptr_df df)
			  {
				  auto d{static_cast<basic_shared_ptr_allocator *>(p)};
				  if ((df & _shared_ptr_df::destroy) == _shared_ptr_df::destroy)
					  for (auto &v : d->_value) v.~T();

				  if ((df & _shared_ptr_df::dealloc) == _shared_ptr_df::dealloc)
				  {
					  d->~basic_shared_ptr_allocator();
					  lak::aligned_free(d);
				  }
			  },
			  lak::span<T, N>(new (fam) T[N]{lak::forward<ARGS>(args)...}));
		}

		lak::span<T, N> *get() { return &_value; }

	protected:
		basic_shared_ptr_allocator(void (*del)(lak::basic_shared_ptr_deleter *,
		                                       lak::_shared_ptr_df),
		                           lak::span<T, N> data)
		: META(del), _value(data)
		{
		}
	};

	template<typename T>
	struct basic_shared_ptr_ptr
	{
		using internal_value_type = T;
		using pointer             = T *;
		using reference           = T &;
	};

	template<typename T>
	struct basic_shared_ptr_ptr<T[]>
	{
		using internal_value_type = lak::span<T>;
		using pointer             = const lak::span<T> *;
		using reference           = const lak::span<T> &;
	};

	template<typename T, size_t N>
	struct basic_shared_ptr_ptr<T[N]>
	{
		using internal_value_type = lak::span<T, N>;
		using pointer             = const lak::span<T, N> *;
		using reference           = const lak::span<T, N> &;
	};

	template<typename T, typename META>
	struct basic_shared_ptr_value_type
	{
		template<typename U>
		static constexpr bool castable = lak::is_same_v<T, U>;

		using metadata_type        = META;
		using allocator_type       = lak::basic_shared_ptr_allocator<T, META>;
		allocator_type *_allocator = nullptr;

		template<typename... ARGS>
		static basic_shared_ptr_value_type make(ARGS &&...args)
		{
			return {._allocator = allocator_type::make(lak::forward<ARGS>(args)...)};
		}

		typename lak::basic_shared_ptr_ptr<T>::internal_value_type *get() const
		{
			return _allocator ? _allocator->get() : nullptr;
		}

		metadata_type *get_meta() const { return static_cast<META *>(_allocator); }

		void reset() { _allocator = nullptr; }

		template<typename U>
		lak::basic_shared_ptr_value_type<U, META> rebind()
		{
			return lak::basic_shared_ptr_value_type<U, META>{
			  ._allocator =
			    static_cast<lak::basic_shared_ptr_allocator<U, META> *>(_allocator)};
		}
	};

	template<typename T, typename META>
	struct basic_castable_shared_ptr_value_type
	{
		template<typename U>
		static constexpr bool castable = lak::concepts::static_castable<T *, U *>;

		using metadata_type      = META;
		using allocator_type     = lak::basic_shared_ptr_allocator<T, META>;
		metadata_type *_metadata = nullptr;
		lak::basic_shared_ptr_ptr<T>::internal_value_type *_value = nullptr;

		template<typename... ARGS>
		static basic_castable_shared_ptr_value_type make(ARGS &&...args)
		{
			auto a{allocator_type::make(lak::forward<ARGS>(args)...)};
			if (!a) return {._metadata = nullptr, ._value = nullptr};
			return {._metadata = static_cast<metadata_type *>(a),
			        ._value    = a->get()};
		}

		typename lak::basic_shared_ptr_ptr<T>::internal_value_type *get() const
		{
			return _value;
		}

		metadata_type *get_meta() const { return _metadata; }

		void reset()
		{
			_metadata = nullptr;
			_value    = nullptr;
		}

		template<typename U>
		lak::basic_castable_shared_ptr_value_type<U, META> rebind()
		{
			return lak::basic_castable_shared_ptr_value_type<U, META>{
			  ._metadata = _metadata, ._value = static_cast<U *>(_value)};
		}
	};

	template<typename META>
	struct basic_castable_shared_ptr_value_type<void, META>
	{
		template<typename U>
		static constexpr bool castable =
		  lak::concepts::reinterpret_castable<void *, U *>;

		using metadata_type      = META;
		metadata_type *_metadata = nullptr;
		void *_value             = nullptr;

		void *get() const { return _value; }

		metadata_type *get_meta() const { return _metadata; }

		void reset()
		{
			_metadata = nullptr;
			_value    = nullptr;
		}

		template<typename U>
		lak::basic_castable_shared_ptr_value_type<U, META> rebind()
		{
			return lak::basic_castable_shared_ptr_value_type<U, META>{
			  ._metadata = _metadata, ._value = reinterpret_cast<U *>(_value)};
		}
	};

	template<typename T>
	using _tiny_shared_ptr_value_type =
	  lak::basic_shared_ptr_value_type<T, lak::basic_shared_ptr_metadata>;

	template<typename T>
	using _shared_ptr_value_type =
	  lak::basic_castable_shared_ptr_value_type<T,
	                                            lak::basic_shared_ptr_metadata>;

	template<typename T>
	using _strong_ptr_value_type =
	  lak::basic_castable_shared_ptr_value_type<T,
	                                            lak::basic_strong_ptr_metadata>;

	template<typename T, template<typename> typename VALUE_TYPE>
	struct basic_shared_ptr
	{
	private:
		using internal_value_type = VALUE_TYPE<T>;

		internal_value_type _data;

		template<typename U, template<typename> typename M>
		friend struct basic_shared_ptr;
		template<typename U>
		friend struct weak_ptr;

		basic_shared_ptr(internal_value_type d) : _data(d) {}

		void reset(internal_value_type d)
		{
			reset();
			_data = d;
		}

		internal_value_type release()
		{
			auto d{_data};
			_data.reset();
			return d;
		}

		internal_value_type release_copy() const
		{
			if (_data.get_meta()) _data.get_meta()->inc_ref();
			return _data;
		}

	public:
		using pointer   = typename lak::basic_shared_ptr_ptr<T>::pointer;
		using reference = typename lak::basic_shared_ptr_ptr<T>::reference;

		template<typename... ARGS>
		static basic_shared_ptr make(ARGS &&...args)
		{
			return lak::basic_shared_ptr<T, VALUE_TYPE>(
			  internal_value_type::make(lak::forward<ARGS>(args)...));
		}

		basic_shared_ptr() = default;
		basic_shared_ptr(const basic_shared_ptr &other)
		: basic_shared_ptr(other.release_copy())
		{
		}
		basic_shared_ptr &operator=(const basic_shared_ptr &other)
		{
			reset(other.release_copy());
			return *this;
		}
		basic_shared_ptr(basic_shared_ptr &&other)
		: basic_shared_ptr(other.release())
		{
		}
		basic_shared_ptr &operator=(basic_shared_ptr &&other)
		{
			reset(other.release());
			return *this;
		}

		template<typename U>
		requires(VALUE_TYPE<U>::template castable<T>)
		basic_shared_ptr(const lak::basic_shared_ptr<U, VALUE_TYPE> &other)
		: basic_shared_ptr(other.release_copy().template rebind<T>())
		{
		}
		template<typename U>
		requires(VALUE_TYPE<U>::template castable<T>)
		basic_shared_ptr &operator=(
		  const lak::basic_shared_ptr<U, VALUE_TYPE> &other)
		{
			reset(other.release_copy().template rebind<T>());
			return *this;
		}
		template<typename U>
		requires(VALUE_TYPE<U>::template castable<T>)
		basic_shared_ptr(lak::basic_shared_ptr<U, VALUE_TYPE> &&other)
		: _data(other.release().template rebind<T>())
		{
		}
		template<typename U>
		requires(VALUE_TYPE<U>::template castable<T>)
		basic_shared_ptr &operator=(lak::basic_shared_ptr<U, VALUE_TYPE> &&other)
		{
			reset(other.release().template rebind<T>());
			return *this;
		}

		~basic_shared_ptr() { reset(); }

		void reset()
		{
			if (_data.get_meta()) _data.get_meta()->dec_ref();
			_data.reset();
		}

		size_t use_count() const
		{
			return _data.get_meta() ? _data.get_meta()->load_ref() : 0U;
		}

		explicit inline operator bool() const { return _data.get() != nullptr; }

		inline reference operator*() const { return *_data.get(); }

		inline pointer operator->() const { return _data.get(); }

		inline pointer get() const { return _data.get(); }
	};

	template<template<typename> typename VALUE_TYPE>
	struct basic_shared_ptr<void, VALUE_TYPE>
	{
	private:
		using internal_value_type = VALUE_TYPE<void>;

		internal_value_type _data;

		template<typename U, template<typename> typename M>
		friend struct basic_shared_ptr;
		template<typename U>
		friend struct weak_ptr;

		basic_shared_ptr(internal_value_type d) : _data(d) {}

		void reset(internal_value_type d)
		{
			reset();
			_data = d;
		}

		internal_value_type release()
		{
			auto d{_data};
			_data.reset();
			return d;
		}

		internal_value_type release_copy() const
		{
			if (_data.get_meta()) _data.get_meta()->inc_ref();
			return _data;
		}

	public:
		using value_type = void;

		basic_shared_ptr() = default;

		basic_shared_ptr(const basic_shared_ptr &other)
		: basic_shared_ptr(other.release_copy())
		{
		}

		basic_shared_ptr(basic_shared_ptr &&other)
		: basic_shared_ptr(other.release())
		{
		}

		basic_shared_ptr &operator=(basic_shared_ptr &&other)
		{
			reset(other.release());
			return *this;
		}

		template<typename U>
		requires(VALUE_TYPE<U>::template castable<void>)
		basic_shared_ptr(const lak::basic_shared_ptr<U, VALUE_TYPE> &other)
		: basic_shared_ptr(other.release_copy().template rebind<void>())
		{
		}
		template<typename U>
		requires(VALUE_TYPE<U>::template castable<void>)
		basic_shared_ptr &operator=(
		  const lak::basic_shared_ptr<U, VALUE_TYPE> &other)
		{
			reset(other.release_copy().template rebind<void>());
			return *this;
		}
		template<typename U>
		requires(VALUE_TYPE<U>::template castable<void>)
		basic_shared_ptr(lak::basic_shared_ptr<U, VALUE_TYPE> &&other)
		: _data(other.release().template rebind<void>())
		{
		}
		template<typename U>
		requires(VALUE_TYPE<U>::template castable<void>)
		basic_shared_ptr &operator=(lak::basic_shared_ptr<U, VALUE_TYPE> &&other)
		{
			reset(other.release().template rebind<void>());
			return *this;
		}

		~basic_shared_ptr() { reset(); }

		void reset()
		{
			if (_data.get_meta()) _data.get_meta()->dec_ref();
			_data.reset();
		}

		size_t use_count() const
		{
			return _data.get_meta() ? _data.get_meta()->load_ref() : 0U;
		}

		explicit inline operator bool() const { return _data.get() != nullptr; }

		inline value_type *get() const { return _data.get(); }
	};

	template<typename T>
	using tiny_shared_ptr =
	  lak::basic_shared_ptr<T, lak::_tiny_shared_ptr_value_type>;
	template<typename T>
	using shared_ptr = lak::basic_shared_ptr<T, lak::_shared_ptr_value_type>;
	template<typename T>
	using strong_ptr = lak::basic_shared_ptr<T, lak::_strong_ptr_value_type>;

	template<typename T, template<typename> typename VALUE_TYPE>
	struct basic_shared_ref
	{
	private:
		lak::basic_shared_ptr<T, VALUE_TYPE> _value;

		template<typename U, template<typename> typename M>
		friend struct basic_shared_ref;

		basic_shared_ref(lak::basic_shared_ptr<T, VALUE_TYPE> &&val)
		: _value(lak::move(val))
		{
			ASSERT(_value);
		}

	public:
		using pointer   = typename lak::basic_shared_ptr_ptr<T>::pointer;
		using reference = typename lak::basic_shared_ptr_ptr<T>::reference;

		static lak::result<basic_shared_ref> make(
		  lak::basic_shared_ptr<T, VALUE_TYPE> ptr)
		{
			if (ptr)
				return lak::ok_t(basic_shared_ref{lak::move(ptr)});
			else
				return lak::err_t{};
		}

		template<typename... ARGS>
		static lak::result<basic_shared_ref> make(ARGS &&...args)
		{
			return make(lak::basic_shared_ptr<T, VALUE_TYPE>::make(
			  lak::forward<ARGS>(args)...));
		}

		basic_shared_ref()                                    = delete;
		basic_shared_ref(const basic_shared_ref &)            = default;
		basic_shared_ref &operator=(const basic_shared_ref &) = default;

		template<typename U>
		requires(VALUE_TYPE<U>::template castable<T>)
		basic_shared_ref(const lak::basic_shared_ref<U, VALUE_TYPE> &other)
		: _value(other._value)
		{
		}
		template<typename U>
		requires(VALUE_TYPE<U>::template castable<T>)
		basic_shared_ref &operator=(
		  const lak::basic_shared_ref<U, VALUE_TYPE> &other)
		{
			_value = other._value;
			return *this;
		}

		size_t use_count() const { return _value.use_count(); }

		inline reference operator*() const { return *_value; }

		inline pointer operator->() const { return _value.get(); }

		inline pointer get() const { return _value.get(); }

		operator lak::basic_shared_ptr<T, VALUE_TYPE>() const { return _value; }
	};

	template<typename T>
	using tiny_shared_ref =
	  lak::basic_shared_ref<T, lak::_tiny_shared_ptr_value_type>;
	template<typename T>
	using shared_ref = lak::basic_shared_ref<T, lak::_shared_ptr_value_type>;
	template<typename T>
	using strong_ref = lak::basic_shared_ref<T, lak::_strong_ptr_value_type>;

	template<typename T>
	struct weak_ptr
	{
	private:
		using internal_value_type = lak::_strong_ptr_value_type<T>;
		using allocator_type      = typename internal_value_type::allocator_type;

		internal_value_type _data;

		template<typename U, template<typename> typename M>
		friend struct basic_shared_ptr;
		template<typename U>
		friend struct weak_ptr;

		weak_ptr(internal_value_type d) : _data(d) {}

		void reset(internal_value_type d)
		{
			reset();
			_data = d;
		}

		internal_value_type release()
		{
			auto d{_data};
			_data.reset();
			return d;
		}

		internal_value_type release_copy() const
		{
			if (_data.get_meta()) _data.get_meta()->inc_weak_ref();
			return _data;
		}

	public:
		weak_ptr() = default;
		weak_ptr(const weak_ptr &other) : weak_ptr(other.release_copy()) {}
		weak_ptr &operator=(const weak_ptr &other)
		{
			reset(other.release_copy());
			return *this;
		}
		weak_ptr(weak_ptr &&other) : weak_ptr(other.release()) {}
		weak_ptr &operator=(weak_ptr &&other)
		{
			reset(other.release());
			return *this;
		}

		weak_ptr(const lak::strong_ptr<T> &other)
		{
			reset();
			if (auto d = other._data;
			    d.get_meta() && d.get_meta()->try_inc_weak_ref())
				_data = d;
		}
		weak_ptr &operator=(const lak::strong_ptr<T> &other)
		{
			reset();
			if (auto d = other._data;
			    d.get_meta() && d.get_meta()->try_inc_weak_ref())
				_data = d;
			return *this;
		}

		~weak_ptr() { reset(); }

		void reset()
		{
			if (_data.get_meta()) _data.get_meta()->dec_weak_ref();
			_data.reset();
		}

		lak::strong_ptr<T> get() const
		{
			lak::strong_ptr<T> result;
			if (_data.get_meta() && _data.get_meta()->try_inc_ref())
				result.reset(_data);
			return result;
		}
	};
}

#endif
