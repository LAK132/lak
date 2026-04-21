#ifndef LAK_COM_PTR_HPP
#define LAK_COM_PTR_HPP

#include "lak/concepts.hpp"
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

		static constexpr auto null_value = T::null_value;

		template<typename... ARGS>
		inline static auto ctor(ARGS &&...args)
		{
			return T::ctor(lak::forward<ARGS>(args)...);
		}

		inline static void dtor(handle_type &handle)
		{
			T::dtor(handle);
			handle = null_value;
		}

		inline static bool valid(const handle_type &handle)
		{
			return handle != null_value;
		}
	};

	namespace concepts
	{
		template<typename T>
		concept unique_com_ptr_traits = requires {
			typename T::handle_type;
			typename T::exposed_type;

			{ typename T::handle_type(T::null_value) };
			{ lak::declval<typename T::handle_type &>() = T::null_value };

			// { T::ctor(...) } -> lak::concepts::of_template<lak::result>;
			{
				T::dtor(lak::declval<typename T::handle_type &>())
			} -> lak::concepts::void_type;
			{
				T::valid(lak::declval<const typename T::handle_type &>())
			} -> lak::concepts::same_as<bool>;
		};
	};

	template<typename T,
	         lak::concepts::unique_com_ptr_traits TRAITS =
	           lak::unique_com_ptr_traits<T>>
	struct basic_unique_com_ptr
	{
		using handle_type  = typename TRAITS::handle_type;
		using exposed_type = typename TRAITS::exposed_type;

		basic_unique_com_ptr() = default;

		basic_unique_com_ptr(basic_unique_com_ptr &&other)
		: _handle(lak::exchange(other._handle, TRAITS::null_value))
		{
		}

		basic_unique_com_ptr &operator=(basic_unique_com_ptr &&other)
		{
			lak::swap(_handle, other._handle);
			return *this;
		}

		~basic_unique_com_ptr() { reset(); }

		template<typename... ARGS>
		static auto make(ARGS &&...args)
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

		void reset() { reset(TRAITS::null_value); }
		void reset(handle_type handle)
		{
			if (TRAITS::valid(_handle)) TRAITS::dtor(_handle);
			_handle = handle;
		}
		handle_type release()
		{
			return lak::exchange(_handle, TRAITS::null_value);
		}

		explicit operator bool() const { return TRAITS::valid(_handle); }
		operator handle_type() const { return _handle; }
		exposed_type operator->() const { return _handle; }
		exposed_type get() const { return _handle; }

	private:
		basic_unique_com_ptr(handle_type handle) : _handle(handle) {}

		static basic_unique_com_ptr creator(handle_type handle)
		{
			return basic_unique_com_ptr(handle);
		}

		handle_type _handle = TRAITS::null_value;
	};

	template<typename T>
	using unique_com_ptr = lak::basic_unique_com_ptr<T>;

	/* --- shared_com_ptr --- */

	template<typename T>
	struct shared_com_ptr_traits
	{
		using handle_type  = typename T::handle_type;
		using exposed_type = typename T::exposed_type;

		static constexpr auto null_value = T::null_value;

		template<typename... ARGS>
		inline static auto ctor(ARGS &&...args)
		{
			return T::ctor(lak::forward<ARGS>(args)...);
		}

		inline static handle_type ref(const handle_type &handle)
		{
			return T::ref(handle);
		}

		inline static void unref(const handle_type &handle)
		{
			return T::unref(handle);
		}

		inline static bool valid(const handle_type &handle)
		{
			return handle != null_value;
		}
	};

	namespace concepts
	{
		template<typename T>
		concept shared_com_ptr_traits = requires {
			typename T::handle_type;
			typename T::exposed_type;

			{ typename T::handle_type(T::null_value) };
			{ lak::declval<typename T::handle_type &>() = T::null_value };

			// { T::ctor(...) } -> lak::concepts::of_template<lak::result>;
			{
				T::ref(lak::declval<const typename T::handle_type &>())
			} -> lak::concepts::same_as<typename T::handle_type>;
			{
				T::unref(lak::declval<const typename T::handle_type &>())
			} -> lak::concepts::void_type;
			{
				T::valid(lak::declval<const typename T::handle_type &>())
			} -> lak::concepts::same_as<bool>;
		};
	};

	template<lak::concepts::unique_com_ptr_traits TRAITS>
	struct ref_count_com_ptr_adapter
	{
		using exposed_type   = typename TRAITS::exposed_type;
		using ref_count_type = lak::reference_count<size_t>;
		struct handle_type
		{
			using value_type = typename TRAITS::handle_type;

		private:
			ref_count_type *ref_count = nullptr;
			value_type handle         = TRAITS::null_value;
			friend struct ref_count_com_ptr_adapter;

		public:
			handle_type()                    = default;
			handle_type(const handle_type &) = default;
			handle_type(nullptr_t) {}
			handle_type(ref_count_type *rc, value_type h) : ref_count(rc), handle(h)
			{
			}

			operator value_type &() { return handle; }
			operator const value_type &() const { return handle; }
		};

		static constexpr nullptr_t null_value{};

		template<typename... ARGS>
		inline static auto ctor(ARGS &&...args)
		{
			auto rc = new ref_count_type(1U);
			DEFER(if (rc) delete rc);
			return TRAITS::ctor(lak::forward<ARGS>(args)...)
			  .map([&](typename handle_type::value_type handle) -> handle_type
			       { return {lak::exchange(rc, nullptr), handle}; });
		}

		inline static handle_type ref(const handle_type &handle)
		{
			++*handle.ref_count;
			return handle;
		}

		inline static void unref(const handle_type &handle)
		{
			if (!--*handle.ref_count)
			{
				delete handle.ref_count;
				TRAITS::dtor(handle.handle);
			}
		}

		inline static bool valid(const handle_type &handle)
		{
			return handle.ref_count != nullptr;
		}
	};

	template<typename T,
	         lak::concepts::shared_com_ptr_traits TRAITS =
	           lak::shared_com_ptr_traits<T>>
	struct basic_shared_com_ptr
	{
		using handle_type  = typename TRAITS::handle_type;
		using exposed_type = typename TRAITS::exposed_type;

		basic_shared_com_ptr() = default;

		basic_shared_com_ptr(basic_shared_com_ptr &&other)
		: _handle(lak::exchange(other._handle, TRAITS::null_value))
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
		static auto make(ARGS &&...args)
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

		void reset() { reset(TRAITS::null_value); }

		explicit operator bool() const { return TRAITS::valid(_handle); }
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
			if (TRAITS::valid(_handle)) TRAITS::unref(_handle);
			_handle = handle;
		}

		handle_type _handle = TRAITS::null_value;
	};

	template<typename T>
	using shared_com_ptr = lak::basic_shared_com_ptr<T>;

	template<typename T>
	using ref_count_com_ptr = lak::basic_shared_com_ptr<
	  T,
	  lak::ref_count_com_ptr_adapter<lak::unique_com_ptr_traits<T>>>;
}

#endif
