#ifndef LAK_ARRAY_HPP
#	define LAK_ARRAY_HPP

#	include "lak/alloc.hpp"
#	include "lak/stdint.hpp"
#	include "lak/type_traits.hpp"
#	include "lak/unique_pages.hpp"
#	include "lak/utility.hpp"

namespace lak
{
	template<typename T, size_t SIZE = lak::dynamic_extent>
	struct array
	{
	private:
		T _data[SIZE] = {};

	public:
		using value_type      = T;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;
		using reference       = T &;
		using const_reference = const T &;
		using pointer         = T *;
		using const_pointer   = const T *;
		using iterator        = T *;
		using const_iterator  = const T *;

		constexpr array()                         = default;
		constexpr array(const array &)            = default;
		constexpr array &operator=(const array &) = default;

		constexpr array(array &&other)            = default;
		constexpr array &operator=(array &&other) = default;

		array(std::initializer_list<T> list);

		constexpr size_t size() const { return SIZE; }
		constexpr size_t max_size() const { return SIZE; }
		constexpr size_t capacity() const { return SIZE; }
		[[nodiscard]] constexpr bool empty() const { return SIZE == 0; }

		constexpr T *data() { return begin(); }
		constexpr const T *data() const { return begin(); }

		constexpr T *begin() { return &_data[0]; }
		constexpr T *end() { return &_data[SIZE]; }

		constexpr const T *begin() const { return &_data[0]; }
		constexpr const T *end() const { return &_data[SIZE]; }

		constexpr const T *cbegin() const { return &_data[0]; }
		constexpr const T *cend() const { return &_data[SIZE]; }

		T &at(size_t index);
		const T &at(size_t index) const;

		constexpr T &operator[](size_t index);
		constexpr const T &operator[](size_t index) const;

		constexpr T &front();
		constexpr const T &front() const;

		constexpr T &back();
		constexpr const T &back() const;
	};

	template<typename T, typename... U>
	constexpr lak::array<T, sizeof...(U) + 1> fixed_array(T t, U... u)
	{
		return {lak::move(t), lak::move(u)...};
	}

	template<typename T>
	struct array<T, lak::dynamic_extent>
	{
	private:
		lak::unique_pages _data = {};
		size_t _size            = 0; // Ts
		size_t _committed       = 0; // bytes

		// leaves the `count` elements before `before` uninitialised, does not move
		// the base pointer. methods calling this should make sure there is enough
		// memory committed for this, and to make sure to initialise the lower
		// elements and update _size accordingly.
		void right_shift(size_t count, size_t before = 0U);

		void reserve_bytes(size_t new_capacity_bytes,
		                   size_t right_shift_count = 0U);

		void commit(size_t new_size, size_t right_shift_count = 0U);

	public:
		using value_type      = T;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;
		using reference       = T &;
		using const_reference = const T &;
		using pointer         = T *;
		using const_pointer   = const T *;
		using iterator        = T *;
		using const_iterator  = const T *;

		array() = default;
		array(const array &);
		array &operator=(const array &);

		array(array &&other);
		array &operator=(array &&other);

		array(size_t initial_size);

		array(std::initializer_list<T> list);

		template<typename ITER>
		array(ITER &&begin, ITER &&end);

		~array();

		size_t size() const { return _size; }
		constexpr size_t max_size() const { return SIZE_MAX; }
		size_t capacity() const { return _committed / sizeof(T); }
		size_t reserved() const { return _data.size_bytes() / sizeof(T); }

		void resize(size_t new_size);
		void resize(size_t new_size, const T &default_value);
		void reserve(size_t new_capacity);

		void clear();
		void force_clear();

		[[nodiscard]] bool empty() const { return _size == 0; }

		T *data() { return static_cast<T *>(_data.data()); }
		const T *data() const { return static_cast<const T *>(_data.data()); }

		T *begin() { return data(); }
		T *end() { return data() + _size; }

		const T *begin() const { return data(); }
		const T *end() const { return data() + _size; }

		const T *cbegin() const { return data(); }
		const T *cend() const { return data() + _size; }

		T &at(size_t index);
		const T &at(size_t index) const;

		T &operator[](size_t index) { return data()[index]; }
		const T &operator[](size_t index) const { return data()[index]; }

		T &front();
		const T &front() const;

		T &back();
		const T &back() const;

		template<typename... ARGS>
		T &emplace_front(ARGS &&...args);

		T &push_front(const T &t);
		T &push_front(T &&t);

		template<typename... ARGS>
		T &emplace_back(ARGS &&...args);

		T &push_back(const T &t);
		T &push_back(T &&t);

		void pop_front();
		void pop_back();

		T *insert(const T *before, const T &value);
		T *insert(const T *before, T &&value);
		T *insert(const T *before, std::initializer_list<T>);

		T *erase(const T *first, const T *last);

		T *erase(const T *element) { return erase(element, element + 1); }
	};

	template<typename T>
	using dynamic_array = lak::array<T, lak::dynamic_extent>;

	template<typename T, lak::alloc::locality LOC = lak::alloc::locality::global>
	struct small_array
	{
		static_assert(LOC == lak::alloc::locality::local ||
		              LOC == lak::alloc::locality::global);

	private:
		lak::span<T> _data = {};
		size_t _size       = 0; // Ts

