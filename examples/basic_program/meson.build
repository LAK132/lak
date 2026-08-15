if not get_option('lak_enable_windowing')
	warning('basic_program requires windowing')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('basic_program requires imgui')
	subdir_done()
endif

executable(
	'basic_program',
	[
		'main.cpp',
	],
	dependencies: [
		lak_dep,
		implot_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('basic_program'),
	install_rpath: get_option('prefix')/get_option('bindir'),
)
