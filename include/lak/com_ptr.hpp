#ifndef LAK_COM_PTR_HPP
#define LAK_COM_PTR_HPP

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

		template<typename... ARGS>
		inline static void dtor(handle_type handle)
		{
			return T::dtor(handle);
		}
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

		void reset()
		{
			if (_handle)
			{
				lak::unique_com_ptr_traits<T>::dtor(_handle);
				_handle = nullptr;
			}
		}

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

		template<typename... ARGS>
		inline static handle_type ref(handle_type handle)
		{
			return T::ref(handle);
		}

		template<typename... ARGS>
		inline static void unref(handle_type handle)
		{
			return T::unref(handle);
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
			lak::shared_com_ptr_traits<T>::unref(_handle);
			_handle = lak::shared_com_ptr_traits<T>::ref(other._handle);
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

		void reset()
		{
			if (_handle)
			{
				lak::shared_com_ptr_traits<T>::unref(_handle);
				_handle = nullptr;
			}
		}

		operator handle_type() const { return _handle; }
		exposed_type operator->() const { return _handle; }
		exposed_type get() const { return _handle; }

	private:
		shared_com_ptr(handle_type handle) : _handle(handle) {}

		static shared_com_ptr creator(handle_type handle)
		{
			return shared_com_ptr(handle);
		}

		handle_type _handle = nullptr;
	};
}

#endif
