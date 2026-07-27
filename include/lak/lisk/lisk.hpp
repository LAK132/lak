#ifndef LAK_LISK_HPP
#define LAK_LISK_HPP

#include "lak/array.hpp"
#include "lak/dsl/utility.hpp"
#include "lak/memory.hpp"
#include "lak/string.hpp"
#include "lak/tuple.hpp"
#include "lak/variant.hpp"

#include <regex>
#include <typeindex>
#include <unordered_map>

namespace lak
{
	namespace lisk
	{
		struct string;
		struct symbol;
		struct number;
		struct pointer;
		struct atom;
		struct lambda;
		struct callable;
		struct expression;
		struct environment;

		using uint_t = unsigned long long;
		using sint_t = signed long long;
		using real_t = long double;

		/* --- traits --- */

		template<typename T>
		struct to_expr_traits;

		template<typename T>
		struct from_expr_traits;

		template<typename T>
		struct list_reader_traits
		{
			// default to allowing both get-ing and eval-ing, this results in less
			// work to get user defined types working with lisk.
			// if the user wants to modify this behaviour, they can still override
			// these values (see specialisations below)
			static constexpr bool allow_get  = true;
			static constexpr bool allow_eval = true;
		};

		namespace concepts
		{
			template<typename T>
			concept to_expr_convertible = requires(const T value) {
				{
					lak::lisk::to_expr_traits<T>::to_expr(value)
				} -> lak::concepts::same_as<lak::lisk::expression>;
			};

			template<typename T>
			concept from_expr_convertible = requires() {
				typename lak::lisk::from_expr_traits<T>::value_type;
				typename lak::lisk::from_expr_traits<T>::error_type;
				lak::is_same_v<T, typename lak::lisk::from_expr_traits<T>::value_type>;
				{
					lak::lisk::from_expr_traits<T>::from_expr(
					  lak::declval<const lak::lisk::expression &>())
				} -> lak::concepts::same_as<
				  lak::result<typename lak::lisk::from_expr_traits<T>::value_type,
				              typename lak::lisk::from_expr_traits<T>::error_type>>;
			};
		}

		template<typename T>
		requires lak::lisk::concepts::to_expr_convertible<T>
		lak::lisk::expression to_expr(const T &value);

		template<typename T>
		requires lak::lisk::concepts::from_expr_convertible<T>
		auto from_expr(const lak::lisk::expression &expr);

		/* --- string --- */

		struct string : public lak::u8string
		{
			string()               = default;
			string(const string &) = default;
			string(string &&)      = default;

			string(const lak::u8string &value) : lak::u8string(value) {}
			string(lak::u8string &&value) : lak::u8string(lak::move(value)) {}
			string(const char8_t *value) : lak::u8string(value) {}
			string(const char *value) : lak::u8string(lak::to_u8string(value)) {}

			string &operator=(const string &) = default;
			string &operator=(string &&)      = default;

			string &operator=(const lak::u8string &value)
			{
				lak::u8string::operator=(value);
				return *this;
			}
			string &operator=(lak::u8string &&value)
			{
				lak::u8string::operator=(lak::move(value));
				return *this;
			}
			string &operator=(const char8_t *value)
			{
				lak::u8string::operator=(value);
				return *this;
			}
			string &operator=(const char *value)
			{
				lak::u8string::operator=(lak::to_u8string(value));
				return *this;
			}

			using lak::u8string::operator[];
		};

		lak::lisk::string to_string(const lak::lisk::string &str);
		const lak::lisk::string &type_name(const lak::lisk::string &);

		/* --- symbol --- */

		struct symbol : public lak::u8string
		{
			symbol()               = default;
			symbol(const symbol &) = default;
			symbol(symbol &&)      = default;

			symbol(const lak::u8string &value) : lak::u8string(value) {}
			symbol(lak::u8string &&value) : lak::u8string(lak::move(value)) {}
			symbol(const char8_t *value) : lak::u8string(value) {}
			symbol(const char *value) : lak::u8string(lak::to_u8string(value)) {}

			symbol &operator=(const symbol &) = default;
			symbol &operator=(symbol &&)      = default;

			symbol &operator=(const lak::u8string &value)
			{
				lak::u8string::operator=(value);
				return *this;
			}
			symbol &operator=(lak::u8string &&value)
			{
				lak::u8string::operator=(lak::move(value));
				return *this;
			}
			symbol &operator=(const char8_t *value)
			{
				lak::u8string::operator=(value);
				return *this;
			}
			symbol &operator=(const char *value)
			{
				lak::u8string::operator=(lak::to_u8string(value));
				return *this;
			}

