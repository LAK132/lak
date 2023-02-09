laktestlua = static_library(
	'laktestlua',
	[
		'lua.cpp',
	],
	override_options: [
		'cpp_std=' + version,
		'warning_level=2',
		'werror=false',
	],
	dependencies: [
		lakcore_dep,
		laklua_dep,
	],
)

laktestlua_dep = declare_dependency(
	link_whole: laktestlua,
)
