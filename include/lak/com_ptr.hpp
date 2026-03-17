#ifndef LAK_COM_PTR_HPP
#define LAK_COM_PTR_HPP

#include "lak/reference_count.hpp"
#include "lak/result.hpp"
#include "lak/utility.hpp"

namespace lak
{
	/* --- unique_com_ptr --- */

	template<typename T>
	struct unique_com_ptr_traits
	{
		using handle_type  = typename T::handle_type;
		using exposed_type = typename T::exposed_type;

		template<typename... ARGS>
		inline static auto ctor(ARGS &&...args)
		{
			return T::ctor(lak::forward<ARGS>(args)...);
		}

		inline static void dtor(handle_type handle) { return T::dtor(handle); }
	};

	template<typename T, typename TRAITS>
	struct basic_unique_com_ptr
	{
		using handle_type  = typename TRAITS::handle_type;
		using exposed_type = typename TRAITS::exposed_type;

		basic_unique_com_ptr() = default;

		basic_unique_com_ptr(basic_unique_com_ptr &&other)
		: _handle(lak::exchange(other._handle, nullptr))
		{
		}

		basic_unique_com_ptr &operator=(basic_unique_com_ptr &&other)
		{
			lak::swap(_handle, other._handle);
			return *this;
		}

		~basic_unique_com_ptr() { reset(); }

		template<typename... ARGS>
		static auto create(ARGS &&...args)
		{
			return lak::unwrap_if_infallible(
			  TRAITS::ctor(lak::forward<ARGS>(args)...).map(&creator));
		}

		template<typename... ARGS>
		auto emplace(ARGS &&...args)
		{
			return TRAITS::ctor(lak::forward<ARGS>(args)...)
			  .map(
			    [&](handle_type handle)
			    {
				    this->reset(handle);
				    return lak::monostate{};
			    });
		}

		void reset() { reset(nullptr); }
		void reset(handle_type handle)
		{
			if (_handle) TRAITS::dtor(_handle);
			_handle = handle;
		}
		handle_type release() { return lak::exchange(_handle, nullptr); }

		operator handle_type() const { return _handle; }
		exposed_type operator->() const { return _handle; }
		exposed_type get() const { return _handle; }

	private:
		basic_unique_com_ptr(handle_type handle) : _handle(handle) {}

		static basic_unique_com_ptr creator(handle_type handle)
		{
			return basic_unique_com_ptr(handle);
		}

		handle_type _handle = nullptr;
	};

	template<typename T>
	using unique_com_ptr =
	  lak::basic_unique_com_ptr<T, lak::unique_com_ptr_traits<T>>;

	/* --- shared_com_ptr --- */

	template<typename T>
	struct shared_com_ptr_traits
	{
		using handle_type  = typename T::handle_type;
		using exposed_type = typename T::exposed_type;

		template<typename... ARGS>
		inline static auto ctor(ARGS &&...args)
		{
			return T::ctor(lak::forward<ARGS>(args)...);
		}

		inline static handle_type ref(handle_type handle)
		{
			return T::ref(handle);
		}

		inline static void unref(handle_type handle) { return T::unref(handle); }
	};

	template<typename TRAITS>
	struct ref_count_com_ptr_adapter
	{
		using exposed_type   = typename TRAITS::exposed_type;
		using ref_count_type = lak::reference_count<size_t>;
		struct handle_type
		{
			using value_type = typename TRAITS::handle_type;

			ref_count_type *ref_count;
			value_type handle;

			handle_type() : ref_count(nullptr), handle(nullptr) {}
			handle_type(nullptr_t) : handle_type() {}
			handle_type(ref_count_type *rc, value_type h) : ref_count(rc), handle(h)
			{
			}

			operator value_type() const { return handle; }
		};

		template<typename... ARGS>
		inline static auto ctor(ARGS &&...args)
		{
			auto rc = new ref_count_type(1U);
			DEFER(if (rc) delete rc);
			return TRAITS::ctor(lak::forward<ARGS>(args)...)
			  .map([&](typename handle_type::value_type handle) -> handle_type
			       { return {lak::exchange(rc, nullptr), handle}; });
		}

		inline static handle_type ref(handle_type handle)
		{
			++*handle.ref_count;
			return handle;
		}

		inline static void unref(handle_type handle)
		{
			if (!--*handle.ref_count)
			{
				delete handle.ref_count;
				return TRAITS::dtor(handle.handle);
			}
		}
	};

	template<typename T, typename TRAITS>
	struct basic_shared_com_ptr
	{
		using handle_type  = typename TRAITS::handle_type;
		using exposed_type = typename TRAITS::exposed_type;

		basic_shared_com_ptr() = default;

		basic_shared_com_ptr(basic_shared_com_ptr &&other)
		: _handle(lak::exchange(other._handle, nullptr))
		{
		}

		basic_shared_com_ptr(const basic_shared_com_ptr &other)
		: _handle(TRAITS::ref(other._handle))
		{
		}

		basic_shared_com_ptr &operator=(basic_shared_com_ptr &&other)
		{
			lak::swap(_handle, other._handle);
			return *this;
		}

		basic_shared_com_ptr &operator=(const basic_shared_com_ptr &other)
		{
			reset(TRAITS::ref(other._handle));
			return *this;
		}

		~basic_shared_com_ptr() { reset(); }

		template<typename... ARGS>
		static auto create(ARGS &&...args)
		{
			return lak::unwrap_if_infallible(
			  TRAITS::ctor(lak::forward<ARGS>(args)...).map(&creator));
		}

		template<typename... ARGS>
		auto emplace(ARGS &&...args)
		{
			return TRAITS::ctor(lak::forward<ARGS>(args)...)
			  .map(
			    [&](handle_type handle)
			    {
				    this->reset(handle);
				    return lak::monostate{};
			    });
		}

		void reset() { reset(nullptr); }

		operator handle_type() const { return _handle; }
		exposed_type operator->() const { return _handle; }
		exposed_type get() const { return _handle; }

	private:
		basic_shared_com_ptr(handle_type handle) : _handle(handle) {}

		static basic_shared_com_ptr creator(handle_type handle)
		{
			return basic_shared_com_ptr(handle);
		}

		void reset(handle_type handle)
		{
			if (_handle) TRAITS::unref(_handle);
			_handle = handle;
		}

		handle_type _handle = nullptr;
	};

	template<typename T>
	using shared_com_ptr =
	  lak::basic_shared_com_ptr<T, lak::shared_com_ptr_traits<T>>;

	template<typename T>
	using ref_count_com_ptr = lak::basic_shared_com_ptr<
	  T,
	  lak::ref_count_com_ptr_adapter<lak::unique_com_ptr_traits<T>>>;
}

#endif
