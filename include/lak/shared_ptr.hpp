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
	template<typename T, typename META>
	struct basic_shared_ptr;
	template<typename T, typename META>
	struct basic_shared_ref;
	template<typename T, typename META>
	struct basic_weak_ptr;

	enum struct _shared_ptr_df : uint8_t
	{
		do_delete  = 1U,
		do_destroy = 2U,
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

	struct basic_shared_ptr_metadata
	{
		void (*deleter)(basic_shared_ptr_metadata *, _shared_ptr_df);
	};

	template<typename T, typename META>
	struct basic_shared_ptr_value_type : public META
	{
		T value;

		template<typename... ARGS>
		basic_shared_ptr_value_type(
		  void (*deleter)(lak::basic_shared_ptr_metadata *, _shared_ptr_df),
		  ARGS &&...args)
		: META(deleter), value(lak::forward<ARGS>(args)...)
		{
		}
	};

	struct _tiny_shared_ptr_metadata : lak::basic_shared_ptr_metadata
	{
		inline void inc_ref() { ++ref_count; }
		inline void dec_ref();
		inline size_t load_ref() { return ref_count.load(); }

		inline _tiny_shared_ptr_metadata(
		  void (*del)(lak::basic_shared_ptr_metadata *, _shared_ptr_df))
		: lak::basic_shared_ptr_metadata(del), ref_count({1U})
		{
		}

		void set_data(void *) {}
		void set_data(const void *) {}

		template<typename T>
		using value_type =
		  lak::basic_shared_ptr_value_type<T, _tiny_shared_ptr_metadata>;

	private:
		lak::reference_count ref_count;
	};

	struct _shared_ptr_metadata : public lak::basic_shared_ptr_metadata
	{
		inline void inc_ref() { ++ref_count; }
		inline void dec_ref();
		inline size_t load_ref() { return ref_count.load(); }

		inline _shared_ptr_metadata(void (*del)(lak::basic_shared_ptr_metadata *,
		                                        _shared_ptr_df))
		: lak::basic_shared_ptr_metadata(del), data(nullptr), ref_count({1U})
		{
		}

		inline void set_data(void *p) { data = p; }
		inline void set_data(const void *p)
		{
			using vp  = void *;
			using cvp = const void *;
			data.~vp();
			new (&cdata) cvp(p);
		}

		union
		{
			void *data;
			const void *cdata;
		};

		template<typename T>
		using value_type =
		  lak::basic_shared_ptr_value_type<T, _shared_ptr_metadata>;

	private:
		lak::reference_count ref_count;
	};

	struct _strong_shared_ptr_metadata : lak::basic_shared_ptr_metadata
	{
		inline void inc_ref() { ++ref_count; }
		inline bool try_inc_ref() { return ref_count.non_zero_inc() != 0U; }
		inline void inc_weak_ref() { ++weak_ref_count; }
		inline void dec_ref();
		inline void dec_weak_ref();
		inline size_t load_ref() { return ref_count.load(); }
		inline size_t load_weak_ref() { return weak_ref_count.load(); }

		inline _strong_shared_ptr_metadata(
		  void (*del)(lak::basic_shared_ptr_metadata *, _shared_ptr_df))
		: lak::basic_shared_ptr_metadata(del),
		  data(nullptr),
		  ref_count({1U}),
		  weak_ref_count({0U})
		{
		}

		inline void set_data(void *p) { data = p; }
		inline void set_data(const void *p)
		{
			using vp  = void *;
			using cvp = const void *;
			data.~vp();
			new (&cdata) cvp(p);
		}

		void (*destroyer)(lak::basic_shared_ptr_metadata *, _shared_ptr_df);
		union
		{
			void *data;
			const void *cdata;
		};

		template<typename T>
		using value_type =
		  lak::basic_shared_ptr_value_type<T, _strong_shared_ptr_metadata>;

	private:
		lak::reference_count ref_count;
		lak::reference_count weak_ref_count;
	};

	template<typename T, typename META>
	struct basic_shared_ptr
	{
	private:
		using internal_value_type =
		  typename META::template value_type<lak::uninitialised<T>>;

		META *_data = nullptr;

		template<typename U, typename M>
		friend struct basic_shared_ptr;
		template<typename U, typename M>
		friend struct basic_weak_ptr;

		basic_shared_ptr(META *d);

		void reset(META *d);

		META *release();

		// increments the ref count and returns a copy of the data pointer
		META *release_copy() const;

		internal_value_type *_get() const
		{
			return static_cast<internal_value_type *>(_data);
		}

	public:
		using value_type = T;

		template<typename... ARGS>
		static basic_shared_ptr make(ARGS &&...args);

		basic_shared_ptr() = default;
		basic_shared_ptr(const basic_shared_ptr &other);
		basic_shared_ptr &operator=(const basic_shared_ptr &other);
		basic_shared_ptr(basic_shared_ptr &&other);
		basic_shared_ptr &operator=(basic_shared_ptr &&other);

		~basic_shared_ptr() { reset(); }

		void reset();

		size_t use_count() const { return _data ? _data->load_ref() : 0U; }

		explicit inline operator bool() const { return _data != nullptr; }

		inline value_type &operator*() const { return _get()->value.value(); }

		inline value_type *operator->() const { return &(_get()->value.value()); }

		inline value_type *get() const
		{
			return _data ? &(_get()->value.value()) : nullptr;
		}
	};

	template<typename T, typename META>
	struct basic_shared_ptr<T[], META>
	{
	private:
		using internal_value_type =
		  typename META::template value_type<lak::span<T>>;

		internal_value_type *_data = nullptr;

		template<typename U, typename M>
		friend struct basic_shared_ptr;
		template<typename U, typename M>
		friend struct basic_weak_ptr;

		basic_shared_ptr(META *d) : _data(static_cast<internal_value_type *>(d)) {}

		void reset(META *d)
		{
			reset();
			_data = static_cast<internal_value_type *>(d);
		}

		META *release() { return lak::exchange(_data, nullptr); }

		// increments the ref count and returns a copy of the data pointer
		META *release_copy() const
		{
			if (_data) _data->inc_ref();
			return _data;
		}

	public:
		using value_type = const lak::span<T>;

		template<typename... ARGS>
		static basic_shared_ptr make(size_t count, ARGS &&...args)
		{
			lak::basic_shared_ptr<T[], META> result;

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

				result._data = new (pre) internal_value_type(
				  [](lak::basic_shared_ptr_metadata *p, _shared_ptr_df df)
				  {
					  auto d{static_cast<internal_value_type *>(p)};
					  if ((df & _shared_ptr_df::do_destroy) ==
					      _shared_ptr_df::do_destroy)
						  for (auto &v : d->value) v.~T();
					  if ((df & _shared_ptr_df::do_delete) == _shared_ptr_df::do_delete)
					  {
						  d->~internal_value_type();
						  lak::aligned_free(d);
					  }
				  },
				  lak::span<T>(new (fam) T[count]{lak::forward<ARGS>(args)...},
				               count));
			} while (false);

			if (result._data) result._data->set_data(result._data->value.data());

			return result;
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

		~basic_shared_ptr() { reset(); }

		void reset()
		{
			if (_data) _data->dec_ref();
			_data = nullptr;
		}

		size_t use_count() const { return _data ? _data->load_ref() : 0U; }

		explicit inline operator bool() const { return _data != nullptr; }

		inline value_type &operator*() const { return _data->value; }

		inline value_type *operator->() const { return &_data->value; }

		inline value_type *get() const { return _data ? &_data->value : nullptr; }
	};

	template<typename META>
	struct basic_shared_ptr<void, META>
	{
	private:
		META *_data = nullptr;

		template<typename U, typename M>
		friend struct basic_shared_ptr;
		template<typename U, typename M>
		friend struct basic_weak_ptr;

		basic_shared_ptr(META *d) : _data(d) {}

		void reset(META *d)
		{
			reset();
			_data = d;
		}

		META *release() { return lak::exchange(_data, nullptr); }

		META *release_copy() const
		{
			if (_data) _data->inc_ref();
			return _data;
		}

	public:
		using value_type = void;

		basic_shared_ptr() = default;

		basic_shared_ptr(const basic_shared_ptr &other)
		: basic_shared_ptr(other.release_copy())
		{
			if (_data) _data->inc_ref();
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

		template<typename T>
		requires(!lak::is_const_v<T>)
		basic_shared_ptr(const lak::basic_shared_ptr<T, META> &other)
		: basic_shared_ptr(other.release_copy())
		{
		}

		template<typename T>
		requires(!lak::is_const_v<T>)
		basic_shared_ptr &operator=(const lak::basic_shared_ptr<T, META> &other)
		{
			reset(other.release_copy());
			return *this;
		}

		template<typename T>
		requires(requires { lak::declval<T>().release(); } && !lak::is_const_v<T>)
		basic_shared_ptr(lak::basic_shared_ptr<T, META> &&other)
		: basic_shared_ptr(other.release())
		{
		}

		~basic_shared_ptr() { reset(); }

		void reset()
		{
			if (_data) _data->dec_ref();
			_data = nullptr;
		}

		size_t use_count() const { return _data ? _data->load_ref() : 0U; }

		explicit inline operator bool() const { return _data != nullptr; }

		inline value_type *get() const { return _data ? _data->data : nullptr; }

		template<typename T>
		explicit operator lak::basic_shared_ptr<T, META>() const
		{
			return lak::basic_shared_ptr<T, META>(release_copy());
		}
	};

	template<typename META>
	struct basic_shared_ptr<const void, META>
	{
	private:
		META *_data = nullptr;

		template<typename U, typename M>
		friend struct basic_shared_ptr;
		template<typename U, typename M>
		friend struct basic_weak_ptr;

		basic_shared_ptr(META *d) : _data(d) {}

		void reset(META *d)
		{
			reset();
			_data = d;
		}

		META *release() { return lak::exchange(_data, nullptr); }

		META *release_copy() const
		{
			if (_data) _data->inc_ref();
			return _data;
		}

	public:
		using value_type = const void;

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

		template<typename T>
		basic_shared_ptr(const lak::basic_shared_ptr<T, META> &other)
		: basic_shared_ptr(other.release_copy())
		{
		}

		template<typename T>
		basic_shared_ptr &operator=(const lak::basic_shared_ptr<T, META> &other)
		{
			reset(other.release_copy());
			return *this;
		}

		template<typename T>
		requires(requires { lak::declval<T>().release(); })
		basic_shared_ptr(lak::basic_shared_ptr<T, META> &&other)
		: basic_shared_ptr(other.release())
		{
		}

		~basic_shared_ptr() { reset(); }

		void reset()
		{
			if (_data) _data->dec_ref();
			_data = nullptr;
		}

		size_t use_count() const { return _data ? _data->load_ref() : 0U; }

		explicit inline operator bool() const { return _data != nullptr; }

		inline value_type *get() const { return _data ? _data->cdata : nullptr; }

		template<typename T>
		operator lak::basic_shared_ptr<const T, META>() const
		{
			return lak::basic_shared_ptr<const T, META>(release_copy());
		}
	};

	template<typename T, typename META>
	struct basic_shared_ref
	{
	private:
		lak::basic_shared_ptr<T, META> _value;

		basic_shared_ref(lak::basic_shared_ptr<T, META> &&val);

	public:
		using value_type = typename lak::basic_shared_ptr<T, META>::value_type;

		static lak::result<basic_shared_ref> make(
		  lak::basic_shared_ptr<T, META> ptr);

		template<typename... ARGS>
		static lak::result<basic_shared_ref> make(ARGS &&...args);

		basic_shared_ref()                                    = delete;
		basic_shared_ref(const basic_shared_ref &)            = default;
		basic_shared_ref(basic_shared_ref &&)                 = default;
		basic_shared_ref &operator=(const basic_shared_ref &) = default;
		basic_shared_ref &operator=(basic_shared_ref &&)      = default;

		size_t use_count() const { return _value.use_count(); }

		inline value_type &operator*() const { return *_value; }

		inline value_type *operator->() const { return _value.get(); }

		inline value_type *get() const { return _value.get(); }
	};

	template<typename T, typename META>
	struct basic_weak_ptr
	{
	private:
		using internal_value_type =
		  typename META::template value_type<lak::uninitialised<T>>;

		META *_data = nullptr;

		template<typename U, typename M>
		friend struct basic_shared_ptr;
		template<typename U, typename M>
		friend struct basic_weak_ptr;

		basic_weak_ptr(META *d);

		void reset(META *d);

		META *release();

		// increments the ref count and returns a copy of the data pointer
		META *release_copy() const;

		internal_value_type *_get() const
		{
			return static_cast<internal_value_type *>(_data);
		}

	public:
		basic_weak_ptr() = default;
		basic_weak_ptr(const basic_weak_ptr &other);
		basic_weak_ptr &operator=(const basic_weak_ptr &other);
		basic_weak_ptr(basic_weak_ptr &&other);
		basic_weak_ptr &operator=(basic_weak_ptr &&other);

		basic_weak_ptr(const lak::basic_shared_ptr<T, META> &other);
		basic_weak_ptr &operator=(const lak::basic_shared_ptr<T, META> &other);

		~basic_weak_ptr() { reset(); }

		void reset();

		lak::basic_shared_ptr<T, META> get() const;
	};
	// :TODO: (apple-)clang doesn't like type deduction on aliases.
	// template<typename T, typename META>
	// basic_weak_ptr(const lak::basic_shared_ptr<T, META> &)
	//   -> basic_weak_ptr<T, META>;

	template<typename T>
	using tiny_shared_ptr = lak::basic_shared_ptr<T, _tiny_shared_ptr_metadata>;
	template<typename T>
	using tiny_shared_ref = lak::basic_shared_ref<T, _tiny_shared_ptr_metadata>;

	template<typename T>
	using shared_ptr = lak::basic_shared_ptr<T, _shared_ptr_metadata>;
	template<typename T>
	using shared_ref = lak::basic_shared_ref<T, _shared_ptr_metadata>;

	template<typename T>
	using strong_ptr = lak::basic_shared_ptr<T, _strong_shared_ptr_metadata>;
	template<typename T>
	using strong_ref = lak::basic_shared_ref<T, _strong_shared_ptr_metadata>;
	template<typename T>
	using weak_ptr = lak::basic_weak_ptr<T, _strong_shared_ptr_metadata>;
}

#include "lak/shared_ptr.inl"

#endif
