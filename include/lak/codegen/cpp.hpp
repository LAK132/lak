#ifndef LAK_CODEGEN_CPP_HPP
#define LAK_CODEGEN_CPP_HPP

#include "lak/array.hpp"
#include "lak/char.hpp"
#include "lak/optional.hpp"
#include "lak/string.hpp"
#include "lak/string_literals/view.hpp"
#include "lak/variant.hpp"

#include "lak/span_manip.hpp"

namespace lak
{
	namespace codegen
	{
		template<typename CHAR = char8_t>
		struct cpp_writer
		{
			lak::string<CHAR> newline = lak::strconv<CHAR>(u8"\n"_view);
			lak::string<CHAR> indent  = lak::strconv<CHAR>(u8"\t"_view);

			struct block_scope
			{
			};

			struct namespace_scope
			{
				lak::string<CHAR> name;
			};

			struct if_scope
			{
			};

			struct else_scope
			{
			};

			struct switch_scope
			{
			};

			struct case_scope
			{
			};

			struct declaration_scope
			{
			};

			struct call_scope
			{
				bool empty = true;
			};

			struct function_call_scope : public call_scope
			{
			};

			struct template_call_scope : public call_scope
			{
			};

			lak::array<lak::variant<block_scope,
			                        namespace_scope,
			                        if_scope,
			                        else_scope,
			                        switch_scope,
			                        case_scope,
			                        declaration_scope,
			                        function_call_scope,
			                        template_call_scope>>
			  scopes;

			struct preproc_header_guard_scope
			{
			};

			struct preproc_if_scope
			{
			};

			lak::array<lak::variant<preproc_header_guard_scope, preproc_if_scope>>
			  preproc_scopes;

			lak::string<CHAR> _result;

			operator lak::string_view<CHAR> const() { return _result; }

			lak::string<CHAR> release() { return lak::exchange(_result, {}); }

			cpp_writer &write_indent()
			{
				_result.reserve(_result.size() + (indent.size() * scopes.size()));
				for (size_t i = 0; i < scopes.size(); ++i) _result += indent;
				return *this;
			}

			cpp_writer &write_newline()
			{
				_result += newline;
				return *this;
			}

			cpp_writer &write_indent_newline()
			{
				write_newline();
				write_indent();
				return *this;
			}

			template<typename C = CHAR>
			cpp_writer &write_preproc(lak::string_view<C> preproc)
			{
				auto pre = lak::strconv<CHAR>(preproc);

				size_t indent_count = preproc_scopes.size();
				if (!preproc_scopes.empty() &&
				    preproc_scopes.front()
				      .template holds<preproc_header_guard_scope>())
					--indent_count;

				_result.reserve(_result.size() + (indent.size() * indent_count) + 1U +
				                pre.size());
				_result += CHAR('#');
				for (size_t i = 0; i < indent_count; ++i) _result += indent;
				_result += pre;
				return *this;
			}

			template<typename C = CHAR>
			cpp_writer &write(lak::string_view<C> str)
			{
				_result += lak::strconv<CHAR>(str);
				return *this;
			}

			template<typename C = CHAR>
			cpp_writer &write(const lak::string<C> &str)
			{
				_result += lak::strconv<CHAR>(str);
				return *this;
			}

			cpp_writer &write(CHAR c)
			{
				_result += c;
				return *this;
			}

			template<typename T>
			void _push_scope(T &&t, CHAR symb = CHAR('{'))
			{
				scopes.push_back(lak::forward<T>(t));
				write(symb);
			}

			template<typename T>
			bool is_scope()
			{
				if (scopes.empty()) return false;
				return scopes.back().template holds<T>();
			}

			template<typename T>
			T &_get_scope()
			{
				ASSERT(!scopes.empty());
				ASSERT(scopes.back().template holds<T>());
				return *scopes.back().template get<T>();
			}

			template<typename T>
			T _pop_scope(bool on_newline = false, CHAR symb = CHAR('}'))
			{
				ASSERT(!scopes.empty());
				ASSERT(scopes.back().template holds<T>());
				auto s = scopes.popped_back();
				if (on_newline) write_indent_newline();
				write(symb);
				return lak::move(*s.template get<T>());
			}

