#ifndef LAK_LUA_LUA_HPP
#define LAK_LUA_LUA_HPP

#include "lak/array.hpp"
#include "lak/com_ptr.hpp"
#include "lak/result.hpp"
#include "lak/string_view.hpp"
#include "lak/tuple.hpp"

extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

namespace lak
{
	namespace lua
	{
		using state_ptr = lak::unique_com_ptr<lua_State>;

		inline int load(lua_State *state, lak::astring_view str)
		{
			return luaL_loadbuffer(state, str.begin(), str.size(), str.begin());
		}

		inline lak::result<lak::astring_view> to_string(lua_State *state,
		                                                int index)
		{
			size_t out_len  = 0;
			const char *res = lua_tolstring(state, index, &out_len);
			if (res)
				return lak::ok_t{lak::astring_view(res, out_len)};
			else
				return lak::err_t{};
		}

		inline lak::result<lua_Number> to_number(lua_State *state, int index)
		{
			int is_num        = 0;
			lua_Number result = lua_tonumberx(state, index, &is_num);
			if (is_num)
				return lak::ok_t{result};
			else
				return lak::err_t{};
		}

		inline lak::result<lua_Integer> to_integer(lua_State *state, int index)
		{
			int is_num         = 0;
			lua_Integer result = lua_tointegerx(state, index, &is_num);
			if (is_num)
				return lak::ok_t{result};
			else
				return lak::err_t{};
		}

		inline bool to_boolean(lua_State *state, int index)
		{
			return lua_toboolean(state, index) != 0;
		}

		template<typename T>
		lak::result<T> get(lua_State *state, int index);

		template<>
		lak::result<lak::astring_view> get<lak::astring_view>(lua_State *state,
		                                                      int index)
		{
			return lak::lua::to_string(state, index);
		}

		template<>
		lak::result<lua_Number> get<lua_Number>(lua_State *state, int index)
		{
			return lak::lua::to_number(state, index);
		}

		template<>
		lak::result<lua_Integer> get<lua_Integer>(lua_State *state, int index)
		{
			return lak::lua::to_integer(state, index);
		}

		template<>
		lak::result<bool> get<bool>(lua_State *state, int index)
		{
			return lak::ok_t{lak::lua::to_boolean(state, index)};
		}

		template<typename... T>
		lak::result<lak::tuple<T...>> tuple_get(
		  lua_State *state, lak::span<const int, sizeof...(T)> index)
		{
			return [&]<size_t... I>(
			         lak::index_sequence<I...>) -> lak::result<lak::tuple<T...>>
			{
				return lak::combine_ok(
				  lak::bind_front(lak::lua::get<T>, state, index[I])...);
			}(lak::index_sequence_for<T...>{});
		}

		template<typename FUNC>
		struct function_wrapper;

		template<
		  typename RTN,
		  lak::concepts::
		    one_of<void, lak::astring_view, lua_Number, lua_Integer>... ARGS>
		struct function_wrapper<RTN(ARGS...)>
		{
			using return_type   = RTN;
			using argument_type = lak::tuple<ARGS...>;

			static constexpr size_t arity = sizeof...(ARGS);

			static auto get_args(lua_State *state)
			{
				return [&]<size_t... I>(lak::index_sequence<0, I...>)
				{
					lak::array<int, sizeof...(ARGS)> index{I...};
					return lak::lua::tuple_get<ARGS...>(state, index);
				}(lak::index_sequence_for<int, ARGS...>{});
			}
		};

		template<typename RTN, typename... ARGS>
		struct function_wrapper<RTN (*)(ARGS...)>
		: public function_wrapper<RTN(ARGS...)>
		{
		};

#define LAK_LUA_WRAPPER(...)                                                  \
	[](lua_State *state) -> int                                                 \
	{                                                                           \
		using func_type    = decltype(__VA_ARGS__);                               \
		using func_wrapper = lak::lua::function_wrapper<func_type>;               \
		if (int arity = lua_gettop(state); arity != func_wrapper::arity)          \
		{                                                                         \
			lua_pushliteral(state, "incorrect arity");                              \
			lua_error(state);                                                       \
			return 0;                                                               \
		}                                                                         \
		if_let_ok (auto args, func_wrapper::get_args(state))                      \
		{                                                                         \
			if constexpr (lak::is_void_v<typename func_wrapper::return_type>)       \
			{                                                                       \
				lak::apply((__VA_ARGS__), lak::move(args));                           \
				return 0;                                                             \
			}                                                                       \
			else                                                                    \
			{                                                                       \
				auto simple_push = lak::overloaded{                                   \
					[&](lua_Number num)                                                 \
					{                                                                   \
						lua_pushnumber(state, num);                                       \
						return 1;                                                         \
					},                                                                  \
					[&](lua_Integer num)                                                \
					{                                                                   \
						lua_pushinteger(state, num);                                      \
						return 1;                                                         \
					},                                                                  \
					[&](lak::astring str)                                               \
					{                                                                   \
						lua_pushlstring(state, str.c_str(), str.size());                  \
						return 1;                                                         \
					},                                                                  \
					[&](lak::astring_view str)                                          \
					{                                                                   \
						lua_pushlstring(state, str.begin(), str.size());                  \
						return 1;                                                         \
					},                                                                  \
				};                                                                    \
				return lak::overloaded{                                               \
					simple_push,                                                        \
					[&]<typename OK, typename ERR>(lak::result<OK, ERR> res)            \
					{                                                                   \
						if constexpr (lak::is_same_v<lak::monostate, OK> &&               \
						              lak::is_same_v<lak::monostate, ERR>)                \
						{                                                                 \
							lua_pushboolean(state, res.is_ok());                            \
							return 1;                                                       \
						}                                                                 \
						if (res.is_ok())                                                  \
						{                                                                 \
							if constexpr (!lak::is_same_v<lak::monostate, OK>)              \
								simple_push(res.unsafe_unwrap());                             \
							if constexpr (!lak::is_same_v<lak::monostate, ERR>)             \
								lua_pushnil(state);                                           \
						}                                                                 \
						else                                                              \
						{                                                                 \
							if constexpr (!lak::is_same_v<lak::monostate, OK>)              \
								lua_pushnil(state);                                           \
							if constexpr (!lak::is_same_v<lak::monostate, ERR>)             \
								simple_push(res.unsafe_unwrap_err());                         \
						}                                                                 \
						return (lak::is_same_v<lak::monostate, OK> ? 0 : 1) +             \
						       (lak::is_same_v<lak::monostate, ERR> ? 0 : 1);             \
					},                                                                  \
				}(lak::apply((__VA_ARGS__), lak::move(args)));                        \
			}                                                                       \
		}                                                                         \
		else                                                                      \
		{                                                                         \
			lua_pushliteral(state, "incorrect argument type");                      \
			lua_error(state);                                                       \
			return 0;                                                               \
		}                                                                         \
	};
	}
}

#include "lua.inl"

#endif
