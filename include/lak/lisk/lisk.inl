#include "lak/lisk/lisk.hpp"

/* --- traits --- */

template<typename T>
requires lak::lisk::concepts::to_expr_convertible<T>
lak::lisk::expression lak::lisk::to_expr(const T &value)
{
	return lak::lisk::to_expr_traits<T>::to_expr(value);
}

template<typename T>
requires lak::lisk::concepts::from_expr_convertible<T>
auto lak::lisk::from_expr(const lak::lisk::expression &expr)
{
	return lak::lisk::from_expr_traits<T>::from_expr(expr);
}

template<>
struct lak::lisk::from_expr_traits<lak::lisk::expression>
{
	using value_type = lak::lisk::expression;
	using error_type = lak::bottom;
	static lak::infallible_result<lak::lisk::expression> from_expr(
	  const lak::lisk::expression &expr)
	{
		return lak::ok_t{expr};
	}
};

static_assert(
  lak::lisk::concepts::from_expr_convertible<lak::lisk::expression>);

template<>
struct lak::lisk::from_expr_traits<lak::lisk::exception>
{
	using value_type = lak::lisk::exception;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::exception> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::exception exc, expr.get_exception())
			return lak::move_ok(exc);
		else
			return lak::err_t{};
	}
};

static_assert(
  lak::lisk::concepts::from_expr_convertible<lak::lisk::exception>);

template<>
struct lak::lisk::from_expr_traits<lak::lisk::uneval_expr>
{
	using value_type = lak::lisk::uneval_expr;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::uneval_expr> from_expr(
	  const lak::lisk::expression &expr)
	{
		return lak::ok_t<lak::lisk::uneval_expr>{{expr}};
	}
};

static_assert(
  lak::lisk::concepts::from_expr_convertible<lak::lisk::uneval_expr>);

template<>
struct lak::lisk::from_expr_traits<lak::lisk::shared_list>
{
	using value_type = lak::lisk::shared_list;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::shared_list> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::shared_list list, expr.get_list())
			return lak::move_ok(list);
		else
			return lak::err_t{};
	}
};

static_assert(
  lak::lisk::concepts::from_expr_convertible<lak::lisk::shared_list>);

template<>
struct lak::lisk::from_expr_traits<lak::lisk::eval_shared_list>
{
	using value_type = lak::lisk::eval_shared_list;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::eval_shared_list> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::eval_shared_list list, expr.get_eval_list())
			return lak::move_ok(list);
		else
			return lak::err_t{};
	}
};

static_assert(
  lak::lisk::concepts::from_expr_convertible<lak::lisk::eval_shared_list>);

template<typename T>
struct lak::lisk::from_expr_traits<T *>
{
	using value_type = T *;
	using error_type = lak::monostate;
	static lak::result<T *> from_expr(const lak::lisk::expression &expr)
	{
		if_let_ok (T * ptr,
		           expr.get_atom()
		             .AND_THEN(get_pointer())
		             .AND_THEN(template get<T>()))
			return lak::ok_t{ptr};
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<int *>);

template<typename T>
struct lak::lisk::from_expr_traits<lak::shared_ptr<T>>
{
	using value_type = lak::shared_ptr<T>;
	using error_type = lak::monostate;
	static lak::result<lak::shared_ptr<T>> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::shared_ptr<T> ptr,
		           expr.get_atom()
		             .AND_THEN(get_pointer())
		             .AND_THEN(template get_shared<T>()))
			return lak::move_ok(ptr);
		else
			return lak::err_t{};
	}
};

static_assert(
  lak::lisk::concepts::from_expr_convertible<lak::shared_ptr<int>>);

/* --- string --- */

template<>
struct std::hash<lak::lisk::string> : public std::hash<lak::u8string>
{
};

template<typename CHAR>
struct lak::format_traits<lak::lisk::string, CHAR>
{
	static lak::string<CHAR> to_string(const lak::lisk::string &str)
	{
		if constexpr (lak::is_same_v<CHAR, char8_t>)
			return str;
		else
			return lak::strconv<CHAR>(str);
	}
};

template<>
struct lak::lisk::from_expr_traits<lak::lisk::string>
{
	using value_type = lak::lisk::string;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::string> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::string s, expr.get_atom().AND_THEN(get_string()))
			return lak::move_ok(s);
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::string>);

/* --- symbol --- */

template<>
struct std::hash<lak::lisk::symbol> : public std::hash<lak::u8string>
{
};

template<typename CHAR>
struct lak::format_traits<lak::lisk::symbol, CHAR>
{
	static lak::string<CHAR> to_string(const lak::lisk::symbol &sym)
	{
		if constexpr (lak::is_same_v<CHAR, char8_t>)
			return sym;
		else
			return lak::strconv<CHAR>(sym);
	}
};

