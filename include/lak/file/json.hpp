#ifndef LAK_FILE_JSON_HPP
#define LAK_FILE_JSON_HPP

#include "lak/array.hpp"
#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"
#include "lak/format.hpp"
#include "lak/result.hpp"
#include "lak/soa_tree.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	namespace json
	{
		struct token;
		struct string;
		struct number;
		struct array;
		struct object;
		struct value;
	}

	template<>
	struct _array_type_is_copyable<lak::json::token> : lak::true_type
	{
	};
	template<>
	struct _array_type_is_copyable<lak::json::string> : lak::true_type
	{
	};
	template<>
	struct _array_type_is_copyable<lak::json::number> : lak::true_type
	{
	};
	template<>
	struct _array_type_is_copyable<lak::json::array> : lak::true_type
	{
	};
	template<>
	struct _array_type_is_copyable<lak::json::object> : lak::true_type
	{
	};
	template<>
	struct _array_type_is_copyable<lak::json::value> : lak::true_type
	{
	};

	namespace json
	{

		using _block = lak::soa_tree<lak::json::token,
		                             lak::json::string,
		                             lak::json::number,
		                             lak::json::array,
		                             lak::json::object,
		                             lak::json::value>;

		struct token
		{
			lak::u8string_view value;
		};

		struct string
		{
			lak::u8string_view value;
		};

		struct number
		{
			lak::u8string_view value;
		};

		struct array
		{
			using value_type = _block::subspan<lak::json::value>;
			value_type values;
			size_t size() const { return values.size; }
		};

		struct object
		{
			using value_type = _block::subspan<lak::json::value>;
			value_type values;
			size_t size() const { return values.size / 2U; }
		};

		struct value
		{
			using value_type = _block::limited_dyn_pointer<lak::json::token,
			                                               lak::json::string,
			                                               lak::json::number,
			                                               lak::json::array,
			                                               lak::json::object>;
			value_type value;
		};

		namespace err
		{
			struct unexpected_type
			{
				_block::index_type expected;
				_block::index_type got;
			};
		}

		struct value_proxy;
		struct array_proxy;
		struct object_proxy;

		struct block
		{
			_block _value;
			lak::array<char8_t> _internal;

			auto &tokens() { return _value.get<lak::json::token>(); }
			auto &tokens() const { return _value.get<lak::json::token>(); }
			auto &strings() { return _value.get<lak::json::string>(); }
			auto &strings() const { return _value.get<lak::json::string>(); }
			auto &numbers() { return _value.get<lak::json::number>(); }
			auto &numbers() const { return _value.get<lak::json::number>(); }
			auto &values() { return _value.get<lak::json::value>(); }
			auto &values() const { return _value.get<lak::json::value>(); }
			auto &arrays() { return _value.get<lak::json::array>(); }
			auto &arrays() const { return _value.get<lak::json::array>(); }
			auto &objects() { return _value.get<lak::json::object>(); }
			auto &objects() const { return _value.get<lak::json::object>(); }

			void intern(); // reallocate view internall to the block

			lak::json::value_proxy root() const;
		};

		struct token_proxy : public lak::u8string_view
		{
		};
		struct string_proxy : public lak::u8string_view
		{
		};
		struct number_proxy : public lak::u8string_view
		{
		};

		struct array_proxy
		{
			const lak::json::_block &block;
			lak::json::array array;

			inline size_t size() const { return array.size(); }

			lak::json::value_proxy operator[](size_t index) const;
		};

		struct object_proxy
		{
			const lak::json::_block &block;
			lak::json::object object;

			inline size_t size() const { return object.size(); }

			lak::pair<lak::u8string_view, lak::json::value_proxy> operator[](
			  size_t index) const;

			lak::optional<lak::json::value_proxy> operator[](
			  lak::u8string_view key) const;

			lak::result<lak::json::value_proxy> get(lak::u8string_view key) const;
		};

		struct value_proxy
		{
			template<typename T>
			using result_type = lak::result<T, lak::json::err::unexpected_type>;
			template<typename T>
			using num_result_type =
			  lak::result<T,
			              lak::variant<lak::json::err::unexpected_type,
			                           lak::err::string_to_numeric>>;

			const lak::json::_block &block;
			lak::json::value value;

			bool is_token() const;
			bool is_string() const;
			bool is_number() const;
			bool is_array() const;
			bool is_object() const;

			result_type<lak::u8string_view> token() const;
			result_type<lak::u8string_view> string() const;
			result_type<lak::u8string_view> number_str() const;
			result_type<lak::json::array_proxy> array() const;
			result_type<lak::json::object_proxy> object() const;

			template<typename NUM>
			num_result_type<NUM> number() const
			{
				return number_str().and_then(
				  [](lak::u8string_view num) -> num_result_type<NUM>
				  {
					  if constexpr (!std::numeric_limits<NUM>::is_integer)
					  {
						  constexpr auto num_parser =
						    lak::dsl::dec_float<lak::dsl::char_literal<U'.'>,
						                        lak::dsl::one_of_chars_str<U"eE">>;
						  auto [intp, fracp, expp] = num_parser.parse(num).UNWRAP().value;
						  RES_TRY_ASSIGN(double v =,
						                 lak::dec_string_to_double(intp, fracp, expp));
						  if (v > std::numeric_limits<NUM>::max() ||
						      v < std::numeric_limits<NUM>::lowest())
							  return lak::err_t{lak::err::string_to_numeric::out_of_bounds};
						  else
							  return lak::ok_t{static_cast<NUM>(v)};
					  }
					  else
					  {
						  RES_TRY_ASSIGN(
						    NUM result =,
						    lak::string_to_int<NUM>(num, lak::numeric_base::dec));
						  return lak::ok_t{result};
					  }
				  });
			}

			inline auto visit(auto &&func) const
			{
				return block[value.value].visit(func);
			}
		};
	}

	namespace dsl
	{
		struct json_t
		{
			static constexpr bool is_pure_match = false;

			using value_type = lak::json::block;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const;
		};

		inline constexpr json_t json;

		static_assert(lak::dsl::concepts::parser<json_t>);
	}

	template<typename CHAR>
	struct format_traits<lak::json::_block::index_type, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::json::_block::index_type &type)
		{
			switch (type.value())
			{
				case lak::json::_block::index_of<lak::json::token>:
					return lak::strconv<CHAR>("token"_view);
				case lak::json::_block::index_of<lak::json::string>:
					return lak::strconv<CHAR>("string"_view);
				case lak::json::_block::index_of<lak::json::number>:
					return lak::strconv<CHAR>("number"_view);
				case lak::json::_block::index_of<lak::json::array>:
					return lak::strconv<CHAR>("array"_view);
				case lak::json::_block::index_of<lak::json::object>:
					return lak::strconv<CHAR>("object"_view);
				case lak::json::_block::index_of<lak::json::value>:
					return lak::strconv<CHAR>("value"_view);
				default:
					return lak::fmt<CHAR, "{:#0X}">(
					  static_cast<uintmax_t>(type.value()));
			}
		}
	};

	template<typename CHAR>
	struct format_traits<lak::json::err::unexpected_type, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::json::err::unexpected_type &err)
		{
			return lak::fmt<CHAR, "expected {}, got {}">(err.expected, err.got);
		}
	};

	template<typename T>
	struct from_json_traits;

	namespace json
	{
		namespace err
		{
			struct missing_object_key
			{
				lak::u8string expected;
			};
		}
	}

	template<typename CHAR>
	struct format_traits<lak::json::err::missing_object_key, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::json::err::missing_object_key &err)
		{
			return lak::fmt<CHAR, "missing object key '{}'">(err.expected);
		}
	};

	namespace concepts
	{
		template<typename T>
		concept has_from_json_traits = requires() {
			typename lak::from_json_traits<T>::value_type;
			typename lak::from_json_traits<T>::error_type;
			{
				lak::from_json_traits<T>::from_json(
				  lak::declval<const lak::json::value_proxy &>())
			} -> lak::concepts::same_as<
			  lak::result<typename lak::from_json_traits<T>::value_type,
			              typename lak::from_json_traits<T>::error_type>>;
		};
	}

	template<typename T>
	requires(lak::concepts::has_from_json_traits<T>)
	auto from_json(const lak::json::value_proxy &vp)
	{
		return lak::from_json_traits<T>::from_json(vp);
	}

	template<typename T, auto... MEMBERS>
	struct from_json_traits_fixed_struct_impl;

	template<typename T, lak::u8const_string STR, auto MEMBER>
	requires(lak::is_of_template_v<
	         lak::remove_cvref_t<decltype(lak::declval<T &>().*MEMBER)>,
	         lak::optional>)
	struct from_json_traits_fixed_struct_impl<T, STR, MEMBER>
	{
		using value_type =
		  typename lak::remove_cvref_t<decltype(lak::declval<T &>().*
		                                        MEMBER)>::value_type;
		using error_type = typename lak::from_json_traits<value_type>::error_type;
		static force_inline lak::error_code<error_type> from_json(
		  const lak::json::object_proxy &obj, T &t)
		{
			if_let_some (auto mem, obj[STR])
			{
				RES_TRY_ASSIGN(t.*MEMBER =, lak::from_json<value_type>(mem));
			}
			return lak::ok_t{};
		}
	};

	template<typename T, lak::u8const_string STR, auto MEMBER>
	requires(!lak::is_of_template_v<
	         lak::remove_cvref_t<decltype(lak::declval<T &>().*MEMBER)>,
	         lak::optional>)
	struct from_json_traits_fixed_struct_impl<T, STR, MEMBER>
	{
		using value_type =
		  lak::remove_cvref_t<decltype(lak::declval<T &>().*MEMBER)>;
		using error_type = lak::unique_errors_t<
		  lak::json::err::missing_object_key,
		  typename lak::from_json_traits<value_type>::error_type>;
		static force_inline lak::error_code<error_type> from_json(
		  const lak::json::object_proxy &obj, T &t)
		{
			RES_TRY_ASSIGN(
			  auto mem =,
			  obj.get(STR).map_err(
			    [](auto &&)
			    { return lak::json::err::missing_object_key{.expected = STR}; }));
			RES_TRY_ASSIGN(t.*MEMBER =, lak::from_json<value_type>(mem));
			return lak::ok_t{};
		}
	};

	template<typename T, lak::u8const_string STR, auto MEMBER, auto... MEMBERS>
	struct from_json_traits_fixed_struct_impl<T, STR, MEMBER, MEMBERS...>
	{
		using error_type = lak::unique_errors_t<
		  typename from_json_traits_fixed_struct_impl<T, STR, MEMBER>::error_type,
		  typename from_json_traits_fixed_struct_impl<T, MEMBERS...>::error_type>;
		static force_inline lak::error_code<error_type> from_json(
		  const lak::json::object_proxy &obj, T &t)
		{
			RES_TRY(
			  from_json_traits_fixed_struct_impl<T, STR, MEMBER>::from_json(obj, t));
			RES_TRY(
			  from_json_traits_fixed_struct_impl<T, MEMBERS...>::from_json(obj, t));
			return lak::ok_t{};
		}
	};

	// LAK_FIXED_STRUCT_FROM_JSON_TRAITS(
	//   type, lak::u8const_string(u8"json-obj-name"), &type::member, ...)
#define LAK_FIXED_STRUCT_FROM_JSON_TRAITS(TYPE, ...)                          \
	template<>                                                                  \
	struct lak::from_json_traits<TYPE>                                          \
	{                                                                           \
		using value_type = TYPE;                                                  \
		using impl_type =                                                         \
		  lak::from_json_traits_fixed_struct_impl<TYPE, __VA_ARGS__>;             \
		using error_type = lak::unique_errors_t<lak::json::err::unexpected_type,  \
		                                        typename impl_type::error_type>;  \
		static lak::result<value_type, error_type> from_json(                     \
		  const lak::json::value_proxy &value)                                    \
		{                                                                         \
			RES_TRY_ASSIGN(auto obj =, value.object());                             \
			value_type result;                                                      \
			RES_TRY(impl_type::from_json(obj, result));                             \
			return lak::move_ok(result);                                            \
		}                                                                         \
	};                                                                          \
	static_assert(lak::concepts::has_from_json_traits<TYPE>);
}

#include "json.inl"

#endif