			using lak::u8string::operator[];
		};

		lak::lisk::string to_string(const lak::lisk::symbol &sym);
		const lak::lisk::string &type_name(const lak::lisk::symbol &);

		/* --- number --- */

		struct number
		{
			using value_type =
			  lak::variant<lak::lisk::uint_t, lak::lisk::sint_t, lak::lisk::real_t>;
			value_type _value;

			number()                = default;
			number(const number &n) = default;

			number &operator=(const number &n) = default;

			inline number(lak::lisk::uint_t u);
			inline number(lak::lisk::sint_t s);
			inline number(lak::lisk::real_t r);

			inline number &operator=(lak::lisk::uint_t u);
			inline number &operator=(lak::lisk::sint_t s);
			inline number &operator=(lak::lisk::real_t r);

			inline bool is_uint() const;
			inline bool is_sint() const;
			inline bool is_real() const;

			inline lak::result<lak::lisk::uint_t &> get_uint() &;
			inline lak::result<const lak::lisk::uint_t &> get_uint() const &;
			inline lak::result<lak::lisk::uint_t> get_uint() &&;

			inline lak::result<lak::lisk::sint_t &> get_sint() &;
			inline lak::result<const lak::lisk::sint_t &> get_sint() const &;
			inline lak::result<lak::lisk::sint_t> get_sint() &&;

			inline lak::result<lak::lisk::real_t &> get_real() &;
			inline lak::result<const lak::lisk::real_t &> get_real() const &;
			inline lak::result<lak::lisk::real_t> get_real() &&;

			template<typename LAMBDA>
			auto visit(LAMBDA &&lambda) const
			{
				return lak::visit(lambda, _value);
			}

			template<typename LAMBDA>
			auto visit(LAMBDA &&lambda)
			{
				return lak::visit(lambda, _value);
			}
		};

		lak::lisk::string to_string(const lak::lisk::number &num);
		const lak::lisk::string &type_name(const lak::lisk::number &);

		lak::lisk::string to_string(lak::lisk::uint_t num);
		const lak::lisk::string &type_name(lak::lisk::uint_t);

		lak::lisk::string to_string(lak::lisk::sint_t num);
		const lak::lisk::string &type_name(lak::lisk::sint_t);

		lak::lisk::string to_string(lak::lisk::real_t num);
		const lak::lisk::string &type_name(lak::lisk::real_t);

		/* --- pointer --- */

		struct pointer
		{
			using value_type =
			  lak::variant<void *, const void *, lak::shared_ptr<void>>;
			value_type _value;
			std::type_index _type;

			inline pointer() : _value((const void *)nullptr), _type(typeid(void)) {}
			pointer(const pointer &ptr) = default;
			pointer(pointer &&ptr)      = default;

			pointer &operator=(const pointer &ptr) = default;
			pointer &operator=(pointer &&ptr)      = default;

			template<typename T>
			pointer(T *ptr);
			template<typename T>
			pointer(const T *ptr);
			template<typename T>
			pointer(const lak::shared_ptr<T> &ptr);

			template<typename T>
			pointer &operator=(T *ptr);
			template<typename T>
			pointer &operator=(const T *ptr);
			template<typename T>
			pointer &operator=(const lak::shared_ptr<T> &ptr);

			template<typename T>
			lak::result<T *> get() const
			{
				if (_type != std::type_index(typeid(T))) return lak::err_t{};

				if_let_ok (void *p, lak::get<void *>(_value))
					return static_cast<T *>(p);

				if constexpr (lak::is_const_v<T>)
					if_let_ok (const void *p, lak::get<const void *>(_value))
						return static_cast<T *>(p);

				if_let_ok (const auto &p, lak::get<lak::shared_ptr<void>>(_value))
					return static_cast<T *>(p.get());

				return lak::err_t{};
			}

			template<typename T>
			lak::result<lak::shared_ptr<T>> get_shared() const
			{
				if (_type != std::type_index(typeid(T))) return lak::err_t{};

				if_let_ok (const auto &p, lak::get<lak::shared_ptr<void>>(_value))
					return lak::ok_t{static_cast<lak::shared_ptr<T>>(p)};

				return lak::err_t{};
			}

			template<typename T>
			bool is_raw_ptr() const;
			template<typename T>
			bool is_raw_const_ptr() const;
			template<typename T>
			bool is_shared_ptr() const;

			template<typename T>
			T *as_raw_ptr() const;
			template<typename T>
			const T *as_raw_const_ptr() const;
			template<typename T>
			lak::shared_ptr<T> as_shared_ptr() const;
		};

		lak::lisk::string to_string(const lak::lisk::pointer &);
		const lak::lisk::string &type_name(const lak::lisk::pointer &);

