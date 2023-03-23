#include "lak/type_traits.hpp"

#include <iterator>

/* --- input_iterator_wrapper --- */

template<typename ITER>
struct input_iterator_wrapper
{
	static_assert(std::input_iterator<ITER>);

	ITER _iter;

	using difference_type = typename std::iterator_traits<ITER>::difference_type;
	using value_type      = typename std::iterator_traits<ITER>::value_type;
	using pointer         = typename std::iterator_traits<ITER>::pointer;
	using reference       = typename std::iterator_traits<ITER>::reference;
	using iterator_category = std::input_iterator_tag;

	bool operator!=(const input_iterator_wrapper &rhs) const
	{
		return _iter != rhs._iter;
	}

	bool operator==(const input_iterator_wrapper &rhs) const
	{
		return _iter == rhs._iter;
	}

	reference operator*() const { return *_iter; }

	pointer operator->() const { return _iter.operator->(); }

	input_iterator_wrapper &operator++()
	{
		++_iter;
		return *this;
	}

	input_iterator_wrapper operator++(int) { return {_iter++}; }
};

static_assert(std::input_iterator<input_iterator_wrapper<int *>>);

template<>
struct input_iterator_wrapper<void>
{
	template<typename ITER>
	using type = input_iterator_wrapper<ITER>;
};

template<typename ITER>
input_iterator_wrapper(ITER &&)
  -> input_iterator_wrapper<lak::remove_cvref_t<ITER>>;

/* --- output_iterator_wrapper --- */

template<typename ITER>
struct output_iterator_wrapper
{
	static_assert(
	  std::output_iterator<ITER,
	                       typename std::iterator_traits<ITER>::value_type>);

	ITER _iter;

	using difference_type = typename std::iterator_traits<ITER>::difference_type;
	using value_type      = typename std::iterator_traits<ITER>::value_type;
	using pointer         = typename std::iterator_traits<ITER>::pointer;
	using reference       = typename std::iterator_traits<ITER>::reference;
	using iterator_category = std::output_iterator_tag;

	struct assign_proxy
	{
		ITER _iter;

		template<typename T>
		void operator=(T &&rhs) const
		{
			*_iter = lak::forward<T>(rhs);
		}
	};

	assign_proxy operator*() const { return {_iter}; }

	output_iterator_wrapper &operator++()
	{
		++_iter;
		return *this;
	}

	output_iterator_wrapper operator++(int) { return {_iter++}; }
};

static_assert(std::output_iterator<output_iterator_wrapper<int *>, int>);

template<>
struct output_iterator_wrapper<void>
{
	template<typename ITER>
	using type = output_iterator_wrapper<ITER>;
};

template<typename ITER>
output_iterator_wrapper(ITER &&)
  -> output_iterator_wrapper<lak::remove_cvref_t<ITER>>;

/* --- forward_iterator_wrapper --- */

template<typename ITER>
struct forward_iterator_wrapper
{
	static_assert(std::forward_iterator<ITER>);

	ITER _iter;

	using difference_type = typename std::iterator_traits<ITER>::difference_type;
	using value_type      = typename std::iterator_traits<ITER>::value_type;
	using pointer         = typename std::iterator_traits<ITER>::pointer;
	using reference       = typename std::iterator_traits<ITER>::reference;
	using iterator_category = std::forward_iterator_tag;

	bool operator!=(const forward_iterator_wrapper &rhs) const
	{
		return _iter != rhs._iter;
	}

	bool operator==(const forward_iterator_wrapper &rhs) const
	{
		return _iter == rhs._iter;
	}

	reference operator*() const { return *_iter; }

	pointer operator->() const { return _iter.operator->(); }

	forward_iterator_wrapper &operator++()
	{
		++_iter;
		return *this;
	}

	forward_iterator_wrapper operator++(int) { return {_iter++}; }
};

static_assert(std::forward_iterator<forward_iterator_wrapper<int *>>);

template<>
struct forward_iterator_wrapper<void>
{
	template<typename ITER>
	using type = forward_iterator_wrapper<ITER>;
};

template<typename ITER>
forward_iterator_wrapper(ITER &&)
  -> forward_iterator_wrapper<lak::remove_cvref_t<ITER>>;

/* --- bidirectional_iterator_wrapper --- */

template<typename ITER>
struct bidirectional_iterator_wrapper
{
	static_assert(std::bidirectional_iterator<ITER>);

	ITER _iter;

