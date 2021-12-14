#ifndef LAK_RESULT_HPP
#define LAK_RESULT_HPP

#include "lak/concepts.hpp"
#include "lak/macro_utils.hpp"
#include "lak/optional.hpp"
#include "lak/type_traits.hpp"
#include "lak/variant.hpp"

namespace lak
{
	/* --- traits --- */

	template<typename... T>
	struct is_result : public lak::false_type
	{
	};
	template<typename... T>
	constexpr bool is_result_v = is_result<T...>::value;

	// the OK type of the result without reference removed
	template<typename RESULT>
	struct result_ok_type : public lak::type_identity<void>
	{
	};
	template<typename RESULT>
	using result_ok_type_t = typename result_ok_type<RESULT>::type;

	// the ERR type of the result without references removed;
	template<typename RESULT>
	struct result_err_type : public lak::type_identity<void>
	{
	};
	template<typename RESULT>
	using result_err_type_t = typename result_err_type<RESULT>::type;

	/* --- infallible --- */

	struct infallible
	{
		infallible() = delete;
	};

	/* --- ok_t --- */

	template<typename T = lak::monostate>
	struct ok_t
	{
		T value;
	};

	template<typename T>
	ok_t(T &&) -> ok_t<T>;

	template<typename T>
	ok_t(T &) -> ok_t<T &>;

	/* --- err_t --- */

	template<typename T = lak::monostate>
	struct err_t
	{
		T value;
	};

	template<typename T>
	err_t(T &&) -> err_t<T>;

	template<typename T>
	err_t(T &) -> err_t<T &>;

	/* --- result --- */

	template<typename OK = lak::monostate, typename ERR = lak::monostate>
	struct [[nodiscard]] result
	{
		using ok_type            = OK;
		using ok_reference       = lak::add_lvalue_reference_t<ok_type>;
		using ok_const_reference = lak::add_lvalue_reference_t<const ok_type>;
		using ok_pointer         = lak::remove_reference_t<OK> *;
		using ok_const_pointer =
		  lak::conditional_t<lak::is_reference_v<OK>,
		                     ok_pointer,
		                     const lak::remove_reference_t<OK> *>;

		using err_type            = ERR;
		using err_reference       = lak::add_lvalue_reference_t<err_type>;
		using err_const_reference = lak::add_lvalue_reference_t<const err_type>;
		using err_pointer         = lak::remove_reference_t<ERR> *;
		using err_const_pointer =
		  lak::conditional_t<lak::is_reference_v<ERR>,
		                     err_pointer,
		                     const lak::remove_reference_t<ERR> *>;

		template<typename T>
		using rebind_ok = lak::result<T, ERR>;
		template<typename T>
		using rebind_err = lak::result<OK, T>;

		template<typename F, typename... T>
		requires lak::concepts::invocable<F, T...> //
		using invoke_rebind_ok = rebind_ok<lak::invoke_result_t<F, T...>>;
		template<typename F, typename... T>
		requires lak::concepts::invocable<F, T...> //
		using invoke_rebind_err = rebind_err<lak::invoke_result_t<F, T...>>;

	private:
		lak::variant<OK, ERR> _value;

		force_inline ok_reference get_ok() { return *_value.template get<0>(); }
		force_inline ok_const_reference get_ok() const
		{
			return *_value.template get<0>();
		}
		force_inline OK &&forward_ok()
		{
			return lak::forward<OK>(*_value.template get<0>());
		}

		force_inline err_reference get_err() { return *_value.template get<1>(); }
		force_inline err_const_reference get_err() const
		{
			return *_value.template get<1>();
		}
		force_inline ERR &&forward_err()
		{
			return lak::forward<ERR>(*_value.template get<1>());
		}

		result(const lak::variant<OK, ERR> &value) : _value(value) {}
		result(lak::variant<OK, ERR> &&value) : _value(lak::move(value)) {}

	public:
		result(const result &) = default;
		result(result &&)      = default;

		result &operator=(const result &) = default;
		result &operator=(result &&) = default;

		static inline result make_ok(OK value)
		{
			return result(lak::variant<OK, ERR>(lak::in_place_index<0>,
			                                    lak::forward<OK>(value)));
		}
		static inline result make_err(ERR value)
		{
			return result(lak::variant<OK, ERR>(lak::in_place_index<1>,
			                                    lak::forward<ERR>(value)));
		}

		template<typename T>
		result(ok_t<T> value) : result(make_ok(lak::forward<T>(value.value)))
		{
		}
		template<typename T>
		result(err_t<T> value) : result(make_err(lak::forward<T>(value.value)))
		{
		}