			template<typename T>
			void _push_preproc_scope(T &&t)
			{
				preproc_scopes.push_back(lak::forward<T>(t));
			}

			template<typename T>
			T _pop_preproc_scope()
			{
				ASSERT(!preproc_scopes.empty());
				ASSERT(preproc_scopes.back().template holds<T>());
				return *preproc_scopes.popped_back().template get<T>();
			}

			cpp_writer &push_block()
			{
				_push_scope(block_scope());
				return *this;
			}

			cpp_writer &pop_block()
			{
				_pop_scope<block_scope>();
				return *this;
			}

			cpp_writer &push_namespace(lak::string_view<CHAR> name)
			{
				write(u8"namespace "_view);
				write(name);
				write(u8" "_view);
				_push_scope(namespace_scope(name));
				return *this;
			}

			cpp_writer &pop_namespace()
			{
				auto ns = _pop_scope<namespace_scope>(true);
				write(" /* "_view);
				write(lak::string_view(ns.name));
				write(" */"_view);
				return *this;
			}

			cpp_writer &push_if(lak::string_view<CHAR> condition)
			{
				write_indent_newline();
				write(u8"if ("_view);
				write(condition);
				write(u8") "_view);
				_push_scope(if_scope());
				return *this;
			}

			cpp_writer &pop_if()
			{
				_pop_scope<if_scope>();
				return *this;
			}

			cpp_writer &push_else_if(lak::string_view<CHAR> condition)
			{
				_pop_scope<if_scope>();
				write(u8"else if ("_view);
				write(condition);
				write(u8") "_view);
				_push_scope(if_scope());
				return *this;
			}

			cpp_writer &push_else()
			{
				_pop_scope<if_scope>();
				write(u8"else "_view);
				_push_scope(else_scope());
				return *this;
			}

			cpp_writer &pop_else()
			{
				_pop_scope<else_scope>();
				return *this;
			}

			cpp_writer &push_switch(lak::string_view<CHAR> condition,
			                        auto &&block_func)
			{
				write_indent_newline();
				write(u8"switch ("_view);
				write(condition);
				write(u8") "_view);
				_push_scope(switch_scope());
				return *this;
			}

			cpp_writer &pop_switch()
			{
				_pop_scope<switch_scope>();
				return *this;
			}

			cpp_writer &write_case_label(lak::string_view<CHAR> value)
			{
				write_indent_newline();
				write(u8"case "_view);
				write(value);
				write(u8":"_view);
				return *this;
			}

			cpp_writer &write_default_case_label()
			{
				write_indent_newline();
				write(u8"default:"_view);
				return *this;
			}

			cpp_writer &push_case_block(auto &&block_func)
			{
				_push_scope(case_scope());
				return *this;
			}

			cpp_writer &pop_case_block(bool fallthrough = false)
			{
				_pop_scope<case_scope>();
				if (!fallthrough) write(u8" break;"_view);
				return *this;
			}

			cpp_writer &push_variable_declaration(lak::string_view<CHAR> type,
			                                      lak::string_view<CHAR> name,
			                                      bool start_newline = true)
			{
				if (start_newline) write_indent_newline();
				write(type);
				write(u8" "_view);
				write(name);
				write(u8" ="_view);
				_push_scope(declaration_scope(), CHAR(' '));
				return *this;
			}

			cpp_writer &pop_declaration()
			{
				_pop_scope<declaration_scope>(false, CHAR(';'));
				return *this;
			}

			cpp_writer &write_variable_declaration(
			  lak::string_view<CHAR> type,
			  lak::string_view<CHAR> name,
			  lak::optional<lak::string_view<CHAR>> value = lak::nullopt,
			  bool start_newline                          = true)
			{
				if (start_newline) write_indent_newline();
				write(type);
				write(u8" "_view);
				write(name);
				if (value)
				{
					write(u8" = "_view);
					write(*value);
				}
				write(u8";"_view);
				return *this;
			}

			cpp_writer &push_function_call(lak::string_view<CHAR> function_name)
			{
				write(function_name);
				_push_scope(function_call_scope(), CHAR('('));
				return *this;
			}

