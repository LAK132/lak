#ifndef LAK_QUEUE_HPP
#define LAK_QUEUE_HPP

#include "lak/array.hpp"
#include "lak/concepts.hpp"

namespace lak
{
	template<typename T>
	struct railcar;

	/* --- railcar_iterator --- */

	template<typename T>
	struct railcar_iterator
	{
	private:
		using container_type =
		  lak::copy_const_t<T, lak::railcar<lak::remove_const_t<T>>>;
		container_type *_container;
		size_t _index;

		template<typename U>
		friend struct railcar;

		template<typename U>
		friend struct railcar_iterator;

		railcar_iterator(container_type *container, size_t index);

	public:
		using value_type        = lak::remove_cv_t<T>;
		using difference_type   = ptrdiff_t;
		using reference         = T &;
		using pointer           = T *;
		using iterator_category = std::random_access_iterator_tag;

		railcar_iterator(const railcar_iterator &other);
		railcar_iterator &operator=(const railcar_iterator &other);
		railcar_iterator &operator=(railcar_iterator &&other);

		reference operator*() const;
		pointer operator->() const;

		reference operator[](difference_type index);

		railcar_iterator &operator++();
		railcar_iterator operator++(int);
		railcar_iterator &operator+=(size_t n);
		railcar_iterator operator+(size_t n);

		railcar_iterator &operator--();
		railcar_iterator operator--(int);
		railcar_iterator &operator-=(size_t n);
		railcar_iterator operator-(size_t n);

		difference_type operator-(const railcar_iterator &other);

		bool operator==(const railcar_iterator &other) const;
		bool operator!=(const railcar_iterator &other) const;
		bool operator>(const railcar_iterator &other) const;
		bool operator<(const railcar_iterator &other) const;
		bool operator>=(const railcar_iterator &other) const;
		bool operator<=(const railcar_iterator &other) const;
	};

	/* --- railcar --- */

	template<typename T>
	struct railcar
	{
	private:
		lak::array<lak::array<T>> _data = {};
		size_t _bin_size                = 0;

		void internal_init_bin_size();
		void internal_alloc_end();

	public:
		using value_type      = T;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;
		using reference       = T &;
		using const_reference = const T &;
		using pointer         = T *;
		using const_pointer   = const T *;
		using iterator        = railcar_iterator<T>;
		using const_iterator  = railcar_iterator<const T>;

		railcar() = default;

		railcar(const railcar &)
		requires lak::concepts::copy_constructible<T>;
		railcar &operator=(const railcar &)
		requires lak::concepts::copy_constructible<T>;

		railcar(railcar &&other);
		railcar &operator=(railcar &&other);

		railcar(std::initializer_list<T> list)
		requires lak::concepts::copy_constructible<T>;

		template<typename ITER>
		requires lak::concepts::copy_constructible<T>
		railcar(ITER &&begin, ITER &&end);

		size_t size() const;
		void resize(size_t new_size);
		void clear();
		void reserve(size_t new_capacity);
		void force_clear();

		[[nodiscard]] bool empty() const { return _data.size() == 0; }

		iterator begin();
		iterator end();

		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

		const_iterator cbegin() const;
		const_iterator cend() const;

		T &at(size_t index);
		const T &at(size_t index) const;

		T &operator[](size_t index);
		const T &operator[](size_t index) const;

		T &front();
		const T &front() const;

		T &back();
		const T &back() const;

		template<typename... ARGS>
		T &emplace_back(ARGS &&...args);

		T &push_back(const T &t)
		requires lak::concepts::copy_constructible<T>;
		T &push_back(T &&t);

		void pop_back();

		T *erase(const T *element);
	};
}

#include "lak/railcar.inl"

#endif