		bool is_ok() const { return _value.index() == 0; }
		bool is_err() const { return _value.index() == 1; }

		/* --- ok --- */

		[[nodiscard]] ok_pointer ok() & { return _value.template get<0>(); }
		[[nodiscard]] ok_const_pointer ok() const &
		{
			return _value.template get<0>();
		}
		[[nodiscard]] lak::optional<OK> ok() &&
		{
			return is_ok() ? lak::optional<OK>(forward_ok()) : lak::nullopt;
		}

		/* --- err --- */

		[[nodiscard]] err_pointer err() & { return _value.template get<1>(); }
		[[nodiscard]] err_const_pointer err() const &
		{
			return _value.template get<1>();
		}
		[[nodiscard]] lak::optional<ERR> err() &&
		{
			return is_err() ? lak::optional<ERR>(forward_err()) : lak::nullopt;
		}

		/* --- map --- */

		template<lak::concepts::invocable<ok_reference> F>
		auto map(F &&f) & -> invoke_rebind_ok<F, ok_reference>
		{
			if (is_ok())
				return lak::ok_t{f(get_ok())};
			else
				return lak::err_t{get_err()};
		}

		template<lak::concepts::invocable<ok_const_reference> F>
		auto map(F &&f) const & -> invoke_rebind_ok<F, ok_const_reference>
		{
			if (is_ok())
				return lak::ok_t{f(get_ok())};
			else
				return lak::err_t{get_err()};
		}

		template<lak::concepts::invocable<OK &&> F>
		auto map(F &&f) && -> invoke_rebind_ok<F, OK &&>
		{
			if (is_ok())
				return lak::ok_t{f(forward_ok())};
			else
				return lak::err_t{forward_err()};
		}

		/* --- map_err --- */

		template<lak::concepts::invocable<err_reference> F>
		auto map_err(F &&f) & -> invoke_rebind_err<F, err_reference>
		{
			if (is_ok())
				return lak::ok_t{get_ok()};
			else
				return lak::err_t{f(get_err())};
		}

		template<lak::concepts::invocable<err_const_reference> F>
		auto map_err(F &&f) const & -> invoke_rebind_err<F, err_const_reference>
		{
			if (is_ok())
				return lak::ok_t{get_ok()};
			else
				return lak::err_t{f(get_err())};
		}

		template<lak::concepts::invocable<ERR &&> F>
		auto map_err(F &&f) && -> invoke_rebind_err<F, ERR &&>
		{
			if (is_ok())
				return lak::ok_t{forward_ok()};
			else
				return lak::err_t{f(forward_err())};
		}

		/* --- map_expect_value --- */

		// err -> err<ERR>
		// ok == expected -> ok<>
		// ok != expected -> err<ERR>

		template<lak::concepts::invocable_result_of<ERR, ok_reference> F>
		auto map_expect_value(ok_const_reference expected, F &&on_unexpected)
		  & -> lak::result<lak::monostate, ERR>
		{
			if (is_ok())
			{
				if (get_ok() == expected)
					return lak::ok_t{};
				else
					return lak::err_t{on_unexpected(get_ok())};
			}
			else
				return lak::err_t{get_err()};
		}

		template<lak::concepts::invocable_result_of<ERR, ok_const_reference> F>
		auto map_expect_value(ok_const_reference expected, F &&on_unexpected)
		  const & -> lak::result<lak::monostate, ERR>
		{
			if (is_ok())
			{
				if (get_ok() == expected)
					return lak::ok_t{};
				else
					return lak::err_t{on_unexpected(get_ok())};
			}
			else
				return lak::err_t{get_err()};
		}

		template<lak::concepts::invocable_result_of<ERR, OK &&> F>
		auto map_expect_value(ok_const_reference expected, F &&on_unexpected)
		  && -> lak::result<lak::monostate, ERR>
		{
			if (is_ok())
			{
				if (get_ok() == expected)
					return lak::ok_t{};
				else
					return lak::err_t{on_unexpected(forward_ok())};
			}
			else
				return lak::err_t{get_err()};
		}

		/* --- and_then --- */

		// and_then(func) ~= map(func).flatten()

		template<
		  lak::concepts::invocable_result_of_template<lak::result, ok_reference> F>
		auto and_then(F &&f) & -> lak::invoke_result_t<F, ok_reference>
		{
			if (is_ok())
				return f(get_ok());
			else
				return lak::err_t{get_err()};
		}

		template<lak::concepts::invocable_result_of_template<lak::result,
		                                                     ok_const_reference> F>
		auto and_then(F &&f) const & -> lak::invoke_result_t<F, ok_const_reference>
		{
			if (is_ok())
				return f(get_ok());
			else
				return lak::err_t{get_err()};
		}

