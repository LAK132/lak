#include "lak/lisk/lisk.hpp"

#include "lak/array.hpp"
#include "lak/format.hpp"
#include "lak/numeric.hpp"
#include "lak/span_manip.hpp"
#include "lak/system/file.hpp"

#include "lisk.ebnf.hpp"

#include <iostream>

/* --- string --- */

lak::lisk::string lak::lisk::to_string(const lak::lisk::string &str)
{
	auto result  = str;
	auto replace = [](lak::lisk::string &str,
	                  const char8_t c,
	                  const lak::u8string &replacement)
	{
		for (auto i = str.find(c); i != lak::lisk::string::npos;
		     i      = str.find(c, i + replacement.size()))
      str.replace(i, 1, replacement.c_str());
	};
	replace(result, u8'\n', u8"\\n");
	replace(result, u8'\r', u8"\\r");
	replace(result, u8'\t', u8"\\t");
	replace(result, u8'\0', u8"\\0");
	replace(result, u8'\"', u8"\\\"");

	return u8"\"" + result + u8"\"";
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::string &)
{
	const static lak::lisk::string name = u8"string";
	return name;
}

/* --- symbol --- */

lak::lisk::string lak::lisk::to_string(const lak::lisk::symbol &sym)
{
	return sym;
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::symbol &)
{
	const static lak::lisk::string name = u8"symbol";
	return name;
}

/* --- number --- */

lak::lisk::string lak::lisk::to_string(const lak::lisk::number &num)
{
	return lak::visit([](auto &&v) { return to_string(v); }, num._value);
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::number &)
{
	const static lak::lisk::string name = "number";
	return name;
}

lak::lisk::string lak::lisk::to_string(lak::lisk::uint_t num)
{
	return lak::fmt<u8"{}">(num);
}

const lak::lisk::string &lak::lisk::type_name(lak::lisk::uint_t)
{
	const static lak::lisk::string name = "uint";
	return name;
}

lak::lisk::string lak::lisk::to_string(lak::lisk::sint_t num)
{
	return lak::fmt<u8"{:+}">(num);
}

const lak::lisk::string &lak::lisk::type_name(lak::lisk::sint_t)
{
	const static lak::lisk::string name = "sint";
	return name;
}

lak::lisk::string lak::lisk::to_string(lak::lisk::real_t num)
{
	return lak::fmt<u8"{:+}">(num);
}

const lak::lisk::string &lak::lisk::type_name(lak::lisk::real_t)
{
	const static lak::lisk::string name = "real";
	return name;
}

/* --- pointer --- */

lak::lisk::string lak::lisk::to_string(const lak::lisk::pointer &)
{
	// :TODO: actually return the value.
	return "<POINTER>";
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::pointer &)
{
	const static lak::lisk::string name = "pointer";
	return name;
}

/* --- atom --- */

lak::lisk::string lak::lisk::to_string(lak::lisk::atom::nil)
{
	return u8"nil";
}

const lak::lisk::string &lak::lisk::type_name(lak::lisk::atom::nil)
{
	const static lak::lisk::string name = u8"nil";
	return name;
}

lak::lisk::string lak::lisk::to_string(bool b)
{
	return b ? u8"true" : u8"false";
}

const lak::lisk::string &lak::lisk::type_name(bool)
{
	const static lak::lisk::string name = u8"bool";
	return name;
}

lak::lisk::string lak::lisk::to_string(const lak::lisk::atom &a)
{
	return a.visit([](auto &&a) { return to_string(a); });
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::atom &)
{
	const static lak::lisk::string name = u8"atom";
	return name;
}

/* --- shared_list --- */

/* --- function --- */

lak::lisk::string lak::lisk::to_string(lak::lisk::function f)
{
	return lak::fmt<u8"<builtin {}>">(reinterpret_cast<uintptr_t>(f));
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::function &)
{
	const static lak::lisk::string name = u8"function";
	return name;
}

/* --- callable --- */

lak::pair<lak::lisk::expression, size_t> lak::lisk::callable::operator()(
  lak::lisk::shared_list l,
  lak::lisk::environment &e,
  bool allow_tail_eval) const
{
	if (is_null()) return {lak::lisk::expression::null{}, 0};

	lak::pair<lak::lisk::expression, size_t> result = lak::visit(
	  [&](auto &&func) { return (*func)(l, e, allow_tail_eval); }, _value);

	if (allow_tail_eval && result.first.is_eval_list())
		result.first = eval(result.first, e, allow_tail_eval);

	return result;
}

lak::lisk::string lak::lisk::to_string(const lak::lisk::callable &c)
{
	return lak::visit(
	  [](auto &&func)
	  { return func ? to_string(*func) : to_string(expression::null{}); },
	  c._value);
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::callable &)
{
	const static lak::lisk::string name = u8"callable";
	return name;
}

/* --- expression --- */

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::shared_list &)
{
	const static lak::lisk::string name = u8"<EXPR LIST>";
	return name;
}

lak::lisk::string lak::lisk::to_string(const lak::lisk::eval_shared_list &list)
{
	return u8"<EVAL " + to_string((const lak::lisk::shared_list &)list) + u8">";
}

const lak::lisk::string &lak::lisk::type_name(
  const lak::lisk::eval_shared_list &)
{
	const static lak::lisk::string name = u8"<EVAL LIST>";
	return name;
}

lak::lisk::string lak::lisk::to_string(
  const lak::lisk::uneval_shared_list &list)
{
	return u8"<UNEVAL " + to_string((const lak::lisk::shared_list &)list) +
	       u8">";
}

const lak::lisk::string &lak::lisk::type_name(
  const lak::lisk::uneval_shared_list &)
{
	const static lak::lisk::string name = u8"<UNEVAL LIST>";
	return name;
}

lak::lisk::string lak::lisk::to_string(const lak::lisk::exception &exc)
{
	return u8"<EXCEPTION '" + exc.message + u8"'>";
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::exception &)
{
	const static lak::lisk::string name = u8"<EXCEPTION>";
	return name;
}

