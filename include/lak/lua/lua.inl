#include "lak/lua/lua.hpp"

template<>
struct lak::unique_com_ptr_traits<lua_State>
{
	using handle_type  = lua_State *;
	using exposed_type = lua_State *;

	inline static lak::result<handle_type> ctor()
	{
		handle_type result = luaL_newstate();
		if (result == nullptr)
			return lak::err_t{};
		else
			return lak::ok_t{result};
	}

	inline static auto dtor(handle_type handle) { lua_close(handle); }
};
