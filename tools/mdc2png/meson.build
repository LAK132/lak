if not get_option('lak_enable_stb_image_write')
	warning('mdc2png requires stb image write')
	subdir_done()
endif

lak_mdc2png = executable(
	'mdc2png',
	[
		'main.cpp',
	],
	dependencies: [
		lakcore_dep,
		lakfile_dep,
		stb_image_write_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('mdc2png'),
	install_rpath: get_option('prefix')/get_option('libdir'),
)