		template<typename T>
		lak::lisk::string to_string(T *);
		template<typename T>
		const lak::lisk::string &type_name(T *);

		template<typename T>
		lak::lisk::string to_string(const T *);
		template<typename T>
		const lak::lisk::string &type_name(const T *);

		template<typename T>
		lak::lisk::string to_string(const lak::shared_ptr<T> &);
		template<typename T>
		const lak::lisk::string &type_name(const lak::shared_ptr<T> &);

		/* --- atom --- */

		struct atom
		{
			struct nil
			{
			};

			using value_type = lak::variant<nil,
			                                lak::lisk::symbol,
			                                lak::lisk::string,
			                                lak::lisk::number,
			                                bool,
			                                lak::lisk::pointer>;
			value_type _value;

			atom()              = default;
			atom(const atom &a) = default;
			atom(atom &&a)      = default;

			atom &operator=(const atom &expr) = default;
			atom &operator=(atom &&expr)      = default;

			inline atom(nil);
			inline atom(const lak::lisk::symbol &sym);
			inline atom(const lak::lisk::string &str);
			inline atom(const lak::lisk::number &num);
			inline atom(bool b);
			inline atom(const lak::lisk::pointer &ptr);

			inline atom &operator=(nil);
			inline atom &operator=(const lak::lisk::symbol &sym);
			inline atom &operator=(const lak::lisk::string &str);
			inline atom &operator=(const lak::lisk::number &num);
			inline atom &operator=(bool b);
			inline atom &operator=(const lak::lisk::pointer &ptr);

			inline bool is_nil() const;
			inline bool is_symbol() const;
			inline bool is_string() const;
			inline bool is_number() const;
			inline bool is_bool() const;
			inline bool is_pointer() const;

			inline lak::result<lak::lisk::symbol &> get_symbol() &;
			inline lak::result<const lak::lisk::symbol &> get_symbol() const &;
			inline lak::result<lak::lisk::symbol> get_symbol() &&;

			inline lak::result<lak::lisk::string &> get_string() &;
			inline lak::result<const lak::lisk::string &> get_string() const &;
			inline lak::result<lak::lisk::string> get_string() &&;

			inline lak::result<lak::lisk::number &> get_number() &;
			inline lak::result<const lak::lisk::number &> get_number() const &;
			inline lak::result<lak::lisk::number> get_number() &&;

			inline lak::result<bool &> get_bool() &;
			inline lak::result<const bool &> get_bool() const &;
			inline lak::result<bool> get_bool() &&;

			inline lak::result<lak::lisk::pointer &> get_pointer() &;
			inline lak::result<const lak::lisk::pointer &> get_pointer() const &;
			inline lak::result<lak::lisk::pointer> get_pointer() &&;

			template<typename LAMBDA>
			auto visit(LAMBDA &&lambda) const
			{
				return lak::visit(lambda, _value);
			}

			template<typename LAMBDA>
			auto visit(LAMBDA &&lambda)
			{
				return lak::visit(lambda, _value);
			}
		};

		lak::lisk::string to_string(lak::lisk::atom::nil);
		const lak::lisk::string &type_name(lak::lisk::atom::nil);

		lak::lisk::string to_string(bool b);
		const lak::lisk::string &type_name(bool);

		lak::lisk::string to_string(const lak::lisk::atom &a);
		const lak::lisk::string &type_name(const lak::lisk::atom &);

		/* --- shared_list --- */

		template<typename T>
		struct basic_shared_list_node
		{
			using pointer_type = lak::shared_ptr<basic_shared_list_node>;

			T value;
			pointer_type next;

			static pointer_type make();
		};

		template<typename T>
		struct basic_shared_list
		{
			mutable lak::shared_ptr<lak::lisk::basic_shared_list_node<T>> _node = {};

			static basic_shared_list make();

			// prepend
			static basic_shared_list extends(const basic_shared_list &other);
			static basic_shared_list append(basic_shared_list &list, const T &v);
			static basic_shared_list append(basic_shared_list &list, T &&v);

			basic_shared_list &operator++();
			basic_shared_list operator++(int);

			const T &value() const;
			T &value();
			const T &next_value() const;
			T &next_value();

			basic_shared_list next(size_t n = 1) const;
			void set_next(basic_shared_list l);

			void clear();
			void clear_next();

			basic_shared_list begin() const;
			basic_shared_list last() const;
			basic_shared_list end() const;

			// 0 for full clone
			basic_shared_list clone(size_t depth = 0) const;

			bool operator==(const basic_shared_list &other) const;
			bool operator!=(const basic_shared_list &other) const;

