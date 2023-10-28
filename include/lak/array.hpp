#ifndef LAK_ARRAY_HPP
#	define LAK_ARRAY_HPP

#	include "lak/alloc.hpp"
#	include "lak/stdint.hpp"
#	include "lak/type_traits.hpp"
#	include "lak/unique_pages.hpp"
#	include "lak/utility.hpp"

namespace lak
{
	template<typename T>
	struct uninit_array
	{
	private:
		// reserved memory
		lak::span<T> _data = {};

		// if page allocating, the number of Ts committed
		size_t _committed = 0U;

		// element count
		size_t _size = 0U;

		static size_t page_threshold();

		bool is_paged(size_t size) const { return size >= page_threshold(); }
		bool is_paged() const { return is_paged(_data.size()); }

		// assumes memory has already been reserved, just commits up to the new
		// capacity
		void commit_impl(size_t new_capacity);

	public:
		using value_type      = T;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;
		using reference       = T &;
		using const_reference = const T &;
		using pointer         = T *;
		using const_pointer   = const T *;
		using iterator        = pointer;
		using const_iterator  = const_pointer;

		uninit_array()                     = default;
		uninit_array(const uninit_array &) = delete;
		uninit_array(uninit_array &&other);
		uninit_array &operator=(const uninit_array &) = delete;
		uninit_array &operator=(uninit_array &&other);

		uninit_array(size_t initial_size) { resize(initial_size); }

		~uninit_array();

		size_t size() const { return _size; }
		constexpr size_t max_size() const { return SIZE_MAX; }
		size_t capacity() const { return _data.size(); }
		size_t committed() const { return is_paged() ? _committed : _data.size(); }

		// returns the old allocation if reallocation is required
		lak::optional<uninit_array> reserve(size_t new_capacity);
		lak::optional<uninit_array> commit(size_t new_capacity);
		lak::optional<uninit_array> resize(size_t new_size);

		lak::optional<uninit_array> push_back();
		void pop_back() { --_size; }

		void clear() { _size = 0U; }
		uninit_array force_clear() { return lak::exchange(*this, uninit_array{}); }

		[[nodiscard]] bool empty() const { return size() == 0U; }

		pointer data() { return _data.data(); }
		const_pointer data() const { return _data.data(); }

		iterator begin() { return data(); }
		iterator end() { return data() + size(); }

		const_iterator begin() const { return data(); }
		const_iterator end() const { return data() + size(); }

		const_iterator cbegin() const { return begin(); }
		const_iterator cend() const { return end(); }

		reference at(size_t index) { return _data[index]; }
		const_reference at(size_t index) const { return _data[index]; }

		reference operator[](size_t index) { return _data[index]; }
		const_reference operator[](size_t index) const { return _data[index]; }

		reference front() { return _data[0]; }
		const_reference front() const { return _data[0]; }

		reference back() { return _data[size() - 1U]; }
		const_reference back() const { return _data[size() - 1U]; }
	};

	template<typename T>
	struct array_force_copyable : lak::false_type
	{
	};
	template<typename T>
	inline constexpr bool array_force_copyable_v =
	  lak::array_force_copyable<T>::value;

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