template<>
struct lak::lisk::from_expr_traits<lak::lisk::symbol>
{
	using value_type = lak::lisk::symbol;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::symbol> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::symbol s, expr.get_atom().AND_THEN(get_symbol()))
			return lak::move_ok(s);
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::symbol>);

/* --- number --- */

inline lak::lisk::number::number(lak::lisk::uint_t u)
: _value(lak::in_place_index<value_type::index_of<lak::lisk::uint_t>>, u)
{
}

inline lak::lisk::number::number(lak::lisk::sint_t s)
: _value(lak::in_place_index<value_type::index_of<lak::lisk::sint_t>>, s)
{
}

inline lak::lisk::number::number(lak::lisk::real_t r)
: _value(lak::in_place_index<value_type::index_of<lak::lisk::real_t>>, r)
{
}

inline lak::lisk::number &lak::lisk::number::operator=(lak::lisk::uint_t u)
{
	_value.emplace<value_type::index_of<lak::lisk::uint_t>>(u);
	return *this;
}

inline lak::lisk::number &lak::lisk::number::operator=(lak::lisk::sint_t s)
{
	_value.emplace<value_type::index_of<lak::lisk::sint_t>>(s);
	return *this;
}

inline lak::lisk::number &lak::lisk::number::operator=(lak::lisk::real_t r)
{
	_value.emplace<value_type::index_of<lak::lisk::real_t>>(r);
	return *this;
}

inline bool lak::lisk::number::is_uint() const
{
	return _value.holds<value_type::index_of<lak::lisk::uint_t>>();
}

inline bool lak::lisk::number::is_sint() const
{
	return _value.holds<value_type::index_of<lak::lisk::sint_t>>();
}

inline bool lak::lisk::number::is_real() const
{
	return _value.holds<value_type::index_of<lak::lisk::real_t>>();
}

inline lak::result<lak::lisk::uint_t &> lak::lisk::number::get_uint() &
{
	return lak::get<lak::lisk::uint_t>(_value);
}

inline lak::result<const lak::lisk::uint_t &> lak::lisk::number::get_uint()
  const &
{
	return lak::get<lak::lisk::uint_t>(_value);
}

inline lak::result<lak::lisk::uint_t> lak::lisk::number::get_uint() &&
{
	return lak::get<lak::lisk::uint_t>(lak::move(_value));
}

inline lak::result<lak::lisk::sint_t &> lak::lisk::number::get_sint() &
{
	return lak::get<lak::lisk::sint_t>(_value);
}

inline lak::result<const lak::lisk::sint_t &> lak::lisk::number::get_sint()
  const &
{
	return lak::get<lak::lisk::sint_t>(_value);
}

inline lak::result<lak::lisk::sint_t> lak::lisk::number::get_sint() &&
{
	return lak::get<lak::lisk::sint_t>(lak::move(_value));
}

inline lak::result<lak::lisk::real_t &> lak::lisk::number::get_real() &
{
	return lak::get<lak::lisk::real_t>(_value);
}

inline lak::result<const lak::lisk::real_t &> lak::lisk::number::get_real()
  const &
{
	return lak::get<lak::lisk::real_t>(_value);
}

inline lak::result<lak::lisk::real_t> lak::lisk::number::get_real() &&
{
	return lak::get<lak::lisk::real_t>(lak::move(_value));
}

lak::lisk::number operator+(lak::lisk::number A, lak::lisk::number B)
{
	return lak::visit([](auto &&A, auto &&B) -> lak::lisk::number
	                  { return lak::lisk::number(A + B); },
	                  A._value,
	                  B._value);
}

lak::lisk::number operator-(lak::lisk::number A, lak::lisk::number B)
{
	return lak::visit([](auto &&A, auto &&B) -> lak::lisk::number
	                  { return lak::lisk::number(A - B); },
	                  A._value,
	                  B._value);
}

lak::lisk::number operator*(lak::lisk::number A, lak::lisk::number B)
{
	return lak::visit([](auto &&A, auto &&B) -> lak::lisk::number
	                  { return lak::lisk::number(A * B); },
	                  A._value,
	                  B._value);
}

lak::lisk::number operator/(lak::lisk::number A, lak::lisk::number B)
{
	return lak::visit([](auto &&A, auto &&B) -> lak::lisk::number
	                  { return lak::lisk::number(A / B); },
	                  A._value,
	                  B._value);
}

lak::lisk::number &operator+=(lak::lisk::number &A, lak::lisk::number B)
{
	return A = A + B;
}

lak::lisk::number &operator-=(lak::lisk::number &A, lak::lisk::number B)
{
	return A = A - B;
}

lak::lisk::number &operator*=(lak::lisk::number &A, lak::lisk::number B)
{
	return A = A * B;
}

