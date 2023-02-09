lua_dep = subproject('lua').get_variable('lua_lib_dep')

laklua = static_library(
	'laklua',
	[
		'lua.cpp',
	],
	override_options: 'cpp_std=' + version,
	dependencies: [
		lakcore_dep,
		lua_dep,
	],
)

laklua_dep = declare_dependency(
	link_with: laklua,
	dependencies: lua_dep,
)