		constexpr array() = default;
		constexpr array(const array &)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>)
		= default;
		constexpr array &operator=(const array &)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>)
		= default;

		constexpr array(array &&other)            = default;
		constexpr array &operator=(array &&other) = default;

		array(std::initializer_list<T> list)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);

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

	template<typename T>
	struct array<T, 0U>
	{
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

		constexpr size_t size() const { return 0; }
		constexpr size_t max_size() const { return 0; }
		constexpr size_t capacity() const { return 0; }
		[[nodiscard]] constexpr bool empty() const { return true; }

		constexpr T *data() { return begin(); }
		constexpr const T *data() const { return begin(); }

		constexpr T *begin() { return nullptr; }
		constexpr T *end() { return nullptr; }

		constexpr const T *begin() const { return nullptr; }
		constexpr const T *end() const { return nullptr; }

		constexpr const T *cbegin() const { return nullptr; }
		constexpr const T *cend() const { return nullptr; }

		T &at(size_t index) { ASSERT_UNREACHABLE(); }
		const T &at(size_t index) const { ASSERT_UNREACHABLE(); }

		constexpr T &operator[](size_t index) { ASSERT_UNREACHABLE(); }
		constexpr const T &operator[](size_t index) const { ASSERT_UNREACHABLE(); }

		constexpr T &front() { ASSERT_UNREACHABLE(); }
		constexpr const T &front() const { ASSERT_UNREACHABLE(); }

		constexpr T &back() { ASSERT_UNREACHABLE(); }
		constexpr const T &back() const { ASSERT_UNREACHABLE(); }
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
		using data_type = lak::uninit_array<T>;
		data_type _data;

		// resizes to `size()+count` and leaves a `count` sized gap of
		// uninitialised elements before `before`
		void right_shift(size_t count, size_t before = 0U);

		void resize_impl(size_t new_size);

	public:
		using value_type      = typename data_type::value_type;
		using size_type       = typename data_type::size_type;
		using difference_type = typename data_type::difference_type;
		using reference       = typename data_type::reference;
		using const_reference = typename data_type::const_reference;
		using pointer         = typename data_type::pointer;
		using const_pointer   = typename data_type::const_pointer;
		using iterator        = typename data_type::iterator;
		using const_iterator  = typename data_type::const_iterator;

		array() = default;
		array(const array &)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);

		array &operator=(const array &)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);

		array(array &&other);
		array &operator=(array &&other);

		array(size_t initial_size);

		array(std::initializer_list<T> list)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);

		template<typename ITER>
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>)
		array(ITER &&begin, ITER &&end);

		~array();

		size_t size() const { return _data.size(); }
		constexpr size_t max_size() const { return _data.max_size(); }
		size_t capacity() const { return _data.capacity(); }
		size_t committed() const { return _data.committed(); }

		void resize(size_t new_size);
		void resize(size_t new_size, const T &default_value)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);
		void reserve(size_t new_capacity);

		void clear();
		void force_clear();

		[[nodiscard]] bool empty() const { return _data.empty(); }

		pointer data() { return _data.data(); }
		const_pointer data() const { return _data.data(); }

		iterator begin() { return _data.begin(); }
		iterator end() { return _data.end(); }

		const_iterator begin() const { return _data.begin(); }
		const_iterator end() const { return _data.end(); }

		const_iterator cbegin() const { return _data.cbegin(); }
		const_iterator cend() const { return _data.cend(); }

		reference at(size_t index) { return _data.at(index); }
		const_reference at(size_t index) const { return _data.at(index); }

		reference operator[](size_t index) { return _data[index]; }
		const_reference operator[](size_t index) const { return _data[index]; }

		reference front() { return _data.front(); }
		const_reference front() const { return _data.front(); }

		reference back() { return _data.back(); }
		const_reference back() const { return _data.back(); }

		template<typename... ARGS>
		reference emplace_front(ARGS &&...args);

		reference push_front(const T &t)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);
		reference push_front(T &&t);

		void pop_front();
		T popped_front();

		template<typename... ARGS>
		reference emplace_back(ARGS &&...args);

		reference push_back(const T &t)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);
		reference push_back(T &&t);

		void pop_back();
		T popped_back();

		iterator insert(const_iterator before, const T &value)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);
		iterator insert(const_iterator before, T &&value);
		iterator insert(const_iterator before, std::initializer_list<T> list)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);

		iterator erase(const_iterator first, const_iterator last);

		iterator erase(const_iterator element)
		{
			return erase(element, element + 1);
		}
	};

	template<typename T>
	using vector = lak::array<T>;

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

		constexpr stack_array() = default;
		constexpr stack_array(const stack_array &)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>)
		= default;
		constexpr stack_array &operator=(const stack_array &)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>)
		= default;

		constexpr stack_array(stack_array &&other)            = default;
		constexpr stack_array &operator=(stack_array &&other) = default;

		stack_array(std::initializer_list<T> list)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);

		template<typename ITER>
		stack_array(ITER &&begin, ITER &&end)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);

		constexpr size_t size() const { return _size; }
		constexpr size_t max_size() const { return MAX_SIZE; }
		constexpr size_t capacity() const { return MAX_SIZE; }

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

		T &push_back(const T &t)
		requires(lak::array_force_copyable_v<T> ||
		         lak::concepts::copy_constructible<T>);
		T &push_back(T &&t);

		void pop_back();
		T popped_back();
	};
}

template<typename T, size_t S>
bool operator==(const lak::array<T, S> &a, const lak::array<T, S> &b);

template<typename T, size_t S>
bool operator!=(const lak::array<T, S> &a, const lak::array<T, S> &b);

#endif

#ifdef LAK_ARRAY_FORWARD_ONLY
#	undef LAK_ARRAY_FORWARD_ONLY
#else
#	ifndef LAK_ARRAY_HPP_IMPL
#		define LAK_ARRAY_HPP_IMPL
#		include "lak/array.inl"
#	endif
#endif