lak::lisk::number &operator/=(lak::lisk::number &A, lak::lisk::number B)
{
	return A = A / B;
}

template<>
struct lak::lisk::from_expr_traits<lak::lisk::number>
{
	using value_type = lak::lisk::number;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::number> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (const auto &n, expr.get_atom().AND_THEN(get_number()))
			return lak::ok_t{n};
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::number>);

template<>
struct lak::lisk::from_expr_traits<lak::lisk::uint_t>
{
	using value_type = lak::lisk::uint_t;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::uint_t> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::uint_t v,
		           expr.get_atom().AND_THEN(get_number()).AND_THEN(get_uint()))
			return lak::ok_t{v};
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::uint_t>);

template<>
struct lak::lisk::from_expr_traits<lak::lisk::sint_t>
{
	using value_type = lak::lisk::sint_t;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::sint_t> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::sint_t v,
		           expr.get_atom().AND_THEN(get_number()).AND_THEN(get_sint()))
			return lak::ok_t{v};
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::sint_t>);

template<>
struct lak::lisk::from_expr_traits<lak::lisk::real_t>
{
	using value_type = lak::lisk::real_t;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::real_t> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::real_t v,
		           expr.get_atom().AND_THEN(get_number()).AND_THEN(get_real()))
			return lak::ok_t{v};
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::real_t>);

/* --- pointer --- */

template<typename T>
lak::lisk::pointer::pointer(T *ptr)
: _value(lak::in_place_index<value_type::index_of<void *>>, ptr),
  _type(typeid(T))
{
}

template<typename T>
lak::lisk::pointer::pointer(const T *ptr)
: _value(lak::in_place_index<value_type::index_of<const void *>>, ptr),
  _type(typeid(T))
{
}

template<typename T>
lak::lisk::pointer::pointer(const lak::shared_ptr<T> &ptr)
: _value(lak::in_place_index<value_type::index_of<lak::shared_ptr<void>>>,
         ptr),
  _type(typeid(T))
{
}

template<typename T>
lak::lisk::pointer &lak::lisk::pointer::operator=(T *ptr)
{
	_value.emplace<value_type::index_of<void *>>(ptr);
	_type = std::type_index(typeid(T));
	return *this;
}

template<typename T>
lak::lisk::pointer &lak::lisk::pointer::operator=(const T *ptr)
{
	_value.emplace<value_type::index_of<const void *>>(ptr);
	_type = std::type_index(typeid(T));
	return *this;
}

template<typename T>
lak::lisk::pointer &lak::lisk::pointer::operator=(
  const lak::shared_ptr<T> &ptr)
{
	_value.emplace<value_type::index_of<lak::shared_ptr<void>>>(ptr);
	_type = std::type_index(typeid(T));
	return *this;
}

template<typename T>
bool lak::lisk::pointer::is_raw_ptr() const
{
	return _type == std::type_index(typeid(T)) &&
	       _value.holds<value_type::index_of<void *>>();
}

template<typename T>
bool lak::lisk::pointer::is_raw_const_ptr() const
{
	return _type == std::type_index(typeid(T)) &&
	       _value.holds<value_type::index_of<const void *>>();
}

template<typename T>
bool lak::lisk::pointer::is_shared_ptr() const
{
	return _type == std::type_index(typeid(T)) &&
	       _value.holds<value_type::index_of<lak::shared_ptr<void>>>();
}

template<typename T>
T *lak::lisk::pointer::as_raw_ptr() const
{
	return static_cast<T *>(lak::get<void *>(_value).UNWRAP());
}

template<typename T>
const T *lak::lisk::pointer::as_raw_const_ptr() const
{
	return static_cast<const T *>(lak::get<const void *>(_value).UNWRAP());
}

template<typename T>
lak::shared_ptr<T> lak::lisk::pointer::as_shared_ptr() const
{
	return lak::shared_ptr<T>(lak::get<lak::shared_ptr<void>>(_value).UNWRAP());
}

template<typename T>
lak::lisk::string lak::lisk::to_string(T *t)
{
	// :TODO:
	return u8"<POINTER>";
}

template<typename T>
const lak::lisk::string &lak::lisk::type_name(T *t)
{
	const static lak::lisk::string name = u8"<" + type_name(*t) + u8"*>";
	return name;
}

template<typename T>
lak::lisk::string lak::lisk::to_string(const T *)
{
	// :TODO:
	return u8"<POINTER>";
}

template<typename T>
const lak::lisk::string &lak::lisk::type_name(const T *)
{
	const static lak::lisk::string name = u8"<const " + type_name(T{}) + u8"*>";
	return name;
}

template<typename T>
lak::lisk::string lak::lisk::to_string(const lak::shared_ptr<T> &)
{
	// :TODO:
	return u8"<POINTER>";
}