lak::lisk::string lak::lisk::to_string(const lak::lisk::expression &expr)
{
	return expr.visit([](auto &&a) { return to_string(a); });
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::expression &)
{
	const static lak::lisk::string name = u8"expression";
	return name;
}

lak::lisk::string lak::lisk::to_string(lak::lisk::expression::null)
{
	return u8"null";
}

const lak::lisk::string &lak::lisk::type_name(lak::lisk::expression::null)
{
	const static lak::lisk::string name = u8"null";
	return name;
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::eval_expr &)
{
	const static lak::lisk::string name = u8"<EVAL EXPRESSION>";
	return name;
}

const lak::lisk::string &lak::lisk::type_name(const lak::lisk::uneval_expr &)
{
	const static lak::lisk::string name = u8"<UNEVAL EXPRESSION>";
	return name;
}

/* --- environment --- */

lak::lisk::environment lak::lisk::environment::extends(
  const lak::lisk::environment &other)
{
	lak::lisk::environment result;
	result._map = value_type::extends(other._map);
	return result;
}

void lak::lisk::environment::define_expr(const lak::lisk::symbol &sym,
                                         const lak::lisk::expression &expr)
{
	_map.value()[sym] = expr;
}

void lak::lisk::environment::define_atom(const lak::lisk::symbol &sym,
                                         const lak::lisk::atom &a)
{
	_map.value()[sym] = a;
}

void lak::lisk::environment::define_list(const lak::lisk::symbol &sym,
                                         const lak::lisk::shared_list &list)
{
	_map.value()[sym] = list;
}

void lak::lisk::environment::define_callable(const lak::lisk::symbol &sym,
                                             const lak::lisk::callable &c)
{
	_map.value()[sym] = c;
}

void lak::lisk::environment::define_function(const lak::lisk::symbol &sym,
                                             const lak::lisk::function &f)
{
	define_callable(sym, f);
}

lak::lisk::expression lak::lisk::environment::operator[](
  const lak::lisk::symbol &sym) const
{
	for (const auto &node : _map)
		if (const auto it = node.value.find(sym); it != node.value.end())
			return it->second;

	return lak::lisk::exception{
	  lak::fmt<u8"Environment lookup failed, couldn't find '{}' in '{}'">(
	    sym, to_string(*this))};
}

lak::lisk::environment lak::lisk::environment::clone(size_t depth) const
{
	lak::lisk::environment result;
	result._map = _map.clone(depth);
	return result;
}

lak::lisk::environment &lak::lisk::environment::squash(size_t depth)
{
	if (depth == 0)
	{
		*this = clone(0);
		while (_map._node->next)
		{
			_map.next_value().merge(_map.value());
			_map++;
		}
	}
	else
	{
		*this = clone(depth + 1);
		while (depth-- > 0 && _map._node->next)
		{
			_map.next_value().merge(_map.value());
			_map++;
		}
	}
	return *this;
}

lak::lisk::string lak::lisk::to_string(const lak::lisk::environment &env)
{
	lak::lisk::string result;
	result += u8"(";
	for (const auto &node : env._map)
		for (const auto &[key, value] : node.value)
			result += u8"(" + to_string(key) + u8" " + to_string(value) + u8") ";
	if (result.back() == u8' ') result.pop_back();
	result += u8")";
	return result;
}

/* --- lambda --- */

lak::lisk::lambda::lambda(lak::lisk::shared_list l,
                          lak::lisk::environment &e,
                          bool allow_tail_eval)
