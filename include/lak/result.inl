#define LAK_DEBUG_FORWARD_ONLY
#include "lak/debug.hpp"
#define LAK_FORMAT_FORWARD_ONLY
#define LAK_STREAMIFY_FORWARD_ONLY
#include "lak/format.hpp"

template<typename OK, typename ERR>
template<typename STR>
lak::result<OK, ERR>::ok_reference lak::result<OK, ERR>::expect(const STR &error_str) &
{
	if (is_err())
	{
		auto print_err_func = [&]<typename SUBERR>(const SUBERR &err)
		{
			if constexpr (lak::is_same_v<SUBERR, lak::monostate>)
			{
				ABORTF_S(lak::to_u8string(error_str));
			}
			else if constexpr (lak::concepts::streamable<SUBERR>)
			{
				ABORTF(error_str, ": ", err);
			}
			else if constexpr (lak::concepts::formattable<SUBERR, char8_t>)
			{
				ABORTF(error_str, ": ", lak::fmt<u8"{}">(err));
			}
			else
			{
				ABORTF(error_str, ": ", TYPE_NAME(err));
			}
		};

		if constexpr (lak::is_variant_v<ERR>)
			get_err().visit(print_err_func);
		else
			print_err_func(get_err());
	}
	return get_ok();
}

template<typename OK, typename ERR>
template<typename STR>
lak::result<OK, ERR>::ok_const_reference lak::result<OK, ERR>::expect(const STR &error_str) const &
{
	if (is_err())
	{
		auto print_err_func = [&]<typename SUBERR>(const SUBERR &err)
		{
			if constexpr (lak::is_same_v<SUBERR, lak::monostate>)
			{
				ABORTF_S(lak::to_u8string(error_str));
			}
			else if constexpr (lak::concepts::streamable<SUBERR>)
			{
				ABORTF(error_str, ": ", err);
			}
			else if constexpr (lak::concepts::formattable<SUBERR, char8_t>)
			{
				ABORTF(error_str, ": ", lak::fmt<u8"{}">(err));
			}
			else
			{
				ABORTF(error_str, ": ", TYPE_NAME(err));
			}
		};

		if constexpr (lak::is_variant_v<ERR>)
			get_err().visit(print_err_func);
		else
			print_err_func(get_err());
	}
	return get_ok();
}

template<typename OK, typename ERR>
template<typename STR>
OK lak::result<OK, ERR>::expect(const STR &error_str) &&
{
	if (is_err())
	{
		auto print_err_func = [&]<typename SUBERR>(const SUBERR &err)
		{
			if constexpr (lak::is_same_v<SUBERR, lak::monostate>)
			{
				ABORTF_S(lak::to_u8string(error_str));
			}
			else if constexpr (lak::concepts::streamable<SUBERR>)
			{
				ABORTF(error_str, ": ", err);
			}
			else if constexpr (lak::concepts::formattable<SUBERR, char8_t>)
			{
				ABORTF(error_str, ": ", lak::fmt<u8"{}">(err));
			}
			else
			{
				ABORTF(error_str, ": ", TYPE_NAME(err));
			}
		};

		if constexpr (lak::is_variant_v<ERR>)
			get_err().visit(print_err_func);
		else
			print_err_func(get_err());
	}
	return forward_ok();
}

template<typename OK, typename ERR>
template<typename STR>
lak::result<OK, ERR>::err_reference lak::result<OK, ERR>::expect_err(const STR &error_str) &
{
	if (is_ok()) ABORTF_S(lak::to_u8string(error_str) /*, ": ", get_ok()*/);
	return get_err();
}

template<typename OK, typename ERR>
template<typename STR>
lak::result<OK, ERR>::err_const_reference lak::result<OK, ERR>::expect_err(const STR &error_str) const &
{
	if (is_ok()) ABORTF_S(lak::to_u8string(error_str) /*, ": ", get_ok()*/);
	return get_err();
}

template<typename OK, typename ERR>
template<typename STR>
ERR lak::result<OK, ERR>::expect_err(const STR &error_str) &&
{
	if (is_ok()) ABORTF_S(lak::to_u8string(error_str) /*, ": ", get_ok()*/);
	return forward_err();
}