template<typename T>
const lak::lisk::string &lak::lisk::type_name(const lak::shared_ptr<T> &t)
{
	const static string name = u8"<managed " + type_name(*t.get()) + u8"*>";
	return name;
}

/* --- atom -- */

inline lak::lisk::atom::atom(nil) : _value(nil{}) {}

inline lak::lisk::atom::atom(const lak::lisk::symbol &sym) : _value(sym) {}

inline lak::lisk::atom::atom(const string &str) : _value(str) {}

inline lak::lisk::atom::atom(const number &num) : _value(num) {}

inline lak::lisk::atom::atom(bool b)
: _value(lak::in_place_index<value_type::index_of<bool>>, b)
{
}

inline lak::lisk::atom::atom(const pointer &ptr) : _value(ptr) {}

inline lak::lisk::atom &lak::lisk::atom::operator=(lak::lisk::atom::nil)
{
	_value.emplace<value_type::index_of<lak::lisk::atom::nil>>();
	return *this;
}

inline lak::lisk::atom &lak::lisk::atom::operator=(
  const lak::lisk::symbol &sym)
{
	_value.emplace<value_type::index_of<lak::lisk::symbol>>(sym);
	return *this;
}

inline lak::lisk::atom &lak::lisk::atom::operator=(
  const lak::lisk::string &str)
{
	_value.emplace<value_type::index_of<lak::lisk::string>>(str);
	return *this;
}

inline lak::lisk::atom &lak::lisk::atom::operator=(
  const lak::lisk::number &num)
{
	_value.emplace<value_type::index_of<lak::lisk::number>>(num);
	return *this;
}

inline lak::lisk::atom &lak::lisk::atom::operator=(bool b)
{
	_value.emplace<value_type::index_of<bool>>(b);
	return *this;
}

inline lak::lisk::atom &lak::lisk::atom::operator=(
  const lak::lisk::pointer &ptr)
{
	_value.emplace<value_type::index_of<lak::lisk::pointer>>(ptr);
	return *this;
}

/* --- is_x --- */

inline bool lak::lisk::atom::is_nil() const
{
	return _value.template holds<lak::lisk::atom::nil>();
}

inline bool lak::lisk::atom::is_symbol() const
{
	return _value.template holds<lak::lisk::symbol>();
}

inline bool lak::lisk::atom::is_string() const
{
	return _value.template holds<lak::lisk::string>();
}

inline bool lak::lisk::atom::is_number() const
{
	return _value.template holds<lak::lisk::number>();
}

inline bool lak::lisk::atom::is_bool() const
{
	return _value.template holds<bool>();
}

inline bool lak::lisk::atom::is_pointer() const
{
	return _value.template holds<lak::lisk::pointer>();
}

/* --- get_x --- */

inline lak::result<lak::lisk::symbol &> lak::lisk::atom::get_symbol() &
{
	return lak::get<lak::lisk::symbol>(_value);
}

inline lak::result<const lak::lisk::symbol &> lak::lisk::atom::get_symbol()
  const &
{
	return lak::get<lak::lisk::symbol>(_value);
}

inline lak::result<lak::lisk::symbol> lak::lisk::atom::get_symbol() &&
{
	return lak::get<lak::lisk::symbol>(lak::move(_value));
}

inline lak::result<lak::lisk::string &> lak::lisk::atom::get_string() &
{
	return lak::get<lak::lisk::string>(_value);
}

inline lak::result<const lak::lisk::string &> lak::lisk::atom::get_string()
  const &
{
	return lak::get<lak::lisk::string>(_value);
}

inline lak::result<lak::lisk::string> lak::lisk::atom::get_string() &&
{
	return lak::get<lak::lisk::string>(lak::move(_value));
}

inline lak::result<lak::lisk::number &> lak::lisk::atom::get_number() &
{
	return lak::get<lak::lisk::number>(_value);
}

inline lak::result<const lak::lisk::number &> lak::lisk::atom::get_number()
  const &
{
	return lak::get<lak::lisk::number>(_value);
}

inline lak::result<lak::lisk::number> lak::lisk::atom::get_number() &&
{
	return lak::get<lak::lisk::number>(lak::move(_value));
}

inline lak::result<bool &> lak::lisk::atom::get_bool() &
{
	return lak::get<bool>(_value);
}

inline lak::result<const bool &> lak::lisk::atom::get_bool() const &
{
	return lak::get<bool>(_value);
}

inline lak::result<bool> lak::lisk::atom::get_bool() &&
{
	return lak::get<bool>(lak::move(_value));
}

inline lak::result<lak::lisk::pointer &> lak::lisk::atom::get_pointer() &
{
	return lak::get<lak::lisk::pointer>(_value);
}