: captured_env(lak::lisk::environment::extends(e))
{
	if_let_ok (auto arg1,
	           lak::lisk::from_expr<lak::lisk::shared_list>(l.value()))
		if_let_ok (auto arg2,
		           lak::lisk::from_expr<lak::lisk::shared_list>(l.next().value()))
		{
			params = arg1.clone();

			lak::lisk::list_reader reader(params, e, allow_tail_eval);

			size_t param_index = 0;
			for (auto &node : params)
			{
				static_assert(
				  lak::lisk::concepts::from_expr_convertible<lak::lisk::symbol>);
				if_let_ok (auto s, reader.template read<lak::lisk::symbol>())
				{
					node.value = lak::move(s);
					++param_index;
				}
				else
				{
					// :TODO: This really should throw an exception. Maybe this should
					// now evaluate to a lambda that returns an exception on call?
					params.clear();
					return;
				}
			}

			exp = arg2;
		}
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::lambda::operator()(
  lak::lisk::shared_list l,
  lak::lisk::environment &e,
  bool allow_tail_eval) const
{
	auto new_env = lak::lisk::environment::extends(captured_env);

	lak::lisk::list_reader reader(params, e, allow_tail_eval);

	size_t param_index = 0;
	for (const auto &node : l)
	{
		auto evaled = reader.list;
		if (!reader)
		{
			if (param_index == 0)
			{
				return {lak::lisk::exception{
				          u8"Too many arguments to call lambda, expected none"},
				        0};
			}
			else
			{
				return {
				  lak::lisk::exception{lak::fmt<
				    u8"Too many arguments to call lambda, expected params are '{}'">(
				    to_string(params))},
				  0};
			}
		}
		else if_let_ok (auto s, reader.template read<lak::lisk::symbol>())
		{
			new_env.define_expr(s, lak::lisk::eval(node.value, e, allow_tail_eval));
			++param_index;
		}
		else
		{
			return {lak::lisk::exception{
			          lak::fmt<u8"Failed to get symbol {} from '{}' for '{}'">(
			            param_index, to_string(node.value), to_string(evaled))},
			        0};
		}
	}

	if (reader)
	{
		return {lak::lisk::exception{
		          lak::fmt<u8"Too few parameters in '{}' to call lambda, "
		                   "expected parameters are '{}'">(to_string(l),
		                                                   to_string(params))},
		        0};
	}

	return {lak::lisk::eval(exp, new_env, allow_tail_eval), param_index};
}

lak::lisk::string lak::lisk::to_string(const lak::lisk::lambda &l)
{
	return u8"(lambda " + to_string(l.params) + u8" " + to_string(l.exp) + u8")";
}

const lak::lisk::string &type_name(const lak::lisk::lambda &)
{
	const static lak::lisk::string name = u8"lambda";
	return name;
}

/* --- eval --- */

lak::pair<lak::lisk::shared_list, size_t> lak::lisk::eval_all(
  lak::lisk::shared_list l, lak::lisk::environment &e, bool allow_tail_eval)
{
	auto result  = lak::lisk::shared_list::make();
	auto end     = result;
	size_t count = 0;
	for (const auto &node : l)
	{
		++count;
		end.set_next(lak::lisk::shared_list::make());
		++end;
		end.value() = lak::lisk::eval(node.value, e, allow_tail_eval);
	}
	return {++result, count};
}

lak::lisk::expression lak::lisk::eval(const lak::lisk::expression &exp,
                                      lak::lisk::environment &e,
                                      bool allow_tail_eval)
{
	if (exp.is_null())
	{
		// Expr was the empty list, which evaluates to nil.
		return lak::lisk::atom::nil{};
	}
	else if (exp.is_eval_list())
	{
		if (!allow_tail_eval) return exp;

		lak::lisk::expression result = exp;

		while_let_ok (const auto &c,
		              result.get_eval_list().and_then(
		                [](const lak::lisk::eval_shared_list &el)
		                { return el.list.value().get_callable(); }))
			result = lak::lisk::eval(c({}, e, false).first, e, false);

		return result;
	}
	else if_let_ok (lak::lisk::atom a, exp.get_atom())
	{
		if_let_ok (lak::lisk::symbol sym, a.get_symbol())
			return e[sym];
		else
			return a;
	}
	else if_let_ok (lak::lisk::shared_list l, exp.get_list())
	{
		// If we're about do do a function call, this should evalutate the symbol
		// to the relevant function pointer.
		auto subexp = lak::lisk::eval(l.value(), e, allow_tail_eval);

		// This is the empty list or nil atom.
		if (subexp.get_list().map_or([](auto &&l) { return l.value().is_null(); },
		                             false) ||
		    subexp.get_atom().map_or([](auto &&a) { return a.is_nil(); }, false))
		{
			return lak::lisk::atom::nil{};
		}
		else if_let_ok (lak::lisk::atom a, subexp.get_atom())
		{
			if_let_ok (lak::lisk::symbol sym, a.get_symbol())
				return e[sym];
			else
				return a;
		}
		else if_let_ok (lak::lisk::shared_list l2, subexp.get_list())
		{
			// :TODO: Comment on why we would ever end up here?
			return l2;
		}
		else if_let_ok (lak::lisk::callable c, subexp.get_callable())
		{
			return c(l.next(), e, allow_tail_eval).first;
		}
		else if_let_ok (lak::lisk::exception exc, subexp.get_exception())
		{
			return exc;
		}
		else
		{
			return lak::lisk::exception{
			  lak::fmt<u8"Failed to eval sub-expression '{}' of '{}', "
			           "got '{}', expected a symbol, atom or callable">(
			    to_string(l.value()), to_string(exp), to_string(subexp))};
		}
	}
	else if_let_ok (lak::lisk::exception exc, exp.get_exception())
	{
		return exc;
	}
	else
	{
		return lak::lisk::exception{
		  lak::fmt<u8"Failed to eval expression '{}' type '{}'">(
		    to_string(exp), exp.visit([](auto &&a) { return type_name(a); }))};
	}
}

/* --- --- */

bool lak::lisk::is_whitespace(const char8_t c)
{
	switch (c)
	{
		case u8' ':
		case u8'\n':
		case u8'\r':
		case u8'\t': return true;
		default:     return false;
	}
}

bool lak::lisk::is_bracket(const char8_t c)
{
	switch (c)
	{
		case u8'(':
		case u8')':
		case u8'[':
		case u8']':
		case u8'{':
		case u8'}': return true;
		default:    return false;
	}
}

bool lak::lisk::is_numeric(const lak::lisk::string &token)
{
	return lak::lisk_parse::number.parse(token).is_ok();
}

bool lak::lisk::is_nil(const lak::lisk::expression &expr)
{
	return expr.get_list().map_or([](auto &&l) { return l.value().is_null(); },
	                              false) ||
	       expr.get_atom().map_or([](auto &&a) { return a.is_nil(); }, false);
}

bool lak::lisk::is_null(const lak::lisk::expression &expr)
{
	return expr.is_null();
}

lak::lisk::string lak::lisk::next_token(lak::u8string_view str,
                                        lak::u8string_view *chars_used)
{
	size_t skipped  = 0U;
	bool in_comment = false;
	for (const auto &c : str)
	{
		if (lak::lisk::is_whitespace(c))
			;
		else if (in_comment)
		{
			if (c == u8'\n') in_comment = false;
		}
		else if (c == u8';')
		{
			in_comment = true;
		}
		else
			break;

		++skipped;
	}

	lak::lisk::string buffer;
	size_t chars_read = 0;

	auto done = [&]
	{
		if (chars_used) *chars_used = str.substr(skipped, chars_read);
	};

	auto begin_next = [&]() -> bool
	{
		if (buffer.empty()) return false;
		done();
		return true;
	};

	auto append = [&](char8_t c)
	{
		buffer += c;
		++chars_read;
	};

	bool in_string          = false;
	bool is_string_escaping = false;
	char string_char        = 0;
	for (const auto &c : str.substr(skipped))
	{
		if (in_string)
		{
			if (is_string_escaping)
			{
				if (c == u8'n')
					append(u8'\n');
				else if (c == u8'r')
					append(u8'\r');
				else if (c == u8't')
					append(u8'\t');
				else if (c == u8'0')
					append(u8'\0');
				else
					append(c);
				is_string_escaping = false;
			}
			else if (c == u8'\\')
			{
				is_string_escaping = true;
				++chars_read;
			}
			else
			{
				append(c);
				if (c == string_char)
				{
					done();
					return buffer;
				}
			}
		}
		else if (c == u8';')
		{
			done();
			return buffer;
		}
		else if (c == u8'"' || c == u8'\'')
		{
			append(c);
			in_string          = true;
			is_string_escaping = false;
			string_char        = c;
		}
		else if (lak::lisk::is_whitespace(c))
		{
			done();
			return buffer;
		}
		else if (lak::lisk::is_bracket(c))
		{
			if (begin_next())
				return buffer;
			else
			{
				append(c);
				done();
				return buffer;
			}
		}
		else
			append(c);
	}

	done();
	return buffer;
}

lak::vector<lak::lisk::string> lak::lisk::tokenise(
  const lak::lisk::string &str, size_t *chars_used)
{
	lak::vector<lak::lisk::string> result;

	size_t chars_read = 0U;
	for (lak::u8string_view _chars_read;; _chars_read = {})
	{
		auto token = lak::lisk::next_token(
		  lak::u8string_view(str).substr(chars_read), &_chars_read);
		auto read =
		  lak::u8string_view(str.c_str() + chars_read, _chars_read.end());
		if (!token.empty())
			result.emplace_back(lak::move(token));
		else if (read.empty())
			break;
		chars_read += read.size();
	}

	if (chars_used) *chars_used = chars_read;

	return result;
}

lak::vector<lak::lisk::string> lak::lisk::root_tokenise(
  const lak::lisk::string &str, size_t *chars_used)
{
	auto result = lak::lisk::tokenise(str, chars_used);
	if (result.empty()) return {};

	result.reserve(result.size() + 3);
	result.insert(result.begin(), {"(", "begin"});
	result.push_back(")");
	return result;
}

lak::lisk::number lak::lisk::parse_number(const lak::lisk::string &token)
{
	if_let_ok (auto m, lak::lisk_parse::number.parse(token))
		return m.value;
	else
		return std::numeric_limits<real_t>::signaling_NaN();
}

lak::lisk::string lak::lisk::parse_string(const lak::lisk::string &token)
{
	if (token.size() == 2) return lak::lisk::string{};
	return lak::lisk::string{token.substr(1, token.size() - 2)};
}

lak::lisk::expression lak::lisk::parse(
  const lak::vector<lak::lisk::string> &tokens)
{
	lak::lisk::shared_list root;
	lak::vector<lak::vector<lak::lisk::shared_list>> stack;

	auto push_element = [&]() -> lak::lisk::shared_list
	{
		if (stack.empty()) return {};
		// Get the previous nill element.
		lak::lisk::shared_list old_element = stack.back().back();
		// If this element holds null_t then no values have been added to this
		// scope yet, so return immediately.
		if (!old_element) return old_element;
		// Else if the last element already has a value, add a new element after
		// it
		auto new_element = lak::lisk::shared_list::make();
		// Set the new nill element as the next element from the previous nill
		// element.
		old_element.set_next(new_element);
		// Push the new nill element into the stack scope.
		stack.back().emplace_back(new_element);
		// Return the old nill element so a value can be added to it.
		return new_element;
	};

	auto push_scope = [&]()
	{
		// Create the root element for the new stack.
		auto scope    = lak::lisk::shared_list::make();
		scope.value() = lak::lisk::expression::null{};

		// If this is the first scope, make sure to mark it as the root.
		// Else this is a nested scope, push it as a value onto the parent scope.
		if (stack.empty())
			root = scope;
		else
			push_element().value() = scope;

		// Push the new scope onto the stack.
		stack.emplace_back();
		stack.back().emplace_back(scope);
	};

	auto pop_scope = [&]() -> bool
	{
		// Pop the scope off the stack.
		if (stack.empty()) return false;
		stack.pop_back();
		return true;
	};

	for (const auto &token : tokens)
	{
		const char8_t c = token.front();
		if (c == u8'(')
		{
			push_scope();
		}
		else if (c == u8'[')
		{
			push_scope();
			push_element().value() = lak::lisk::atom{lak::lisk::symbol("list")};
		}
		else if (c == u8'{')
		{
			push_scope();
			push_element().value() =
			  lak::lisk::atom{lak::lisk::symbol("eval-stack")};
		}
		else if (c == u8')' || c == u8']' || c == u8'}')
		{
			if (!pop_scope())
				return lak::lisk::exception{.message = u8"unexpected end of scope"};
		}
		else
		{
			expression value;

			if (c == u8'"')
				value = lak::lisk::atom{lak::lisk::parse_string(token)};
			else if (token == u8"nil")
				value = lak::lisk::atom{lak::lisk::atom::nil{}};
			else if (token == u8"true")
				value = lak::lisk::atom{true};
			else if (token == u8"false")
				value = lak::lisk::atom{false};
			else if (is_numeric(token))
				value = lak::lisk::atom{lak::lisk::parse_number(token)};
			else
				value = lak::lisk::atom{lak::lisk::symbol(token)};

			if (stack.empty())
				return value;
			else
				push_element().value() = value;
		}
	}

	return lak::lisk::expression{root};
}

lak::lisk::expression lak::lisk::eval_string(const lak::lisk::string &str,
                                             lak::lisk::environment &env)
{
	return lak::lisk::eval(
	  lak::lisk::parse(lak::lisk::tokenise(str)), env, true);
}

lak::lisk::expression lak::lisk::root_eval_string(const lak::lisk::string &str,
                                                  lak::lisk::environment &env)
{
	return lak::lisk::eval(
	  lak::lisk::parse(lak::lisk::root_tokenise(str)), env, true);
}

lak::lisk::expression lak::lisk::tail_eval(lak::lisk::expression expr,
                                           lak::lisk::environment &env,
                                           bool allow_tail)
{
	// grandp   parent   env
	// * ------ * ------ *
	//  `.
	//    `.
	//      `* ------ * tail_env (env clone)
	//    parent (clone)
	lak::lisk::environment tail_env = env.clone(2);
	// grandp   parent   env
	// * ------ * ------ *
	//  `.
	//    `.
	//      `* tail_env (squashed env->parent clone)
	tail_env.squash(2);

	auto result = lak::lisk::shared_list::make();

	// (lambda () (expr))
	lak::lisk::shared_list lambda_list;
	lambda_list.value()      = lak::lisk::shared_list::make(); // no arguments
	lambda_list.next_value() = expr;                           // lambda body
	result.value() =
	  lak::lisk::callable(lak::lisk::lambda(lambda_list, tail_env, allow_tail));

	// Return an eval_shared_list containing a lambda that immediately
	// evaluates our expr.
	return lak::lisk::eval_shared_list{result};
}

bool lak::lisk::reader::iterator::operator==(sentinel) const { return !ref; }

bool lak::lisk::reader::iterator::operator!=(sentinel) const { return ref; }

lak::lisk::expression lak::lisk::reader::iterator::operator*()
{
	return lak::lisk::eval(
	  lak::lisk::parse(ref.tokens.front()), ref.env, ref.allow_tail_eval);
}

lak::lisk::reader::iterator &lak::lisk::reader::iterator::operator++()
{
	ref.tokens.pop_front();
	return *this;
}

lak::lisk::reader::reader(const lak::lisk::environment e, bool allow_tail)
: env(e), allow_tail_eval(allow_tail)
{
}

void lak::lisk::reader::clear()
{
	string_buffer.clear();
	token_buffer.clear();
	tokens.clear();
}

lak::lisk::reader::operator bool() const { return tokens.size() > 0; }

lak::lisk::reader::iterator lak::lisk::reader::begin() { return {*this}; }

lak::lisk::reader::iterator::sentinel lak::lisk::reader::end() const
{
	return {};
}

lak::lisk::reader &lak::lisk::reader::operator+=(const lak::lisk::string &str)
{
	string_buffer += str;

	// Tokenise as much of the buffer as we can.
	size_t chars_used;
	auto new_tokens = lak::lisk::tokenise(string_buffer, &chars_used);
	string_buffer.erase(string_buffer.begin(),
	                    string_buffer.begin() + chars_used);

	// Push the new tokens into the reader's token buffer.
	token_buffer.reserve(token_buffer.size() + new_tokens.size());
	for (auto &token : new_tokens) token_buffer.emplace_back(lak::move(token));

	// Push the groups of tokens into the reader. These should either be
	// individual atoms or complete lists.
	size_t scope_count = 0;
	for (auto it = token_buffer.begin(); it != token_buffer.end(); ++it)
	{
		if (*it == u8"(")
		{
			++scope_count;
		}
		else if (*it == u8")")
		{
			--scope_count;
		}
		if (scope_count == 0)
		{
			auto begin = token_buffer.begin();
			auto end   = it + 1;

			if (tokens.empty() || !tokens.back().empty()) tokens.emplace_back();
			tokens.back().reserve(end - begin);

			for (auto it2 = begin; it2 != end; ++it2)
				tokens.back().emplace_back(lak::move(*it2));

			token_buffer.erase(begin, end);

			it = token_buffer.begin();
			if (it == token_buffer.end()) break;
		}
	}

	return *this;
}

lak::lisk::expression lak::lisk::builtin::list_env(lak::lisk::environment &env,
                                                   bool)
{
	auto root = lak::lisk::shared_list::make();

	lak::lisk::shared_list previous = root;
	lak::lisk::shared_list l        = root;
	for (const auto &node : env._map)
	{
		for (const auto &[key, value] : node.value)
		{
			auto entry         = lak::lisk::shared_list::make();
			entry.value()      = lak::lisk::atom{key};
			entry.next_value() = value;

			l.value() = entry;
			l.set_next(lak::lisk::shared_list::make());

			previous = l++;
		}
	}
	previous.clear_next();

	return root;
}

lak::lisk::expression lak::lisk::builtin::null_check(lak::lisk::environment &,
                                                     bool,
                                                     lak::lisk::expression exp)
{
	return lak::lisk::atom{lak::lisk::is_null(exp)};
}

lak::lisk::expression lak::lisk::builtin::nil_check(lak::lisk::environment &,
                                                    bool,
                                                    lak::lisk::expression exp)
{
	return lak::lisk::atom{lak::lisk::is_nil(exp)};
}

lak::lisk::expression lak::lisk::builtin::zero_check(lak::lisk::environment &,
                                                     bool,
                                                     lak::lisk::number num)
{
	return lak::lisk::atom{num.visit([](auto &&n) -> bool { return n == 0; })};
}

// lak::lisk::expression equal_check(lak::lisk::shared_list l,
// lak::lisk::environment &)
// {
//   if (lak::lisk::eval(l.value(), e) == lak::lisk::eval(l.next().value(), e))
//     return
//     lak::lisk::expression{lak::lisk::atom{lak::lisk::number{lak::lisk::uint_t{1U}}}};
//   else
//     return lak::lisk::atom::nil{};
// }

lak::lisk::expression lak::lisk::builtin::conditional(
  lak::lisk::environment &env,
  bool allow_tail,
  bool b,
  lak::lisk::uneval_expr cond,
  lak::lisk::uneval_expr alt)
{
	return b ? lak::lisk::eval(cond.expr, env, allow_tail)
	         : lak::lisk::eval(alt.expr, env, allow_tail);
}

lak::lisk::expression lak::lisk::builtin::define(lak::lisk::environment &env,
                                                 bool,
                                                 lak::lisk::symbol sym,
                                                 lak::lisk::expression exp)
{
	env.define_expr(sym, exp);
	return lak::lisk::atom::nil{};
}

lak::lisk::expression lak::lisk::builtin::evaluate(lak::lisk::environment &env,
                                                   bool allow_tail,
                                                   lak::lisk::expression exp)
{
	return lak::lisk::eval(exp, env, allow_tail);
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::evaluate_stack(
  lak::lisk::shared_list l, lak::lisk::environment &env, bool allow_tail)
{
	lak::lisk::shared_list head;
	size_t input_length = 0;
	for (; l; ++l, ++input_length)
	{
		auto expr = lak::lisk::eval(l.value(), env, allow_tail);
		if_let_ok (auto c, lak::lisk::from_expr<lak::lisk::callable>(expr))
		{
			auto [result, pop_count] = c(head, env, allow_tail);
			for (size_t i = 0; i < pop_count; ++i) ++head;
			if (head) head = lak::lisk::shared_list::extends(head);
			head.value() = lak::move(result);
		}
		else
		{
			if (head) head = lak::lisk::shared_list::extends(head);
			head.value() = expr;
		}
	}

	return {lak::lisk::expression{head}, input_length};
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::begin(
  lak::lisk::shared_list l, lak::lisk::environment &env, bool allow_tail)
{
	// Evaluate every element in the list and return the result of the last.
	lak::pair<lak::lisk::expression, size_t> result;
	result.second = 0;
	for (const auto &node : l)
	{
		result.first = lak::lisk::eval(node.value, env, allow_tail);
		++result.second;
	}
	return result;
}

lak::lisk::expression lak::lisk::builtin::repeat(lak::lisk::environment &env,
                                                 bool allow_tail,
                                                 lak::lisk::uint_t count,
                                                 lak::lisk::uneval_expr exp)
{
	while (count-- > 0) lak::lisk::eval(exp.expr, env, allow_tail);
	return lak::lisk::atom::nil{};
}

lak::lisk::expression lak::lisk::builtin::repeat_while(
  lak::lisk::environment &env, bool allow_tail, lak::lisk::uneval_expr exp)
{
	while (!lak::lisk::is_nil(lak::lisk::eval(exp.expr, env, allow_tail)));
	return lak::lisk::atom::nil{};
}

lak::lisk::expression lak::lisk::builtin::foreach (
  lak::lisk::environment &env,
  bool allow_tail,
  lak::lisk::symbol sym,
  lak::lisk::shared_list iterlist,
  lak::lisk::uneval_expr exp)
{
	for (const auto &node : iterlist)
	{
		lak::lisk::environment new_env = env;
		new_env.define_expr(sym, node.value);
		lak::lisk::eval(exp.expr, new_env, allow_tail);
	}
	return lak::lisk::atom::nil{};
}

lak::lisk::expression lak::lisk::builtin::map(lak::lisk::environment &env,
                                              bool allow_tail,
                                              lak::lisk::shared_list iterlist,
                                              lak::lisk::uneval_expr exp)
{
	auto subexp = lak::lisk::eval(exp.expr, env, allow_tail);
	if_let_ok (auto f, lak::lisk::from_expr<lak::lisk::function>(subexp))
	{
		auto arg    = lak::lisk::shared_list::make();
		auto result = lak::lisk::shared_list::make();
		auto end    = result;
		for (const auto &node : iterlist)
		{
			arg.value()      = node.value;
			end.next_value() = f(arg, env, allow_tail).first;
			++end;
		}
		return ++result;
	}
	else if_let_ok (auto lf, lak::lisk::from_expr<lak::lisk::lambda>(subexp))
	{
		auto arg    = lak::lisk::shared_list::make();
		auto result = lak::lisk::shared_list::make();
		auto end    = result;
		for (const auto &node : iterlist)
		{
			arg.value()      = node.value;
			end.next_value() = lf(arg, env, allow_tail).first;
			++end;
		}
		return ++result;
	}
	else
		return subexp.visit(
		  [](auto &&a)
		  {
			  return lak::lisk::type_error("Map error", a, "a function or lambda");
		  });
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::tail_call(
  lak::lisk::shared_list list, lak::lisk::environment &env, bool allow_tail)
{
	// (tail (func args...))
	return {lak::lisk::tail_eval(list.value(), env, allow_tail), 1};
}

lak::lisk::expression lak::lisk::builtin::car(lak::lisk::environment &,
                                              bool,
                                              lak::lisk::shared_list l)
{
	return l.value();
}

lak::lisk::expression lak::lisk::builtin::cdr(lak::lisk::environment &,
                                              bool,
                                              lak::lisk::shared_list l)
{
	return l.next();
}

lak::lisk::expression lak::lisk::builtin::cons(lak::lisk::environment &,
                                               bool,
                                               lak::lisk::expression exp,
                                               lak::lisk::shared_list l)
{
	lak::lisk::shared_list result;
	result.value() = exp;
	result.set_next(l);
	return result;
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::join(
  lak::lisk::shared_list l, lak::lisk::environment &env, bool allow_tail)
{
	lak::lisk::shared_list first;
	lak::lisk::shared_list end;
	size_t count = 0;

	if_let_ok (auto f,
	           lak::lisk::from_expr<lak::lisk::shared_list>(
	             lak::lisk::eval(l.value(), env, allow_tail)))
	{
		first = f;
		++count;
		end = first.last();
	}
	else
	{
		return {lak::lisk::type_error("Join error", l.value(), "a list"), 0};
	}

	for (const auto &node : l.next())
	{
		++count;
		if_let_ok (auto next,
		           lak::lisk::from_expr<lak::lisk::shared_list>(
		             lak::lisk::eval(node.value, env, allow_tail)))
		{
			end.set_next(next);
			end = next.last();
		}
		else
		{
			return {lak::lisk::type_error("Join error", node.value, "a list"), 0};
		}
	}

	return {first, count};
}

lak::lisk::expression lak::lisk::builtin::range_list(lak::lisk::environment &,
                                                     bool,
                                                     lak::lisk::number start,
                                                     lak::lisk::uint_t count,
                                                     lak::lisk::number step)
{
	auto result = lak::lisk::shared_list::make();
	auto end    = result;
	for (lak::lisk::uint_t i = 0; i < count; ++i)
	{
		end.next_value() = lak::lisk::atom{start + (step * lak::lisk::number{i})};
		++end;
	}
	return ++result;
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::make_list(
  lak::lisk::shared_list l, lak::lisk::environment &env, bool allow_tail)
{
	auto result{lak::lisk::eval_all(l, env, allow_tail)};
	return {lak::lisk::expression(result.first), result.second};
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::make_lambda(
  lak::lisk::shared_list l, lak::lisk::environment &env, bool allow_tail)
{
	// :TODO: check that there's exactly 2 arguments
	return {lak::lisk::callable(lak::lisk::lambda(l, env, allow_tail)), 2};
}

lak::lisk::expression lak::lisk::builtin::make_uint(lak::lisk::environment &,
                                                    bool,
                                                    lak::lisk::expression exp)
{
	static_assert(lak::lisk::concepts::from_expr_convertible<lak::lisk::string>);
	auto to_uint = [](auto &&n) -> lak::lisk::number
	{ return lak::lisk::uint_t(n); };
	if_let_ok (auto n, lak::lisk::from_expr<lak::lisk::number>(exp))
		return lak::lisk::atom{n.visit(to_uint)};
	else if_let_ok (auto s, lak::lisk::from_expr<lak::lisk::string>(exp))
		return lak::lisk::atom{lak::lisk::parse_number(s).visit(to_uint)};
	else
		return lak::lisk::atom{
		  lak::lisk::parse_number(to_string(exp)).visit(to_uint)};
}

lak::lisk::expression lak::lisk::builtin::make_sint(lak::lisk::environment &,
                                                    bool,
                                                    lak::lisk::expression exp)
{
	auto to_sint = [](auto &&n) -> lak::lisk::number
	{ return lak::lisk::sint_t(n); };
	if_let_ok (auto n, lak::lisk::from_expr<lak::lisk::number>(exp))
		return lak::lisk::atom{n.visit(to_sint)};
	else if_let_ok (auto s, lak::lisk::from_expr<lak::lisk::string>(exp))
		return lak::lisk::atom{lak::lisk::parse_number(s).visit(to_sint)};
	else
		return lak::lisk::atom{
		  lak::lisk::parse_number(to_string(exp)).visit(to_sint)};
}

lak::lisk::expression lak::lisk::builtin::make_real(lak::lisk::environment &,
                                                    bool,
                                                    lak::lisk::expression exp)
{
	auto to_real = [](auto &&n) -> lak::lisk::number
	{ return (lak::lisk::real_t)(n); };
	if_let_ok (auto n, lak::lisk::from_expr<lak::lisk::number>(exp))
		return lak::lisk::atom{n.visit(to_real)};
	else if_let_ok (auto s, lak::lisk::from_expr<lak::lisk::string>(exp))
		return lak::lisk::atom{lak::lisk::parse_number(s).visit(to_real)};
	else
		return lak::lisk::atom{
		  lak::lisk::parse_number(to_string(exp)).visit(to_real)};
}

lak::lisk::expression lak::lisk::builtin::make_string(
  lak::lisk::environment &, bool, lak::lisk::expression exp)
{
	if_let_ok (auto s, lak::lisk::from_expr<lak::lisk::string>(exp))
		return lak::lisk::expression(s);
	else
		return lak::lisk::expression(lak::lisk::string{to_string(exp)});
}

lak::lisk::expression lak::lisk::builtin::read_string(lak::lisk::environment &,
                                                      bool)
{
	lak::astring str;
	std::getline(std::cin, str);
	if (std::cin.good())
		return lak::lisk::expression{
		  lak::lisk::atom{lak::lisk::string(lak::to_u8string(str))}};
	return lak::lisk::atom::nil{};
}

lak::lisk::expression lak::lisk::builtin::parse_string(
  lak::lisk::environment &, bool, lak::lisk::string str)
{
	return lak::lisk::parse(lak::lisk::tokenise(str));
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::print_string(
  lak::lisk::shared_list l, lak::lisk::environment &env, bool allow_tail)
{
	lak::lisk::expression result = lak::lisk::eval(l.value(), env, allow_tail);
	// If the list evaluates to a pure string, then print it verbatim.
	// Otherwise to_string the result.
	if_let_ok (auto str, lak::lisk::from_expr<lak::lisk::string>(result))
		std::cout << str;
	else
		std::cout << to_string(result);
	return {lak::lisk::atom::nil{}, 1};
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::print_line(
  lak::lisk::shared_list l, lak::lisk::environment &env, bool allow_tail)
{
	// No arguments, just print a newline.
	if (lak::lisk::is_nil(l)) std::cout << "\n";

	lak::lisk::expression result = lak::lisk::eval(l.value(), env, allow_tail);
	// If the list evaluates to a pure string, then print it verbatim.
	// Otherwise to_string the result.
	if_let_ok (auto str, lak::lisk::from_expr<lak::lisk::string>(result))
		std::cout << str << "\n";
	else
		std::cout << to_string(result) << "\n";
	return {lak::lisk::atom::nil{}, 1};
}

static lak::error_code_result<lak::pair<lak::lisk::string, lak::fs::path>>
_read_file(lak::lisk::environment &env, lak::lisk::string file)
{
	std::filesystem::path fp{static_cast<const lak::u8string &>(file)};
	if (fp.is_relative())
	{
		if_let_ok (auto cwda, env[lak::lisk::symbol("cwd")].get_atom())
			if_let_ok (auto cwd, cwda.get_string())
				fp =
				  std::filesystem::path{static_cast<const lak::u8string &>(cwd)} / fp;
	}
	return lak::read_file_str<char8_t>(fp).map(
	  [&](auto &&str) -> lak::pair<lak::lisk::string, lak::fs::path>
	  { return {lak::lisk::string(lak::move(str)), lak::move(fp)}; });
}

lak::lisk::expression lak::lisk::builtin::read_file(
  lak::lisk::environment &env, bool, lak::lisk::string file)
{
	match_result(_read_file(env, lak::move(file)))
	{
		match_let_ok(auto ok, {
			return lak::lisk::atom{lak::lisk::string(lak::move(ok.first))};
		});
		match_let_err(auto err, {
			return lak::lisk::exception{.message = lak::fmt<u8"{}">(err)};
		});
	}
}

lak::lisk::expression lak::lisk::builtin::include_file(
  lak::lisk::environment &env, bool, lak::lisk::string file)
{
	match_result(_read_file(env, lak::move(file)))
	{
		match_let_ok(auto ok, {
			auto e = lak::lisk::environment::extends(env);
			e.define_atom("cwd",
			              lak::lisk::atom(lak::lisk::string(
			                ok.second.parent_path().generic_u8string())));
			return lak::lisk::root_eval_string(lak::move(ok.first), e);
		});
		match_let_err(auto err, {
			return lak::lisk::exception{.message = lak::fmt<u8"{}">(err)};
		});
	}
}

lak::lisk::expression lak::lisk::builtin::add(lak::lisk::environment &,
                                              bool,
                                              lak::lisk::number a,
                                              lak::lisk::number b)
{
	return lak::lisk::atom{a + b};
}

lak::lisk::expression lak::lisk::builtin::sub(lak::lisk::environment &,
                                              bool,
                                              lak::lisk::number a,
                                              lak::lisk::number b)
{
	return lak::lisk::atom{a - b};
}

lak::lisk::expression lak::lisk::builtin::mul(lak::lisk::environment &,
                                              bool,
                                              lak::lisk::number a,
                                              lak::lisk::number b)
{
	return lak::lisk::atom{a * b};
}

lak::lisk::expression lak::lisk::builtin::div(lak::lisk::environment &,
                                              bool,
                                              lak::lisk::number a,
                                              lak::lisk::number b)
{
	return lak::lisk::atom{a / b};
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::sum(
  lak::lisk::shared_list l, lak::lisk::environment &env, bool allow_tail)
{
	if (!l) return {lak::lisk::atom::nil{}, 0};

	lak::lisk::number result;
	size_t count = 0;

	if_let_ok (auto n,
	           lak::lisk::from_expr<lak::lisk::number>(
	             lak::lisk::eval(l.value(), env, allow_tail)))
	{
		++count;
		result = n;
	}
	else
	{
		return {lak::lisk::type_error("Add error", l.value(), "a number"), 0};
	}

	for (const auto &it : l.next())
	{
		++count;
		if_let_ok (auto n,
		           lak::lisk::from_expr<lak::lisk::number>(
		             lak::lisk::eval(it.value, env, allow_tail)))
		{
			result += n;
		}
		else
		{
			return {lak::lisk::type_error("Add error", l.value(), "a number"), 0};
		}
	}

	return {lak::lisk::expression{lak::lisk::atom{result}}, count};
}

lak::pair<lak::lisk::expression, size_t> lak::lisk::builtin::product(
  lak::lisk::shared_list l, lak::lisk::environment &env, bool allow_tail)
{
	if (!l) return {lak::lisk::atom::nil{}, 0};

	lak::lisk::number result;
	size_t count = 0;

	if_let_ok (auto n,
	           lak::lisk::from_expr<lak::lisk::number>(
	             lak::lisk::eval(l.value(), env, allow_tail)))
	{
		++count;
		result = n;
	}
	else
	{
		return {lak::lisk::type_error("Mul error", l.value(), "a number"), 0};
	}

	for (const auto &it : l.next())
	{
		++count;
		if_let_ok (auto n,
		           lak::lisk::from_expr<lak::lisk::number>(
		             lak::lisk::eval(it.value, env, allow_tail)))
		{
			result *= n;
		}
		else
		{
			return {lak::lisk::type_error("Mul error", l.value(), "a number"), 0};
		}
	}

	return {lak::lisk::expression{lak::lisk::atom{result}}, count};
}

lak::lisk::environment lak::lisk::builtin::default_env()
{
	lak::lisk::environment e;

	e.define_atom("cwd",
	              lak::lisk::atom(lak::lisk::string(
	                std::filesystem::current_path().generic_u8string())));

	e.define_atom("pi", lak::lisk::atom(lak::lisk::number(3.14159L)));

	e.define_function("env", LISK_FUNCTION_WRAPPER(list_env));
	e.define_function("null?", LISK_FUNCTION_WRAPPER(null_check));
	e.define_function("nil?", LISK_FUNCTION_WRAPPER(nil_check));
	e.define_function("zero?", LISK_FUNCTION_WRAPPER(zero_check));
	// e.define_function("eq?", LISK_FUNCTION_WRAPPER(equal_check));
	e.define_function("if", LISK_FUNCTION_WRAPPER(conditional));
	e.define_function("define", LISK_FUNCTION_WRAPPER(define));
	e.define_function("eval", LISK_FUNCTION_WRAPPER(evaluate));
	e.define_function("eval-stack", evaluate_stack);
	e.define_function("begin", begin);
	e.define_function("repeat", LISK_FUNCTION_WRAPPER(repeat));
	e.define_function("while", LISK_FUNCTION_WRAPPER(repeat_while));
	e.define_function("foreach", LISK_FUNCTION_WRAPPER(foreach));
	e.define_function("map", LISK_FUNCTION_WRAPPER(map));
	e.define_function("tail", tail_call);

	e.define_function("car", LISK_FUNCTION_WRAPPER(car));
	e.define_function("cdr", LISK_FUNCTION_WRAPPER(cdr));
	e.define_function("cons", LISK_FUNCTION_WRAPPER(cons));
	e.define_function("join", join);

	e.define_function("range", LISK_FUNCTION_WRAPPER(range_list));
	e.define_function("list", make_list);
	e.define_function("lambda", make_lambda);
	e.define_function("uint", LISK_FUNCTION_WRAPPER(make_uint));
	e.define_function("sint", LISK_FUNCTION_WRAPPER(make_sint));
	e.define_function("real", LISK_FUNCTION_WRAPPER(make_real));
	e.define_function("string", LISK_FUNCTION_WRAPPER(make_string));

	e.define_function("read", LISK_FUNCTION_WRAPPER(read_string));
	e.define_function("parse", LISK_FUNCTION_WRAPPER(parse_string));
	e.define_function("print", print_string);
	e.define_function("println", print_line);

	e.define_function("read-file", LISK_FUNCTION_WRAPPER(read_file));
	e.define_function("include", LISK_FUNCTION_WRAPPER(include_file));

	e.define_function("+", LISK_FUNCTION_WRAPPER(add));
	e.define_function("-", LISK_FUNCTION_WRAPPER(sub));
	e.define_function("*", LISK_FUNCTION_WRAPPER(mul));
	e.define_function("/", LISK_FUNCTION_WRAPPER(div));

	e.define_function("sum", sum);
	e.define_function("product", product);

	return e;
}