		template<lak::concepts::invocable_result_of_template<lak::result, OK &&> F>
		auto and_then(F &&f) && -> lak::invoke_result_t<F, OK &&>
		{
			if (is_ok())
				return f(forward_ok());
			else
				return lak::err_t{forward_err()};
		}

		/* --- or_else --- */

		template<lak::concepts::invocable_result_of_template<lak::result,
		                                                     err_reference> F>
		auto or_else(F &&f) & -> lak::invoke_result_t<F, err_reference>
		{
			if (is_ok())
				return lak::ok_t{get_ok()};
			else
				return f(get_err());
		}

		template<
		  lak::concepts::invocable_result_of_template<lak::result,
		                                              err_const_reference> F>
		auto or_else(F &&f) const & -> lak::invoke_result_t<F, err_const_reference>
		{
			if (is_ok())
				return lak::ok_t{get_ok()};
			else
				return f(get_err());
		}

		template<
		  lak::concepts::invocable_result_of_template<lak::result, ERR &&> F>
		auto or_else(F &&f) && -> lak::invoke_result_t<F, ERR &&>
		{
			if (is_ok())
				return lak::ok_t{forward_ok()};
			else
				return f(forward_err());
		}

		/* --- expect --- */

		template<typename STR>
		ok_reference expect(const STR &error_str) &
		{
			if (is_err())
			{
				if constexpr (lak::is_same_v<ERR, lak::monostate>)
				{
					ABORTF(error_str);
				}
				else
				{
					ABORTF(error_str, ": ", get_err());
				}
			}
			return get_ok();
		}

		template<typename STR>
		ok_const_reference expect(const STR &error_str) const &
		{
			if (is_err())
			{
				if constexpr (lak::is_same_v<ERR, lak::monostate>)
				{
					ABORTF(error_str);
				}
				else
				{
					ABORTF(error_str, ": ", get_err());
				}
			}
			return get_ok();
		}

		template<typename STR>
		OK expect(const STR &error_str) &&
		{
			if (is_err())
			{
				if constexpr (lak::is_same_v<ERR, lak::monostate>)
				{
					ABORTF(error_str);
				}
				else
				{
					ABORTF(error_str, ": ", get_err());
				}
			}
			return forward_ok();
		}

		/* --- expect_err --- */

		template<typename STR>
		err_reference expect_err(const STR &error_str) &
		{
			if (is_ok()) ABORTF(error_str /*, ": ", get_ok()*/);
			return get_err();
		}

		template<typename STR>
		err_const_reference expect_err(const STR &error_str) const &
		{
			if (is_ok()) ABORTF(error_str /*, ": ", get_ok()*/);
			return get_err();
		}

		template<typename STR>
		ERR expect_err(const STR &error_str) &&
		{
			if (is_ok()) ABORTF(error_str /*, ": ", get_ok()*/);
			return forward_err();
		}

		/* --- unwrap --- */

		ok_reference unwrap() & { return expect("unwrap failed"); }

		ok_const_reference unwrap() const & { return expect("unwrap failed"); }

		OK unwrap() && { return lak::forward<OK>(expect("unwrap failed")); }

		/* --- unwrap_err --- */

		err_reference unwrap_err() & { return expect_err("unwrap_err failed"); }

		err_const_reference unwrap_err() const &
		{
			return expect_err("unwrap_err failed");
		}

		ERR unwrap_err() &&
		{
			return lak::forward<ERR>(expect_err("unwrap_err failed"));
		}

		/* --- unsafe_unwrap --- */

		ok_reference unsafe_unwrap() & { return get_ok(); }

		ok_const_reference unsafe_unwrap() const & { return get_ok(); }

		OK unsafe_unwrap() && { return lak::forward<OK>(get_ok()); }

		/* --- unsafe_unwrap_err --- */

		err_reference unsafe_unwrap_err() & { return get_err(); }

		err_const_reference unsafe_unwrap_err() const & { return get_err(); }

		ERR unsafe_unwrap_err() && { return lak::forward<ERR>(get_err()); }

		/* --- flatten --- */

		template<
		  typename U                                                       = OK,
		  lak::enable_if_i<lak::is_same_v<ERR, lak::result_err_type_t<U>>> = 0>
		ok_type flatten() const &
		{
			if (is_ok())
				return get_ok();
			else
				return lak::err_t{get_err()};
		}

		template<
		  typename U                                                       = OK,
		  lak::enable_if_t<lak::is_same_v<ERR, lak::result_err_type_t<U>>> = 0>
		ok_type flatten() &&
		{
			if (is_ok())
				return forward_ok();
			else
				return lak::err_t{forward_err()};
		}