			operator bool() const;

			const lak::lisk::basic_shared_list_node<T> &operator*() const;
			lak::lisk::basic_shared_list_node<T> &operator*();

			const lak::lisk::basic_shared_list_node<T> *operator->() const;
			lak::lisk::basic_shared_list_node<T> *operator->();
		};

		template<typename T>
		lak::lisk::string to_string(lak::lisk::basic_shared_list<T> list);

		template<typename T>
		const lak::lisk::string &type_name(
		  const lak::lisk::basic_shared_list<T> &);

		using shared_list = lak::lisk::basic_shared_list<lak::lisk::expression>;

		struct eval_shared_list
		{
			lak::lisk::shared_list list;
		};

		struct uneval_shared_list
		{
			lak::lisk::shared_list list;
		};

		/* --- function --- */

		template<typename FUNC>
		struct function_signature;

		template<typename R, typename... ARGS>
		struct function_signature<R (*)(ARGS...)>
		{
			using return_type = R;
			using arguments   = lak::tuple<ARGS...>;
		};

		template<typename R, typename... ARGS>
		struct function_signature<R(ARGS...)>
		{
			using return_type = R;
			using arguments   = lak::tuple<ARGS...>;
		};

		template<typename FUNC>
		using function_return_t = typename function_signature<FUNC>::return_type;

		template<typename FUNC>
		using function_arguments_t = typename function_signature<FUNC>::arguments;

		template<typename TUPLE>
		struct as_function_arguments;

		template<typename... ARGS>
		struct as_function_arguments<lak::tuple<environment &, bool, ARGS...>>
		{
			using arguments = lak::tuple<ARGS...>;
		};

		template<typename TUPLE>
		using as_function_arguments_t =
		  typename as_function_arguments<TUPLE>::arguments;

		typedef lak::pair<lak::lisk::expression, size_t> (*function)(
		  lak::lisk::basic_shared_list<lak::lisk::expression>,
		  lak::lisk::environment &,
		  bool);

#define LISK_FUNCTION_WRAPPER(F)                                              \
	static_cast<lak::lisk::function>(                                           \
	  [](lak::lisk::shared_list l,                                              \
	     lak::lisk::environment &e,                                             \
	     bool allow_tail) -> lak::pair<lak::lisk::expression, size_t>           \
	  {                                                                         \
			static_assert(                                                          \
			  lak::is_same_v<                                                       \
			    lak::tuple_element_t<0,                                             \
			                         lak::lisk::function_arguments_t<decltype(F)>>, \
			    lak::lisk::environment &>);                                         \
                                                                              \
			static_assert(                                                          \
			  lak::is_same_v<                                                       \
			    lak::tuple_element_t<1,                                             \
			                         lak::lisk::function_arguments_t<decltype(F)>>, \
			    bool>);                                                             \
                                                                              \
			static_assert(lak::is_same_v<lak::lisk::function_return_t<decltype(F)>, \
			                             lak::lisk::expression>);                   \
                                                                              \
			using arguments_t = lak::lisk::as_function_arguments_t<                 \
			  lak::lisk::function_arguments_t<decltype(F)>>;                        \
			static_assert(lak::is_tuple_v<arguments_t>);                            \
                                                                              \
			arguments_t args;                                                       \
			lak::lisk::exception exc;                                               \
                                                                              \
			if (!lak::lisk::impl::get_or_eval_arg_as(l, e, allow_tail, exc, args))  \
			{                                                                       \
				return lak::pair<lak::lisk::expression, size_t>(exc, 0);              \
			}                                                                       \
			else                                                                    \
			{                                                                       \
				lak::lisk::expression exp{lak::apply(                                 \
				  F, lak::tuple_cat(lak::forward_as_tuple(e, allow_tail), args))};    \
                                                                              \
				return lak::pair<lak::lisk::expression, size_t>(                      \
				  lak::move(exp), lak::tuple_size_v<arguments_t>);                    \
			}                                                                       \
	  })

		lak::lisk::string to_string(lak::lisk::function f);
		const lak::lisk::string &type_name(const lak::lisk::function &);

		/* --- callable --- */

		struct callable
		{
			using lambda_ptr = lak::shared_ptr<lak::lisk::lambda>;
			using value_type = lak::variant<lambda_ptr, lak::lisk::function>;
			value_type _value;

			// :TODO: add a way to track the number of arguments that a callable will
			// take, and if it's variadic then make it return how many it read.

			callable()                  = default;
			callable(const callable &c) = default;
			callable(callable &&c)      = default;

			callable &operator=(const callable &c) = default;
			callable &operator=(callable &&c)      = default;

