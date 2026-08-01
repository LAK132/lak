#include "lak/file/json.hpp"

#include <unordered_map>

template<typename CHAR>
struct lak::from_json_traits<lak::string<CHAR>>
{
	using value_type = lak::string<CHAR>;
	using error_type = lak::json::err::unexpected_type;
	static lak::result<value_type, error_type> from_json(
	  const lak::json::value_proxy &value)
	{
		RES_TRY_ASSIGN(auto str =, value.string());
		return lak::ok_t{lak::strconv<CHAR>(str)};
	}
};

template<lak::concepts::one_of<LAK_ALL_STD_UNSIGNED_INTEGERS,
                               LAK_ALL_STD_SIGNED_INTEGERS,
                               LAK_ALL_STD_FLOATS> T>
struct lak::from_json_traits<T>
{
	using value_type = T;
	using error_type =
	  lak::variant<lak::json::err::unexpected_type, lak::err::string_to_numeric>;
	static lak::result<value_type, error_type> from_json(
	  const lak::json::value_proxy &value)
	{
		return value.template number<value_type>();
	}
};

template<>
struct lak::from_json_traits<bool>
{
	using value_type = bool;
	using error_type = lak::variant<lak::json::err::unexpected_type,
	                                lak::err::value_out_of_range>;
	static lak::result<value_type, error_type> from_json(
	  const lak::json::value_proxy &value)
	{
		RES_TRY_ASSIGN(auto tok =, value.token());
		if (tok == u8"true"_view)
			return lak::ok_t{true};
		else if (tok == u8"false"_view)
			return lak::ok_t{false};
		else
			return lak::err_t{lak::err::value_out_of_range{}};
	}
};

template<typename T, size_t S>
requires(lak::concepts::has_from_json_traits<T> && S == lak::dynamic_extent)
struct lak::from_json_traits<lak::array<T, S>>
{
	using value_type = lak::array<T, S>;
	using error_type = lak::from_json_traits<T>::error_type;
	static lak::result<value_type, error_type> from_json(
	  const lak::json::value_proxy &value)
	{
		RES_TRY_ASSIGN(auto arr =, value.array());
		value_type result;
		result.reserve(arr.size());
		for (size_t i = 0U; i < arr.size(); ++i)
		{
			RES_TRY_ASSIGN(auto val =, lak::from_json<T>(arr[i]));
			result.push_back(lak::move(val));
		}
		return lak::move_ok(result);
	}
};

template<typename CHAR, typename T>
requires(lak::concepts::has_from_json_traits<T>)
struct lak::from_json_traits<std::unordered_map<lak::string<CHAR>, T>>
{
	using value_type = std::unordered_map<lak::string<CHAR>, T>;
	using error_type = lak::from_json_traits<T>::error_type;
	static lak::result<value_type, error_type> from_json(
	  const lak::json::value_proxy &value)
	{
		RES_TRY_ASSIGN(auto obj =, value.object());
		value_type result;
		for (size_t i = 0U; i < obj.size(); ++i)
		{
			auto [key, val] = obj[i];
			RES_TRY_ASSIGN(auto v =, lak::from_json<T>(val));
			result.insert_or_assign(lak::strconv<CHAR>(key), lak::move(v));
		}
		return lak::move_ok(result);
	}
};