		/* --- if_ok --- */

		template<typename FUNCTOR>
		result &if_ok(FUNCTOR &&functor) &
		{
			if (is_ok()) functor(get_ok());
			return *this;
		}

		template<typename FUNCTOR>
		const result &if_ok(FUNCTOR &&functor) const &
		{
			if (is_ok()) functor(get_ok());
			return *this;
		}

		template<typename FUNCTOR>
		result &&if_ok(FUNCTOR &&functor) &&
		{
			if (is_ok()) functor(forward_ok());
			return lak::move(*this);
		}

		/* --- if_err --- */

		template<typename FUNCTOR>
		result &if_err(FUNCTOR &&functor) &
		{
			if (is_err()) functor(get_err());
			return *this;
		}

		template<typename FUNCTOR>
		const result &if_err(FUNCTOR &&functor) const &
		{
			if (is_err()) functor(get_err());
			return *this;
		}

		template<typename FUNCTOR>
		result &&if_err(FUNCTOR &&functor) &&
		{
			if (is_err()) functor(forward_err());
			return lak::move(*this);
		}

		/* --- if_ok_copy_to --- */

		template<typename T = ok_type, typename T_ = T>
		requires(lak::is_same_v<T, T_>) //
		  result if_ok_copy_to(T_ &out)
		const &
		{
			if constexpr (lak::is_same_v<T, ok_type>)
			{
				if (is_ok()) out = get_ok();
			}
			else
			{
				if (is_ok()) out = T(get_ok());
			}
			return *this;
		}

		/* --- if_err_copy_to --- */

		template<typename T = err_type, typename T_ = T>
		requires(lak::is_same_v<T, T_>) //
		  result if_err_copy_to(T_ &out)
		const &
		{
			if constexpr (lak::is_same_v<T, err_type>)
			{
				if (is_err()) out = get_err();
			}
			else
			{
				if (is_err()) out = T(get_err());
			}
			return *this;
		}

		/* --- if_ok_move_to --- */

		template<typename T = ok_type, typename T_ = T>
		requires(lak::is_same_v<T, T_>) //
		  result if_ok_move_to(T_ &out)
		{
			if constexpr (lak::is_same_v<T, ok_type>)
			{
				if (is_ok()) out = forward_ok();
			}
			else
			{
				if (is_ok()) out = T(forward_ok());
			}
			return *this;
		}

		/* --- if_err_move_to --- */

		template<typename T = err_type, typename T_ = T>
		requires(lak::is_same_v<T, T_>) //
		  result if_err_move_to(T_ &out)
		{
			if constexpr (lak::is_same_v<T, err_type>)
			{
				if (is_err()) out = forward_err();
			}
			else
			{
				if (is_err()) out = T(forward_err());
			}
			return *this;
		}

		/* --- discard --- */

		void discard() const {}
	};

	/* --- ok_or_err --- */

	template<typename T>
	T &ok_or_err(lak::result<T, T> &result)
	{
		if (result.is_ok())
			return result.unsafe_unwrap();
		else
			return result.unsafe_unwrap_err();
	}

	template<typename T>
	const T &ok_or_err(const lak::result<T, T> &result)
	{
		if (result.is_ok())
			return result.unsafe_unwrap();
		else
			return result.unsafe_unwrap_err();
	}

	template<typename T>
	T ok_or_err(lak::result<T, T> &&result)
	{
		if (result.is_ok())
			return lak::move(result).unsafe_unwrap();
		else
			return lak::move(result).unsafe_unwrap_err();
	}

	/* --- result_from_pointer --- */

	template<typename T>
	lak::result<T &> result_from_pointer(T *ptr)
	{
		if (ptr)
			return lak::ok_t<T &>{*ptr};
		else
			return lak::err_t{};
	}

	static_assert(
	  lak::is_same_v<decltype(result_from_pointer(lak::declval<int *>())),
	                 lak::result<int &>>);
	static_assert(
	  lak::is_same_v<decltype(result_from_pointer(lak::declval<const int *>())),
	                 lak::result<const int &>>);

	/* --- copy_result_from_pointer --- */

	template<typename T>
	lak::result<lak::remove_const_t<T>> copy_result_from_pointer(T *ptr)
	{
		if (ptr)
			return lak::ok_t<lak::remove_const_t<T>>{*ptr};
		else
			return lak::err_t{};
	}