			cpp_writer &pop_function_call()
			{
				_pop_scope<function_call_scope>(false, CHAR(')'));
				return *this;
			}

			cpp_writer &next_function_argument(bool on_newline = false)
			{
				auto &scope = _get_scope<function_call_scope>();
				if (!scope.empty)
					write(on_newline ? u8","_view : u8", "_view);
				else
					scope.empty = false;
				if (on_newline) write_indent_newline();
				return *this;
			}

			cpp_writer &push_template_call(lak::string_view<CHAR> template_name)
			{
				write(template_name);
				_push_scope(template_call_scope(), CHAR('<'));
				return *this;
			}

			cpp_writer &pop_template_call()
			{
				_pop_scope<template_call_scope>(false, CHAR('>'));
				return *this;
			}

			cpp_writer &next_template_argument(bool on_newline = false)
			{
				auto &scope = _get_scope<template_call_scope>();
				if (!scope.empty)
					write(on_newline ? u8","_view : u8", "_view);
				else
					scope.empty = false;
				if (on_newline) write_indent_newline();
				return *this;
			}

			cpp_writer &write_string(lak::string_view<CHAR> str,
			                         lak::string_view<CHAR> prefix,
			                         lak::string_view<CHAR> suffix = {})
			{
				lak::string<CHAR> result;
				if (!prefix.empty()) write(prefix);
				write(CHAR('"'));

				auto [first, second] =
				  lak::split_before<const CHAR>(lak::span<const CHAR>(str), CHAR('"'));
				while (!second.empty())
				{
					second = second.subspan(1U);
					write(u8"\\\""_view);
					if (second.empty()) break;
					lak::tie(first, second) =
					  lak::split_before<const CHAR>(second, CHAR('"'));
					write(lak::string_view(first));
				}

				write(str);
				write(CHAR('"'));
				if (!suffix.empty()) write(suffix);
				return *this;
			}

			cpp_writer &push_preproc_if(lak::string_view<CHAR> cond)
			{
				write_preproc(u8"if "_view);
				write(cond);
				write_newline();
				_push_preproc_scope(preproc_if_scope());
				return *this;
			}

			cpp_writer &push_preproc_ifdef(lak::string_view<CHAR> cond)
			{
				write_preproc(u8"ifdef "_view);
				write(cond);
				write_newline();
				_push_preproc_scope(preproc_if_scope());
				return *this;
			}

			cpp_writer &write_preproc_define(lak::string_view<CHAR> def)
			{
				write_preproc(u8"define "_view);
				write(def);
				write_newline();
				return *this;
			}

			cpp_writer &pop_preproc_if()
			{
				_pop_preproc_scope<preproc_if_scope>();
				write_preproc(u8"endif"_view);
				write_newline();
				return *this;
			}

			cpp_writer &push_header_guard(lak::string_view<CHAR> name)
			{
				ASSERT(preproc_scopes.empty());

				write_preproc(u8"ifndef "_view);
				write(name);
				write_newline();
				write_preproc(u8"define "_view);
				write(name);
				write_newline();

				_push_preproc_scope(preproc_header_guard_scope());

				write_newline();
				return *this;
			}

			cpp_writer &pop_header_guard()
			{
				_pop_preproc_scope<preproc_header_guard_scope>();
				write_newline();
				write_preproc(u8"endif"_view);
				write_newline();
				return *this;
			}

			cpp_writer &write_comment(lak::string_view<CHAR> str)
			{
				write(u8"/* "_view);
				write(str);
				write(u8" */"_view);
				return *this;
			}

			cpp_writer &write_include_ang(lak::string_view<CHAR> path)
			{
				write_preproc(u8"include <"_view);
				write(path);
				write(u8">"_view);
				write_newline();
				return *this;
			}

			cpp_writer &write_include_str(lak::string_view<CHAR> path)
			{
				write_preproc(u8"include \""_view);
				write(path);
				write(u8"\""_view);
				write_newline();
				return *this;
			}
		};
	}
}

LAK_EXTERN_TEMPLATE_FOREACH_CHAR(lak::codegen::cpp_writer);

#endif