			inline callable(const lak::lisk::lambda &l);
			inline callable(const lak::lisk::function &f);

			inline callable &operator=(const lak::lisk::lambda &l);
			inline callable &operator=(const lak::lisk::function &f);

			inline bool is_null() const;
			inline bool is_lambda() const;
			inline bool is_function() const;

			inline bool empty() const { return is_null(); }
			inline operator bool() const { return !is_null(); }

			inline lak::result<lak::lisk::lambda &> get_lambda() &;
			inline lak::result<const lak::lisk::lambda &> get_lambda() const &;
			inline lak::result<lak::lisk::lambda> get_lambda() &&;

			inline lak::result<const lak::lisk::function &> get_function() const;

			lak::pair<lak::lisk::expression, size_t> operator()(
			  lak::lisk::basic_shared_list<lak::lisk::expression> l,
			  lak::lisk::environment &e,
			  bool allow_tail_eval) const;
		};

		lak::lisk::string to_string(const lak::lisk::callable &c);
		const lak::lisk::string &type_name(const lak::lisk::callable &);

		/* --- expression --- */

		struct exception
		{
			lak::lisk::string message;
		};

		struct expression
		{
			struct null
			{
			};

			using value_type = lak::variant<null,
			                                lak::lisk::atom,
			                                lak::lisk::eval_shared_list,
			                                lak::lisk::shared_list,
			                                lak::lisk::callable,
			                                lak::lisk::exception>;

			value_type _value;

			expression()                       = default;
			expression(const expression &expr) = default;
			expression(expression &&expr)      = default;

			expression &operator=(const expression &expr) = default;
			expression &operator=(expression &&expr)      = default;

			inline expression(null);
			inline expression(const lak::lisk::atom &a);
			inline expression(lak::lisk::atom::nil);
			inline expression(const lak::lisk::shared_list &list);
			inline expression(const lak::lisk::eval_shared_list &list);
			inline expression(const lak::lisk::callable &c);
			inline expression(const lak::lisk::exception &exc);

			inline expression &operator=(null);
			inline expression &operator=(const lak::lisk::atom &a);
			inline expression &operator=(lak::lisk::atom::nil);
			inline expression &operator=(const lak::lisk::shared_list &list);
			inline expression &operator=(const lak::lisk::eval_shared_list &list);
			inline expression &operator=(const lak::lisk::callable &c);
			inline expression &operator=(const lak::lisk::exception &exc);

			inline bool is_null() const;
			inline bool is_atom() const;
			inline bool is_list() const;
			inline bool is_eval_list() const;
			inline bool is_callable() const;
			inline bool is_exception() const;

			inline bool empty() const { return is_null() || is_exception(); }
			inline operator bool() const { return !empty(); }

			inline lak::result<lak::lisk::atom &> get_atom() &;
			inline lak::result<const lak::lisk::atom &> get_atom() const &;
			inline lak::result<lak::lisk::atom> get_atom() &&;

			inline lak::result<lak::lisk::shared_list &> get_list() &;
			inline lak::result<const lak::lisk::shared_list &> get_list() const &;
			inline lak::result<lak::lisk::shared_list> get_list() &&;

			inline lak::result<lak::lisk::eval_shared_list &> get_eval_list() &;
			inline lak::result<const lak::lisk::eval_shared_list &> get_eval_list()
			  const &;
			inline lak::result<lak::lisk::eval_shared_list> get_eval_list() &&;

			inline lak::result<lak::lisk::callable &> get_callable() &;
			inline lak::result<const lak::lisk::callable &> get_callable() const &;
			inline lak::result<lak::lisk::callable> get_callable() &&;

			inline lak::result<lak::lisk::exception &> get_exception() &;
			inline lak::result<const lak::lisk::exception &> get_exception() const &;
			inline lak::result<lak::lisk::exception> get_exception() &&;

			template<typename LAMBDA>
			auto visit(LAMBDA &&lambda) const
			{
				return lak::visit(lambda, _value);
			}

			template<typename LAMBDA>
			auto visit(LAMBDA &&lambda)
			{
				return lak::visit(lambda, _value);
			}
		};

		struct eval_expr
		{
			lak::lisk::expression expr;
		};
		struct uneval_expr
		{
			lak::lisk::expression expr;
		};

		const lak::lisk::string &type_name(const lak::lisk::shared_list &);

		lak::lisk::string to_string(const lak::lisk::eval_shared_list &list);
		const lak::lisk::string &type_name(const lak::lisk::eval_shared_list &);

		lak::lisk::string to_string(const lak::lisk::uneval_shared_list &list);
		const lak::lisk::string &type_name(const lak::lisk::uneval_shared_list &);