		// leaves the `count` elements before `before` uninitialised, does not move
		// the base pointer. methods calling this should make sure there is enough
		// memory committed for this, and to make sure to initialise the lower
		// elements and update _size accordingly.
		void right_shift(size_t count, size_t before = 0U);

	public:
		using value_type      = T;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;
		using reference       = T &;
		using const_reference = const T &;
		using pointer         = T *;
		using const_pointer   = const T *;
		using iterator        = T *;
		using const_iterator  = const T *;

		small_array() = default;
		small_array(const small_array &);
		small_array &operator=(const small_array &);

		small_array(small_array &&other);
		small_array &operator=(small_array &&other);

		small_array(size_t initial_size);

		small_array(std::initializer_list<T> list);

		template<typename ITER>
		small_array(ITER &&begin, ITER &&end);

		~small_array();

		size_t size() const { return _size; }
		constexpr size_t max_size() const { return SIZE_MAX; }
		size_t capacity() const { return _data.size(); }
		size_t reserved() const { return _data.size(); }

		void resize(size_t new_size);
		void resize(size_t new_size, const T &default_value);
		void reserve(size_t new_capacity);

		void clear();
		void force_clear();

		[[nodiscard]] bool empty() const { return _size == 0; }

		T *data() { return _data.data(); }
		const T *data() const { return _data.data(); }

		T *begin() { return data(); }
		T *end() { return data() + _size; }

		const T *begin() const { return data(); }
		const T *end() const { return data() + _size; }

		const T *cbegin() const { return data(); }
		const T *cend() const { return data() + _size; }

		T &at(size_t index);
		const T &at(size_t index) const;

		T &operator[](size_t index) { return data()[index]; }
		const T &operator[](size_t index) const { return data()[index]; }

		T &front();
		const T &front() const;

		T &back();
		const T &back() const;

		template<typename... ARGS>
		T &emplace_front(ARGS &&...args);

		T &push_front(const T &t);
		T &push_front(T &&t);

		void pop_front();

		template<typename... ARGS>
		T &emplace_back(ARGS &&...args);

		T &push_back(const T &t);
		T &push_back(T &&t);

		void pop_back();

		T *insert(const T *before, const T &value);
		T *insert(const T *before, T &&value);
		T *insert(const T *before, std::initializer_list<T> values);

		T *erase(const T *first, const T *last);

		T *erase(const T *element) { return erase(element, element + 1); }
	};

	template<typename T>
	using global_small_array = lak::small_array<T, lak::alloc::locality::global>;

	template<typename T>
	using local_small_array = lak::small_array<T, lak::alloc::locality::local>;

	template<typename T>
	using vector = global_small_array<T>;

	template<typename T, size_t MAX_SIZE>
	struct stack_array
	{
	private:
		T _data[MAX_SIZE] = {};
		size_t _size      = 0U;

	public:
		using value_type      = T;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;
		using reference       = T &;
		using const_reference = const T &;
		using pointer         = T *;
		using const_pointer   = const T *;
		using iterator        = T *;
		using const_iterator  = const T *;

		constexpr stack_array()                               = default;
		constexpr stack_array(const stack_array &)            = default;
		constexpr stack_array &operator=(const stack_array &) = default;

		constexpr stack_array(stack_array &&other)            = default;
		constexpr stack_array &operator=(stack_array &&other) = default;

		stack_array(std::initializer_list<T> list);

		template<typename ITER>
		stack_array(ITER &&begin, ITER &&end);

		constexpr size_t size() const { return _size; }
		constexpr size_t max_size() const { return MAX_SIZE; }
		constexpr size_t capacity() const { return MAX_SIZE; }
		constexpr size_t reserved() const { return MAX_SIZE; }

		[[nodiscard]] constexpr bool empty() const { return _size == 0; }

		constexpr T *data() { return begin(); }
		constexpr const T *data() const { return begin(); }

		constexpr T *begin() { return &_data[0]; }
		constexpr T *end() { return &_data[_size]; }

		constexpr const T *begin() const { return &_data[0]; }
		constexpr const T *end() const { return &_data[_size]; }

		constexpr const T *cbegin() const { return &_data[0]; }
		constexpr const T *cend() const { return &_data[_size]; }

		T &at(size_t index);
		const T &at(size_t index) const;

		constexpr T &operator[](size_t index);
		constexpr const T &operator[](size_t index) const;

		constexpr T &front();
		constexpr const T &front() const;

		constexpr T &back();
		constexpr const T &back() const;

		T &push_back(const T &t);
		T &push_back(T &&t);

		void pop_back();
	};
}

template<typename T, size_t S>
bool operator==(const lak::array<T, S> &a, const lak::array<T, S> &b);

template<typename T, size_t S>
bool operator!=(const lak::array<T, S> &a, const lak::array<T, S> &b);

template<typename T, lak::alloc::locality LOC>
bool operator==(const lak::small_array<T, LOC> &a,
                const lak::small_array<T, LOC> &b);

template<typename T, lak::alloc::locality LOC>
bool operator!=(const lak::small_array<T, LOC> &a,
                const lak::small_array<T, LOC> &b);

#endif

#ifdef LAK_ARRAY_FORWARD_ONLY
#	undef LAK_ARRAY_FORWARD_ONLY
#else
#	ifndef LAK_ARRAY_HPP_IMPL
#		define LAK_ARRAY_HPP_IMPL
#		include "lak/array.inl"
#	endif
#endif