	static_assert(
	  lak::is_same_v<decltype(copy_result_from_pointer(lak::declval<int *>())),
	                 lak::result<int>>);
	static_assert(lak::is_same_v<decltype(copy_result_from_pointer(
	                               lak::declval<const int *>())),
	                             lak::result<int>>);
	static_assert(lak::is_same_v<decltype(copy_result_from_pointer(
	                               lak::declval<const int **>())),
	                             lak::result<const int *>>);

	template<typename OK, typename... ERR>
	using results = lak::result<OK, lak::variant<ERR...>>;

	template<typename ERR>
	using error_code = lak::result<lak::monostate, ERR>;

	template<typename... ERR>
	using error_codes = lak::error_code<lak::variant<ERR...>>;

	template<typename OK>
	using infallible_result = lak::result<OK, lak::infallible>;

	template<typename... T>
	struct is_result<lak::result<T...>> : public lak::true_type
	{
	};

	template<typename OK, typename ERR>
	struct result_ok_type<lak::result<OK, ERR>> : public lak::type_identity<OK>
	{
	};

	template<typename OK, typename ERR>
	struct result_err_type<lak::result<OK, ERR>> : public lak::type_identity<ERR>
	{
	};

	namespace concepts
	{
		/* --- result_with_ok --- */

		template<typename T, typename OK>
		concept result_with_ok =
		  lak::is_result_v<T> && lak::is_same_v<lak::result_ok_type_t<T>, OK>;

		/* --- result_with_err --- */

		template<typename T, typename ERR>
		concept result_with_err =
		  lak::is_result_v<T> && lak::is_same_v<lak::result_err_type_t<T>, ERR>;
	}

#define if_let_ok(VALUE, ...)                                                 \
	if (auto &&UNIQUIFY(RESULT_) = __VA_ARGS__; UNIQUIFY(RESULT_).is_ok())      \
		do_with (VALUE =                                                          \
		           lak::forward<decltype(UNIQUIFY(RESULT_))>(UNIQUIFY(RESULT_))   \
		             .unsafe_unwrap())

#define if_let_err(VALUE, ...)                                                \
	if (auto &&UNIQUIFY(RESULT_) = __VA_ARGS__; UNIQUIFY(RESULT_).is_err())     \
		do_with (VALUE =                                                          \
		           lak::forward<decltype(UNIQUIFY(RESULT_))>(UNIQUIFY(RESULT_))   \
		             .unsafe_unwrap_err())

#ifndef NOLOG
#	define EXPECT(...)                                                         \
		expect(lak::streamify(DEBUG_FATAL_LINE_FILE __VA_OPT__(, ) __VA_ARGS__))
#	define EXPECT_ERR(...)                                                     \
		expect_err(                                                               \
		  lak::streamify(DEBUG_FATAL_LINE_FILE __VA_OPT__(, ) __VA_ARGS__))

#	define UNWRAP()     EXPECT("unwrap failed")
#	define UNWRAP_ERR() EXPECT_ERR("unwrap_err failed")
#else
#	define EXPECT(...)     expect(lak::streamify(__VA_ARGS__))
#	define EXPECT_ERR(...) expect_err(lak::streamify(__VA_ARGS__))

#	define UNWRAP()     unwrap()
#	define UNWRAP_ERR() unwrap_err()
#endif

#define IF_OK(...)                                                            \
	if_ok([&](const auto &val) { DEBUG(__VA_ARGS__, ": ", val); })
#define IF_ERR(...)                                                           \
	if_err([&](const auto &err) { ERROR(__VA_ARGS__, ": ", err); })
#define IF_ERR_WARN(...)                                                      \
	if_err([&](const auto &err) { WARNING(__VA_ARGS__, ": ", err); })

#define RES_TRY_FLUENT(...)                                                   \
	auto UNIQUIFY(RESULT_) = __VA_ARGS__;                                       \
	if (UNIQUIFY(RESULT_).is_err())                                             \
		return lak::err_t{lak::move(UNIQUIFY(RESULT_)).unsafe_unwrap_err()};      \
	lak::move(UNIQUIFY(RESULT_)).unsafe_unwrap()

#define RES_TRY_ASSIGN(ASSIGN, ...)                                           \
	auto UNIQUIFY(RESULT_) = __VA_ARGS__;                                       \
	if (UNIQUIFY(RESULT_).is_err())                                             \
		return lak::err_t{lak::move(UNIQUIFY(RESULT_)).unsafe_unwrap_err()};      \
	ASSIGN lak::move(UNIQUIFY(RESULT_)).unsafe_unwrap()

#define RES_TRY(...)                                                          \
	do                                                                          \
	{                                                                           \
		if_let_err (auto &&err, __VA_ARGS__)                                      \
			return lak::err_t{lak::forward<decltype(err)>(err)};                    \
	} while (false)

}

#endif