		lak::lisk::string to_string(const lak::lisk::exception &exc);
		const lak::lisk::string &type_name(const lak::lisk::exception &);

		lak::lisk::string to_string(lak::lisk::expression::null);
		const lak::lisk::string &type_name(lak::lisk::expression::null);

		lak::lisk::string to_string(const lak::lisk::expression &expr);
		const lak::lisk::string &type_name(const lak::lisk::expression &);

		// lak::lisk::string to_string(const lak::lisk::eval_expr &expr);
		const lak::lisk::string &type_name(const lak::lisk::eval_expr &);

		// lak::lisk::string to_string(const lak::lisk::uneval_expr &expr);
		const lak::lisk::string &type_name(const lak::lisk::uneval_expr &);

		/* --- environment --- */

		struct environment
		{
			using value_type = lak::lisk::basic_shared_list<
			  std::unordered_map<lak::lisk::symbol, lak::lisk::expression>>;
			value_type _map = {};

			environment()                    = default;
			environment(const environment &) = default;
			environment(environment &&)      = default;

			environment &operator=(const environment &) = default;
			environment &operator=(environment &&)      = default;

			static environment extends(const environment &other);

			void define_expr(const lak::lisk::symbol &sym,
			                 const lak::lisk::expression &expr);
			void define_atom(const lak::lisk::symbol &sym, const lak::lisk::atom &a);
			void define_list(const lak::lisk::symbol &sym,
			                 const lak::lisk::shared_list &list);
			void define_callable(const lak::lisk::symbol &sym,
			                     const lak::lisk::callable &c);
			void define_function(const lak::lisk::symbol &sym,
			                     const lak::lisk::function &f);

			lak::lisk::expression operator[](const lak::lisk::symbol &sym) const;

			environment clone(size_t depth = 0) const;
			environment &squash(size_t depth);
		};

		lak::lisk::string to_string(const lak::lisk::environment &env);

		/* --- lambda --- */

		struct lambda
		{
			lak::lisk::shared_list params;
			lak::lisk::shared_list exp;
			lak::lisk::environment captured_env;

			lambda()                          = default;
			lambda(const lambda &)            = default;
			lambda &operator=(const lambda &) = default;

			lambda(lak::lisk::shared_list l,
			       lak::lisk::environment &e,
			       bool allow_tail_eval);

			lak::pair<lak::lisk::expression, size_t> operator()(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &e,
			  bool allow_tail_eval) const;
		};

		lak::lisk::string to_string(const lak::lisk::lambda &l);
		const lak::lisk::string &type_name(const lak::lisk::lambda &);

		/* --- eval --- */

		lak::pair<lak::lisk::shared_list, size_t> eval_all(
		  lak::lisk::shared_list l,
		  lak::lisk::environment &e,
		  bool allow_tail_eval);

		lak::lisk::expression eval(const lak::lisk::expression &exp,
		                           lak::lisk::environment &e,
		                           bool allow_tail_eval);

		namespace impl
		{
			template<typename... TYPES>
			bool get_or_eval_arg_as(lak::lisk::shared_list in_list,
			                        lak::lisk::environment &e,
			                        bool allow_tail,
			                        lak::lisk::exception &exc,
			                        lak::tuple<TYPES...> &out_arg);
		}

		template<>
		struct list_reader_traits<lak::lisk::shared_list>
		{
			static constexpr bool allow_get  = false;
			static constexpr bool allow_eval = true;
		};

		template<>
		struct list_reader_traits<lak::lisk::eval_shared_list>
		{
			static constexpr bool allow_get  = false;
			static constexpr bool allow_eval = true;
		};

		template<>
		struct list_reader_traits<lak::lisk::expression>
		{
			static constexpr bool allow_get  = false;
			static constexpr bool allow_eval = true;
		};

		template<>
		struct list_reader_traits<lak::lisk::eval_expr>
		{
			static constexpr bool allow_get  = false;
			static constexpr bool allow_eval = true;
		};

		template<>
		struct list_reader_traits<lak::lisk::uneval_shared_list>
		{
			static constexpr bool allow_get  = true;
			static constexpr bool allow_eval = false;
		};

		template<>
		struct list_reader_traits<lak::lisk::uneval_expr>
		{
			static constexpr bool allow_get  = true;
			static constexpr bool allow_eval = false;
		};

		struct list_reader
		{
			lak::lisk::shared_list list;
			lak::lisk::environment env;
			bool allow_tail_eval;

			list_reader(lak::lisk::shared_list l,
			            lak::lisk::environment e,
			            bool allow_tail)
			: list(l), env(e), allow_tail_eval(allow_tail)
			{
			}

