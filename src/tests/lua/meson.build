laktestlua = static_library(
	'laktestlua',
	[
		'lua.cpp',
	],
	override_options: {
		'warning_level': '2',
	},
	dependencies: [
		lakcore_dep,
		laklua_dep,
	],
)

laktestlua_dep = declare_dependency(
	link_whole: laktestlua,
)
