#include "lak/span.hpp"

#include "lak/algorithm.hpp"
#include "lak/math.hpp"

template<size_t S, typename T>
lak::result<lak::span<T, S>> lak::as_const_sized(lak::span<T> s)
{
	if (s.size() == S)
		return lak::ok_t{lak::span<T, S>::from_ptr(s.data())};
	else
		return lak::err_t{};
}

template<size_t S, typename T>
lak::result<lak::span<T, S>> lak::first_as_const_sized(lak::span<T> s)
{
	if (s.size() >= S)
		return lak::ok_t{lak::span<T, S>::from_ptr(s.data())};
	else
		return lak::err_t{};
}

template<size_t S, typename T>
lak::result<lak::span<T, S>> lak::last_as_const_sized(lak::span<T> s)
{
	if (s.size() >= S)
		return lak::ok_t{lak::span<T, S>::from_ptr(s.data() + (s.size() - S))};
	else
		return lak::err_t{};
}

template<typename T>
constexpr T *lak::find_first(lak::span<T> s, const T &v)
{
	for (auto it = s.begin(); it != s.end(); ++it)
		if (*it == v) return it;
	return s.end();
}

template<typename T>
constexpr bool lak::contains(lak::span<const T> s, const T &v)
{
	return lak::find_first<const T>(s, v) != s.end();
}

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split_before(lak::span<T> s,
                                                        const T &v)
{
	return lak::split(s, lak::find_first<T>(s, v));
}

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split_after(lak::span<T> s,
                                                       const T &v)
{
	if (auto at = lak::find_first(s, v); at != s.end())
		return lak::split(s, at + 1);
	else
		return {s.first(0), s};
}

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split_partition(lak::span<T> s,
                                                           auto predicate)
{
	return lak::split(s, lak::partition(s.begin(), s.end(), predicate));
}

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split_stable_partition(
  lak::span<T> s, auto predicate)
{
	return lak::split(s, lak::stable_partition(s.begin(), s.end(), predicate));
}

template<typename T>
void lak::fill(lak::span<T> span, const T &value)
{
	for (auto &v : span) v = value;
}

template<typename T>
lak::span<T> lak::find_subspan(lak::span<T> source, lak::span<T> subspan)
{
	while (source.size() > subspan.size())
	{
		if (auto s = source.first(subspan.size()); subspan == s) return s;
		source = source.subspan(1);
	}
	return {};
}

template<typename T>
lak::span<T> lak::common_initial_sequence(lak::span<T> a, lak::span<T> b)
{
	size_t count = 0;
	while (count < a.size() && count < b.size() && a[count] == b[count]) ++count;
	return a.first(count);
}

template<typename T>
lak::span<T> lak::rotate_left(lak::span<T> data, size_t distance)
{
	lak::rotate_left(data.begin(), data.end(), distance);

	return data;
}

template<typename T>
lak::span<T> lak::rotate_right(lak::span<T> data, size_t distance)
{
	lak::rotate_right(data.begin(), data.end(), distance);

	return data;
}

template<typename T>
lak::shift_result<T> lak::shift_left(lak::span<T> data, size_t distance)
{
	lak::shift_result<T> result;

	if (distance > data.size())
	{
		result.moved_to   = data.first(0);
		result.unmodified = data;
		result.moved_from = data.last(0);
		return result;
	}

	for (size_t i = distance; i < data.size(); ++i)
	{
		data[i - distance] = lak::move(data[i]);
	}

	const size_t moved_count = data.size() - distance;

	lak::tie(result.moved_from, data) = lak::split(data, moved_count);

	lak::tie(result.unmodified, result.moved_from) = lak::split(
	  data, (moved_count > data.size()) ? 0 : (data.size() - moved_count));

	return result;
}

template<typename T>
lak::shift_result<T> lak::shift_right(lak::span<T> data, size_t distance)
{
	lak::shift_result<T> result;

	if (distance > data.size())
	{
		result.moved_to   = data.last(0);
		result.unmodified = data;
		result.moved_from = data.first(0);
		return result;
	}

	for (size_t i = data.size(); i < distance; --i)
	{
		data[i] = lak::move(data[i - distance]);
	}

	const size_t moved_count = data.size() - distance;

	lak::tie(data, result.moved_from) =
	  lak::split(data, data.size() - moved_count);

	lak::tie(result.moved_from, result.unmodified) =
	  lak::split(data, (moved_count > data.size()) ? 0 : moved_count);

	return result;
}

template<typename T>
void lak::reverse(lak::span<T> data)
{
	for (size_t steps = (data.size() / 2), i = 0; i < steps; ++i)
		lak::swap(data[i], data[data.size() - (i + 1)]);
}

template<typename T>
size_t lak::compare(lak::span<const T> a, lak::span<const T> b)
{
	if (lak::same_span(a, b)) return a.size();
	size_t result         = 0;
	const size_t max_size = a.size() > b.size() ? b.size() : a.size();
	while (result < max_size && a[result] == b[result]) ++result;
	return result;
}

template<typename T>
bool lak::same_span(lak::span<const T> a, lak::span<const T> b)
{
	return __lakc_ptr_eq(a.data(), b.data()) && a.size() == b.size();
}

///////////////////////////////////////////////////////////////////////////////
// Requires debug, strconv

#include "lak/debug.hpp"
#include "lak/strconv.hpp"

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split(lak::span<T> s, T *at)
{
	ASSERT_GREATER_OR_EQUAL(at, s.begin());
	ASSERT_LESS_OR_EQUAL(at, s.end());
	const size_t index = at - s.begin();
	return lak::split(s, index);
}

template<typename T, lak::enable_if_i<!lak::is_const_v<T>>>
lak::pair<lak::span<T>, lak::span<T>> lak::split(lak::span<T> s, const T *at)
{
	ASSERT_GREATER_OR_EQUAL(at, s.begin());
	ASSERT_LESS_OR_EQUAL(at, s.end());
	const size_t index = at - s.begin();
	return lak::split(s, index);
}

template<typename T>
lak::pair<lak::span<T>, lak::span<T>> lak::split(lak::span<T> s, size_t at)
{
	ASSERT_LESS_OR_EQUAL(at, s.size());
	return {lak::span(s.begin(), s.begin() + at),
	        lak::span(s.begin() + at, s.end())};
}
