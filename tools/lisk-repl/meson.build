executable(
	'lisk-repl',
	[
		'main.cpp'
	],
	dependencies: [
		lakcore_dep,
		laklisk_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('lisk-repl'),
	install_rpath: get_option('prefix')/get_option('libdir'),
)
