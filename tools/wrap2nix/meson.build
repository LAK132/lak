lak_wrap2nix = executable(
	'wrap2nix',
	[
		'main.cpp',
	],
	dependencies: [
		lakcore_dep,
		lakfile_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('wrap2nix'),
	install_rpath: get_option('prefix')/get_option('libdir'),
)
