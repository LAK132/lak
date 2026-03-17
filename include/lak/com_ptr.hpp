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

	template<typename T>
	struct unique_com_ptr
	{
		using handle_type  = typename lak::unique_com_ptr_traits<T>::handle_type;
		using exposed_type = typename lak::unique_com_ptr_traits<T>::exposed_type;

		unique_com_ptr() = default;

		unique_com_ptr(unique_com_ptr &&other)
		: _handle(lak::exchange(other._handle, nullptr))
		{
		}

		unique_com_ptr &operator=(unique_com_ptr &&other)
		{
			lak::swap(_handle, other._handle);
			return *this;
		}

		~unique_com_ptr() { reset(); }

		template<typename... ARGS>
		static auto create(ARGS &&...args)
		{
			return lak::unwrap_if_infallible(
			  lak::unique_com_ptr_traits<T>::ctor(lak::forward<ARGS>(args)...)
			    .map(&creator));
		}

		template<typename... ARGS>
		auto emplace(ARGS &&...args)
		{
			return lak::unique_com_ptr_traits<T>::ctor(lak::forward<ARGS>(args)...)
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
			if (_handle) lak::unique_com_ptr_traits<T>::dtor(_handle);
			_handle = handle;
		}
		handle_type release() { return lak::exchange(_handle, nullptr); }

		operator handle_type() const { return _handle; }
		exposed_type operator->() const { return _handle; }
		exposed_type get() const { return _handle; }

	private:
		unique_com_ptr(handle_type handle) : _handle(handle) {}

		static unique_com_ptr creator(handle_type handle)
		{
			return unique_com_ptr(handle);
		}

		handle_type _handle = nullptr;
	};

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

	// usage:
	// template<>
	// struct lak::unique_com_ptr<my_type>
	// {
	// 	// implement as usual
	// };
	// template<>
	// struct lak::shared_com_ptr_traits<my_type>
	// : public lak::ref_count_unique_com_ptr_traits_impl<my_type> {};
	template<typename T>
	struct ref_count_unique_com_ptr_traits_impl
	{
		using exposed_type = typename lak::unique_com_ptr_traits<T>::exposed_type;
		using ref_count_type = lak::reference_count<size_t>;
		struct handle_type
		{
			using value_type = typename lak::unique_com_ptr_traits<T>::handle_type;

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
			return lak::unique_com_ptr_traits<T>::ctor(lak::forward<ARGS>(args)...)
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
				return lak::unique_com_ptr_traits<T>::dtor(handle.handle);
			}
		}
	};

	template<typename T>
	struct shared_com_ptr
	{
		using handle_type  = typename lak::shared_com_ptr_traits<T>::handle_type;
		using exposed_type = typename lak::shared_com_ptr_traits<T>::exposed_type;

		shared_com_ptr() = default;

		shared_com_ptr(shared_com_ptr &&other)
		: _handle(lak::exchange(other._handle, nullptr))
		{
		}

		shared_com_ptr(const shared_com_ptr &other)
		: _handle(lak::shared_com_ptr_traits<T>::ref(other._handle))
		{
		}

		shared_com_ptr &operator=(shared_com_ptr &&other)
		{
			lak::swap(_handle, other._handle);
			return *this;
		}

		shared_com_ptr &operator=(const shared_com_ptr &other)
		{
			reset(lak::shared_com_ptr_traits<T>::ref(other._handle));
			return *this;
		}

		~shared_com_ptr() { reset(); }

		template<typename... ARGS>
		static auto create(ARGS &&...args)
		{
			return lak::unwrap_if_infallible(
			  lak::shared_com_ptr_traits<T>::ctor(lak::forward<ARGS>(args)...)
			    .map(&creator));
		}

		template<typename... ARGS>
		auto emplace(ARGS &&...args)
		{
			return lak::shared_com_ptr_traits<T>::ctor(lak::forward<ARGS>(args)...)
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
		shared_com_ptr(handle_type handle) : _handle(handle) {}

		static shared_com_ptr creator(handle_type handle)
		{
			return shared_com_ptr(handle);
		}

		void reset(handle_type handle)
		{
			if (_handle) lak::shared_com_ptr_traits<T>::unref(_handle);
			_handle = handle;
		}

		handle_type _handle = nullptr;
	};
}

#endif