	using difference_type = typename std::iterator_traits<ITER>::difference_type;
	using value_type      = typename std::iterator_traits<ITER>::value_type;
	using pointer         = typename std::iterator_traits<ITER>::pointer;
	using reference       = typename std::iterator_traits<ITER>::reference;
	using iterator_category = std::bidirectional_iterator_tag;

	bool operator!=(const bidirectional_iterator_wrapper &rhs) const
	{
		return _iter != rhs._iter;
	}

	bool operator==(const bidirectional_iterator_wrapper &rhs) const
	{
		return _iter == rhs._iter;
	}

	reference operator*() const { return *_iter; }

	pointer operator->() const { return _iter.operator->(); }

	bidirectional_iterator_wrapper &operator++()
	{
		++_iter;
		return *this;
	}

	bidirectional_iterator_wrapper operator++(int) { return {_iter++}; }

	bidirectional_iterator_wrapper &operator--()
	{
		--_iter;
		return *this;
	}

	bidirectional_iterator_wrapper operator--(int) { return {_iter--}; }
};

static_assert(
  std::bidirectional_iterator<bidirectional_iterator_wrapper<int *>>);

template<>
struct bidirectional_iterator_wrapper<void>
{
	template<typename ITER>
	using type = bidirectional_iterator_wrapper<ITER>;
};

template<typename ITER>
bidirectional_iterator_wrapper(ITER &&)
  -> bidirectional_iterator_wrapper<lak::remove_cvref_t<ITER>>;

/* --- random_access_iterator_wrapper --- */

template<typename ITER>
struct random_access_iterator_wrapper
{
	static_assert(std::random_access_iterator<ITER>);

	ITER _iter;

	using difference_type = typename std::iterator_traits<ITER>::difference_type;
	using value_type      = typename std::iterator_traits<ITER>::value_type;
	using pointer         = typename std::iterator_traits<ITER>::pointer;
	using reference       = typename std::iterator_traits<ITER>::reference;
	using iterator_category = std::random_access_iterator_tag;

	bool operator!=(const random_access_iterator_wrapper &rhs) const
	{
		return _iter != rhs._iter;
	}

	bool operator==(const random_access_iterator_wrapper &rhs) const
	{
		return _iter == rhs._iter;
	}

	reference operator*() const { return *_iter; }

	pointer operator->() const { return _iter.operator->(); }

	random_access_iterator_wrapper &operator++()
	{
		++_iter;
		return *this;
	}

	random_access_iterator_wrapper operator++(int) { return {_iter++}; }

	random_access_iterator_wrapper &operator--()
	{
		--_iter;
		return *this;
	}

	random_access_iterator_wrapper operator--(int) { return {_iter--}; }

	random_access_iterator_wrapper &operator+=(difference_type rhs)
	{
		_iter += rhs;
		return *this;
	}

	random_access_iterator_wrapper operator+(difference_type rhs) const
	{
		return {_iter + rhs};
	}

	friend random_access_iterator_wrapper operator+(
	  difference_type lhs, const random_access_iterator_wrapper &rhs)
	{
		return {lhs + rhs._iter};
	}

	random_access_iterator_wrapper &operator-=(difference_type rhs)
	{
		_iter -= rhs;
		return *this;
	}

	random_access_iterator_wrapper operator-(difference_type rhs) const
	{
		return {_iter - rhs};
	}

	difference_type operator-(const random_access_iterator_wrapper &rhs) const
	{
		return _iter - rhs._iter;
	}

	reference operator[](difference_type i) const { return _iter[i]; }

	bool operator<(const random_access_iterator_wrapper &rhs) const
	{
		return _iter < rhs._iter;
	}

	bool operator<=(const random_access_iterator_wrapper &rhs) const
	{
		return _iter <= rhs._iter;
	}

	bool operator>(const random_access_iterator_wrapper &rhs) const
	{
		return _iter > rhs._iter;
	}

	bool operator>=(const random_access_iterator_wrapper &rhs) const
	{
		return _iter >= rhs._iter;
	}
};

static_assert(
  std::random_access_iterator<random_access_iterator_wrapper<int *>>);

template<>
struct random_access_iterator_wrapper<void>
{
	template<typename ITER>
	using type = random_access_iterator_wrapper<ITER>;
};

template<typename ITER>
random_access_iterator_wrapper(ITER &&)
  -> random_access_iterator_wrapper<lak::remove_cvref_t<ITER>>;
