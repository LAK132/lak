#ifndef LAK_SPAN_MANIP
#	define LAK_SPAN_MANIP

#	include "lak/concepts.hpp"
#	include "lak/stdint.hpp"
#	include "lak/tuple.hpp"
#	include "lak/type_traits.hpp"

#	define LAK_RESULT_FORWARD_ONLY
#	include "lak/result.hpp"

#	define LAK_SPAN_FORWARD_ONLY
#	include "lak/span.hpp"

namespace lak
{
	template<size_t S, typename T>
	lak::result<lak::span<T, S>> as_const_sized(lak::span<T> s);

	template<size_t S, typename T>
	lak::result<lak::span<T, S>> first_as_const_sized(lak::span<T> s);

	template<size_t S, typename T>
	lak::result<lak::span<T, S>> last_as_const_sized(lak::span<T> s);

	// return s.end() if v is not found
	template<typename T>
	constexpr T *find_first(lak::span<T> s, const T &v);

	template<typename T>
	constexpr bool contains(lak::span<const T> s, const T &v);

	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split(lak::span<T> s, T *at);

	template<typename T>
	requires(!lak::is_const_v<T>)
	lak::pair<lak::span<T>, lak::span<T>> split(lak::span<T> s, const T *at);

	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split(lak::span<T> s, size_t at);

	// {s, s.last(0)} if s does not contain a v
	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split_before(lak::span<T> s,
	                                                   const T &v);

	// {s.first(0), s} if s does not contain a v
	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split_after(lak::span<T> s,
	                                                  const T &v);

	// .first is all elements for which predicate returned true
	// .second is all elements for which predicate return false
	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split_partition(lak::span<T> s,
	                                                      auto predicate);

	// .first is all elements for which predicate returned true
	// .second is all elements for which predicate return false
	template<typename T>
	lak::pair<lak::span<T>, lak::span<T>> split_stable_partition(lak::span<T> s,
	                                                             auto predicate);

	template<typename T>
	void fill(lak::span<T> span, const T &value);

	// Find the subspan of source that is equal to sub.
	template<typename T>
	lak::span<T> find_subspan(lak::span<T> source, lak::span<T> sub);

	template<typename T>
	lak::span<T> common_initial_sequence(lak::span<T> a, lak::span<T> b);

	template<typename T>
	lak::span<T> rotate_left(lak::span<T> data, size_t distance = 1);

	template<typename T>
	lak::span<T> rotate_right(lak::span<T> data, size_t distance = 1);

	template<typename T>
	struct shift_result
	{
		lak::span<T> moved_to;
		lak::span<T> unmodified;
		lak::span<T> moved_from;
	};

	template<typename T>
	lak::shift_result<T> shift_left(lak::span<T> data, size_t distance = 1);

	template<typename T>
	lak::shift_result<T> shift_right(lak::span<T> data, size_t distance = 1);

	template<typename T>
	void reverse(lak::span<T> data);

	template<typename T>
	size_t compare(lak::span<const T> a, lak::span<const T> b);

	// Check if the spans are pointing to the same underlying data.
	template<typename T>
	bool same_span(lak::span<const T> a, lak::span<const T> b);

	template<typename T, typename U, typename BIN_OP>
	U accumulate(lak::span<T> data, U init, BIN_OP func);
}

#endif

#ifdef LAK_SPAN_MANIP_FORWARD_ONLY
#	undef LAK_SPAN_MANIP_FORWARD_ONLY
#else
#	ifndef LAK_SPAN_MANIP_HPP_IMPL
#		define LAK_SPAN_MANIP_HPP_IMPL

#		include "lak/span_manip.inl"

#	endif
#endif