			inline operator bool() const { return list; }

			template<typename T>
			requires lak::lisk::concepts::from_expr_convertible<T>
			lak::result<typename lak::lisk::from_expr_traits<T>::value_type> read();
		};

		/* --- --- */

		bool is_whitespace(const char8_t c);
		bool is_bracket(const char8_t c);
		bool is_numeric(const lak::lisk::string &token);

		bool is_nil(const lak::lisk::expression &expr);
		bool is_null(const lak::lisk::expression &expr);

		lak::vector<lak::lisk::string> tokenise(const lak::lisk::string &str,
		                                        size_t *chars_used = nullptr);
		lak::vector<lak::lisk::string> root_tokenise(const lak::lisk::string &str,
		                                             size_t *chars_used = nullptr);

		lak::lisk::number parse_number(const lak::lisk::string &token);
		lak::lisk::string parse_string(const lak::lisk::string &token);
		lak::lisk::expression parse(const lak::vector<lak::lisk::string> &tokens);

		// Top level eval function.
		lak::lisk::expression eval_string(const lak::lisk::string &str,
		                                  lak::lisk::environment &env);
		lak::lisk::expression root_eval_string(const lak::lisk::string &str,
		                                       lak::lisk::environment &env);

		// Delays evaluation of the expression.
		lak::lisk::expression tail_eval(lak::lisk::expression expr,
		                                lak::lisk::environment &env,
		                                bool allow_tail);

		template<typename T>
		lak::lisk::expression type_error(const lak::lisk::string &message,
		                                 const T &t,
		                                 const lak::lisk::string &expected)
		{
			return lak::lisk::exception{
			  lak::fmt<u8"{}: '{}' is '{}', expected '{}'">(
			    message, to_string(t), type_name(t), expected)};
		}

		struct reader
		{
			struct iterator
			{
				struct sentinel
				{
				};

				reader &ref;

				bool operator==(sentinel) const;

				bool operator!=(sentinel) const;

				lak::lisk::expression operator*();

				iterator &operator++();
			};

			reader() = default;
			reader(const lak::lisk::environment e, bool allow_tail = true);

			lak::lisk::environment env;
			bool allow_tail_eval;

			lak::lisk::string string_buffer;
			lak::vector<lak::lisk::string> token_buffer;
			lak::vector<lak::vector<lak::lisk::string>> tokens;

			void clear();

			operator bool() const;

			iterator begin();

			iterator::sentinel end() const;

			reader &operator+=(const lak::lisk::string &str);
		};

		namespace builtin
		{
			/* --- env --- */

			lak::lisk::expression list_env(lak::lisk::environment &env,
			                               bool allow_tail);

			lak::lisk::environment default_env();

			/* --- check --- */

			lak::lisk::expression null_check(lak::lisk::environment &env,
			                                 bool allow_tail,
			                                 lak::lisk::expression exp);

			lak::lisk::expression nil_check(lak::lisk::environment &env,
			                                bool allow_tail,
			                                lak::lisk::expression exp);

			lak::lisk::expression zero_check(lak::lisk::environment &env,
			                                 bool allow_tail,
			                                 lak::lisk::number num);

			// expr equal_check(lak::lisk::shared_list l, lak::lisk::environment
			// &env);

			/* --- --- */

			lak::lisk::expression conditional(lak::lisk::environment &env,
			                                  bool allow_tail,
			                                  bool b,
			                                  lak::lisk::uneval_expr cond,
			                                  lak::lisk::uneval_expr alt);

			/* --- --- */

			lak::lisk::expression define(lak::lisk::environment &env,
			                             bool allow_tail,
			                             lak::lisk::symbol sym,
			                             lak::lisk::expression exp);

			lak::lisk::expression evaluate(lak::lisk::environment &env,
			                               bool allow_tail,
			                               lak::lisk::expression exp);

			lak::pair<lak::lisk::expression, size_t> evaluate_stack(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &env,
			  bool allow_tail);

			/* --- --- */

			lak::pair<lak::lisk::expression, size_t> begin(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &env,
			  bool allow_tail);

			lak::lisk::expression repeat(lak::lisk::environment &env,
			                             bool allow_tail,
			                             lak::lisk::uint_t count,
			                             lak::lisk::uneval_expr exp);

			lak::lisk::expression repeat_while(lak::lisk::environment &env,
			                                   bool allow_tail,
			                                   lak::lisk::uneval_expr exp);

			lak::lisk::expression foreach (lak::lisk::environment &env,
			                               bool allow_tail,
			                               lak::lisk::symbol sym,
			                               lak::lisk::shared_list iterlist,
			                               lak::lisk::uneval_expr exp);