inline lak::result<const lak::lisk::pointer &> lak::lisk::atom::get_pointer()
  const &
{
	return lak::get<lak::lisk::pointer>(_value);
}

inline lak::result<lak::lisk::pointer> lak::lisk::atom::get_pointer() &&
{
	return lak::get<lak::lisk::pointer>(lak::move(_value));
}

template<>
struct lak::lisk::from_expr_traits<lak::lisk::atom::nil>
{
	using value_type = lak::lisk::atom::nil;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::atom::nil> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (const auto &a, expr.get_atom())
			if (a.is_nil()) return lak::ok_t<lak::lisk::atom::nil>{};
		return lak::err_t{};
	}
};

static_assert(
  lak::lisk::concepts::from_expr_convertible<lak::lisk::atom::nil>);

template<>
struct lak::lisk::from_expr_traits<bool>
{
	using value_type = bool;
	using error_type = lak::monostate;
	static lak::result<bool> from_expr(const lak::lisk::expression &expr)
	{
		if_let_ok (const auto &list, expr.get_list())
		{
			if (!list._node || (list._node->value.is_null() && !list._node->next))
				return lak::ok_t{false};
		}
		else if_let_ok (const auto &atom, expr.get_atom())
		{
			if_let_ok (bool b, atom.get_bool())
				return lak::ok_t{b};
			else if (atom.is_nil())
				return lak::ok_t{false};
		}
		return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<bool>);

template<>
struct lak::lisk::from_expr_traits<lak::lisk::atom>
{
	using value_type = lak::lisk::atom;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::atom> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::atom a, expr.get_atom())
			return lak::move_ok(a);
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::atom>);

/* --- shared_list --- */

