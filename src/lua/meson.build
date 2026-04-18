laklua = static_library(
	'laklua',
	[
		'lua.cpp',
	],
	dependencies: [
		lakcore_dep,
		lua_dep,
	],
)

laklua_dep = declare_dependency(
	link_with: laklua,
	dependencies: lua_dep,
)
meson.override_dependency('lak-lua', laklua_dep)