			lak::lisk::expression map(lak::lisk::environment &env,
			                          bool allow_tail,
			                          lak::lisk::shared_list iterlist,
			                          lak::lisk::uneval_expr exp);

			lak::pair<lak::lisk::expression, size_t> tail_call(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &env,
			  bool allow_tail);

			/* --- --- */

			lak::lisk::expression car(lak::lisk::environment &env,
			                          bool allow_tail,
			                          lak::lisk::shared_list l);

			lak::lisk::expression cdr(lak::lisk::environment &env,
			                          bool allow_tail,
			                          lak::lisk::shared_list l);

			lak::lisk::expression cons(lak::lisk::environment &env,
			                           bool allow_tail,
			                           lak::lisk::expression exp,
			                           lak::lisk::shared_list l);

			lak::pair<lak::lisk::expression, size_t> join(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &env,
			  bool allow_tail);

			/* ---  --- */

			lak::lisk::expression range_list(lak::lisk::environment &env,
			                                 bool allow_tail,
			                                 lak::lisk::number start,
			                                 lak::lisk::uint_t count,
			                                 lak::lisk::number step);

			/* --- constructor --- */

			lak::pair<lak::lisk::expression, size_t> make_list(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &env,
			  bool allow_tail);

			lak::pair<lak::lisk::expression, size_t> make_lambda(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &env,
			  bool allow_tail);

			lak::lisk::expression make_uint(lak::lisk::environment &env,
			                                bool allow_tail,
			                                lak::lisk::expression exp);

			lak::lisk::expression make_sint(lak::lisk::environment &env,
			                                bool allow_tail,
			                                lak::lisk::expression exp);

			lak::lisk::expression make_real(lak::lisk::environment &env,
			                                bool allow_tail,
			                                lak::lisk::expression exp);

			lak::lisk::expression make_string(lak::lisk::environment &env,
			                                  bool allow_tail,
			                                  lak::lisk::expression exp);

			/* --- string --- */

			lak::lisk::expression read_string(lak::lisk::environment &,
			                                  bool allow_tail);

			lak::lisk::expression parse_string(lak::lisk::environment &env,
			                                   bool allow_tail,
			                                   lak::lisk::string str);

			lak::pair<lak::lisk::expression, size_t> print_string(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &env,
			  bool allow_tail);

			lak::pair<lak::lisk::expression, size_t> print_line(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &env,
			  bool allow_tail);

			/* --- path --- */

			lak::lisk::expression read_file(lak::lisk::environment &env,
			                                bool allow_tail,
			                                lak::lisk::string file);

			lak::lisk::expression include_file(lak::lisk::environment &env,
			                                   bool allow_tail,
			                                   lak::lisk::string file);

			/* --- math --- */

			lak::lisk::expression add(lak::lisk::environment &env,
			                          bool allow_tail,
			                          lak::lisk::number a,
			                          lak::lisk::number b);

			lak::lisk::expression sub(lak::lisk::environment &env,
			                          bool allow_tail,
			                          lak::lisk::number a,
			                          lak::lisk::number b);

			lak::lisk::expression mul(lak::lisk::environment &env,
			                          bool allow_tail,
			                          lak::lisk::number a,
			                          lak::lisk::number b);

			lak::lisk::expression div(lak::lisk::environment &env,
			                          bool allow_tail,
			                          lak::lisk::number a,
			                          lak::lisk::number b);

			lak::pair<lak::lisk::expression, size_t> sum(lak::lisk::shared_list l,
			                                             lak::lisk::environment &env,
			                                             bool allow_tail);

			lak::pair<lak::lisk::expression, size_t> product(
			  lak::lisk::shared_list l,
			  lak::lisk::environment &env,
			  bool allow_tail);
		};
	}
}

inline lak::lisk::number operator+(lak::lisk::number A, lak::lisk::number B);
inline lak::lisk::number operator-(lak::lisk::number A, lak::lisk::number B);
inline lak::lisk::number operator*(lak::lisk::number A, lak::lisk::number B);
inline lak::lisk::number operator/(lak::lisk::number A, lak::lisk::number B);
inline lak::lisk::number &operator+=(lak::lisk::number &A,
                                     lak::lisk::number B);
inline lak::lisk::number &operator-=(lak::lisk::number &A,
                                     lak::lisk::number B);
inline lak::lisk::number &operator*=(lak::lisk::number &A,
                                     lak::lisk::number B);
inline lak::lisk::number &operator/=(lak::lisk::number &A,
                                     lak::lisk::number B);

#include "lisk.inl"

#endif