template<typename T>
typename lak::lisk::basic_shared_list_node<T>::pointer_type
lak::lisk::basic_shared_list_node<T>::make()
{
	return pointer_type::make();
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::make()
{
	return {lak::lisk::basic_shared_list_node<T>::make()};
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::extends(
  const basic_shared_list &other)
{
	auto result = make();
	result.set_next(other);
	return result;
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::append(
  basic_shared_list &list, const T &v)
{
	lak::lisk::basic_shared_list result = list._node ? list.next() : list;
	result.value()                      = v;
	return result;
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::append(
  basic_shared_list &list, T &&v)
{
	lak::lisk::basic_shared_list result = list._node ? list.next() : list;
	result.value()                      = lak::move(v);
	return result;
}

template<typename T>
lak::lisk::basic_shared_list<T> &lak::lisk::basic_shared_list<T>::operator++()
{
	if (_node) _node = _node->next;
	return *this;
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::operator++(
  int)
{
	auto result = *this;
	operator++();
	return result;
}

template<typename T>
const T &lak::lisk::basic_shared_list<T>::value() const
{
	if (!_node) _node = lak::lisk::basic_shared_list_node<T>::make();
	return _node->value;
}

template<typename T>
T &lak::lisk::basic_shared_list<T>::value()
{
	if (!_node) _node = lak::lisk::basic_shared_list_node<T>::make();
	return _node->value;
}

template<typename T>
const T &lak::lisk::basic_shared_list<T>::next_value() const
{
	if (!_node) _node = lak::lisk::basic_shared_list_node<T>::make();
	if (!_node->next) _node->next = lak::lisk::basic_shared_list_node<T>::make();
	return _node->next->value;
}

template<typename T>
T &lak::lisk::basic_shared_list<T>::next_value()
{
	if (!_node) _node = lak::lisk::basic_shared_list_node<T>::make();
	if (!_node->next) _node->next = lak::lisk::basic_shared_list_node<T>::make();
	return _node->next->value;
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::next(
  size_t n) const
{
	auto result = _node;
	while (result && n-- > 0) result = result->next;
	return {result};
}

template<typename T>
void lak::lisk::basic_shared_list<T>::set_next(
  lak::lisk::basic_shared_list<T> l)
{
	if (!_node) _node = lak::lisk::basic_shared_list_node<T>::make();
	_node->next = l._node;
}

template<typename T>
void lak::lisk::basic_shared_list<T>::clear()
{
	_node.reset();
}

template<typename T>
void lak::lisk::basic_shared_list<T>::clear_next()
{
	if (_node) _node->next.reset();
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::begin() const
{
	// :TODO: .empty doesn't work here if we're dealing with unoreded_maps
	// where the beginning map might be empty!!!
	if (_node && !_node->next && _node->value.empty())
		return end();
	else
		return *this;
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::last() const
{
	auto result = *this;
	while (result._node && result._node->next) ++result;
	return result;
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::end() const
{
	return {};
}

template<typename T>
lak::lisk::basic_shared_list<T> lak::lisk::basic_shared_list<T>::clone(
  size_t depth) const
{
	auto result = make();
	auto last   = result;
	auto end    = last;
	if (depth == 0)
	{
		for (const auto &node : *this)
		{
			last.value() = node.value;
			last.set_next(make());
			end = last;
			++last;
		}
		end.clear_next();
	}
	else
	{
		auto iter = *this;
		while (depth-- > 0 && iter)
		{
			last.value() = iter.value();
			last.set_next(make());
			end = last;
			++last;
			++iter;
		}
		if (iter)
			end.set_next(iter);
		else
			end.clear_next();
	}
	return result;
}

template<typename T>
bool lak::lisk::basic_shared_list<T>::operator==(
  const lak::lisk::basic_shared_list<T> &other) const
{
	return _node.get() == other._node.get();
}

template<typename T>
bool lak::lisk::basic_shared_list<T>::operator!=(
  const lak::lisk::basic_shared_list<T> &other) const
{
	return !operator==(other);
}

template<typename T>
lak::lisk::basic_shared_list<T>::operator bool() const
{
	return _node && (_node->next || !_node->value.empty());
}

template<typename T>
const lak::lisk::basic_shared_list_node<T> &
lak::lisk::basic_shared_list<T>::operator*() const
{
	if (!_node) _node = lak::lisk::basic_shared_list_node<T>::make();
	return *_node;
}

template<typename T>
lak::lisk::basic_shared_list_node<T> &
lak::lisk::basic_shared_list<T>::operator*()
{
	if (!_node) _node = lak::lisk::basic_shared_list_node<T>::make();
	return *_node;
}

template<typename T>
const lak::lisk::basic_shared_list_node<T> *
lak::lisk::basic_shared_list<T>::operator->() const
{
	if (!_node) _node = lak::lisk::basic_shared_list_node<T>::make();
	return _node.get();
}

template<typename T>
lak::lisk::basic_shared_list_node<T> *
lak::lisk::basic_shared_list<T>::operator->()
{
	if (!_node) _node = lak::lisk::basic_shared_list_node<T>::make();
	return _node.get();
}

template<typename T>
lak::lisk::string lak::lisk::to_string(lak::lisk::basic_shared_list<T> list)
{
	if (!list._node) return "";
	if (list.value().empty()) return "()";

	lak::lisk::string result;

	result += '(';
	do
	{
		result += to_string(list.value());
		++list;
		if (list) result += ' ';
	} while (list);
	result += ')';

	return result;
}

template<typename T>
const lak::lisk::string &lak::lisk::type_name(
  const lak::lisk::basic_shared_list<T> &)
{
	const static lak::lisk::string name =
	  "basic_shared_list<" + type_name(T{}) + ">";
	return name;
}

/* --- function --- */

template<>
struct lak::lisk::from_expr_traits<lak::lisk::function>
{
	using value_type = lak::lisk::function;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::function> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::function f,
		           expr.get_callable().AND_THEN(get_function()))
			return lak::move_ok(f);
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::function>);

/* --- callable --- */

inline lak::lisk::callable::callable(const lak::lisk::lambda &l)
: _value(lak::in_place_index<decltype(_value)::index_of<lambda_ptr>>,
         lambda_ptr::make(l))
{
}

inline lak::lisk::callable::callable(const lak::lisk::function &f)
: _value(lak::in_place_index<decltype(_value)::index_of<lak::lisk::function>>,
         f)
{
}

lak::lisk::callable &lak::lisk::callable::operator=(const lak::lisk::lambda &l)
{
	_value.template emplace<decltype(_value)::index_of<lambda_ptr>>(
	  lambda_ptr::make(l));
	return *this;
}

lak::lisk::callable &lak::lisk::callable::operator=(
  const lak::lisk::function &f)
{
	_value.template emplace<decltype(_value)::index_of<lak::lisk::function>>(f);
	return *this;
}

inline bool lak::lisk::callable::is_null() const
{
	return !(is_lambda() || is_function());
}

inline bool lak::lisk::callable::is_lambda() const
{
	return lak::get<lambda_ptr>(_value).map_or(
	  [](const auto &l) -> bool { return (bool)l; }, false);
}

inline bool lak::lisk::callable::is_function() const
{
	return lak::get<lak::lisk::function>(_value).map_or(
	  [](const auto &l) -> bool { return l; }, false);
}

inline lak::result<lak::lisk::lambda &> lak::lisk::callable::get_lambda() &
{
	return lak::get<lambda_ptr>(_value).and_then(
	  [](lambda_ptr &p) { return lak::result_from_pointer(p.get()); });
}

inline lak::result<const lak::lisk::lambda &> lak::lisk::callable::get_lambda()
  const &
{
	return lak::get<lambda_ptr>(_value).and_then(
	  [](const lambda_ptr &p)
	  {
		  return lak::result_from_pointer(
		    static_cast<const lak::lisk::lambda *>(p.get()));
	  });
}

inline lak::result<lak::lisk::lambda> lak::lisk::callable::get_lambda() &&
{
	return lak::get<lambda_ptr>(_value).and_then(
	  [](lambda_ptr &p) { return lak::move_result_from_pointer(p.get()); });
}

inline lak::result<const lak::lisk::function &>
lak::lisk::callable::get_function() const
{
	return lak::get<lak::lisk::function>(_value);
}

template<>
struct lak::lisk::from_expr_traits<lak::lisk::callable>
{
	using value_type = lak::lisk::callable;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::callable> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::callable c, expr.get_callable())
			return lak::move_ok(c);
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::callable>);

/* --- expression --- */

lak::lisk::expression::expression(null)
: _value(lak::in_place_index<value_type::index_of<null>>)
{
}

lak::lisk::expression::expression(const lak::lisk::atom &a) : _value(a) {}

lak::lisk::expression::expression(lak::lisk::atom::nil)
: _value(lak::in_place_index<value_type::index_of<lak::lisk::atom>>,
         lak::lisk::atom::nil{})
{
}

lak::lisk::expression::expression(const lak::lisk::shared_list &list)
: _value(list)
{
}

lak::lisk::expression::expression(const lak::lisk::eval_shared_list &list)
: _value(list)
{
}

lak::lisk::expression::expression(const lak::lisk::callable &c) : _value(c) {}

lak::lisk::expression::expression(const lak::lisk::exception &exc)
: _value(exc)
{
}

lak::lisk::expression &lak::lisk::expression::operator=(null)
{
	_value.emplace<decltype(_value)::index_of<null>>();
	return *this;
}

lak::lisk::expression &lak::lisk::expression::operator=(
  const lak::lisk::atom &a)
{
	_value.emplace<decltype(_value)::index_of<atom>>(a);
	return *this;
}

lak::lisk::expression &lak::lisk::expression::operator=(lak::lisk::atom::nil)
{
	_value.emplace<decltype(_value)::index_of<atom>>(atom::nil{});
	return *this;
}

lak::lisk::expression &lak::lisk::expression::operator=(
  const lak::lisk::shared_list &list)
{
	_value.emplace<decltype(_value)::index_of<shared_list>>(list);
	return *this;
}

lak::lisk::expression &lak::lisk::expression::operator=(
  const lak::lisk::eval_shared_list &list)
{
	_value.emplace<decltype(_value)::index_of<eval_shared_list>>(list);
	return *this;
}

lak::lisk::expression &lak::lisk::expression::operator=(
  const lak::lisk::callable &c)
{
	_value.emplace<decltype(_value)::index_of<callable>>(c);
	return *this;
}

lak::lisk::expression &lak::lisk::expression::operator=(
  const lak::lisk::exception &exc)
{
	_value.emplace<decltype(_value)::index_of<exception>>(exc);
	return *this;
}

bool lak::lisk::expression::is_null() const
{
	return _value.template holds<null>();
}

bool lak::lisk::expression::is_atom() const
{
	return _value.template holds<lak::lisk::atom>();
}

bool lak::lisk::expression::is_list() const
{
	return _value.template holds<lak::lisk::shared_list>();
}

bool lak::lisk::expression::is_eval_list() const
{
	return _value.template holds<lak::lisk::eval_shared_list>();
}

bool lak::lisk::expression::is_callable() const
{
	return _value.template holds<lak::lisk::callable>();
}

bool lak::lisk::expression::is_exception() const
{
	return _value.template holds<lak::lisk::exception>();
}

inline lak::result<lak::lisk::atom &> lak::lisk::expression::get_atom() &
{
	return lak::get<lak::lisk::atom>(_value);
}

inline lak::result<const lak::lisk::atom &> lak::lisk::expression::get_atom()
  const &
{
	return lak::get<lak::lisk::atom>(_value);
}

inline lak::result<lak::lisk::atom> lak::lisk::expression::get_atom() &&
{
	return lak::get<lak::lisk::atom>(lak::move(_value));
}

inline lak::result<lak::lisk::shared_list &> lak::lisk::expression::get_list()
  &
{
	return lak::get<lak::lisk::shared_list>(_value);
}

inline lak::result<const lak::lisk::shared_list &>
lak::lisk::expression::get_list() const &
{
	return lak::get<lak::lisk::shared_list>(_value);
}

inline lak::result<lak::lisk::shared_list> lak::lisk::expression::get_list() &&
{
	return lak::get<lak::lisk::shared_list>(lak::move(_value));
}

inline lak::result<lak::lisk::eval_shared_list &>
lak::lisk::expression::get_eval_list() &
{
	return lak::get<lak::lisk::eval_shared_list>(_value);
}

inline lak::result<const lak::lisk::eval_shared_list &>
lak::lisk::expression::get_eval_list() const &
{
	return lak::get<lak::lisk::eval_shared_list>(_value);
}

inline lak::result<lak::lisk::eval_shared_list>
lak::lisk::expression::get_eval_list() &&
{
	return lak::get<lak::lisk::eval_shared_list>(lak::move(_value));
}

inline lak::result<lak::lisk::callable &> lak::lisk::expression::get_callable()
  &
{
	return lak::get<lak::lisk::callable>(_value);
}

inline lak::result<const lak::lisk::callable &>
lak::lisk::expression::get_callable() const &
{
	return lak::get<lak::lisk::callable>(_value);
}

inline lak::result<lak::lisk::callable> lak::lisk::expression::get_callable()
  &&
{
	return lak::get<lak::lisk::callable>(lak::move(_value));
}

inline lak::result<lak::lisk::exception &>
lak::lisk::expression::get_exception() &
{
	return lak::get<lak::lisk::exception>(_value);
}

inline lak::result<const lak::lisk::exception &>
lak::lisk::expression::get_exception() const &
{
	return lak::get<lak::lisk::exception>(_value);
}

inline lak::result<lak::lisk::exception> lak::lisk::expression::get_exception()
  &&
{
	return lak::get<lak::lisk::exception>(lak::move(_value));
}

/* --- environment --- */

/* --- lambda --- */

template<>
struct lak::lisk::from_expr_traits<lak::lisk::lambda>
{
	using value_type = lak::lisk::lambda;
	using error_type = lak::monostate;
	static lak::result<lak::lisk::lambda> from_expr(
	  const lak::lisk::expression &expr)
	{
		if_let_ok (lak::lisk::lambda l, expr.get_callable().AND_THEN(get_lambda()))
			return lak::move_ok(l);
		else
			return lak::err_t{};
	}
};

static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::lambda>);

/* --- eval --- */

template<typename T>
requires lak::lisk::concepts::from_expr_convertible<T>
lak::result<typename lak::lisk::from_expr_traits<T>::value_type>
lak::lisk::list_reader::read()
{
	static_assert(lak::lisk::list_reader_traits<T>::allow_get ||
	                lak::lisk::list_reader_traits<T>::allow_eval,
	              "Type must be get-able or eval-able");

	if (!list) return lak::err_t{};

	if constexpr (lak::lisk::list_reader_traits<T>::allow_get)
		if_let_ok (auto out, lak::lisk::from_expr<T>(list.value()))
		{
			++list;
			return lak::move_ok(out);
		}

	if constexpr (lak::lisk::list_reader_traits<T>::allow_eval)
		if_let_ok (auto out,
		           lak::lisk::from_expr<T>(
		             lak::lisk::eval(list.value(), env, allow_tail_eval)))
		{
			++list;
			return lak::move_ok(out);
		}

	return lak::err_t{};
}

template<typename... TYPES>
bool lak::lisk::impl::get_or_eval_arg_as(lak::lisk::shared_list in_list,
                                         lak::lisk::environment &e,
                                         bool allow_tail,
                                         lak::lisk::exception &exc,
                                         lak::tuple<TYPES...> &out_arg)
{
	auto _get_or_eval_arg_as =
	  []<typename... TS, size_t... I>(lak::lisk::shared_list in_list,
	                                  lak::lisk::environment &e,
	                                  bool allow_tail,
	                                  lak::lisk::exception &exc,
	                                  lak::tuple<TS...> &out_arg,
	                                  lak::index_sequence<I...>) -> bool
	{
		lak::tuple<lak::remove_cv_t<TS>...> result;

		lak::lisk::list_reader reader(in_list, e, allow_tail);

		[[maybe_unused]] auto _get_or_eval = [&]<typename T>(T &element,
		                                                     size_t i) -> bool
		{
			if_let_ok (auto e, reader.template read<T>())
			{
				element = lak::move(e);
				return true;
			}
			else
			{
				exc.message = lak::fmt<
				  u8"Failed to evaluate element {} '{}' of '{}', expected type '{}'">(
				  i,
				  to_string(reader.list.value()),
				  to_string(reader.list),
				  type_name(element));
				return false;
			}
		};
		if ((_get_or_eval(result.template get<I>(), I) && ...))
		{
			out_arg = lak::move(result);
			return true;
		}
		else
			return false;
	};

	if constexpr (sizeof...(TYPES) == 0)
		return true;
	else
		return _get_or_eval_arg_as(in_list,
		                           e,
		                           allow_tail,
		                           exc,
		                           out_arg,
		                           lak::index_sequence_for<TYPES...>{});
}
