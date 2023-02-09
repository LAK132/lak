#include "lak/lua/lua.hpp"

#include "lak/string_literals.hpp"

#include "lak/test.hpp"

thread_local lua_Integer my_thing = 0;

lak::result<lua_Integer> my_wrapped_func(lua_Integer num)
{
	if (num % 2 == 0)
		return lak::ok_t{num};
	else
		return lak::err_t{};
}

BEGIN_TEST(lua)
{
	auto state{lak::lua::state_ptr::create().UNWRAP()};
	luaL_openlibs(state);
	lua_CFunction my_func = [](lua_State *state) -> int
	{
		int argc = lua_gettop(state);
		ASSERT_EQUAL(argc, 2);
		auto [n1, n2] = lak::lua::tuple_get<lua_Integer, lua_Integer>(
		                  state, lak::array<int, 2>{1, 2})
		                  .UNWRAP();
		my_thing = n1 + n2;
		return 0;
	};
	lua_register(state, "my_thing", my_func);

	lua_CFunction my_wrapped = LAK_LUA_WRAPPER(my_wrapped_func);
	lua_register(state, "my_wrapped", my_wrapped);

	const auto str = R"(
		my_thing(2, 3)
		print(my_wrapped(1))
		print(my_wrapped(2))
		print(my_wrapped(3))
		print(my_wrapped(4))
		-- print(my_wrapped(nil))
		-- print(my_wrapped(nil, nil))
	)"_view;

	my_thing = 0;
	if (lak::lua::load(state, str) || lua_pcall(state, 0, 0, 0))
	{
		ERROR(lak::lua::to_string(state, -1).UNWRAP());
		lua_pop(state, 1);
		return 1;
	}
	ASSERT_EQUAL(my_thing, 5);

	return 0;
}
END_TEST()
